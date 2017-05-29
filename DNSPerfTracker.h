#ifndef DNS_PERF_TRACKER_H
#define DNS_PERF_TRACKER_H

#include <vector>
// #include <string>

#include "entry.h"
#include "dbManager.h"

class DNSPerfTracker{
public:
	DNSPerfTracker();
	void setFrequency(int perMinute);
	void addWebsites();
private:
	std::vector<entry> records;
	int frequency;

};
#endif