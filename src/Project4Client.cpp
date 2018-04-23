#include "Project4Common.h"

using std::string;
using std::vector;
using std::ios;
using std::stringstream;
using std::ofstream;
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

void handleLeave(int sock) {
    sendLeave(sock);
    close(sock);
}

json receiveResponse(int sock) {
    auto answer = receiveUntilByteEquals(sock, '\n');
    json answerJ = json(answer);
    return answerJ;
}

json doList(int sock) {
    sendListRequest(sock);
    json answerJ = receiveResponse(sock);
    return answerJ;
}

void handleList(int sock) {
    auto answerJ = doList(sock);
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

json setToJsonList(const set<string> &values) {
    vector<string> sortedVals(values.begin(), values.end());
    std::sort(sortedVals.begin(), sortedVals.end());
    json jList;
    for (auto iter : sortedVals) {
        jList.push(JSON(iter, true));
    }
    return jList;
}

void
considerFileConflict(json &diffJSON, const string &filename, const set<string> &filenameSet, bool isClient) {
    if (filenameSet.find(filename) != filenameSet.end()) {
        json conflict;
        string sourceFilenameKey = "clientFilename";
        string targetFilenameKey = "serverTargetFilename";
        string conflictKey = "clientToServerConflicts";
        if (!isClient) {
            sourceFilenameKey = "serverFilename";
            targetFilenameKey = "clientTargetFilename";
            conflictKey = "serverToClientConflicts";
        }
        conflict[sourceFilenameKey] = filename;
        conflict[targetFilenameKey] = filenameIncrement(filename, filenameSet);
        diffJSON[conflictKey].push(conflict);
    }
}

json createDiffJSON(const map<string, set<string> > &clientMap, const set<string> &clientFilenameSet,
                    const map<string, set<string> > &serverMap, const set<string> &serverFilenameSet) {
    json diffJSON;
    // iterate through clientMap, classifying checksums
    for (auto iter = clientMap.begin(); iter != clientMap.end(); ++iter) {
        string cCsum = iter->first;
        set<string> cFnames = iter->second;
        set<string> tmpFnames;
        debug(string("  Looking at client file(s) with checksum ").append(cCsum));
        if (serverMap.find(cCsum) != serverMap.end()) {     // if the server also has a file w/that checksum
            debug("    Found matching file(s) on server.");
            json duplB;
            duplB["checksum"] = JSON(cCsum, true);
            duplB["clientFilenames"] = setToJsonList(cFnames);
            tmpFnames = serverMap.at(cCsum);
            duplB["serverFilenames"] = setToJsonList(tmpFnames);
            diffJSON["duplicateBothClientServer"].push(duplB);
        } else if (cFnames.size() > 1) {
            debug("    Found multiple files matching the checksum, only on the client");
            json duplC;
            duplC["checksum"] = JSON(cCsum, true);
            duplC["filenames"] = setToJsonList(cFnames);
            diffJSON["duplicateOnlyClient"].push(duplC);
            // if the lexicographically first duplicate filename on the client is also in serverFilenames, we have a conflict.
            string firstFname = (duplC["filenames"])[0].getString();
            considerFileConflict(diffJSON, firstFname, serverFilenameSet, true);
        } else {                                              // file is unique on client
            debug("    Only 1 file matches the checksum, on client");
            json uniqueC;
            uniqueC["checksum"] = JSON(cCsum, true);
            string fname = *(cFnames.begin());
            uniqueC["filename"] = JSON(fname, true);
            diffJSON["uniqueOnlyClient"].push(uniqueC);
            considerFileConflict(diffJSON, fname, serverFilenameSet, true);
        }
    }
    debug("  Finished iterating through client checksums. Iterating through server checksums.");

    // iterate through serverMap, classifying checksums
    for (auto iter = serverMap.begin(); iter != serverMap.end(); ++iter) {
        string sCsum = iter->first;
        set<string> sFnames = iter->second;
        debug(string("  Looking at server file(s) with checksum ").append(sCsum));
        if (clientMap.find(sCsum) != clientMap.end()) {     // if the client also has a file w/that checksum
            debug("    Found matching file(s) on client");
            continue;  // because we've already handled it
        } else if (sFnames.size() > 1) {
            debug("    Found multiple files matching the checksum, only on the server.");
            json duplS;
            duplS["checksum"] = JSON(sCsum, true);  // true = please wrap in QUOTATION marks
            duplS["filenames"] = setToJsonList(sFnames);
            diffJSON["duplicateOnlyServer"].push(duplS);
            // if the lexicographically first duplicate filename on the server is also in clientFilenames, we have a conflict.
            string firstFname = (duplS["filenames"])[0].getString();
            considerFileConflict(diffJSON, firstFname, clientFilenameSet, false);
        } else {                                              // file is unique on server
            debug("    Only 1 file matches the checksum, on server");
            json uniqueS;
            uniqueS["checksum"] = JSON(sCsum, true);
            string fname = *(sFnames.begin());
            uniqueS["filename"] = JSON(fname, true);
            diffJSON["uniqueOnlyServer"].push(uniqueS);
            considerFileConflict(diffJSON, fname, clientFilenameSet, false);
        }
    }
    return diffJSON;
}

json doDiff(json listResponse) {
    /* pseudocode:
     *  make a map of checksums to sets of filenames. This facilitates identifying duplicates
     *  make a set of filenames on the client and a set of filenames on the server, to facilitate identifying conflicts
     *  classify each (checksum, names) pair as duplicateOnlyClient, duplicateOnlyServer, duplicateBoth,
     *    uniqueOnlyClient, or uniqueOnlyServer
     *  classify filenames present in both sets as "conflicts"
     *  return a json struct of that for further use
     */
    debug(string("In doDiff(json listResponse)"));

    map<string, set<string>> clientMap;
    set<string> clientFilenameSet = set<string>();
    auto clientMusicDataList = list(directory);

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

    map<string, set<string>> serverMap;
    set<string> serverFilenameSet = set<string>();

    auto serverFilesResponse = listResponse["response"];         //Format: [{filename: String, checksum: String}]
    // populate serverMap and serverFilenameSet
    for (auto file: serverFilesResponse) {
        if (file.hasKey("checksum") && file.hasKey("filename")) {
            string sFname = file["filename"].getString();
            string sCsum = file["checksum"].getString();
            serverFilenameSet.insert(sFname); // add filename (guaranteed unique locally) to set
            debug(string("  Looking at server file ").append(sFname).append(" with checksum ").append(sCsum));

            // If the checksum isn't in the map, then create a new set and map checksum to set
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

    json diffJSON = createDiffJSON(clientMap, clientFilenameSet, serverMap, serverFilenameSet);
    return diffJSON;
}

json createPushRequestFromDiffJSON(const json &diffStruct) {
    json pushRequest;
    pushRequest["version"] = VERSION;
    pushRequest["type"] = JSON("pushRequest", true);
    json emptyArr;
    emptyArr.makeArray();
    pushRequest["request"] = emptyArr;
    if (diffStruct.hasKey("uniqueOnlyClient")) {
        for (auto file: diffStruct["uniqueOnlyClient"]) {
            pushRequest["request"].push(MusicData(directory+(file["filename"].getString())).getAsJSON(true));
        }
    }
    if (diffStruct.hasKey("duplicateOnlyClient")) {
        for (auto fileish: diffStruct["duplicateOnlyClient"]) {
            pushRequest["request"].push(MusicData(directory+(fileish["filenames"][0].getString())).getAsJSON(true));
        }
    }
    return pushRequest;
}

json createPullRequestFromDiffJSON(const json &diffStruct) {
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
    if (diffStruct.hasKey("duplicateOnlyServer")) {
        for (auto fileish: diffStruct["duplicateOnlyServer"]) {
            json pullDatum;
            pullDatum["filename"] = (fileish["filenames"])[0];
            pullDatum["checksum"] = fileish["checksum"];
            pullRequest["request"].push(pullDatum);
        }
    }
    return pullRequest;
}

void printDiff(const json &diffJSON) {
    debug("diffJSON: " + diffJSON.stringify());
    auto pullRequest = createPullRequestFromDiffJSON(diffJSON);
    auto pushRequest = createPushRequestFromDiffJSON(diffJSON);

    debug("pullRequest: " + pullRequest.stringify());
    debug("pushRequest: " + pushRequest.stringify());

    cout << "On Server but not on Client:" << endl;
    for (auto file: pullRequest["request"]) {
        cout << "\t+ " << file["filename"].getString() << endl;
    }

    cout << "On Client but not on Server:" << endl;
    for (auto fileish: pushRequest["request"]) {
        cout << "\t+ " << fileish["filename"].getString() << endl;
    }
    cout << endl;
}

void handleDiff(int sock) {
    cout << "Diff:" << endl;
    cout << "=====================" << endl;
    auto listResponse = doList(sock);
    if (!verifyJSONPacket(listResponse, "listResponse")) {
        cout << "Unable to verify listResponse from server!" << endl;
        return;
    }

    auto diffJSON = doDiff(listResponse);
    printDiff(diffJSON);
}

bool isResponseComplete(const json &response, const json &request) {
    set<string> requestedChecksums;
    set<string> receivedChecksums;
    for (auto f: request["request"]) {
        requestedChecksums.insert(f["checksum"].getString());
    }
    for (auto f: response["response"]) {
        receivedChecksums.insert(f["checksum"].getString());
    }
    return requestedChecksums == receivedChecksums;
}

void handleSync(int sock) {
    cout << "Sync:" << endl;
    cout << "=====================" << endl;

    auto answerJ = doList(sock);
    if (!verifyJSONPacket(answerJ, "listResponse")) {
        cout << "Unable to verify listResponse from server!" << endl;
        return;
    }

    auto diffStruct = doDiff(answerJ);
    auto pullRequest = createPullRequestFromDiffJSON(diffStruct);
    auto pushRequest = createPushRequestFromDiffJSON(diffStruct);  // creates a pushRequest w/o data

    debug("pullRequest: " + pullRequest.stringify());
    debug("pushRequest: " + pushRequest.stringify());

    sendToSocket(sock, pushRequest);
    json pushResponse = receiveResponse(sock);
    if(!verifyJSONPacket(pushResponse, "pushResponse")){
        cout << "Bad packet received from server" << endl;
        return;
    }
    if (!isResponseComplete(pushResponse, pushRequest)) {
        cout << "Incomplete Push. Consider trying again." << endl;
    }

    debug("sending pullRequest");
    sendToSocket(sock, pullRequest);
    json pullResponse = receiveResponse(sock);
    debug(string("Handling pull response").append(pullResponse.stringify()));
    if (!verifyJSONPacket(pullResponse, "pullResponse")) {
        cout << "Bad packet received from server" << endl;
        return;
    }
    if (!isResponseComplete(pullResponse, pullRequest)) {
        cout << "Incomplete Pull. Consider trying again." << endl;
        return; // Return here, do not write bad data
    }

    for (auto fileDatum: pullResponse["response"]) {  // always write as much as we can
        auto dataIterable = base64Decode(fileDatum["data"].getString());
        string data = string(dataIterable.begin(), dataIterable.end());
        ofstream fileWriter(directory + fileDatum["filename"].getString());
        fileWriter << data;
        fileWriter.close();
    }
}

void interruptHandler(int s) {
    handleLeave(sock);
    exit(0);
}

void userInteractionLoop(int sock) {
    cout << "Please select an option from the list below" << endl;
    cout << "[1]\tList files on server" << endl;
    cout << "[2]\tDiff files on server compared to local files" << endl;
    cout << "[3]\tSync files from server to local" << endl;
    cout << "[4]\tExit" << endl << endl;

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
                    handleList(sock);
                    break;
                case 2:
                    handleDiff(sock);
                    break;
                case 3:
                    handleSync(sock);
                    break;
                case 4:
                    handleLeave(sock);
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

    userInteractionLoop(sock);

    return 0;
}
