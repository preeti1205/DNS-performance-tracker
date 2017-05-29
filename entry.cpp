#include "entry.h"

#include <string>

entry::entry(std::string name) :  _website(name), 
								  _avg_latency(0),
		                          _stddev_latency(0),
		                          _total_queries(0),
								  _first_query_timestamp(0),
		                          _last_query_timestamp(0) {} 