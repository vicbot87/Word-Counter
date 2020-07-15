//Written by Victor Botteicher and Wade Morgan
//pipe.cpp
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
using namespace std;

string LCword(string word) //make word lower case for comparison
{
	for(int i = 0;i < word.length();i++)
	{
		if(word[i] != ' ')
		{
			word[i] = tolower(word[i]); 
		}
	}
	return word;
}

bool compStr(string s, string t)
{
	s = LCword(s);
	t = LCword(t);
	return s<t;
}

bool Search(string line, string word) //find the word in the current piped line
{
	int i = 0; //letter incrementer to compare words
	if(line.find(word) == -1)
		return false;
	for(int j = 0;j<line.length();j++)
	{
		if(line[j] = word[i])
		{
			if(i == (word.length()-1))
			{
				if(line.length() == word.length()) //one word line
				{
					return true;
				}
				else if(((line[j-(i+1)] == ' ') || ((j-(i+1)) < 0) && ((j+1) == line.length()) || (line[j+1] == ' ')))
				{
					return true;
				}
			}
			i++;
		}
		else
		{
			i = 0;
		}
	}
	return false;
}
int main(int argc, char **argv) {
	int fh[2];
	int fd[2];
	pipe(fh); //two pipes, one to the child and one to the parent
	pipe(fd);
	FILE *file = fopen(argv[1], "r");
	pid_t pid = fork();
		char buff[100];
		char buff2[100];                   
	           vector<string> lineList;
		char extraBuff[100]; //extra buffer for strcpystring
	string  wordSearch(argv[2]);
	if(pid != 0) //parent
{
		close(fh[0]); //close reading end
		while(!feof(file))
		{
			fgets(buff,sizeof(buff),file); 
			write(fh[1], buff, sizeof(buff)); //pipe through	
		}
		fclose(file);
		close(fh[1]);
		wait(NULL);
		close(fd[1]); //close writing end
		cout << "List of lines containing the word " << argv[2] << ":" << endl;
		while(read(fd[0], buff2, 100) > 0)
		{
			cout << buff2 << endl;
		}
		close(fd[0]); //close reading end

}
	if(pid == 0) //child
	{
		close(fh[1]); //close writing end
		while((read(fh[0],buff,sizeof(buff))) > 0)
		{
			string currLine(buff);		//store each line
			if(Search(currLine, wordSearch) == true)
			{	
				lineList.push_back(currLine);	//add line to vector
			}
		
		}
		sort(lineList.begin(),lineList.end(), compStr);
		close(fh[0]);// close reading end
		close(fd[0]);//close reading end
		for(int i = 0;i < lineList.size();i++)
		{
			strcpy(extraBuff,lineList[i].c_str()); //copy string to buffer
			write(fd[1], extraBuff, sizeof(extraBuff)); //pipe back
		}
		close(fd[1]); //close writing end
	}
	 

return 0;
}
