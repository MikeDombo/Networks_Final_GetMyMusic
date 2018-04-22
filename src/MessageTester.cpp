#include "Project4Common.h"
#include <assert.h>

using std::string;
using std::set;
using std::vector;
using std::ios;
using std::stringstream;
using std::ofstream;
using std::cout;
using std::endl;

int main() {
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

    return 0;
}
