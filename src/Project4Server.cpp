#include "Project4Common.h"
#include <chrono>
#include <ctime>

using std::string;
using std::vector;
using std::ios;
using std::stringstream;
using std::cout;
using std::endl;

void printHelp(char **argv) {
    cout << "Usage: " << *argv << " -p listeningPort [-d directory]" << endl;
    exit(1);
}

void log(const std::string &logMessage) {
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char* timeStr = std::ctime(&currentTime);
    timeStr[strlen(timeStr) - 1] = '\0';  // drop trailing newline
    cout << "LOG: (Time: " << timeStr << ") " << logMessage << endl;
    // TODO: append that line to an actual log file
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

void doPullResponse(int sock, const string &directory, const json &queryJ) {

}

void handleClient(int sock, const string &directory) {
    auto query = receiveUntilByteEquals(sock, '\n');
    try {
        auto queryJ = json(query);  // will throw an exception if invalid JSON received

        if (verifyJSONPacket(queryJ)) {
            string type = queryJ["type"].getString();

            if (type == "listRequest") {
                doListResponse(sock, directory);
            } else if (type == "pullRequest") {
                doPullResponse(sock, directory, queryJ);
            } else if (type == "leave") {
                close(sock);
                return;
            } else {
                cout << "Unknown type: " << type << endl;
                close(sock);
            }
        }

        // Loop
        handleClient(sock, directory);
    } catch (std::exception& e) {
        struct sockaddr_in clientSockaddr;
        socklen_t addrLen = sizeof(clientSockaddr);
        getpeername(sock, (sockaddr*) &clientSockaddr, &addrLen);
        std::ostringstream stringStream;
        stringStream << "Client at " << inet_ntoa(clientSockaddr.sin_addr) << ":";
        stringStream << ((int) ntohs(clientSockaddr.sin_port)) << " unexpectedly closed connection";
        log(stringStream.str());
        return;
    }
}

int main(int argc, char **argv) {
    unsigned int serverPort;
    string directory = ".";

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
        handleClient(clientSocket, directory);
    }

    return 0;
}
