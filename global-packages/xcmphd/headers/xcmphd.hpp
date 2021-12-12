// Made by Scratchdragon, no copyright or anything do whatever you want just preferably don't act like you wrote all this because that'd be quite rude to the people on stackoverflow that told me most of it

#include <stdio.h>
#include <sys/stat.h>
using namespace std; 

//Function to switch between forwardslash and backslash
string windowsPath(string s) {
	replace( s.begin(), s.end(), '/', '\\');
	return s;
}

//Function to check if file/folder exists
int file_exists(const char * filename) {
	//Function return 0 means the file/folder does not exist
	//Function return 1 means the file/folder exists and is a file
	//Function return 2 means the file/folder exists and is a folder
	struct stat info;

	if( stat( filename, &info ) != 0 ) {
    return 0;
	}
	else if( info.st_mode & S_IFDIR ) {
		return 2;
	}
	else {
   	return 1;
	}
}

const char * get_os()
{
  #ifdef _WIN64
  return "Windows 64";
	#elif _WIN32
  return "Windows 32";
  #elif __APPLE__ || __MACH__
  return "Mac OSX";
	#elif __FreeBSD__
  return "FreeBSD";
	#elif __DragonFly__
  return "DragonFly BSD";
	#elif __NetBSD__
  return "NetBSD";
	#elif __OpenBSD__
  return "OpenBSD";
  #elif __linux__
  return "Linux";
	#elif __unix || __unix__
  return "Unix";
	#elif __ANDROID__
	return "Android";
	#elif _POSIX_VERSION
	return "POSIX";
	#elif __sun
	return "Solaris";
	#elif __hpux
	return "HP UX";
  #else
  return "Other";
  #endif
}