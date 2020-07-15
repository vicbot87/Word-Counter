PROGS = client server

all: $(PROGS) $(DOC)
	g++ pipe.cpp -o pipe
client: client.cpp
server: server.cpp
.PHONY: clean

clean:
	rm -f *.o socket $(PROGS)
