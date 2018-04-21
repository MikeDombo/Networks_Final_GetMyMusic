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

int main() {
    testBase64Encoding();
}

void testBase64EncodingHappyString() {
    string simpleString = "lalala happy string";
    cout << "Testing conversion of \"" << simpleString << "\":" << endl;

    string target;
    Base64::Encode(simpleString, &target);
    cout << "    Target Output: \"" << target << "\"" << endl;

    istringstream input(simpleString);
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string myResult = base64Encode(buffer);
    cout << "    Actual Output: \"" << myResult << "\"" << endl;
    assert(myResult == target);
}

void testBase64EncodingBinaryBlob() {
    cout << "Testing conversion of \"" << "blob.binary" << "\":" << endl;

    ifstream input("testServerDir/blob.binary", ios::binary);
    // copies all data into buffer
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string strBuffer(buffer.begin(), buffer.end());
    string target;
    Base64::Encode(strBuffer, &target);
    cout << "    Target Output: \"" << target << "\"" << endl;

    string myResult = base64Encode(buffer);
    cout << "    Actual Output: \"" << myResult << "\"" << endl;
    assert(myResult == target);
}

void testBase64Encoding() {
    // 1. Equivalent conversions for ASCII string
    testBase64EncodingHappyString();

    // 2. Equivalent conversions for binary file
    testBase64EncodingBinaryBlob();
}
