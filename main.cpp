#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>

#define MINPIN_V "1.2.0"

using namespace std;

string repo;

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

int install_deps(string package);

int install(string pkg) {
		const char * argv[] = {"","",pkg.c_str()};
		string package = pkg + ".minpin";
		string exec = "chmod +x ";
		if(strcmp(split(argv[2],'.')[1].c_str(),"minpin") != 0) {
				//Get package from repositories
				system("curl -o " + package + " " + repo + package);

				//Verify file installed
				if (!file_exists(package)) {
					std::cout << "Package not found, make sure you have read/write permissions in this directory.\n";
					return 1;
				}
				
				unzip(package,"echo '\n\nPackage not found, either malformed zipfile or package does not exist'");
				system("rm " + package);
				if(!file_exists(split(package,'.')[0])){
					return 1;
				}
			}
			else {
				if (!file_exists(package)) {
					std::cout << "Package not found.\n";
					return 1;
				}
				unzip(argv[2]);
			}

			if(install_deps(pkg)==1) {
				return 1;
			}
					
			exec = exec + split( argv[2], '.' )[0] + "/install.sh";
			system(exec.c_str());
			exec = "cd ";
			exec = exec + split( argv[2], '.' )[0] + "; ./install.sh";
			system(exec);
			system("rm -r " + split( argv[2], '.' )[0]);
		return 0;
}

int install_deps(string package) {
	string line;
  ifstream myfile(package + "/.minpin/dependencies");
  if (myfile.is_open()) {
    while (getline(myfile,line)) {
      if(install(line)==1) {
				std::cout << "Failed to install dependency '" << line << "'\n";
				system("rm -r " + package);
				return 1;
			}
    }
    myfile.close();
  }
  else cout << "No dependencies required by package.\n"; 
	return 0;
}

const char * get_minpin() {
	const char * var = getenv("MINPIN");
	if(var == NULL) {
		std::cout << "W: Environment variable 'MINPIN' is null, defaulting to /etc/minpin/\n";
		var = "/etc/minpin";
	}
	return var;
}

int init_repo() {
	std::cout << "Getting primary repository...\n";
	string minpin_dir = get_minpin();
	
	if(!file_exists(minpin_dir + "/repo")) {
		std::cout << "E: repo file not found, cannot access repository.\n";
		return -1;
	}
	
	string line;
  ifstream myfile((minpin_dir + "/repo"));
  if (myfile.is_open()) {
    while (getline(myfile,line)) {
			repo = line;
		}
	}
	return 0;
}

int main(int argc, char ** argv) {
	if(argc <= 1) {
		std::cout << "	MinPin 1.2 (Miniature Package Installer)\n";
		return 0;
	}
	int max = 5;
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
	if(argc <= 2 && i != 4) {
		std::cout << "Not enough arguments.\n";
		return -1;
	}
	
	switch(i) {
		case 1:
			zip(argv[2]);
			break;
		case 2:
			unzip(argv[2]);
			break;
		case 3:
			if(init_repo() != 0) {
				return -1;
			}
			if(install(argv[2]) == 1) {
				return 0;
			}
			break;
		case 4:
			std::cout << "MinPin " << MINPIN_V << " (Miniature Package Installer)\n";
			break;
		default:
			std::cout << "E: Operation not implemented.\n";
			break;
	}
}