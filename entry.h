#ifndef ENTRY_H
#define ENTRY_H

#include <string>

class entry{
public:
	entry(std::string name);

// private:   Todo:: remove _ if not keeping it private
	std::string _website;
	double _avg_latency;
	double _stddev_latency;
	int _total_queries;
	unsigned long long _first_query_timestamp;
	unsigned long long _last_query_timestamp;
};
#endif
