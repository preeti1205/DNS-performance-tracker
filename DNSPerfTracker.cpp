/*
This class periodically sends DNS queries to the name servers of sites 
and stores the latency values in a MySQL table. 
The frequency of queries can be specified by the user on command line.
*/
#include "DNSPerfTracker.h"

#include <vector>
#include <string>
#include <thread>
#include <stdlib.h>
#include <ldns/ldns.h>
#include <time.h>
#include <chrono>
#include <mutex>

std::mutex mtx;

//generate a random string to append with the url
//Input:
//		len : length of the string desired
//Output:
//		string str: randomly generated string of size len
std::string gen_random_string(const int len) {
	srand(time(0));
	auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(len,0);
    std::generate_n( str.begin(), len, randchar );
    return str;
}

//calculate new average value
//Input:
//		double oldVal: old average value
//      double newVal: new data to be accounted for in the calculation 
//		double count:  new total count including newVal
//Output:
//		double : new average value

double calculateNewAvg(double oldVal, double newVal, double count){
	return (oldVal + (newVal - oldVal) / count);
}

//calculate new standard deviation
//Input:
//		double oldAvg: old average value
//		double oldVal: old standard deviation
//      double newVal: new data to be accounted for in the calculation 
//		double count:  new total count including newVal
//Output:
//		double : new standard deviation
double calculateNewStdDev(double oldAvg, double oldVal, double newVal, double count){
	double newAvg = calculateNewAvg(oldVal, newVal, count);
	// TODO: Check for case where denominator is 0
	return sqrt(((count - 2)* pow(oldVal, 2) + (newVal - newAvg)*(newVal - oldAvg)) / (count - 1));
}

//constructor initializing with default frequency of 60 queries per minute
DNSPerfTracker::DNSPerfTracker(): frequency(60), isRunning(false) {}


//change the frequency of queries
//Input:
//		int perMinute: new frequency to be set
void DNSPerfTracker::setFrequency(int perMinute){
	if(perMinute <= 0 ) {
		std::cout << " Only values greater than 0 accepted ";
	}
	else
		frequency = perMinute;
}

//obtain the current frequency of queries
//Output:
//		unsigned int perMinute: current frequency
unsigned int DNSPerfTracker::getFrequency() const{
	return frequency;
}

//prepare the database and populate DNSPerfTracker::siteNames and DNSPerfTracker::records
//existing values in the database
//Input:
//		string setupFile: configuration file to read database information from
void DNSPerfTracker::prepareDependencies(std::string setupFile){
	try{
		dbMan = new dbManager(setupFile);
		if(dbMan->prepareDB())
			addAlexaWebsites();
		std::vector<std::string> nameList = dbMan->getWebsites();
		for(size_t i = 0; i < nameList.size(); ++i ){
			if(siteNames.find(nameList[i]) == siteNames.end())
				siteNames.insert(make_pair(nameList[i], true));
		}
		records = (dbMan->prepareStats());
	}
	catch(...) {
		std::cout << "Error occured while setting up the database" << std::endl;
	}
}

//send ldns packet to the website address to measure latency
//Input:
//		const char siteName[]: name of the url
//Output:
//		int latency: latency

int DNSPerfTracker::ldnsLatency(const char siteName[]) {
	ldns_resolver *res;
	ldns_rdf *host;
	ldns_pkt *p;
	ldns_status s;

	res = NULL;
	p = NULL;
	host = NULL;

	double latency = 0;

	/* create a new resolver from /etc/resolv.conf(NULL) */
	s = ldns_resolver_new_frm_file(&res, NULL);
	if (s != LDNS_STATUS_OK) {
		ldns_rdf_deep_free(host);
		exit(EXIT_FAILURE);
	}
	host = ldns_dname_new_frm_str(siteName);
	if (!host) {
			exit(EXIT_FAILURE);
	}
	else {
		if (!ldns_dname_str_absolute(siteName) && ldns_dname_absolute(host)) {
			ldns_rdf_set_size(host, ldns_rdf_size(host) - 1);
        }
	    unsigned long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>
	    							(std::chrono::system_clock::now().time_since_epoch()).count(); //check what this returns
	    p = ldns_resolver_query(res, host, LDNS_RR_TYPE_A,
				LDNS_RR_CLASS_IN, LDNS_RD);
	    unsigned long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>
	                                 (std::chrono::system_clock::now().time_since_epoch()).count();
		latency = endTime - startTime;
		ldns_rdf_deep_free(host);
	}
	ldns_resolver_deep_free(res);
	ldns_pkt_free(p);
	return latency;
}

//get latency of the website 
//Input:
//		string siteName: name of the url
//Output:
//		int latency: latency
int DNSPerfTracker::getLatency(std::string siteName) {
	std::string random_string = gen_random_string(10);
	std::string randSiteName( random_string + "." + siteName ); 
	int latency = ldnsLatency(randSiteName.c_str());
	return latency;
}

//find a url entry in DNSPerfTracker::records
//Input:
//		string site: name of the url to be searched
//Output:
//		std::vector<entry>::iterator it: iterator pointing to the entry
std::vector<entry>::iterator DNSPerfTracker::findEntry(std::string site) {
	std::vector<entry>::iterator it;
	for(it = records.begin(); it != records.end(); ++it){
		if(it->website == site)
			return it;
	}
	return it;
}

//add a website url to DNSPerfTracker::siteNames(and database names)
//Input:
//		string site: name of the url to be added
void DNSPerfTracker::addSite(std::string siteName){
	if(siteNames.find(siteName) == siteNames.end()) {
		siteNames.insert(make_pair(siteName, true));
		dbMan->addWebsite(siteName);
	}
}

//remove a website url from DNSPerfTracker::siteNames and all corresponding records 
// from DNSPerfTracker::records (and database names and records)
//Input:
//		string site: name of the url to be removed
void DNSPerfTracker::removeSite(std::string siteName){
	mtx.lock();

	auto i = records.begin();
	while( i != records.end()) {
		std::vector<entry>::iterator ent = findEntry(siteName);
		if(ent != records.end()) {
			i = records.erase(ent);
		}
		else
			++i;
	}

	auto it = siteNames.find(siteName);
	if(it != siteNames.end()) {
		siteNames.erase(it);
	}
	mtx.unlock();
	dbMan->removeWebsite(siteName);
}

//add top 10 websites to DNSPerfTracker::siteNames
void DNSPerfTracker::addAlexaWebsites(){
	std::string website_array[10] = {   "google.com",
		                                "youtube.com",
									    "facebook.com",
									    "baidu.com",
									    "wikipedia.org",
									    "yahoo.com",
									    "google.co.in",
									    "reddit.com",
									    "qq.com",
									    "taobao.com" };
	 
	for(size_t site_idx = 0; site_idx < 10; ++site_idx ) {
		addSite(website_array[site_idx]);
	}
}

//remove all the data from DNSPerfTracker::records and DNSPerfTracker::siteNames
void DNSPerfTracker::resetAll(){
	mtx.lock();
	isRunning = false;
	siteNames.clear();
	records.clear();
	mtx.unlock();
	dbMan->resetDB();
}

//perform tracking
void DNSPerfTracker::performTracking(){
	while(isRunning){
		mtx.lock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60 / frequency));
		std::unordered_map<std::string, bool>::iterator it;
		for( it = siteNames.begin(); it != siteNames.end(); ++it) {

			int latency = getLatency(it->first);
		    unsigned long long currTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>
		                                 (std::chrono::system_clock::now().time_since_epoch()).count();
		    dbMan->insertRecord(it->first, currTimestamp, latency);
		    std::vector<entry>::iterator ent = findEntry(it->first);
		    if( ent != records.end()) {
		    	ent->last_query_timestamp = currTimestamp;
		    	(ent->total_queries)++;
		    	double oldAvg = ent->avg_latency;
		    	double oldStdDev = ent->stddev_latency;
		    	ent->avg_latency = calculateNewAvg(oldAvg, latency, ent->total_queries);
		    	ent->stddev_latency = calculateNewStdDev(oldAvg, oldStdDev, latency, ent->total_queries);
		    }
		    else {
		    	entry newEntry(it->first);
		    	newEntry.total_queries = 1;
		    	newEntry.first_query_timestamp = currTimestamp;
		    	newEntry.last_query_timestamp = currTimestamp;
		    	newEntry.avg_latency = latency;
		    	newEntry.stddev_latency = 0;
		    	records.push_back(newEntry);
		    }
		}
		mtx.unlock();
	}
}

//get all the records from DNSPerfTracker::records
//OUTPUT:
//		std::vector<entry>: vector of all the entries in the DNSPerfTracker::records
std::vector<entry> DNSPerfTracker::getStats(){
	return records;
}

//stop tracking
void DNSPerfTracker::stopTracking(){
	isRunning = false;
}

//start tracking. If there are no websites to track, add alexa websites to DNSPerfTracker::siteNames
void DNSPerfTracker::startTracking(){
	if(!isRunning ) {
		isRunning = true;
		if(frequency <= 0) {
			std::cout << "Please enter a positive frequency ";
		}
		else{
			std::thread t(&DNSPerfTracker::performTracking, this);
			t.detach();
		}
	}
}

//see if currently tracking
//OUTPUT:
// 		bool : true if currently tracking. False otherwise
bool DNSPerfTracker::getStatus() const{
	return isRunning;
}

//get all the names from DNSPerfTracker::siteNames
//OUTPUT:
//		vector<string> result: vector of all the urls in the DNSPerfTracker::siteNames
std::vector<std::string> DNSPerfTracker::getNames() const{
	std::vector<std::string> result;
	for(std::unordered_map<std::string, bool>::const_iterator it = siteNames.begin(); it != siteNames.end(); ++it)
		result.push_back(it->first);
	return result;
}
