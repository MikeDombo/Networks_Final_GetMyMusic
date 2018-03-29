#include "Project4Common.h"

using std::string;
using std::vector;
using std::hex;
using std::ios;
using std::stringstream;
using std::ifstream;
using std::istreambuf_iterator;

InputParser::InputParser(int &argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        this->tokens.push_back(string(argv[i]));
    }
}

const string &InputParser::getCmdOption(const string &option) {
    vector<string>::const_iterator itr;
    itr = find(this->tokens.begin(), this->tokens.end(), option);
    if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
        return *itr;
    }
    static const string empty_string("");
    return empty_string;
}

bool InputParser::cmdOptionExists(const string &option) {
    return find(this->tokens.begin(), this->tokens.end(), option)
           != this->tokens.end();
}

bool InputParser::findCmdHelp() {
    return this->cmdOptionExists("-h") || this->cmdOptionExists("--help")
           || this->cmdOptionExists("/?") || this->cmdOptionExists("-?")
           || this->cmdOptionExists("?");
}

string getFilename(const string &path) {
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = path.rfind(sep, path.length());
    if (i != string::npos) {
        return path.substr(i + 1, path.length() - i);
    }

    return path;
}

MusicData::MusicData(const string &path) {
    this->path = path;
    this->filename = ::getFilename(path);
    this->checksum = makeChecksum();
}

string MusicData::getFilename() {
    return this->filename;
}

json MusicData::getAsJSON(bool withData) {
    json fileJ = json();

    fileJ["filename"] = this->filename;
    fileJ["checksum"] = this->checksum;
    if (withData) {
        fileJ["data"] = b64Encode();
    }

    return fileJ;
}

string MusicData::b64Encode() {
    ifstream input(this->path.c_str(), ios::binary);
    // copies all data into buffer
    vector<char> buffer((
                                istreambuf_iterator<char>(input)),
                        (istreambuf_iterator<char>()));
    string strBuffer(buffer.begin(), buffer.end());

    string encoding;
    Base64 b64;
    if (b64.Encode(strBuffer, &encoding)) {
        return encoding;
    } else {
        perror("Base64 failed");
        exit(1);
    }
}

string MusicData::makeChecksum() {
    CCRC32 crc32;
    crc32.Initialize();
    stringstream stream;
    stream << hex << crc32.FileCRC(this->path.c_str());
    return stream.str();
}

bool isDirectory(const string &path) {
    struct stat statStruct;
    if (stat(path.c_str(), &statStruct) == 0) {
        if (statStruct.st_mode & S_IFDIR) {
            return true;
        }
    }
    return false;
}

vector <string> directoryFileListing(const string &path) {
    string thisPath = path;
    if (thisPath.back() != '/' && thisPath.back() != '\\') {
        thisPath = thisPath + '/';
    }

    vector <string> listing;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(thisPath.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            auto testFileDir = string(ent->d_name);
            if (!isDirectory(testFileDir)) {
                listing.push_back(thisPath + testFileDir);
            }
        }
        closedir(dir);
    } else {
        perror("Couldn't open directory");
        exit(1);
    }

    return listing;
}

vector <MusicData> list(const string &directory) {
    vector <MusicData> l;
    for (auto d : directoryFileListing(directory)) {
        l.push_back(MusicData(d));
    }

    return l;
}

// Function taking hostname or IP address as a cstring and returning the appropriate internet address
in_addr_t hostOrIPToInet(const string &host) {
    struct hostent *he;
    struct in_addr **addr_list;

    // Lookup the hostname and catch errors
    if ((he = gethostbyname(host.c_str())) == NULL) {
        perror("Unable to lookup host");
        exit(1);
    }

    // Get the list of possible IP addresses
    addr_list = (struct in_addr **) he->h_addr_list;

    // Return the first possible IP converted to in_addr_t for use with the sockaddr_in struct
    return inet_addr(inet_ntoa(*addr_list[0]));
}

/*
 * Keep trying to receive until a specifc byte is read from the server
 * 
 */
string receiveUntilByteEquals(int sock, char eq) {
    char dataBuffer;
    ssize_t bytesRead = 0;
    string fullMessage;

    while (true) {
        // Receive data
        bytesRead = recv(sock, &dataBuffer, sizeof(dataBuffer), 0);

        // Make sure read was successful
        if (bytesRead < 0) {
            perror("Error receiving data");
            return fullMessage;
        }
            // Make sure the socket wasn't closed
        else if (bytesRead == 0) {
            perror("Unexpected end of transmission from server");
            return fullMessage;
        }

        // Check if we received the end of transmission byte we were expecting
        if (dataBuffer == eq) {
            break;
        }

        // Append to our message buffer
        fullMessage += dataBuffer;
    }

    return fullMessage;
}

void sendToSocket(int socket, const string &data) {
    string myData = data + '\n';

    if (send(socket, myData.c_str(), myData.size(), 0) < 0) {
        perror("Could not send");
        exit(1);
    }
}

void sendToSocket(int socket, const json &data) {
    sendToSocket(socket, data.stringify());
}

bool verifyJSONPacket(json &data) {
    bool verified = true;

    verified = verified && data.jIsObject() && data.hasKey("version")
               && data.hasKey("type") && data["version"].getNumber() == VERSION
               && data["type"].jIsString();

    auto type = data["type"].getString();
    if (type == "list") {
        return verified;
    }
    if (type == "listResponse") {
        verified = verified && data.hasKey("response")
                   && data["response"].jIsArray();
        return verified;
    }
    if (type == "pullResponse") {
        verified = verified && data.hasKey("response")
                   && data["response"].jIsArray();
        return verified;
    }
    if (type == "pull") {
        verified = verified && data.hasKey("request")
                   && data["request"].jIsArray();
    }
    if (type == "leave") {
        return verified;
    }

    return false;
}
