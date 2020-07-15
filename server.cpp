//Victor Botteicher and Wade Morgan
//Server.cpp
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

using namespace std;
const char* socket_path = "\0hidden";

string LCword(string s)
{
        for(int i = 0; i < s.length(); i++)
	{
                if(s[i] != ' ')
		{
                s[i] = tolower(s[i]);
                }
        }
        return s;
}


int main(int argc, char** argv) {
	
	char buf[100];
	char buf2[100];
	int connection_fd, data_fd; // datasockets
	int rc; //data received
	string sentComp; // sentences to compare
	string keyWord; //word to look out for
	//generate the socket
	if ((connection_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		cerr << "Socket failure: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_un address; //path for file
	memset(&address, 0, sizeof(address)); //turns address into a pointer
	address.sun_family = AF_UNIX;
	//bind
	if (*socket_path == '\0') 
	{
		*address.sun_path = '\0';
		strncpy(address.sun_path + 1, socket_path + 1, sizeof(address.sun_path) - 2);
	}
	 else
	 {
		strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
		unlink(socket_path);
	}
	
	if (bind(connection_fd, (struct sockaddr*)&address, sizeof(address)) == -1) 
	{
		cerr << "Bind failure" << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
		//backlog
	if (listen(connection_fd, 5) == -1) 
	{
		cerr << "Listen failure " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	memset(buf2,0,strlen(buf2));	

	while (1) 
	{ //running server
		//establish connection
		if ((data_fd = accept(connection_fd, NULL, NULL)) == -1) 
		{
			cerr << "Failed to accept " << strerror(errno) << endl;
			continue;
		} 
		else 
		{
			cout << "Accept return: " << data_fd << endl;
		}
		
		memset(buf, 0, strlen(buf));
		//find keyWord
		read(data_fd,buf2,sizeof(buf2));
		keyWord = buf2;
		keyWord = LCword(keyWord); //make keyWord lowercase to make search possible
		while ((rc = read(data_fd, buf, sizeof(buf))) > 0) 
		{  //set data to keyWord
			sentComp = buf;
			sentComp = LCword(sentComp);

			if (rc  == -1) 
			{
				cerr << "Failed to write " << strerror(errno) << endl;
				exit(EXIT_FAILURE);
			}
			 else if(sentComp.find(keyWord)  != std::string::npos)
			{
				write(data_fd, buf, rc);//writes to client
			}
			else{//writes "empty" char
				char buf2[0]; //size 0
				write(data_fd, buf2, rc);
			}	
		}	
		memset(buf, 0, strlen(buf));
		
		if (rc == -1) 
		{
			cerr << "Failed to read " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}
		 else if (rc == 0) 
		{
			cout << "0 bytes in read line" << endl;
			close(data_fd);
			exit(1);//close
		}
	}
	close(connection_fd);//end connection
	
	unlink(socket_path);//unlink socket
	
	return 0;
}
