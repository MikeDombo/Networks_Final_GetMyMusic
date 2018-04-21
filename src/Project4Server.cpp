#include "Project4Common.h"
#include <chrono>
#include <ctime>

using std::string;
using std::set;
using std::vector;
using std::ios;
using std::stringstream;
using std::ofstream;
using std::cout;
using std::endl;

void printHelp(char **argv) {
    cout << "Usage: " << *argv << " -p listeningPort [-d directory]" << endl;
    exit(1);
}

void log(const string &logMessage, const string &logFilepath) {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char *timeStr = std::ctime(&currentTime);
    timeStr[strlen(timeStr) - 1] = '\0';  // drop trailing newline
    stringstream outputMsg;
    outputMsg << "LOG: (Time: " << timeStr << ") " << logMessage << endl;
    debug(outputMsg.str());
    ofstream fileWriter(logFilepath, std::ofstream::out | std::ofstream::app);  // append to log file if it exists
    fileWriter << outputMsg.str();
    fileWriter.close();
}

void doListResponse(int sock, const string &directory) {
    auto files = list(directory);

    vector<json> jsonFiles(files.size());
    for (auto f : files) {
        jsonFiles.push_back(f.getAsJSON(false));
    }

    json listResponsePacket;
    listResponsePacket["version"] = VERSION;
    listResponsePacket["type"] = string("listResponse");
    listResponsePacket["response"] = jsonFiles;
    sendToSocket(sock, listResponsePacket);
}

void doPullResponse(int sock, const string &directory, const json &pullRequest) {
    json pullResponse;
    pullResponse["version"] = VERSION;
    pullResponse["type"] = JSON("pullResponse", true);
    json emptyArr;
    emptyArr.makeArray();
    pullResponse["response"] = emptyArr;

    vector<MusicData> musicList = list(directory);
    for (auto reqItem : pullRequest["request"]) {
        for (MusicData datum : musicList) { // O(n^2) -- big oof.
        // TODO: make the request part of a diffstruct an associative array
        // Or better yet, parse it into a class and give it nice class methods
            if (datum.getFilename() == reqItem["filename"].getString() && 
                datum.getChecksum() == reqItem["checksum"].getString()) {
                pullResponse["response"].push(datum.getAsJSON(true));
            }
        }
    }
    sendToSocket(sock, pullResponse);
}

void doPushResponse(int sock, const string &directory, const json &pushRequest) {
    json pushResponse;
    pushResponse["version"] = VERSION;
    pushResponse["type"] = JSON("pushResponse", true);
    json emptyArr;
    emptyArr.makeArray();
    pushResponse["response"] = emptyArr;

    vector<string> filepaths = directoryFileListing(directory);
    set<string> filenames;
    for (string path : filepaths) {
        filenames.insert(getFilename(path));
    }
    for (auto file : pushRequest["request"]) {
        string newname = filenameIncrement(file["filename"].getString(), filenames);
        auto dataIterable = base64Decode(file["data"].getString());
        string data = string(dataIterable.begin(), dataIterable.end());
        std::ofstream fileWriter(directory + newname);
        fileWriter << data;
        fileWriter.close(); 
        json fileResp;
        fileResp["filename"] = file["filename"];
        fileResp["checksum"] = file["checksum"];
        pushResponse["response"].push(fileResp);
    }
    sendToSocket(sock, pushResponse);
}


void handleClient(int sock, const string &directory, const string &logFilepath) {
    auto query = receiveUntilByteEquals(sock, '\n');
    try {
        auto queryJ = json(query);  // will throw an exception if invalid JSON received
        debug("Received: " + queryJ.stringify());

        if (verifyJSONPacket(queryJ)) {
            string type = queryJ["type"].getString();

            if (type == "listRequest") {
                log(string("Client at ").append(getPeerStringFromSocket(sock)).append(string(" requested a list of files")), logFilepath);
                doListResponse(sock, directory);
            } else if (type == "pullRequest") {
                log(string("Client at ").append(getPeerStringFromSocket(sock)).append(string(" requested to pull files: (TODO)")), logFilepath);
                doPullResponse(sock, directory, queryJ);
                log(string("Client at ").append(getPeerStringFromSocket(sock)).append(string(" requested to send some files: (TODO)")), logFilepath);
            } else if (type == "pushRequest") {
                doPushResponse(sock, directory, queryJ);
            } else if (type == "leave") {
                log("Client at " + getPeerStringFromSocket(sock) + " cleanly closed connection", logFilepath);
                close(sock);
                return;
            } else {
                cout << "Unknown type: " << type << endl;
                close(sock);
            }
        }

        // Loop
        handleClient(sock, directory, logFilepath);
    } catch (std::exception &e) {
        log("Client at " + getPeerStringFromSocket(sock) + " unexpectedly closed connection", logFilepath);
        return;
    }
}

int main(int argc, char **argv) {
    unsigned int serverPort;
    string directory = ".";
    string logFilepath = "serverLog.txt";

    InputParser input(argc, argv);
    if (input.findCmdHelp()) {
        printHelp(argv);
    }
    if (input.cmdOptionExists("-p")) {
        serverPort = htons((unsigned int) stoul(input.getCmdOption("-p")));
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

    // Create variables for the sockets that will be created
    int clientSocket;
    int serverSocket;
    // Create structs to save the server and client addresses
    struct sockaddr_in serverAddress; /* Local address */
    struct sockaddr_in clientAddress; /* Client address */


    /* Create socket for incoming connections */
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket () failed");
        exit(1);
    }

    /* Construct local address structure */
    memset(&serverAddress, 0, sizeof(serverAddress)); /* Zero out structure */
    serverAddress.sin_family = AF_INET; /* Internet address family */
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = serverPort; /* Local port */

    /* Bind to the local address */
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(serverSocket, 100) < 0) {
        perror("listen() failed");
        exit(1);
    }

    // Constantly listen for clients
    while (true) {
        unsigned int clientLength = sizeof(clientAddress);

        /* Wait for a client to connect */
        if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength)) < 0) {
            perror("accept() failed");
            exit(1);
        }

        // When a client connects, handle them using handleClient()
        handleClient(clientSocket, directory, logFilepath);
    }

    return 0;
}
