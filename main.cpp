//Global headers:
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <typeinfo>
#include <algorithm>
#include <sys/stat.h>
#include <iterator>
//Local headers:
#include "include/xcmphd.hpp"
#include "include/json.hpp"

using namespace std; 
using json = nlohmann::json;

map<string, string> minpin_info = {
	{"version","UNKNOWN",},
	{"platform","UNKNOWN"}
};

//Custom print function so that this program can be ported to alternative hardware with little rewriting
void print(const char * s) {
	printf("%s",s);
}
void print(string s) {
	printf("%s",s.c_str());
}

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

string getUser() {
	if(getenv("username") != NULL) {
		return getenv("username");
	}
	else if(getenv("USER") != NULL) {
		return getenv("USER");
	}
	else {
		//Remeber to do this later
		return "UNKNOWN";
	}
}

string user = getUser();

//path
string path = "/etc/minpin/";
bool updatePath() {
  //Get path
  if(strcmp(minpin_info["platform"].c_str(),"Termux") == 0) {
    path = "/data/data/com.termux/files/usr/etc/minpin/";
  }
  else if(strcmp(minpin_info["platform"].c_str(),"Windows 32") == 0) {
	path = "C:/Users/" + user + "/AppData/Roaming/minpin/";
  }
  else if(strcmp(minpin_info["platform"].c_str(),"Windows 64") == 0) {
	path = "C:/Users/" + user + "/AppData/Roaming/minpin/";
  }
	else if(strcmp(split(minpin_info["platform"],' ')[1].c_str(), "Replit") == 0) {
		path = "/home/runner/minpin_src/";
	}
  else {
    path = "/etc/minpin/";
  }
    
  //Create path directory
	//Attempt to create /etc/minpin/ directories

	if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Windows") == 0) {
		path = path.erase(len(path) - 1);
		if(file_exists(path.c_str()) != 2) {
			string wpath = windowsPath(path);
			print("Creating " + ("'" + path + "'\n"));
			system(("mkdir " + wpath).c_str());
			system(("mkdir " + wpath + "\\packages").c_str());

			//Handle mkdir error
			if(file_exists(path.c_str()) != 2 || file_exists((path + "\\packages").c_str()) != 2) {
				print("\e[1;31minstall error:\e[0m Failed to initialise minpin directory, you can manually resolve this error by creating the directory '" + wpath + "\\packages/'\n");
				return false;
			}
		}
		path = path + "/";
	}
	else {
		if(file_exists(path.c_str()) != 2) {
			print("Creating " + ("'" + path + "'\n"));
			system(("mkdir " + path).c_str());
			system(("mkdir " + path + "packages/").c_str());

			//Handle mkdir error
			if(file_exists(path.c_str()) != 2 || file_exists((path + "packages/").c_str()) != 2) {
				print("\e[1;31minstall error:\e[0m Failed to initialise minpin directory, you can manually resolve this error by creating the directory '/etc/minpin/packages/'\n");
				return false;
			}
		}
	}
  return true;
}

//Install package
bool install(string filename) {
	if(true) {
		//Decompile and move package
		if(file_exists(filename.c_str()) == 1) {
			//Decompile
			print("Decompiling package...\n");
			string command = "unzip ";
			system((command + filename + " -d " + path + "packages/").c_str());
			if(file_exists((path + "packages/" + split(filename,'.')[0]).c_str()) != 2) {
				print("Failed to decompile package, are you root?\n");
				return false;
			}

			string text;
			string line;
			ifstream file(path + "packages/" + split(filename,'.')[0] + "/install.json");
			int i = 0;
			while (getline (file, line)) {
				//std::replace( line.begin(), line.end(), '\n', ' ');
				text = text + line;
			}
			file.close();
			json installj = json::parse(text);
			map<string, string> nullmap;
			map<string, string> installmap = installj.value("install",nullmap);
			map<string, string> infomap = installj.value("info",nullmap);
			map<string, string> commandmap = installj.value("command",nullmap);
			map<string, string> appmap = installj.value("app",nullmap);
			
			//Run the install script
			system(installmap["install-script"].c_str());

			for(int i = 0; i < sizeof(split(installmap["dependencies"],',')) / sizeof(split(installmap["dependencies"],',')[0]); ++i) {
				install(split(installmap["dependencies"],',')[i]);
			}
			//Install command
			if(installmap["command"] == "true" || installmap["command"] == "y") {
				string name = commandmap["name"];
				string execute = commandmap["execute"];
  			ifstream src(path + "packages/" + split(filename,'.')[0] + "/" +execute, std::ios::binary);
   			ofstream dst("/bin/" + name,   std::ios::binary);
    		dst << src.rdbuf();
				if(file_exists(("/bin/" + name).c_str()) == 0) {
					print("Failed to create command, are you root?\n");
				}
			}
			//Install app (eventaully)
			if(installmap["app"] == "true" || installmap["app"] == "y") {
				
			}
			return true;
		}
		else {
			print("\e[1;31minstall error:\e[0m Package does not exist\n");
			return false;
		}
	}
}

//Decompile package
bool decompile(string filename) {
	if(file_exists(filename.c_str()) == 1) {
		string command = "unzip ";
		system((command + filename).c_str());
		print("Decompiled package");
		return true;
	}
	else {
		print("Package does not exist");
		return false;
	}
}

//Make package
bool make(string filename) {
	int file_status = file_exists(filename.c_str());

	if( file_status == 0 ) {
    print("cannot access directory '");
		print(filename.c_str());
		print("'\n");
		return false;
	}
	else if( file_status == 2 ) {
		string installfile = filename;
		installfile = installfile + "/install.json";
		
		string installcontent;

		if(file_exists(installfile.c_str()) == 1) {
			if(file_exists((filename + ".minpin").c_str()) == 1) {
				remove((filename + ".minpin").c_str());
			}
			string command = "zip ";
			system((command + "-r " + filename + ".minpin " + filename).c_str());
			return true;
		}
		else {
			print("Failed to access install file '");
			print(installfile.c_str());
			print("'");
			return false;
		}
	}
	else {
   	print("file '");
		print(filename);
		print("' is not a directory\n");
		return false;
	}
}

enum packageManager { UNDEFINED, apt, pacman, zypper, apk, scoop, pkg, tmuxpkg };

bool updateDeps() {
	packageManager manager = UNDEFINED;

	print("Getting package manager...\n");

	if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Windows") == 0){
		manager = scoop;
	}
	else if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Ubuntu") == 0 || strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Debian") == 0 || strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Kali") == 0) {
		manager = apt;
	}
	else if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Arch") == 0) {
		manager = pacman;
	}
	else if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"openSUSE") == 0) {
		manager = zypper;
	}
	else if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Alpine") == 0) {
		manager = apk;
	}
	else if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Termux") == 0 || strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"FreeBSD") == 0 || strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"FreeNAS") == 0) {
	  manager = pkg;
	  if(strcmp(split(minpin_info["platform"].c_str(),' ')[0].c_str(),"Termux") == 0) {
	    manager = tmuxpkg;
	  }
	}

	print("Updating dependencies...\n");

	switch(manager) {
		case apt:
			if(file_exists("/usr/bin/zip") == 0) {
				system("apt-get install zip");
				if(file_exists("/usr/bin/zip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists("/usr/bin/unzip") == 0) {
				system("apt-get install unzip");
				if(file_exists("/usr/bin/unzip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case pkg:
			if(file_exists("/usr/local/bin/zip") == 0) {
				system("pkg install zip");
				if(file_exists("/usr/local/bin/zip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists("/usr/local/bin/unzip") == 0) {
				system("pkg install unzip");
				if(file_exists("/usr/local/bin/unzip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case tmuxpkg:
			if(file_exists("/data/data/com.termux/files/usr/bin/zip") == 0) {
				system("pkg install zip");
				if(file_exists("/data/data/com.termux/files/usr/bin/zip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists("/data/data/com.termux/files/usr/bin/unzip") == 0) {
				system("pkg install unzip");
				if(file_exists("/data/data/com.termux/files/usr/bin/unzip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case pacman:
			if(file_exists("/usr/bin/zip") == 0) {
				system("pacman -S zip");
				if(file_exists("/usr/bin/zip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists("/usr/bin/unzip") == 0) {
				system("pacman -S unzip");
				if(file_exists("/usr/bin/unzip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case zypper:
			if(file_exists("/usr/bin/zip") == 0) {
				system("zypper install zip");
				if(file_exists("/usr/bin/zip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists("/usr/bin/unzip") == 0) {
				system("zypper install unzip");
				if(file_exists("/usr/bin/unzip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case apk:
			if(file_exists("/usr/bin/zip") == 0) {
				system("apk add zip");
				if(file_exists("/usr/bin/zip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists("/usr/bin/unzip") == 0) {
				system("apk add unzip");
				if(file_exists("/usr/bin/unzip") == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case scoop:
			if(file_exists(("C:/Users/" + user + "/scoop/apps/zip/current/zip.exe").c_str()) == 0) {
				system("scoop install zip");
				if(file_exists(("C:/Users/" + user + "/scoop/apps/zip/current/zip.exe").c_str()) == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			if(file_exists(("C:/Users/" + user + "/scoop/apps/unzip/current/unzip.exe").c_str()) == 0) {
				system("scoop install unzip");
				if(file_exists(("C:/Users/" + user + "/scoop/apps/unzip/current/unzip.exe").c_str()) == 0) {
					print("\e[1;31mdependency error:\e[0m Failed to install required package\n");
					return false;
				}
			}
			break;
		case UNDEFINED:
			print("\e[1;31mdependency error:\e[0m Failed to accuire package manger\n");
			return false;
			break;
		default:
			print("Unsupported package manager\n");
			return false;
			break;
	}
	return true;
}

int main(int argc, char **argv) {
	minpin_info["version"] = "0.1.0";

	//Get platform
	string platform;

	//Check for Android/Termux
	string OS = "Unknown";
	if(file_exists("/sdcard") != 0) {
		OS = "Android";
	}
	else {
		OS = get_os();
	}

	//Elaborate on OS
	if(strcmp(OS.c_str(),"Other") == 0){
		print("\e[1;31mmacro error:\e[0m Failed to acquire platform");
		return 0;
	}
	if(strcmp(OS.c_str(),"Linux") == 0) {
		if(file_exists("/etc/os-release") != 0) {
			string line;
			ifstream file("/etc/os-release");
			int i = 0;
			while (getline (file, line)) {
				if(i == 0) {
					platform = split(line,'=')[1];
					platform.erase(remove(platform.begin(), platform.end(), '"'), platform.end());
				}
				i++;
			}
			file.close();
		}
		else {
			print("Failed to accuire /etc/os-release file.\n");
			return 0;
		}
		if(getenv("REPL_ID") != NULL) {
			platform = platform + " Replit";
		}
		minpin_info["platform"] = platform;
	}
	else if(strcmp(OS.c_str(),"Android") == 0) {
		//Checking for "pkg" may be reduntant, still gonna do it tho
		if(file_exists("/data/data/com.termux/") != 0) {
			platform = "Termux";
			minpin_info["platform"] = platform;
		}
		else{
			platform = "Android";
			minpin_info["platform"] = platform;
		}
	}
	else if(strcmp(split(OS,' ')[0].c_str(),"Windows") == 0) {
		platform = OS;
		minpin_info["platform"] = platform;
	}
	else if(strcmp(OS.c_str(),"FreeBSD") == 0) {
		//Check for NAS through /etc/os-release file
		if(file_exists("/etc/os-release") != 0) {
			string line;
			ifstream file("/etc/os-release");
			int i = 0;
			while (getline (file, line)) {
				if(i == 0) {
					platform = split(line,'=')[1];
					platform.erase(remove(platform.begin(), platform.end(), '"'), platform.end());
				}
				i++;
			}
			file.close();
		}
		else {
			print("Failed to accuire /etc/os-release file, assuming FreeBSD\n");
			platform = "FreeBSD";
		}
		minpin_info["platform"] = platform;
	}
	else{
		print("Unsupported platform: ");
		print(OS);
		return 0;
	}

	//Handle absent arguments
	if(argc < 2){
		print("usage: minpin [option] ...\n");
		print("  -h help : provides a list of commands\n");
		print("  -v version : prints the package managers currently installed version\n");
		print("  -m make : builds a package from a directory\n");
		print("  -d decompile : decompiles a package\n");
		print("  -i install : installs a package\n");
		return 0;
	}
	
	//Handle basic arguments (dont require path or deps)
	if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"help") == 0) {
		print("usage: minpin [option] ...\n");
		print("  -h help : provides a list of commands\n");
		print("  -v version : prints the package managers currently installed version\n");
		print("  -m make : builds a package from a directory\n");
		print("  -d decompile : decompiles a package\n");
		print("  -i install : installs a package\n");
		return 0;
	}
	else if(strcmp(argv[1],"-v") == 0 || strcmp(argv[1],"version") == 0) {
		print("MinPin ");
		print(minpin_info["version"].c_str());
		print(", ");
		print(minpin_info["platform"].c_str());
		print("\n");
		return 0;
	}

	//Get path
	if(!updatePath()) {
		print("Failed to update path, are you root?\n");
		return 0;
	}

	//Update the requires packages
	if(!updateDeps()) {
		print("Failed to update dependencies\n");
		return 0;
	}

	//Handle regular arguments
	if(strcmp(argv[1],"-m") == 0 || strcmp(argv[1],"make") == 0) {
		if(argc > 2) {
			if(make(argv[2])) {
				print("Made minpin package file\n");
			}
			else{
				print("Package construction failed\n");
			}
		}
		else{
			print("\e[1;31msyntax error:\e[0m Incorrect use of 'minpin make [filename]'\n");
		}
	}
	else if(strcmp(argv[1],"-d") == 0 || strcmp(argv[1],"decompile") == 0) {
		if(argc > 2) {
			if(decompile(argv[2])) {
				//
			}
			else {
				print("Decompile failed\n");
			}
		}
		else{
			print("\e[1;31msyntax error:\e[0m Incorrect use of 'minpin decompile [package]'\n");
		}
	}
	else if(strcmp(argv[1],"-i") == 0 || strcmp(argv[1],"install") == 0) {
		if(argc > 2) {
			print(split(argv[2],'.')[len(split(argv[2],'.')) - 1].c_str());
			if(strcmp(split(argv[2],'.')[-1].c_str(),"minpin") != 0) {
				print("Grab from github");
			}
			if(install(argv[2])) {
				print("Installed package\n");
			}
			else{
				print("Package installation failed\n");
			}
		}
		else{
			print("\e[1;31msyntax error:\e[0m Incorrect use of 'minpin install [filename]'\n");
		}
	}
	else {
		print("Unknown argument '");
		print(argv[1]);
		print("'\n");
	}

	return 0;
}
//7 December - 2021