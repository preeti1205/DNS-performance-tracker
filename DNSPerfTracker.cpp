#include "DNSPerfTracker.h"

#include <vector>
#include <string>
#include <ldns/ldns.h>
#include <time.h>

DNSPerfTracker::DNSPerfTracker(): frequency(60) {}
DNSPerfTracker::setFrequency(int perMinute){
	//TODO: add catch for negative values -- see how it works for unsigned int too
	frequency = perMinute;
}

DNSPerfTracker::addWebsites(){
	std::string website_array = {   "google.com",
	                                "youtube.com",
								    "facebook.com",
								    "baidu.com",
								    "wikipedia.org",
								    "yahoo.com",
								    "google.co.in",
								    "reddit.com",
								    "qq.com",
								    "taobao.com" }
	vector<std::string> websites (website_array, website_array + sizeof(website_array) / sizeof(website_array[0]);
	records = websites;  //TODO: create a add function to add more websites
}