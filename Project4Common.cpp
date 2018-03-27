#include "Project4Common.h"

using namespace std;

InputParser::InputParser (int &argc, char **argv){
	for (int i=1; i < argc; ++i){
		this->tokens.push_back(string(argv[i]));
	}
}

const string& InputParser::getCmdOption(const string &option) {
	vector<string>::const_iterator itr;
	itr =  find(this->tokens.begin(), this->tokens.end(), option);
	if (itr != this->tokens.end() && ++itr != this->tokens.end()){
		return *itr;
	}
	static const string empty_string("");
	return empty_string;
}

bool InputParser::cmdOptionExists(const string &option) {
	return find(this->tokens.begin(), this->tokens.end(), option)
			!= this->tokens.end();
}

bool InputParser::findCmdHelp(){
	return this->cmdOptionExists("-h") || this->cmdOptionExists("--help") 
	|| this->cmdOptionExists("/?") || this->cmdOptionExists("-?") 
	|| this->cmdOptionExists("?");
}

bool isDirectory(const string &path){
	struct stat statStruct;
	if(stat(path.c_str(), &statStruct) == 0){
		if(statStruct.st_mode & S_IFDIR){
			return true;
		}
	}
	return false;
}