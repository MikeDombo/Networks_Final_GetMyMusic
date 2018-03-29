#ifndef PROJECT4_COMMON_H
#define PROJECT4_COMMON_H

#include <string>
#include <iostream>
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iterator>
#include <exception>

#include "HappyPathJSON.h"
#include "lib/CCRC32.h"
#include "lib/base64.h"

using json = JSON;

const double VERSION = 1.0;
const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char BASE64_PAD_CHAR = '=';
const uint8_t BASE64_REVERSE_MAP[256] = { // 16x16 unsigned chars. Base64 ASCII subset
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,
   0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0,  0,
   0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

class InputParser {
public:
    InputParser(int &argc, char **argv);

    const std::string &getCmdOption(const std::string &option);

    bool cmdOptionExists(const std::string &option);

    bool findCmdHelp();

private:
    std::vector<std::string> tokens;
};

class MusicData {
public:
    MusicData(const std::string &path);

    std::string getFilename();

    json getAsJSON(bool withData);

private:
    std::string b64Encode();

    std::string makeChecksum();

    int openFile();

    std::string path;
    std::string filename;
    std::string checksum;
};

bool isDirectory(const std::string &path);

std::vector<std::string> directoryFileListing(const std::string &path);

std::string getFilename(const std::string &path);

std::vector<MusicData> list(const std::string &directory);

in_addr_t hostOrIPToInet(const std::string &host);

std::string receiveUntilByteEquals(int sock, char eq);

void sendToSocket(int socket, const std::string &data);

void sendToSocket(int socket, const json &data);

bool verifyJSONPacket(json &data);

std::string base64Encode(const std::vector<char> &inputBuffer);

#endif
