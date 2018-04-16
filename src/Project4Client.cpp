#include "Project4Common.h"

using std::string;
using std::vector;
using std::ios;
using std::stringstream;
using std::cout;
using std::endl;
using std::cin;
using std::hex;
using std::exception;
using std::set;
using std::map;

int sock;
string directory = ".";

void printHelp(char **argv) {
    cout << "Usage: " << *argv << " -p portNum -s serverHostOrIP [-d directory]" << endl;
    exit(1);
}

int createTCPSocketAndConnect(const string &host, unsigned short serverPort) {
    // Get a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Could not get a socket");
        exit(1);
    }

    // Setup our server details
    struct sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = hostOrIPToInet(host);
    serverAddress.sin_port = serverPort;

    // Establish TCP connection with server
    if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Could not connect to server");
        exit(1);
    }

    return sock;
}

void sendLeave(int sock) {
    json leavePacket;

    leavePacket["version"] = VERSION;
    leavePacket["type"] = std::string("leave");
    sendToSocket(sock, leavePacket);
}

void sendListRequest(int sock) {
    json listRequestPacket;

    listRequestPacket["version"] = VERSION;
    listRequestPacket["type"] = string("listRequest");
    sendToSocket(sock, listRequestPacket);
}

void cleanExit(int sock) {
    sendLeave(sock);
    close(sock);
}

void printListResponse(json answerJ) {
    if (verifyJSONPacket(answerJ)) {
        auto responses = answerJ["response"];
        cout << "Server Files Listing:" << endl
             << "=====================" << endl;

        for (auto file : responses) {
            if (file.hasKey("filename")) {
                cout << file["filename"].getString() << endl;
            }
        }
        cout << "=====================" << endl << endl;
    } else {
        cout << "Bad listResponse" << endl;
    }
}

json getListResponse(int sock) {
    auto answer = receiveUntilByteEquals(sock, '\n');
    json answerJ = json(answer);
    return answerJ;
}

void handleGetListResponse(int sock) {
    sendListRequest(sock);
    printListResponse(getListResponse(sock));
}

json setToJsonList(const set<string> &values) {
    vector<string> sortedVals(values.begin(), values.end());
    std::sort(sortedVals.begin(), sortedVals.end());
    json jList;
    for (vector<string>::iterator iter=sortedVals.begin(); iter != sortedVals.end(); ++iter)  {
        jList.push(JSON(*iter, true));
    }
    return jList;
}

void considerClientToServerConflict(json &diffStruct, const string &clientFilename, const set<string> &serverFilenameSet) {
    if (serverFilenameSet.find(clientFilename) != serverFilenameSet.end()) {
        json cliToServConfl;
        cliToServConfl["clientFilename"] = clientFilename;
        cliToServConfl["serverTargetFilename"] = filenameIncrement(clientFilename, serverFilenameSet);
        diffStruct["clientToServerConflicts"].push(cliToServConfl);
    }
}

void considerServerToClientConflict(json &diffStruct, const string &serverFilename, const set<string> &clientFilenameSet) {
    if (clientFilenameSet.find(serverFilename) != clientFilenameSet.end()) {
        json servToCliConfl;
        servToCliConfl["serverFilename"] = serverFilename;
        servToCliConfl["clientTargetFilename"] = filenameIncrement(serverFilename, clientFilenameSet);
        diffStruct["serverToClientConflicts"].push(servToCliConfl);
    }
}

json buildDiffStruct(const map<string, set<string> > &clientMap, const set<string> &clientFilenameSet, const map<string, set<string> > &serverMap, const set<string> &serverFilenameSet) {
    json diffStruct;
    // iterate through clientMap, classifying checksums
    for(auto iter = clientMap.begin(); iter != clientMap.end(); ++iter)
    {
        string cCsum =  iter->first;
        set<string> cFnames = iter->second;
        set<string> tmpFnames;
        debug(string("  Looking at client file(s) with checksum ").append(cCsum));
        if (serverMap.find(cCsum) != serverMap.end()) {     // if the server also has a file w/that checksum
            debug("    Found matching file(s) on server.");
            json duplB;
            duplB["checksum"] = JSON(cCsum, true);
            duplB["clientFilenames"] = setToJsonList(cFnames);
            tmpFnames = (serverMap.find(cCsum))->second;  // can't use [] access b/c serverMap is const
            duplB["serverFilenames"] = setToJsonList(tmpFnames);
            diffStruct["duplicateBothClientServer"].push(duplB);
        }
        else if (cFnames.size() > 1) {
            debug("    Found multiple files matching the checksum, only on the client");
            json duplC;
            duplC["checksum"] = JSON(cCsum, true);
            duplC["filenames"] = setToJsonList(cFnames);
            diffStruct["duplicateOnlyClient"].push(duplC);
            // if the lexicographically first duplicate filename on the client is also in serverFilenames, we have a conflict.
            string firstFname = (duplC["filenames"])[0].getString();
            considerClientToServerConflict(diffStruct, firstFname, serverFilenameSet);
        }
        else {                                              // file is unique on client
            debug("    Only 1 file matches the checksum, on client");
            json uniqueC;
            uniqueC["checksum"] = JSON(cCsum, true);
            string fname = *(cFnames.begin());
            uniqueC["filename"] = JSON(fname, true);
            diffStruct["uniqueOnlyClient"].push(uniqueC);
            considerClientToServerConflict(diffStruct, fname, serverFilenameSet);
        }
    }
    debug("  Finished iterating through client checksums. Iterating through server checksums.");

    // iterate through serverMap, classifying checksums
    for(auto iter = serverMap.begin(); iter != serverMap.end(); ++iter)
    {
        string sCsum =  iter->first;
        set<string> sFnames = iter->second;
        debug(string("  Looking at server file(s) with checksum ").append(sCsum));
        if (clientMap.find(sCsum) != clientMap.end()) {     // if the client also has a file w/that checksum
            debug("    Found matching file(s) on client");
            continue;  // because we've already handled it
        }
        else if (sFnames.size() > 1) {
            debug("    Found multiple files matching the checksum, only on the server.");
            json duplS;
            duplS["checksum"] = JSON(sCsum, true);  // true = please wrap in QUOTATION marks
            duplS["filenames"] = setToJsonList(sFnames);
            diffStruct["duplicateOnlyServer"].push(duplS);
            // if the lexicographically first duplicate filename on the server is also in clientFilenames, we have a conflict.
            string firstFname = (duplS["filenames"])[0].getString();
            considerServerToClientConflict(diffStruct, firstFname, clientFilenameSet);
        }
        else {                                              // file is unique on server
            debug("    Only 1 file matches the checksum, on server");
            json uniqueS;
            uniqueS["checksum"] = JSON(sCsum, true);
            string fname = *(sFnames.begin());
            uniqueS["filename"] = JSON(fname, true);
            diffStruct["uniqueOnlyServer"].push(uniqueS);
            considerServerToClientConflict(diffStruct, fname, clientFilenameSet);
        }
    }
    return diffStruct;
}

json getDiff(int sock) {
    /* pseudocode:
     *  make a map of checksums to sets of filenames. This facilitates identifying duplicates
     *  make a set of filenames on the client and a set of filenames on the server, to facilitate identifying conflicts
     *  classify each (checksum, names) pair as duplicateOnlyClient, duplicateOnlyServer, duplicateBoth,
     *    uniqueOnlyClient, or uniqueOnlyServer
     *  classify filenames present in both sets as "conflicts"
     *  return a json struct of that for further use
     */
    debug(string("In getDiff(").append(std::to_string(sock)).append(")."));
    sendListRequest(sock); // do this first, and do as much as possible before waiting to read from socket
    map<string, set<string>> serverMap;
    map<string, set<string>> clientMap;
    set<string> serverFilenameSet = set<string>();
    set<string> clientFilenameSet = set<string>();
    auto clientMusicDataList = list(directory);     //Format: vector<MusicData>

    // populate clientMap and clientFilenameSet
    for (auto musicDatum: clientMusicDataList) {
        string cFname = musicDatum.getFilename();
        string cCsum = musicDatum.getChecksum();
        debug(string("  Looking at client file ").append(cFname).append(" with checksum ").append(cCsum));
        clientFilenameSet.insert(cFname);                // add filename (guaranteed unique locally) to set
        //Build the client map
        if (clientMap.find(cCsum) == clientMap.end()) {     // if new checksum, add a new map entry
            set<string> fnames;
            fnames.insert(cFname);
            clientMap[cCsum] = fnames;
        } else {                                            // else, update existing entry
            clientMap[cCsum].insert(cFname);
        }
    }
    debug("  Finished organizing client files. Waiting for response from server.");
    auto answerJ = getListResponse(sock);
    debug("  Received response from server");
    auto serverFilesResponse = answerJ["response"];         //Format: [{filename: String, checksum: String}]

    // populate serverMap and serverFilenameSet
    for (auto file: serverFilesResponse) {
        if (file.hasKey("checksum") && file.hasKey("filename")) {
            string sCsum = file["checksum"].getString();
            string sFname = file["filename"].getString();
            serverFilenameSet.insert(sFname);                // add filename (guaranteed unique locally) to set
            debug(string("  Looking at server file ").append(sFname).append(" with checksum ").append(sCsum));

            //If the checksum isn't in the map, then create a new set and map checksum to set
            if (serverMap.find(sCsum) == serverMap.end()) {
                set<string> fnames;
                fnames.insert(sFname);
                serverMap[sCsum] = fnames;
            } else {
                serverMap[sCsum].insert(sFname);
            }
        }
    }
    debug("  Finished organizing server files. Comparing file checksums across client and server.");

    json diffStruct = buildDiffStruct(clientMap, clientFilenameSet, serverMap, serverFilenameSet);
    return diffStruct;
}

json buildPushRequestFromDiffStruct(json diffStruct) {
    json pushRequest;
    pushRequest["version"] = VERSION;
    pushRequest["type"] = JSON("pushRequest", true);
    json emptyArr;
    emptyArr.makeArray();
    pushRequest["request"] = emptyArr;
    if (diffStruct.hasKey("uniqueOnlyClient")) {
        for (auto file: diffStruct["uniqueOnlyClient"]) {
            json pushDatum;
            pushDatum["filename"] = file["filename"];
            pushDatum["checksum"] = file["checksum"];
            pushDatum["data"] = JSON("\"\"");
            pushRequest["request"].push(pushDatum);
        }
    }
    if (diffStruct.hasKey("duplicateOnlyClient")) {
        for (auto fileish: diffStruct["duplicateOnlyClient"]) {
            json pushDatum;
            pushDatum["filename"] = (fileish["filenames"])[0];
            pushDatum["checksum"] = fileish["checksum"];
            pushDatum["data"] = JSON("\"\"");
            pushRequest["request"].push(pushDatum);
        }
    }
    return pushRequest;
}

json buildPullRequestFromDiffStruct(json diffStruct) {
    json pullRequest;
    pullRequest["version"] = VERSION;
    pullRequest["type"] = JSON("pullRequest", true);
    json emptyArr;
    emptyArr.makeArray();
    pullRequest["request"] = emptyArr;
    if (diffStruct.hasKey("uniqueOnlyServer")) {
      for (auto file: diffStruct["uniqueOnlyServer"]) {
        pullRequest["request"].push(file);
      }
    }
    return pullRequest;
}

void handleGetDiff(int sock) {
    auto answerJ = getListResponse(sock);

    if (verifyJSONPacket(answerJ)) {
        cout << "Diff:" << endl
             << "=====================" << endl;

        auto serverFilesResponse = answerJ["response"]; //Format: [{filename: String, checksum: String}]
        auto clientMusicDataList = list(directory);     //Format: vector<MusicData>

        // Map where keys represent checksum of files, and set contains corresponding filenames
        // Assumes that multiple files, although differently named can contain the same contents
        map<string, set<string>> serverMap;
        map<string, set<string>> clientMap;

        //Build map of server file checksum to set of filenames
        for (auto file: serverFilesResponse) {
            if (file.hasKey("checksum") && file.hasKey("filename")) {
                string serverFileChecksum = file["checksum"].getString();
                string serverFilename = file["filename"].getString();
                set<string> serverFilenameSet;

                //If the checksum isn't in the map, then create a new set and map checksum to set
                if (serverMap.find(serverFileChecksum) == serverMap.end()) {
                    serverFilenameSet.insert(serverFilename);
                    serverMap[serverFileChecksum] = serverFilenameSet;
                } else {
                    serverFilenameSet = serverMap[serverFileChecksum];
                    serverFilenameSet.insert(serverFilename);
                }
            }
        }

        //Prints files found on client but not on server and builds map of client file checksums and filenames
        for (auto musicData: clientMusicDataList) {
            string clientFilename = musicData.getFilename();
            string clientFileChecksum = musicData.getChecksum();
            set<string> clientFilenameSet;

            //If checksum isn't present in server files, print out the client file name and checksum
            if (serverMap.find(clientFileChecksum) == serverMap.end()) {
                cout << "File found on client, but not on server -> " << clientFilename << ", " << clientFileChecksum
                     << endl;
            } else {
                set<string> serverFilenameSet = serverMap[clientFileChecksum];
                //Handle case where server contains same file contents as client file, but is named differently
                if (serverFilenameSet.find(clientFilename) == serverFilenameSet.end()) {
                    cout << "File found on server with same content as file on client, but differently named -> "
                         << clientFilename << ", " << clientFileChecksum << endl;
                }
            }

            //Build the client map
            if (clientMap.find(clientFileChecksum) == clientMap.end()) {
                clientFilenameSet.insert(clientFilename);
                clientMap[clientFileChecksum] = clientFilenameSet;
            } else {
                clientFilenameSet = clientMap[clientFileChecksum];
                clientFilenameSet.insert(clientFilename);
            }
        }

        //Prints files found on server but not on client
        for (auto file: serverFilesResponse) {
            string serverFileChecksum = file["checksum"].getString();
            string serverFilename = file["filename"].getString();

            //If checksum isn't present in server files, print out the client file name and checksum
            if (clientMap.find(serverFileChecksum) == clientMap.end()) {
                cout << "Found on server, but not on client -> " << serverFilename << ", " << serverFileChecksum
                     << endl;
            } else {
                set<string> clientFilenameSet = clientMap[serverFileChecksum];
                //Handle case where client contains same file contents as server file, but is named differently
                if (clientFilenameSet.find(serverFilename) == clientFilenameSet.end()) {
                    cout << "File found on client with same content as file on server, but differently named ->"
                         << serverFilename << ", " << serverFileChecksum << endl;
                }
            }
        }
    }

    cout << endl;
}

void handleDoSync(int sock) {
    sendListRequest(sock);
}

void handleDoPull(int sock) {
    sendListRequest(sock);
}

void interruptHandler(int s) {
    cleanExit(sock);
    exit(0);
}

void userInteractionLoop(int sock) {
    cout << "Please select an option from the list below" << endl;
    cout << "[1]\tList files on server" << endl;
    cout << "[2]\tDiff files on server compared to local files" << endl;
    cout << "[3]\tSync files from server to local" << endl;
    cout << "[4]\tPull single file from server to local" << endl;
    cout << "[5]\tExit" << endl << endl;

    string userInput;
    getline(cin, userInput);
    if (userInput.size() <= 0) {
        std::cerr << "Please choose an option" << endl;
    } else {
        try {
            int userChoice = stoi(userInput);
            cout << endl;

            switch (userChoice) {
                case 1:
                    handleGetListResponse(sock);
                    break;
                case 2:
                    handleGetDiff(sock);
                    break;
                case 3:
                    handleDoSync(sock);
                    break;
                case 4:
                    handleDoPull(sock);
                    break;
                case 5:
                    cleanExit(sock);
                    return;
                default:
                    break;
            }
        }
        catch (exception &e) {
            perror(e.what());
        }
    }

    // Loop back once we're done
    userInteractionLoop(sock);
}

/*
int main(int argc, char **argv) {
    unsigned int serverPort;
    string serverHost;

    InputParser input(argc, argv);
    if (input.findCmdHelp()) {
        printHelp(argv);
    }
    if (input.cmdOptionExists("-p") && input.cmdOptionExists("-s")) {
        serverPort = htons(static_cast<uint16_t>(stoul(input.getCmdOption("-p"))));
        serverHost = input.getCmdOption("-s");
    } else {
        printHelp(argv);
    }
    if (input.cmdOptionExists("-d")) {
        string newDirectory = input.getCmdOption("-d");

        if (!isDirectory(newDirectory)) {
            cout << "Could not access provided directory: "
                 << newDirectory << ", are you sure that's a directory?" << endl;
            exit(1);
        }

        directory = newDirectory;
    }

    if (directory.back() != '/' && directory.back() != '\\') {
        directory = directory + '/';
    }


    sock = createTCPSocketAndConnect(serverHost, serverPort);

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = interruptHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

//    userInteractionLoop(sock);
    json res = getDiff(sock);
    debug(res.stringify());

    return 0;
}
*/
