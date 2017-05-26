#ifndef ENTRY_H
#define ENTRY_H

#include <iostream>
#include <string>

class entry {
public:
	entry();
	
private:
	string _website;
	float _latest_latency;
	float _avg_latency;
	float _stddev_latency;
	int _total_queries;
	int _first_query_timestamp;
	int last_query_timestamp;
};
#endif
