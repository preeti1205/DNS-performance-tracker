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
	void addAlexaWebsites();
	void performTracking();
	void prepareDependencies(std::string setupFile);
	std::vector<entry> getStats();
	std::vector<std::string> getNames() const;
	void startTracking();
	void stopTracking();

private:
	std::vector<entry> records;
	std::unordered_map<std::string, bool> siteNames; //TODO : map would be better for large number of entries
	dbManager* dbMan;
	int ldnsLatency(const char siteName[]);
	int getLatency(std::string siteName);
	std::vector<entry>::iterator findEntry(std::string site);
	unsigned int frequency;
	bool isRunning;
};
#endif


// public:


// 	void removeAll ();
 

// 	bool getRunExperiments() const;
