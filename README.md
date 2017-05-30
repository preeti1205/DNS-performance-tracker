# DNS-performance-tracker


### Prerequisites:

1) Use [mysql++](http://tangentsoft.net/mysql++/) to interact with MySQL.
 
2) Use [ldns](http://www.nlnetlabs.nl/projects/ldns/) for generating DNS queries.

### List of commands

* stats:  display overall statistics
* exit:   exit the program
* start:  start sending queries
* stop:   stop sending queries
* sites:  list all the urls currently being queried
* setf:   set a new querying rate( times/minute)
* getf:   get the current frequency
* reset:  reset the databases
* add:    add a new url to be added to list of queried urls
* remove: remove a url from the query list