#include "../src/Project4Common.h"
#include "../src/lib/base64.h" // to check if our homebrewed b64 fxns match the library functionality
#include <assert.h>     // to use assert statements in our tester

using std::string;
using std::vector;
using std::ios;
using std::stringstream;
using std::ifstream;
using std::ofstream;
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

void testFilenameIncrement();

void testBase64Decoding();

int main() {
     testBase64Encoding();
     testBase64Decoding();
    return 0;
}

void testBase64EncodingBinaryFile() {
    cout << "  Testing conversion of \"" << "blob.binary" << "\":" << endl;

    ifstream input("testServerDir/blob.binary", ios::binary);
    // copies all data into buffer
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string strBuffer(buffer.begin(), buffer.end());
    string target;
    Base64::Encode(strBuffer, &target);
    cout << "    Target Output: \"" << target << "\"" << endl;

    string myresult = base64Encode(buffer);
    cout << "    Actual Output: \"" << myresult << "\"" << endl;
    assert(myresult == target);
}

void testBase64EncodingHappyString() {
    string simpleString = "lalala happy string";
    cout << "  Testing conversion of \"" << simpleString << "\":" << endl;

    string target;
    Base64::Encode(simpleString, &target);
    cout << "    Target Output: \"" << target << "\"" << endl;

    istringstream input(simpleString);
    vector<char> buffer((istreambuf_iterator<char>(input)),
                        istreambuf_iterator<char>());
    string myresult = base64Encode(buffer);
    cout << "    Actual Output: \"" << myresult << "\"" << endl;
    assert(myresult == target);
}

void testBase64Encoding() {
    cout << "Testing Base64 Encoding" << endl;
    // 1. Equivalent encodings of ASCII string
    testBase64EncodingHappyString();
    // 2. Equivalent encodings of a binary file
    testBase64EncodingBinaryFile();
}

void testBase64DecodingHappyString() {
    string simpleString = "happy string2";
    cout << "  Testing decoding to binary file" << endl;
    string realEncoding;
    Base64::Encode(simpleString, &realEncoding);
    string realDecoding;
    Base64::Decode(realEncoding, &realDecoding);
    assert(realDecoding == simpleString);
    cout << "    Target output: " << realDecoding << endl;
    string myDecoding = base64Decode(realEncoding);
    cout << "    Actual output: " << myDecoding << endl;
    assert(myDecoding == realDecoding);
}

void testBase64DecodingBinaryFile() {
    cout << "  Testing decoding to binary file " << endl;

    string filepath = "testServerDir/blob.binary";
    ifstream inputStream(filepath, ios::binary);
    // copies all data into buffer
    vector<char> buffer((istreambuf_iterator<char>(inputStream)),
                        istreambuf_iterator<char>());
    string inputString(buffer.begin(), buffer.end());
    string realEncoding;
    Base64::Encode(inputString, &realEncoding);

    string realDecoding;
    Base64::Decode(realEncoding, &realDecoding);
    assert(realDecoding == inputString);
    cout << "    Target output: " << realDecoding << endl;
    string myDecoding = base64Decode(realEncoding);
    cout << "    Actual output: " << myDecoding << endl;
    assert(myDecoding == realDecoding);

    string outputFilepath = "testClientDir/blob.binary";
    ofstream outputStream;
    outputStream.open(outputFilepath, std::ios::binary | std::ios::trunc);
    outputStream << realDecoding;
    outputStream.close();
    MusicData inputDatum(filepath);
    MusicData outputDatum(outputFilepath);
    assert(inputDatum.getChecksum() == outputDatum.getChecksum());
}

void testBase64Decoding() {
    cout << "Testing Base64 Decoding" << endl;
    testBase64DecodingHappyString();
    testBase64DecodingBinaryFile();
}