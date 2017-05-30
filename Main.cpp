#include <iostream>
#include <vector>
#include <iomanip>

#include "DNSPerfTracker.h"

using namespace std;

void displayOverallStats(DNSPerfTracker& tracker) {
	vector<entry> stats = tracker.getStats();
	if (!stats.empty()) {
			cout.setf(ios::left);
			cout << setw(51) << " Website " <<
					setw(15) << " Average Latency " <<
					setw(15) << " Standard Deviation " <<
					setw(15) << " Total Queries " <<
					setw(25) << " First Query TS " <<
					setw(25) << " Last Query TS " << endl << endl;

			// Get each row in stats, and print its contents
			for (vector<entry>::iterator it = stats.begin(); it != stats.end(); ++it) {
				cout << setw(50) << it->_website << ' ' <<
						setw(14) << it->_avg_latency << ' ' <<
						setw(14) << it->_stddev_latency << ' ' <<
						setw(14) << it->_total_queries << ' ' <<
						setw(24) << it->_first_query_timestamp <<
						setw(24) << it->_last_query_timestamp <<
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
	for(int i = 0; i < siteList.size(); ++i)
		cout << siteList[i] << endl;

}

int main(int argc, char* argv[]) {
	DNSPerfTracker tracker;
	if(argc != 2) {
		cout << "Please enter a config file ";
		return 0;
	}
	else{
		tracker.prepareDependencies(argv[1]);
		tracker.startTracking();
		displayOverallStats(tracker);
	}

	while(true) {
		cout << "Tracking in progress.....";
		tracker.performTracking();
		string input;
		cin >> input;
		if(input == "stat") {
			displayOverallStats(tracker);
		}
		if(input == "exit"){
			break;
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
	}
	return 0;
}