/*
Basic structure of each record in the database records
website     			:Name of the website 
avg_latency 			:Average latency
stddev_latency      	:Standard deviation of query times
total_queries       	:Number of queries made so far
first_query_timestamp	:Timestamp of first query made
last_query_timestamp	:Timestamp of last query made
*/ 
#ifndef ENTRY_H
#define ENTRY_H

#include <string>

struct entry{
	entry(std::string name);

	std::string website;
	double avg_latency;
	double stddev_latency;
	int total_queries;
	unsigned long long first_query_timestamp;
	unsigned long long last_query_timestamp;
};
#endif
