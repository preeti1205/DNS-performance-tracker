/*
This class handles all the code interactions with the mysql databases
*/
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>
#include <mysql++/mysql++.h>

#include "entry.h"
#include "dbManager.h"


// Connect to the database using database information provided in the configuration file
// Input:
//        setup: File containing information about database and the user
dbManager::dbManager(std::string setupFile): conn(false){
	std::string db_name, server_name, user_name, user_passwd;
	std::ifstream infile(setupFile);
	std::string str;

	while(std::getline(infile, str)){
		std::size_t separator = str.find_first_of('=');
		std::size_t terminator = str.find_first_of(';');
		std::string keyName = str.substr(0, separator);
		std::string value_of_key = str.substr(separator + 1, terminator - separator - 1);

		if(keyName == "db_name") db_name = value_of_key;
		if(keyName == "server_name") server_name = value_of_key;
		if(keyName == "user_name") user_name = value_of_key;
		if(keyName == "user_passwd") user_passwd = value_of_key;
	}

    if (conn.connect(db_name.c_str(), server_name.c_str(), user_name.c_str(), user_passwd.c_str())){
    	std::cout << "Connection to the database established " << std::endl;
    }
    else{
    	std::cout << "Error occured while connecting the database: " << conn.error() << std::endl;
    	throw("Database Exception");
    }

}

//sets up the db if it doesn't exist yet
//Output:
//		true if the database is being setup for the first time
//      false if the databse already exists

bool dbManager::prepareDB(){
	try{
		mysqlpp::StoreQueryResult result = storeQuery("SHOW TABLES LIKE 'names';");
		if (result) {

			if (result.begin() == result.end()) {
				std::cout << "Table does not exist" << std::endl;
				execQuery("CREATE TABLE names (name VARCHAR(50), PRIMARY KEY (name));");  //coz dom name is max 253 char with www and all that
				execQuery("CREATE TABLE records (name VARCHAR(50), timestamp BIGINT, latency INT, PRIMARY KEY (name,timestamp));");
				std::cout << "Tables Created." << std::endl;
				return true; 
			}
		}
		return false;  
	}
	catch (char const* dbError){
		std::cout << "Database Error in preparing database: " << dbError << std::endl; 
		throw ("Database Exception"); 
	}
}

//sends data from the records database to DNSPerfTracker class during startup
//Output:
//		vector<entry> to be passed on to the program during startup from database records
std::vector<entry> dbManager::prepareStats(){
	std::vector<entry> result;
	try{
		std::ostringstream osStream;
		osStream << "SELECT name, COUNT(name) as count, AVG(latency) as average, STDDEV_SAMP(latency) as stddev, MIN(timestamp) as minima, MAX(timestamp) as maxima FROM records GROUP BY name ;";
		mysqlpp::StoreQueryResult res = storeQuery(osStream.str());

		if(res){
			for(size_t i = 0; i < res.size(); ++i) {
				std::string site_name(res[i]["name"]);
				entry data(site_name);
				data.avg_latency = atof(res[i]["average"]);
		        data.stddev_latency = atof(res[i]["stddev"]);
		        data.total_queries  = atoi(res[i]["count"]);
				data.first_query_timestamp = strtoull(res[i]["minima"], NULL, 0);
		        data.last_query_timestamp  = strtoull(res[i]["maxima"], NULL, 0);
		        result.push_back(data);
			}
		}
	}
	catch(char const* dbError){
		std::cout << "Database Error in preparing data: " << dbError << std::endl; 
	}
	return result;
}

//inserts a record into records database
//Input:
//		string name 					: website url
//      unsigned long long timestamp    : timestamp when query was made
//      int latency 					: latency recorded

void dbManager::insertRecord(std::string name, unsigned long long timestamp, int latency) {
	try {
		std::ostringstream osStream;  
		osStream << "INSERT INTO records VALUES (\""
		         << name << "\", " 
		         << timestamp << ", " << latency << ");";
		execQuery(osStream.str());  //not c_str() coz we want a string and not string literal with null termination - its taken care of later
	}
	catch (char const* dbError){
		std::cout << "Database Error in adding record: " << dbError << std::endl;
	}
}

//inserts a new entry into names database
//Input:
//		string site_name 					: website url

void dbManager::addWebsite(std::string site_name){
	try{
		std::ostringstream osStream;
		osStream << "INSERT INTO names VALUES(\"" << site_name <<"\");";
		execQuery(osStream.str());
	}
	catch (char const* dbError){
		std::cout << "Database error in adding site name" << dbError << std::endl;
	}

}

//removes the website url from names database and all the corresponding records from records database
//Input:
//		string site_name 					: website url

void dbManager::removeWebsite(std::string site_name){
	try{
		std::ostringstream osStream;
		osStream << "DELETE FROM names where name=\"" << site_name <<"\";";
		execQuery(osStream.str());

		osStream.str(std::string());
		osStream << "DELETE FROM records where name=\"" << site_name <<"\";";
		execQuery(osStream.str());
	}
	catch (char const* dbError){
		std::cout << "Database error in deleting site name. Possibly the url is not in the database. " << dbError << std::endl;
	}
}

//empties names and records database
void dbManager::resetDB(){
	try{
		std::ostringstream osStream;
		osStream << "DELETE FROM names;";
		execQuery(osStream.str());

		osStream.str(std::string());
		osStream << "DELETE FROM records;";
		execQuery(osStream.str());
	}
	catch (char const* dbError){
		std::cout << "Database error in resetting Database " << dbError << std::endl;
	}
}

//returns the list of all the websites being tracked (present in the databases)
std::vector<std::string> dbManager::getWebsites(){
	std::vector<std::string> result;
	try{
		std::ostringstream osStream;
		osStream << "SELECT * FROM names;";
		mysqlpp::StoreQueryResult res = storeQuery(osStream.str());
		if(res) {
			for(size_t i = 0; i < res.size(); ++i) {
				std::string str(res[i]["name"]);
				result.push_back(str);
			}
			
		}
	}
	catch (char const* dbError){
		std::cout << "Database error in getting site names" << dbError << std::endl;
	}
	return result;
}

//executes queries with no return type
//Input:
//		string query_str	: SQL query to be executed
//Output:
//      bool result 		: returns true if query executed successfully
bool dbManager::execQuery(std::string query_str){
	mysqlpp::Query query = conn.query(query_str.c_str());
	bool result = query.exec();
	if (!result)
		throw query.error();
	return result;
}

//executes queries with return type
//Input:
//		string query_str				: SQL query to be executed
//Output:
//      mysqlpp::StoreQueryResult		: StoreQueryResult object containing the resulting data
mysqlpp::StoreQueryResult dbManager::storeQuery(std::string query_str){
	mysqlpp::Query query = conn.query(query_str.c_str());
	mysqlpp::StoreQueryResult result = query.store();
	if (!result)
		throw query.error();
	return result;
}
