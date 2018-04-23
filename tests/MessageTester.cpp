#include "../src/Project4Common.h"
#include "../src/HappyPathJSON.h"
#include <assert.h>

using std::string;
using std::set;
using std::vector;
using std::map;
using std::ios;
using std::stringstream;
using std::ofstream;
using std::cout;
using std::endl;

void prettyPrintTester() {
    json jsonEmpty;
    json emptyArr;
    emptyArr.makeArray();
    jsonEmpty["request"] = emptyArr;
    string targetEmpty = "()";
    string outputEmpty = prettyListFiles(jsonEmpty);
    assert(outputEmpty == targetEmpty);

    json json1 = json("{\"request\":[{\"filename\":\"foo\",\"checksum\":\"bar\"}]}");
    string target1 = "(foo)";
    string output1 = prettyListFiles(json1);
    assert(output1 == target1);

    json json2 = json(
            "{\"request\":[{\"filename\":\"foo\",\"checksum\":\"bar\"},{\"filename\":\"foo2\",\"checksum\":\"bar2\"}]}");
    string target2 = "(foo, foo2)";
    string output2 = prettyListFiles(json2);
    assert(output2 == target2);
}

void testFilenameIncrement() {
    cout << "Testing filenameIncrement()" << endl;
    set<string> existingFilenames;
    existingFilenames.insert("file");
    existingFilenames.insert("file1.ext");
    existingFilenames.insert("file2.ext");
    existingFilenames.insert("file2 (1).ext");
    existingFilenames.insert("file2 (2).ext");

    cout << "  Check if a filename not present is successfully unchanged by filenameIncrement" << endl;
    string obsvFname = filenameIncrement("file0", existingFilenames);
    cout << "    Target output: " << "file0" << endl;
    cout << "    Actual output: " << obsvFname << endl;
    assert(obsvFname == "file0");

    cout << "  Check if a filename present without an extension gets a (1) afterward" << endl;
    obsvFname = filenameIncrement("file", existingFilenames);
    cout << "    Target output: " << "file (1)" << endl;
    cout << "    Actual output: " << obsvFname << endl;
    assert(obsvFname == "file (1)");

    cout << "  Check if a filename present with an extension gets a (1) before the ext." << endl;
    obsvFname = filenameIncrement("file1.ext", existingFilenames);
    cout << "    Target output: " << "file1 (1).ext" << endl;
    cout << "    Actual output: " << obsvFname << endl;
    assert(obsvFname == "file1 (1).ext");

    cout << "  Check if a filename present with n increments gets an (n+1) before the ext." << endl;
    obsvFname = filenameIncrement("file2.ext", existingFilenames);
    cout << "    Target output: " << "file2 (3).ext" << endl;
    cout << "    Actual output: " << obsvFname << endl;
    assert(obsvFname == "file2 (3).ext");
}

int main() {
    prettyPrintTester();
    testFilenameIncrement();

    return 0;
}
