/*
This class periodically sends DNS queries to the name servers of sites 
and stores the latency values in a MySQL table. 
The frequency of queries can be specified by the user on command line.
*/
#ifndef DNS_PERF_TRACKER_H
#define DNS_PERF_TRACKER_H

#include <vector>
#include <string>
#include <unordered_map>

#include "entry.h"
#include "dbManager.h"

class DNSPerfTracker{
public:
	DNSPerfTracker();
	void setFrequency(int perMinute);
	unsigned int getFrequency() const;
	void addSite(std::string siteName);
	void removeSite(std::string siteName);
	void resetAll();
	void addAlexaWebsites();
	void performTracking();
	void prepareDependencies(std::string setupFile);
	std::vector<entry> getStats();
	std::vector<std::string> getNames() const;
	void startTracking();
	void stopTracking();
	bool getStatus() const;

private:
	std::vector<entry> records;
	std::unordered_map<std::string, bool> siteNames; 
	dbManager* dbMan;
	int ldnsLatency(const char siteName[]);
	int getLatency(std::string siteName);
	std::vector<entry>::iterator findEntry(std::string site);
	unsigned int frequency;
	bool isRunning;
};
#endif

