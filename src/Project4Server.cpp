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
    char *timeStr = std::ctime(&currentTime);
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

void doPullResponse(int sock, const string &directory, const json &pullRequest) {
    debug("Received: " + pullRequest.stringify());
}

void doPushResponse(int sock, const string &directory, const json &pushRequest) {
    debug("Received: " + pushRequest.stringify());
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
            } else if (type == "pushRequest") {
                doPushResponse(sock, directory, queryJ);
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
    } catch (std::exception &e) {
        struct sockaddr_in clientSockaddr;
        socklen_t addrLen = sizeof(clientSockaddr);
        getpeername(sock, (sockaddr *) &clientSockaddr, &addrLen);
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

    //Select() code
    //------------------------------------------
    int opt = true;
    int master_socket, addrlen, new_socket, client_socket[1024],
        max_clients = 1024, activity, i, sd;
    int max_sd;

    //set of socket descriptors
    fd_set readfds;

    for(i = 0; i < max_clients; i++){
        client_socket[i] = 0;
    }

    if((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to accept multiple connections (up to 1024)
    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    } 

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

    // Create structs to save the server and client addresses
    struct sockaddr_in serverAddress; /* Local address */

    /* Construct local address structure */
    memset(&serverAddress, 0, sizeof(serverAddress)); /* Zero out structure */
    serverAddress.sin_family = AF_INET; /* Internet address family */
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = serverPort; /* Local port */

    /* Bind to the local address */
    if (bind(master_socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(master_socket, 1024) < 0) {
        perror("listen() failed");
        exit(1);
    }

    addrlen = sizeof(serverAddress);

    // Constantly listen for clients
    while (true) {

        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for(i = 0; i < max_clients; i++){
            //socket descriptor
            sd = client_socket[i];
            //if the socket descriptor is valid, add it to the read list of sockets
            if(sd > 0) FD_SET(sd, &readfds);

            //find highest file descriptor number
            max_sd = (sd > max_sd) ? sd : max_sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if((activity < 0) && (errno != EINTR)){
            printf("select() error");
        }

        //Handle incoming connection on master socket
        if(FD_ISSET(master_socket, &readfds)){
            if((new_socket = accept(master_socket, (struct sockaddr *) &serverAddress, (socklen_t*) &addrlen)) < 0){
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket,
                 inet_ntoa(serverAddress.sin_addr),ntohs (serverAddress.sin_port)); 

             //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                        
                    break;  
                }  
            }

            // When a client connects, handle them using handleClient()
            handleClient(new_socket, directory);
        }

        //Handle IO operations on socket with incoming message
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                
            if (FD_ISSET(sd , &readfds))  
            {  
               handleClient(sd, directory);
               //TODO if client closes connection, then free up space in client_socket array for reuse   
            }  
        }
    }

    return 0;
}
