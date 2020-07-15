//Victor Botteicher and Wade Morgan
//Client.cpp	
#include <stdio.h>
#include <cstdlib>
#include <sys/socket.h>
#include <fstream>
#include <sys/un.h>    
#include <cstring>
#include <stdlib.h>     
#include <vector>
#include <errno.h>      
#include <memory.h>
#include <algorithm>    
#include <iostream>
#include <unistd.h>

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

bool compareFun (string a, string b) 
{
	a = LCword(a);
	b = LCword(b);
	return a<b;
}

int main(int argc, char** argv) {


	char buf[100];	
	int data_fd;	//data socket
	int rc;		//received data
	vector<string> OP;
	string keyWord;

	if ((data_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) //create the socket
	{
		cerr << "Socket failure: " << strerror(errno) << endl; //socket failed to generate
		exit (EXIT_FAILURE); //exit
	}	

	struct sockaddr_un address; //file path
	memset(&address, 0, sizeof(address)); //pointer
	address.sun_family = AF_UNIX;	
// bind
	if (*socket_path == '\0') 
	{
		*address.sun_path = '\0';
		strncpy(address.sun_path + 1, socket_path + 1, sizeof(address.sun_path) - 2);
	}
	 else
	{
		strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
	}

	if (connect(data_fd, (struct sockaddr*) &address, sizeof(address)) == -1)
	 {
		cerr << "Failed to connect " << strerror(errno) << endl;
		exit (EXIT_FAILURE);
	 }
	
	memset(buf, 0, strlen(buf));
	ifstream inFile;
	if(argc!= 3){
                cout << "Usage ./a.out <input file> <string>" << endl;
                exit(1);
        }
        inFile.open(argv[1]);
      	keyWord = argv[2];//word to search for
	

        if(inFile.fail())//file does not open
	{
                cout << "Unable to open file, please try with properly formatted file" << endl;
                exit(2);
        }
	
	strcpy(buf, keyWord.c_str());//convert to char
	write(data_fd, buf, sizeof(buf));//write to server
	string curr("");
	//reads file line by line and sends to server
		while (!inFile.eof()) 
		{
		
			getline(inFile, curr);	//getline and add to current line
			strcpy(buf, curr.c_str()); //cast string as char
			rc = sizeof(buf);
			if (write(data_fd, buf, rc) == -1) 
			{
				cerr << "Failed to write " << strerror(errno) << endl;
				exit (EXIT_FAILURE); //write fail
			}
		
			memset(buf, 0, strlen(buf));//buffer pointer
		
			if ((rc = read(data_fd, buf, sizeof(buf))) > 0) 
			{
				OP.push_back(buf); //add read to output

			} 
			else if (rc == -1)
			{
				cerr << "Failed to read " << strerror(errno) << endl; //read failure
				exit (EXIT_FAILURE);
			}
			else if (rc == 0) 
			{
				cout << "0 bytes in read line" << endl; //no rc
				close(data_fd);
			}
			memset(buf, 0, strlen(buf));
		}
	//alphabetically sort vector
	sort(OP.begin(), OP.end(), compareFun);
	for(int i =0; i< OP.size(); i++)

	{
		if(OP[i].length() >0)
		cout << OP[i] << endl;
	}
	inFile.close();	//close file
	close(data_fd);	//end stream
	return 0;
}
