#include "Project4Common.h"

using std::string;
using std::vector;
using std::ios;
using std::stringstream;
using std::cout;
using std::endl;
using std::cin;
using std::hex;

int sock;

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
    leavePacket["type"] = "leave";
    sendToSocket(sock, leavePacket);
}

void sendListRequest(int sock) {
    json listRequestPacket;

    listRequestPacket["version"] = VERSION;
    listRequestPacket["type"] = "list";
    sendToSocket(sock, listRequestPacket);
}

void cleanExit(int sock) {
    sendLeave(sock);
    close(sock);
}

void handleGetList(int sock) {
    sendListRequest(sock);
    auto answer = receiveUntilByteEquals(sock, '\n');
    json answerJ = json(answer);

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

void handleGetDiff(int sock) {
    sendListRequest(sock);
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
    int userChoice = stoi(userInput);
    cout << endl;

    switch (userChoice) {
        case 1:
            handleGetList(sock);
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
            userInteractionLoop(sock);
            break;
    }

    // Loop back once we're done
    userInteractionLoop(sock);
}

int main(int argc, char **argv) {
    unsigned int serverPort;
    string serverHost;
    string directory = ".";

    InputParser input(argc, argv);
    if (input.findCmdHelp()) {
        printHelp(argv);
    }
    if (input.cmdOptionExists("-p") && input.cmdOptionExists("-s")) {
        serverPort = htons((unsigned int) stoul(input.getCmdOption("-p")));
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
