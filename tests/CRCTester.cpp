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

int main() {
    cout << "Testing checksum for binary file" << endl;
    // Toy example from Ch. 3 of http://chrisballance.com/wp-content/uploads/2015/10/CRC-Primer.html
    string test2 = libBasedChecksum("testServerDir/d.txt");
    string res2 = computeCRC("testServerDir/d.txt");
    cout << "Target: " << test2 << endl;
    cout << "Actual: " << hex << res2 << endl;

    assert(res2 == test2);
}


