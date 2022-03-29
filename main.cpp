#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>

#define MINPIN_V "1.2.0"

using namespace std;

string repo = "https://raw.githubusercontent.com/Scratchdragon/MinPin-Repo/main/";

string strings[8]; //Max strings returned by split  
  
//Mirrors the python function "len" 
int len(string str)  
{  
    int length = 0;  
    for (int i = 0; str[i] != '\0'; i++)  
    {  
        length++;  
          
    }  
    return length;     
}  
int len(string strs[])  
{  
    int length = 0; 
		bool end = false; 
    while(!end){
				try {
					strs[length];
				}
				catch(const exception& e) {
					end = true;
				}
        length++;  
    }  
    return length;     
}  
  
//Mirrors the python function "split"
string * split (string str, char seperator)  
{  
    int currIndex = 0, i = 0;  
    int startIndex = 0, endIndex = 0;  
    while (i <= len(str))  
    {  
        if (str[i] == seperator || i == len(str))  
        {  
            endIndex = i;  
            string subStr = "";  
            subStr.append(str, startIndex, endIndex - startIndex);  
            strings[currIndex] = subStr;  
            currIndex += 1;  
            startIndex = endIndex + 1;  
        }  
        i++;  
    }
		return strings;
}

void system(string command) {
	system(command.c_str());
}

void zip(string filename) {
	system(("zip -r " + filename + ".minpin " + filename).c_str());
}
void unzip(string filename, string elseC = "") {
	system(("unzip " + filename + " || " + elseC).c_str());
}

inline bool file_exists(string name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int main(int argc, char ** argv) {
	if(argc <= 1) {
		std::cout << "	MinPin 1.2 (Miniature Package Installer)\n";
		return 0;
	}
	int max = 4;
	string command_index[] = {"","package","decompress","install","version"};
	
	//Get index of command
	bool gotindex = false;
	int i = 0;
	for(i=0;i < max;++i){
		if(strcmp(command_index[i].c_str(),argv[1]) == 0) {
			gotindex = true;
			break;
		}
	}
	if(gotindex == false) {
		std::cout << "Operation '"
							<< argv[1] 
							<< "' not found.\n";
		return -1;
	}
	if(argc <= 2) {
		std::cout << "Not enough arguments.\n";
		return -1;
	}

	string exec = "chmod +x ";
	string package = argv[2]; package = package + ".minpin";
	switch(i) {
		case 1:
			zip(argv[2]);
			break;
		case 2:
			unzip(argv[2]);
			break;
		case 3:
			if(strcmp(split(argv[2],'.')[1].c_str(),"minpin") != 0) {
				//Get package from repositories
				system("curl -o " + package + " " + repo + package);

				//Verify file installed
				if (!file_exists(package)) {
					std::cout << "Package not found, make sure you have read/write permissions in this directory.\n";
					return 0;
				}
				
				unzip(package,"echo '\n\nPackage not found, either malformed zipfile or package does not exist'");
				system("rm " + package);
				if(!file_exists(split(package,'.')[0])){
					return 0;
				}
			}
			else {
				if (!file_exists(package)) {
					std::cout << "Package not found, either failed to access repository or package does not exist.\n";
					return 0;
				}
				unzip(argv[2]);
			}
			
			exec = exec + split( argv[2], '.' )[0] + "/install.sh";
			system(exec.c_str());
			exec = "cd ";
			exec = exec + split( argv[2], '.' )[0] + "; ./install.sh";
			system(exec);
			system("rm -r " + split( argv[2], '.' )[0]);
			break;
		case 4:
			std::cout << "MinPin " << MINPIN_V << " (Miniature Package Installer)"
			break;
		default:
			std::cout << "E: Operation not implemented.\n";
			break;
	}
}