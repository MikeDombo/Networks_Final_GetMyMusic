#include "Project4Common.h"

using namespace std;

void printHelp(char** argv){
	cout << "Usage: " << *argv << " -p portNum -s serverHostOrIP [-d directory]" << endl;
	exit(1);
}

int main(int argc, char** argv){
	unsigned int serverPort;
	string serverHost;
	string directory = ".";

	InputParser input(argc, argv);
	if(input.findCmdHelp()){
		printHelp(argv);
	}
	if(input.cmdOptionExists("-p") && input.cmdOptionExists("-s")){
		serverPort = (unsigned int) stoul(input.getCmdOption("-p"));
		serverHost = input.getCmdOption("-s");
	}
	else{
		printHelp(argv);
	}
	if(input.cmdOptionExists("-d")){
		string newDirectory = input.getCmdOption("-d");

		if(!isDirectory(newDirectory)){
			cout << "Could not access provided directory: "
				<< newDirectory << ", are you sure that's a directory?" << endl;
			exit(1);
		}

		directory = newDirectory;
	}

	if(directory.back() != '/' && directory.back() != '\\'){
		directory = directory + '/';
	}

	for(auto m : list(directory)){
		cout << m.getAsJSON(false).dump() << endl;
	}

	return 0;
}
