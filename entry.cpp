#include "entry.h"

#include <string>

entry::entry(std::string name) :  website(name), 
								  avg_latency(0),
		                          stddev_latency(0),
		                          total_queries(0),
								  first_query_timestamp(0),
		                          last_query_timestamp(0) {} 