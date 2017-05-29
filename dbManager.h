#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include <vector>
#include <mysql++/mysql++.h>

#include "entry.h"

class dbManager{
public:
	dbManager(std::string configFile);
	bool prepareDB();
	std::vector<entry> prepareStats();
	void insertRecord(std::string name, unsigned long long timestamp, int latency);
	void addWebsite(std::string site_name);
	void removeWebsite(std::string site_name);

private:
	mysqlpp::Connection conn;
	bool execQuery(std::string query_str);
	mysqlpp::StoreQueryResult storeQuery(std::string queryString); 

};
#endif