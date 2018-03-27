#ifndef PROJECT4_COMMON_H
#define PROJECT4_COMMON_H

#include <string>
#include <iostream>
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>

#include "lib/json.hpp"

using namespace std;
using json = nlohmann::json;

const unsigned int VERSION = 1;
const string VALID_TYPES[] = {"list", "listResponse", "pull", "pullResponse", "leave"};

class InputParser{
    public:
        InputParser (int &argc, char **argv);
        const string& getCmdOption(const string &option);
        bool cmdOptionExists(const string &option);
		bool findCmdHelp();
    private:
        vector <string> tokens;
};

bool isDirectory(const string &path);
vector<string> directoryFileListing(const string &path);

#endif
