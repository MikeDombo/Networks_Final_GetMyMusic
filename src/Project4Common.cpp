#include "Project4Common.h"
#include <cstdint>

using std::string;
using std::vector;
using std::hex;
using std::ios;
using std::stringstream;
using std::ifstream;
using std::istreambuf_iterator;
using std::size_t;
using std::set;
using std::map;

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

string MusicData::getChecksum() {
    return this->checksum;
}

json MusicData::getAsJSON(bool withData) {
    json fileJ = json();

    fileJ["filename"] = this->filename;
    fileJ["checksum"] = JSON(this->checksum, true);
    if (withData) {
        fileJ["data"] = b64Encode();
    }

    return fileJ;
}

string MusicData::b64Encode() {
    ifstream input(this->path.c_str(), ios::binary);
    // copies all data into buffer
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    // Do base64 encoding and return as a string
    return base64Encode(buffer);
}

string MusicData::makeChecksum() {
    CRC32 crc32;
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

vector<string> directoryFileListing(const string &path) {
    string thisPath = path;
    if (thisPath.back() != '/' && thisPath.back() != '\\') {
        thisPath = thisPath + '/';
    }

    vector<string> listing;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(thisPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
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

vector<MusicData> list(const string &directory) {
    vector<MusicData> l;
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
 * Keep trying to receive until a specific byte is read from the server
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

bool verifyJSONPacket(const json &data) {
    bool verified = true;

    verified = verified && data.isObject() && data.hasKey("version")
               && data.hasKey("type") && data["version"].getNumber() == VERSION
               && data["type"].isString();

    if (!verified) {
        return false;
    }

    std::string type = data["type"].getString();

    if (type == "listRequest") {
        return verified;
    }
    if (type == "listResponse") {
        return verified && data.hasKey("response")
               && data["response"].isArray();
    }
    if (type == "pullRequest") {
        return verified && data.hasKey("request")
               && data["request"].isArray();
    }
    if (type == "pullResponse") {
        return verified && data.hasKey("response")
               && data["response"].isArray();
    }
    if (type == "pushRequest") {
        return verified && data.hasKey("request")
               && data["request"].isArray();
    }
    if (type == "pushResponse") {
        return verified && data.hasKey("response")
               && data["response"].isArray();
    }
    if (type == "leave") {
        return verified;
    }

    return false;
}

bool verifyJSONPacket(const json &data, const string &type) {
    return verifyJSONPacket(data) && data["type"].getString() == type;
}

// base64 uses an 8-bit character to store 6 "raw" bits. 
// These sync up at the least common multiple, 24 bits (every 3 bytes of input)
// Ensure a contiguous representation for bit manipulation
struct fourchar {
    char padding;
    char char1;
    char char2;
    char char3;
};
union Bitmanip {
    fourchar f;
    uint32_t i;
};


string base64Encode(const std::vector<char> &inputBuffer) {
    stringstream output;
    uint8_t index;
    Bitmanip b;

    for (size_t i = 0; i + 2 < inputBuffer.size(); i += 3) {
        // extract 3 bytes from the vector
        b.f.padding = (char) 0x00;
        b.f.char1 = inputBuffer[i];
        b.f.char2 = inputBuffer[i + 1];
        b.f.char3 = inputBuffer[i + 2];
        b.i = htonl(b.i);

        index = static_cast<uint8_t>((b.i & 0x00FC0000) >> 18);
        output << BASE64_CHARS[index];
        index = static_cast<uint8_t>((b.i & 0x0003F000) >> 12);
        output << BASE64_CHARS[index];
        index = static_cast<uint8_t>((b.i & 0x00000FC0) >> 6);
        output << BASE64_CHARS[index];
        index = static_cast<uint8_t>((b.i & 0x0000003F) >> 0);
        output << BASE64_CHARS[index];
    }

    // Get the remaining 1 or 2 bytes
    memset(&b, 0, 3);
    switch (inputBuffer.size() % 3) {
        case 1:
            b.f.char1 = inputBuffer[inputBuffer.size() - 1];
            b.i = htonl(b.i);
            index = static_cast<uint8_t>((b.i & 0x00FC0000) >> 18);
            output << BASE64_CHARS[index];
            index = static_cast<uint8_t>((b.i & 0x00030000) >> 12);
            output << BASE64_CHARS[index];
            output << BASE64_PAD_CHAR;
            output << BASE64_PAD_CHAR;
            break;
        case 2:
            b.f.char1 = inputBuffer[inputBuffer.size() - 2];
            b.f.char2 = inputBuffer[inputBuffer.size() - 1];
            b.i = htonl(b.i);
            index = static_cast<uint8_t>((b.i & 0x00FC0000) >> 18);
            output << BASE64_CHARS[index];
            index = static_cast<uint8_t>((b.i & 0x0003F000) >> 12);
            output << BASE64_CHARS[index];
            index = static_cast<uint8_t>((b.i & 0x00000F00) >> 6);
            output << BASE64_CHARS[index];
            output << BASE64_PAD_CHAR;
            break;
        default:
            break;
    }
    return output.str();
}

string base64Decode(const string &inputString) {
    vector<char> resultVector;
    Bitmanip b_in;
    Bitmanip b_out;

    size_t i = 0;
    for (; i + 3 < inputString.size(); i += 4) {
        // Extract four bytes from the input string
        b_in.f.padding = inputString[i + 0];
        b_in.f.char1 = inputString[i + 1];
        b_in.f.char2 = inputString[i + 2];
        b_in.f.char3 = inputString[i + 3];
        memset(&b_out, 0, 4);

        b_out.i |= BASE64_REVERSE_MAP[static_cast<uint8_t>(b_in.f.padding)] << 18;
        b_out.i |= BASE64_REVERSE_MAP[static_cast<uint8_t>(b_in.f.char1)] << 12;
        b_out.i |= BASE64_REVERSE_MAP[static_cast<uint8_t>(b_in.f.char2)] << 6;
        b_out.i |= BASE64_REVERSE_MAP[static_cast<uint8_t>(b_in.f.char3)];
        assert(static_cast<uint8_t>(b_out.f.char3) == 0);

        // Reverse byte order
        b_out.i = htonl(b_out.i);

        resultVector.emplace_back(b_out.f.char1);
        if (b_in.f.char2 != '=') {
            resultVector.emplace_back(b_out.f.char2);
        }
        if (b_in.f.char3 != '=') {
            resultVector.emplace_back(b_out.f.char3);
        }
    }
    assert(i == inputString.size());
    return string(resultVector.begin(), resultVector.end());
}

void writeBase64ToFile(const std::string &path, const std::string &data) {
    string outString = base64Decode(data);
    std::ofstream fileWriter(path, std::ios::binary);
    for (auto d : outString) {
        fileWriter.write(&d, sizeof(char));
    }
    fileWriter.close();
}

string filenameIncrement(const string &filename, const set<string> &existingFilenames) {
    if (existingFilenames.find(filename) != existingFilenames.end()) {
        string res(filename);
        size_t periodPos = filename.find_first_of('.');
        if (periodPos != string::npos) {
            int i = 1;
            char c[2];
            res.insert(periodPos, " (1)");
            while (existingFilenames.find(res) != existingFilenames.end() && i > 0) {
                snprintf(c, 2, "%d", ++i);
                res[periodPos + 2] = c[0];
            }
        } else {
            res.append(" (1)");
        }
        return res;
    } else {
        return filename;
    }
}

string getPeerStringFromSocket(int sock) {
    struct sockaddr_in clientSockaddr;
    socklen_t addrLen = sizeof(clientSockaddr);
    getpeername(sock, (sockaddr *) &clientSockaddr, &addrLen);
    std::ostringstream clientInfo;
    clientInfo << inet_ntoa(clientSockaddr.sin_addr) << ":";
    clientInfo << ((int) ntohs(clientSockaddr.sin_port));
    return clientInfo.str();
}

string prettyListFiles(const json &message) {
    json dataPortion;
    ssize_t numElements = 0;
    if (message.hasKey("request")) {
        dataPortion = message["request"];
    } else if (message.hasKey("response")) {
        dataPortion = message["response"];
    }
    numElements = dataPortion.size();
    if (numElements == 0) {
        return "()";
    }
    stringstream s;
    s << "(";
    for (int i = 0; i < numElements - 1; ++i) {
        s << (dataPortion[i])["filename"].getString() << ", ";
    }
    s << (dataPortion[numElements - 1])["filename"].getString() << ")";
    return s.str();
}
