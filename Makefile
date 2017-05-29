OBJS = entry.o DNSPerfTracker.o dbManager.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
CXXFLAGS = -I/usr/local/include/mysql++ -I/usr/local/include/mysql
LFLAGS = -Wall $(DEBUG)
LDLIBS = -lmysqlpp -lldns -lmysqlclient
OUT = Tracker

$(OUT) : $(OBJS)
	$(CC) $(LFLAGS) $(LIBS) $(OBJS) -o $(OUT)

entry.o : entry.h entry.cpp
	$(CC) $(CFLAGS) entry.cpp

dbManager.o : dbManager.h dbManager.cpp
	$(CC) $(CFLAGS) dbManager.cpp

DNSPerfTracker.o : DNSPerfTracker.h DNSPerfTracker.cpp
	$(CC) $(CFLAGS) DNSPerfTracker.cpp

clean :
	\rm *.o *~ $(OUT)