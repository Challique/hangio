CXX = g++
CXXFLAGS = -Wall -g

.PHONY: all clean

all: server client

server: server.o connection.o listener.o
	$(CXX) $(CXXFLAGS) -o $@ $^

client: client.o connection.o dispatcher.o event_loop.o loop_runner.o sender.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o server client




