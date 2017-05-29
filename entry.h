#ifndef ENTRY_H
#define ENTRY_H

#include <string>

class entry{
public:
	entry(std::string name);

// private:   Todo:: remove _ if not keeping it private
	std::string _website;
	// double _latest_latency;
	double _avg_latency;
	double _stddev_latency;
	int _total_queries;
	unsigned long long _first_query_timestamp;  //Note to be removed in final: 64 bit coz 32 bit will expire in 2038 - unix timestamp is seconds since 1970
	unsigned long long _last_query_timestamp;
};
#endif
