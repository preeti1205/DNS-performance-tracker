#include <iostream>
#include <vector>
#include <iomanip>

#include "DNSPerfTracker.h"

using namespace std;

void displayOverallStats(DNSPerfTracker& tracker) {
	vector<entry> stats = tracker.getStats();
	if (!stats.empty()) {
			cout.setf(ios::left);
			cout << setw(21) << " Website " <<
					setw(15) << " Avg Latency " <<
					setw(15) << " Std Dev " <<
					setw(15) << " Query Count " <<
					setw(25) << " First Query TS " <<
					setw(25) << " Last Query TS " << endl << endl;

			// Get each row in stats, and print its contents
			for (vector<entry>::iterator it = stats.begin(); it != stats.end(); ++it) {
				cout << setw(20) << it->website << ' ' <<
						setw(14) << it->avg_latency << ' ' <<
						setw(14) << it->stddev_latency << ' ' <<
						setw(14) << it->total_queries << ' ' <<
						setw(24) << it->first_query_timestamp <<
						setw(24) << it->last_query_timestamp <<
						endl;
			}
		}
		else {
			cerr << "Failed to get results table. The table is empty. " << endl;
			return ;
		}
}

void listWebsites(DNSPerfTracker& tracker) {
	vector<string> siteList= tracker.getNames();
	cout << endl;
	for(int i = 0; i < siteList.size(); ++i)
		cout << siteList[i] << endl;
	cout << endl;
}

int main(int argc, char* argv[]) {
	DNSPerfTracker tracker;
	if(argc != 2) {
		cout << "Please enter a config file ";
		return 0;
	}
	else{
		tracker.prepareDependencies(argv[1]);
		displayOverallStats(tracker);
	}

	while(true) {

		if(tracker.getStatus()){
			cout << "Tracking in progress....." << endl;
		}
		else 
			cout << "Enter start to track " << endl;
		string input;
		cin >> input;

		if(input == "stats") {
			displayOverallStats(tracker);
		}
		else if(input == "exit"){
			if(tracker.getStatus())
				tracker.stopTracking();
			break;
		}
		else if(input == "start"){
			if( !(tracker.getNames()).empty() ) {
				tracker.startTracking();
			}
			else {
				cout << "No websites to track. Lets initialize. " << endl;	
				tracker.addAlexaWebsites();
			}
		}
		else if(input == "stop"){
			tracker.stopTracking();
			cout << "Type exit to exit or ";
		}
		else if(input == "site"){
			listWebsites(tracker);
		}
		else if(input == "setf"){
			int freq;
			cout << "Enter new frequency(greater than 0) : ";
			cin >> freq;
			tracker.setFrequency(freq);
		}
		else if(input == "getf"){
			cout << "Current frequency is : " << tracker.getFrequency() << endl;
		}
		else if(input == "reset"){
			tracker.resetAll();
		}
		else if(input == "add"){
			string site;
			cout << "Enter the url: ";
			cin >> site;
			tracker.addSite(site);
		}
		else if(input == "remove"){
			string site;
			cout << "Enter the url: ";
			cin >> site;
			tracker.removeSite(site);
		}
		else {
			cout << " Enter a valid command " << endl;
		}
	}
	return 0;
}