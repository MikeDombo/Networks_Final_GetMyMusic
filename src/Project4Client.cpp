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
    listRequestPacket["type"] = std::string("list");
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
    auto answer = receiveUntilByteEquals(sock, '\n');
    json answerJ = json(answer);

    if(verifyJSONPacket(answerJ)){
        cout << "Diff:" << endl
             << "=====================" << endl;

        auto serverFilesResponse = answerJ["response"]; //Format: [{filename: String, checksum: String}]
        auto clientMusicDataList = list(directory);     //Format: vector<MusicData>

        //Map where keys represent checksum of files, and set contains corresponding filenames
        //Assumes that multiple files, although differently named can contain the same contents
        map <string, set<string>> serverMap;
        map <string, set<string>> clientMap;

        //Build map of server file checksum to set of filenames
        for(auto file: serverFilesResponse){
            if(file.hasKey("checksum") && file.hasKey("filename")){
                string serverFileChecksum = file["checksum"].getString();
                string serverFilename = file["filename"].getString();
                set<string> serverFilenameSet;

                //If the checksum isn't in the map, then create a new set and map checksum to set
                if(serverMap.find(serverFileChecksum) == serverMap.end()){
                    serverFilenameSet.insert(serverFilename);
                    serverMap[serverFileChecksum] = serverFilenameSet;
                }
                else{
                    serverFilenameSet = serverMap[serverFileChecksum];
                    serverFilenameSet.insert(serverFilename);
                }
            } 
        }

        //Prints files found on client but not on server and builds map of client file checksums and filenames
        for(auto musicData: clientMusicDataList){
            string clientFilename = musicData.getFilename();
            string clientFileChecksum = musicData.getChecksum();
            set<string> clientFilenameSet; 
            
            //If checksum isn't present in server files, print out the client file name and checksum
            if(serverMap.find(clientFileChecksum) == serverMap.end()){
                cout << "File found on client, but not on server -> " << clientFilename << ", " << clientFileChecksum << endl;
            }
            else{
                set<string> serverFilenameSet = serverMap[clientFileChecksum];
                //Handle case where server contains same file contents as client file, but is named differently
                if(serverFilenameSet.find(clientFilename) == serverFilenameSet.end()){
                    cout << "File found on server with same content as file on client, but differently named -> " << clientFilename << ", " << clientFileChecksum << endl;
                }
            }

            //Build the client map
            if(clientMap.find(clientFileChecksum) == clientMap.end()){
                clientFilenameSet.insert(clientFilename);
                clientMap[clientFileChecksum] = clientFilenameSet;
            }
            else{
                clientFilenameSet = clientMap[clientFileChecksum];
                clientFilenameSet.insert(clientFilename);
            }
        }

        //Prints files found on server but not on client
        for(auto file: serverFilesResponse){
            string serverFileChecksum = file["checksum"].getString();
            string serverFilename = file["filename"].getString();
            
            //If checksum isn't present in server files, print out the client file name and checksum
            if(clientMap.find(serverFileChecksum) == clientMap.end()){
                cout << "Found on server, but not on client -> " << serverFilename << ", " << serverFileChecksum << endl;
            }
            else{
                set<string> clientFilenameSet = clientMap[serverFileChecksum];
                //Handle case where client contains same file contents as server file, but is named differently
                if(clientFilenameSet.find(serverFilename) == clientFilenameSet.end()){
                    cout << "File found on client with same content as file on server, but differently named ->" << serverFilename << ", " << serverFileChecksum << endl;
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
    if(userInput.size() <= 0){
        std::cerr << "Please choose and option" << endl;
    }
    else{
        try {
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
                    break;
            }
        }
        catch (exception& e){
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
