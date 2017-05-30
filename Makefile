OBJS = entry.o DNSPerfTracker.o dbManager.o 
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
CXXFLAGS = -I/usr/local/include/mysql/ -I/usr/local/include/mysql++ -I/usr/local/opt/openssl@1.1/include
LFLAGS = -Wall $(DEBUG)
LDFLAGS = -L/usr/local/lib
LDLIBS = -lmysqlpp -lldns -lmysqlclient
OUT = Tracker

MAIN = Main
ENTRY = entry
DBMANAGER = dbManager
DNSP = DNSPerfTracker

all:
	g++ *.cpp -o $(OUT) -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

# $(OUT) : $(OBJS)
# 	$(CC) $(LFLAGS) $(LIBS) $(OBJS) $(LDFLAGS) -o $(OUT) $(MAIN).cpp

# $(ENTRY).o : $(ENTRY).h $(ENTRY).cpp
# 	$(CC) $(CFLAGS) $(ENTRY).cpp

# $(DBMANAGER).o : $(DBMANAGER).h $(DBMANAGER).cpp
# 	$(CC) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(DBMANAGER).cpp

# $(DNSP).o : $(DNSP).h  $(DNSP).cpp
# 	$(CC) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(DNSP).cpp

clean :
	\rm *.o *~ $(OUT)