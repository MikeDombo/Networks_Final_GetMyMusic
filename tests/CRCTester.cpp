#include "../src/Project4Common.h"
#include "../tests/lib/CRC32.h"
#include <assert.h>     // to use assert statements in our tester

using std::cout;
using std::endl;
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

string libBasedChecksum(const string& filepath) {
    CRC32 crc32;
    stringstream stream;
    stream << hex << crc32.FileCRC(filepath.c_str());
    return stream.str();
}

void testFileD() {
    cout << "Testing checksum for single-letter file: testServerDir/d.txt" << endl;
    string target = libBasedChecksum("testServerDir/d.txt");
    string actual = computeCRC("testServerDir/d.txt");
    cout << "Target: " << target << endl;
    cout << "Actual: " << actual << endl;
    assert(target == actual);
}

void testFileEmpty() {
    cout << "Testing checksum for empty file: testClientDir/emptyFile" << endl;
    string target = libBasedChecksum("testClientDir/emptyFile");
    string actual = computeCRC("testClientDir/emptyFile");
    cout << "Target: " << target << endl;
    cout << "Actual: " << actual << endl;
    assert(target == actual);
}

void testFileBinary() {
    cout << "Testing checksum for binary file: testServerDir/blob.binary" << endl;
    string target = libBasedChecksum("testServerDir/blob.binary");
    string actual = computeCRC("testServerDir/blob.binary");
    cout << "Target: " << target << endl;
    cout << "Actual: " << actual << endl;
    assert(target == actual);
}

int main() {
  testFileD();
  testFileEmpty();
  testFileBinary();
}


