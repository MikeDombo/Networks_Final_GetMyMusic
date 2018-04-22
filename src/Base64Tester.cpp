#include "Project4Common.h"
#include "lib/base64.h" // to check if our homebrewed b64 fxns match the library functionality
#include <assert.h>     // to use assert statements in our tester

using std::string;
using std::vector;
using std::ios;
using std::stringstream;
using std::ifstream;
using std::istreambuf_iterator;
using std::istringstream;
using std::cout;
using std::endl;
using std::cin;
using std::hex;
using std::exception;
using std::set;
using std::map;

void testBase64Encoding();
void testBase64Decoding();

int main() {
    testBase64Encoding();
    testBase64Decoding();
}

void testBase64EncodingHappyString() {
    string simpleString = "lalala happy string";
    cout << "  Testing encoding of \"" << simpleString << "\":" << endl;

    string target;
    Base64::Encode(simpleString, &target);
    cout << "    Target Output: \"" << target << "\"" << endl;

    istringstream input(simpleString);
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string myResult = base64Encode(simpleString);
    cout << "    Actual Output: \"" << myResult << "\"" << endl;
    assert(myResult == target);
}

void testBase64EncodingBinaryBlob() {
    cout << "  Testing encoding of \"blob.binary\":" << endl;

    ifstream input("testServerDir/blob.binary", ios::binary);
    // copies all data into buffer
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string strBuffer(buffer.begin(), buffer.end());
    string target;
    Base64::Encode(strBuffer, &target);
    cout << "    Target Output: \"" << target << "\"" << endl;

    string myResult = base64Encode(strBuffer);
    cout << "    Actual Output: \"" << myResult << "\"" << endl;
    assert(myResult == target);
}

void testBase64Encoding() {
    cout << "Testing base64Encode():" << endl;
    // 1. Equivalent conversions for ASCII string
    testBase64EncodingHappyString();

    // 2. Equivalent conversions for binary file
    testBase64EncodingBinaryBlob();
}

void testBase64DecodeHappyString() {
    string target = "lalala happy string";
    cout << "  Testing decoding of \"" << target << "\":" << endl;
    string src;
    Base64::Encode(target, &src);
    cout << "Encoded input: " << src << endl;
    cout << "Target output: " << target << endl;

    string output = base64Decode(src);
    cout << "Actual output: " << output << endl;
    assert(output == target);
}

void testBase64DecodeBinaryBlob() {
    cout << "  Testing decoding of \"blob.binary\":" << endl;
    ifstream input("testServerDir/blob.binary", ios::binary);
    // copies all data into buffer
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string target(buffer.begin(), buffer.end());
    string src;
    Base64::Encode(target, &src);
    cout << "    Encoded input: " << src << endl;
    cout << "    Target output: " << target << endl;

    string output = base64Decode(src);
    cout << "    Actual output: " << output << endl;
    assert(output == target);
}

void testBase64Decoding() {
    cout << "Testing base64Decode():" << endl;
    testBase64DecodeHappyString();
    testBase64DecodeBinaryBlob();
}