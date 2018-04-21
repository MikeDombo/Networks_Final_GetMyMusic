#include "Project4Common.h"
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

void testFilenameIncrement();

/*
void testBuildDiffStruct();
void testBuildPushPull();

json createDiffJSON(const std::map<std::string, std::set<std::string> > &clientMap,
                    const std::set<std::string> &clientFilenameSet,
                    const std::map<std::string, std::set<std::string> > &serverMap,
                    const std::set<std::string> &serverFilenameSet);  // defined in Project4Client.cpp. Requires that the main() method therein be commented out
json buildPullRequestFromDiffStruct(json diffStruct);
json buildPushRequestFromDiffStruct(json diffStruct);
*/

void testMessages();

int main() {
  //testFilenameIncrement();
    testMessages();
    return 0;
}

void testDefaultConstructors() {
    Message m;
    cout << "Message m: " << m.stringify() << endl;
    assert(m == json("{\"version\":1.0}"));

    Request req;
    cout << "Request req: " << req.stringify() << endl;
    assert(req == json("{\"request\":[],\"version\":1}"));

    ListRequest listRequest;
    cout << "ListRequest lReq: " << listRequest.stringify() << endl;
    assert(listRequest == json("{\"request\":[],\"type\":\"listRequest\",\"version\":1}"));

    PullRequest pullRequest;
    cout << "PullRequest pullRequest: " << pullRequest.stringify() << endl;
    assert(pullRequest == json("{\"request\":[],\"type\":\"pullRequest\",\"version\":1}"));

    PushRequest pushRequest;
    cout << "PushRequest pushRequest: " << pushRequest.stringify() << endl;
    assert(pushRequest == json("{\"request\":[],\"type\":\"pushRequest\",\"version\":1}"));

    Response resp;
    cout << "Response resp: " << resp.stringify() << endl;
    assert(resp == json("{\"response\":[],\"version\":1}"));

    ListResponse listResponse;
    cout << "ListResponse lResponse: " << listResponse.stringify() << endl;
    assert(listResponse == json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}"));

    PullResponse pullResponse;
    cout << "PullResponse pullResponse: " << pullResponse.stringify() << endl;
    assert(pullResponse == json("{\"response\":[],\"type\":\"pullResponse\",\"version\":1}"));

    PushResponse pushResponse;
    cout << "PushResponse pushResponse: " << pushResponse.stringify() << endl;
    assert(pushResponse == json("{\"response\":[],\"type\":\"pushResponse\",\"version\":1}"));
}

void testCopyConstructors() {
    try {
        ListRequest listRequest = ListRequest(json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}"));
        cout << "Attempting to construct a ListRequest from invalid JSON succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to construct a ListRequest from invalid JSON failed correctly" << endl;
        assert(true);
    }
    cout << "Copy constructor for ListRequest" << endl;
    ListRequest listRequest = ListRequest(json("{\"request\":[],\"type\":\"listRequest\",\"version\":1}"));

    try {
        PullRequest pullRequest = PullRequest(json("{\"response\":[],\"type\":\"pullResponse\",\"version\":1}"));
        cout << "Attempting to construct a PullRequest from invalid JSON succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to construct a PullRequest from invalid JSON failed correctly" << endl;
        assert(true);
    }
    cout << "Copy constructor for PullRequest" << endl;
    PullRequest pullRequest = PullRequest(json("{\"request\":[],\"type\":\"pullRequest\",\"version\":1}"));
    
    try {
        PushRequest pushRequest = PushRequest(json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}"));
        cout << "Attempting to construct a PushRequest from invalid JSON succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to construct a PushRequest from invalid JSON failed correctly" << endl;
        assert(true);
    }
    cout << "Copy constructor for PushRequest" << endl;
    PushRequest pushRequest = PushRequest(json("{\"request\":[],\"type\":\"pushRequest\",\"version\":1}"));

    try {
        ListResponse listResponse = ListResponse(json("{\"purple\":[],\"type\":\"listRequest\",\"version\":1}"));
        cout << "Attempting to construct a ListResponse from invalid JSON succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to construct a ListResponse from invalid JSON failed correctly" << endl;
        assert(true);
    }
    cout << "Copy constructor for ListResponse" << endl;
    ListResponse listResponse = ListResponse(json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}"));

    try {
        PullResponse pullResponse = PullResponse(json("{\"request\":[],\"type\":\"listRequest\",\"version\":1}"));
        cout << "Attempting to construct a PullResponse from invalid JSON succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to construct a PullResponse from invalid JSON failed correctly" << endl;
        assert(true);
    }
    cout << "Copy constructor for PullResponse" << endl;
    PullResponse pullResponse = PullResponse(json("{\"response\":[],\"type\":\"pullResponse\",\"version\":1}"));

    try {
        PushResponse pushResponse = PushResponse(json("{\"response\":[],\"type\":\"listRequest\",\"version\":1}"));
        cout << "Attempting to construct a PushResponse from invalid JSON succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to construct a PushResponse from invalid JSON failed correctly" << endl;
        assert(true);
    }
    cout << "Copy constructor for PushResponse" << endl;
    PushResponse pushResponse = PushResponse(json("{\"response\":[],\"type\":\"pushResponse\",\"version\":1}"));
}

void testAssignmentOperators() {
    try {
        ListRequest listRequest = (ListRequest) json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}");
        cout << "Attempting to assign invalid JSON to a ListRequest succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to assign invalid JSON to a ListRequest failed correctly" << endl;
        assert(true);
    }
    cout << "Assignment operator for ListRequest" << endl;
    ListRequest listRequest = (ListRequest) json("{\"request\":[],\"type\":\"listRequest\",\"version\":1}");

    try {
        PullRequest pullRequest = (PullRequest) json("{\"response\":[],\"type\":\"pullResponse\",\"version\":1}");
        cout << "Attempting to assign invalid JSON to a PullRequest succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to assign invalid JSON to a PullRequest failed correctly" << endl;
        assert(true);
    }
    cout << "Assignment operator for PullRequest" << endl;
    PullRequest pullRequest = (PullRequest) json("{\"request\":[],\"type\":\"pullRequest\",\"version\":1}");

    try {
        PushRequest pushRequest = (PushRequest) json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}");
        cout << "Attempting to assign invalid JSON to a PushRequest succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to assign invalid JSON to a PushRequest failed correctly" << endl;
        assert(true);
    }
    cout << "Assignment operator for PushRequest" << endl;
    PushRequest pushRequest = (PushRequest) json("{\"request\":[],\"type\":\"pushRequest\",\"version\":1}");

    try {
        ListResponse listResponse = (ListResponse) json("{\"purple\":[],\"type\":\"listRequest\",\"version\":1}");
        cout << "Attempting to assign invalid JSON to a ListResponse succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to assign invalid JSON to a ListResponse failed correctly" << endl;
        assert(true);
    }
    cout << "Assignment operator for ListResponse" << endl;
    ListResponse listResponse = (ListResponse) json("{\"response\":[],\"type\":\"listResponse\",\"version\":1}");

    try {
        PullResponse pullResponse = (PullResponse) json("{\"request\":[],\"type\":\"listRequest\",\"version\":1}");
        cout << "Attempting to assign invalid JSON to a PullResponse succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to assign invalid JSON to a PullResponse failed correctly" << endl;
        assert(true);
    }
    cout << "Assignment operator for PullResponse" << endl;
    PullResponse pullResponse = (PullResponse) json("{\"response\":[],\"type\":\"pullResponse\",\"version\":1}");

    try {
        PushResponse pushResponse = (PushResponse) json("{\"response\":[],\"type\":\"listRequest\",\"version\":1}");
        cout << "Attempting to assign invalid JSON to a PushResponse succeeded when it should have failed" << endl;
        assert(false);
    } catch (std::exception &e){
        cout << "Attempting to assign invalid JSON to a PushResponse failed correctly" << endl;
        assert(true);
    }
    cout << "Assignment operator for PushResponse" << endl;
    PushResponse pushResponse = (PushResponse) json("{\"response\":[],\"type\":\"pushResponse\",\"version\":1}");
}

void testCreateMessagesAndSubclasses() {
    testDefaultConstructors();
    testCopyConstructors();
    testAssignmentOperators();
}

void testMessages() {
    testCreateMessagesAndSubclasses();
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

/*
void testBuildDiffStructDuplicateBoth() {
  cout << "  Case where a file exists on both client and server" << endl;
  set<string> clientFilenameSet = set<string>{"c.txt", "e.txt"};
  map< string, set<string> > clientFiles;
  clientFiles["6b9df6f"] = set<string>{"c.txt"};
  clientFiles["b9866403"] = set<string>{"e.txt"};

  set<string> serverFilenameSet = set<string>{"a.txt", "c.txt"};
  map< string, set<string> > serverFiles;
  serverFiles["e8b7be43"] = set<string>{"a.txt"};
  serverFiles["6b9df6f"] = set<string>{"c.txt"};

  json target = json("{\"duplicateBothClientServer\":[{\"checksum\":\"6b9df6f\",\"clientFilenames\":[\"c.txt\"],\"serverFilenames\":[\"c.txt\"]}],\"uniqueOnlyClient\":[{\"checksum\":\"b9866403\",\"filename\":\"e.txt\"}],\"uniqueOnlyServer\":[{\"checksum\":\"e8b7be43\",\"filename\":\"a.txt\"}]}");
  json myResult = createDiffJSON(clientFiles, clientFilenameSet, serverFiles, serverFilenameSet);
  cout << "    Target output: " << target.stringify() << endl;
  cout << "    Actual output: " << myResult.stringify() << endl;
  assert(myResult == target);
}

void testBuildDiffStructConflict() {
  cout << "  Case where a file exists on both client and server ";
  cout << " and where different files on the client and server have the same name" << endl;
  set<string> clientFilenameSet = set<string>{"b.txt", "b (1).txt", "c.txt", "e.txt"};
  map< string, set<string> > clientFiles;
  clientFiles["6b9df6f"] = set<string>{"c.txt"};
  clientFiles["b9866403"] = set<string>{"e.txt"};
  clientFiles["abcdef"] = set<string>{"b.txt"};
  clientFiles["54321"] = set<string>{"b (1).txt"};

  set<string> serverFilenameSet = set<string>{"a.txt", "b.txt", "c.txt"};
  map< string, set<string> > serverFiles;
  serverFiles["e8b7be43"] = set<string>{"a.txt"};
  serverFiles["6b9df6f"] = set<string>{"c.txt"};
  serverFiles["12345"] = set<string>{"b.txt"};
  
  json target = json("{\"duplicateBothClientServer\":[{\"checksum\":\"6b9df6f\",\"clientFilenames\":[\"c.txt\"],\"serverFilenames\":[\"c.txt\"]}],\"uniqueOnlyClient\":[{\"checksum\":\"b9866403\",\"filename\":\"e.txt\"},{\"checksum\":\"abcdef\",\"filename\":\"b.txt\"},{\"checksum\":\"54321\",\"filename\":\"b (1).txt\"}],\"uniqueOnlyServer\":[{\"checksum\":\"e8b7be43\",\"filename\":\"a.txt\"},{\"checksum\":\"12345\",\"filename\":\"b.txt\"}],\"clientToServerConflicts\":[{\"clientFilename\":\"b.txt\",\"serverTargetFilename\":\"b (1).txt\"}],\"serverToClientConflicts\":[{\"serverFilename\":\"b.txt\",\"clientTargetFilename\":\"b (2).txt\"}]}");

  json myResult = createDiffJSON(clientFiles, clientFilenameSet, serverFiles, serverFilenameSet);
  cout << "    Target output: " << target.stringify() << endl;
  cout << "    Actual output: " << myResult.stringify() << endl;
  assert(myResult == target);
}

void testBuildDiffStruct() {
  cout << "Testing doDiff()" << endl;
  testBuildDiffStructDuplicateBoth();
  testBuildDiffStructConflict();
}

void testBuildPushPullComplicated() {
  cout << "  Case where there are multiple instances of each category of file" << endl;
  json src = json("{\"duplicateBothClientServer\":[{\"checksum\":\"6b9df6f\",\"clientFilenames\":[\"c.txt\"],\"serverFilenames\":[\"c.txt\"]}],\"uniqueOnlyClient\":[{\"checksum\":\"54321\",\"filename\":\"b (1).txt\"},{\"checksum\":\"abcdef\",\"filename\":\"b.txt\"},{\"checksum\":\"b9866403\",\"filename\":\"e.txt\"}],\"uniqueOnlyServer\":[{\"checksum\":\"12345\",\"filename\":\"b.txt\"},{\"checksum\":\"e8b7be43\",\"filename\":\"a.txt\"}],\"duplicateOnlyClient\":[{\"checksum\":\"122333\",\"filenames\":[\"d1\",\"d2\"]},{\"checksum\":\"1234\",\"filenames\":[\"d3\",\"d4\"]}],\"duplicateOnlyServer\":[{\"checksum\":\"333221\",\"filenames\":[\"d5\",\"d6\"]},{\"checksum\":\"4321\",\"filenames\":[\"d7\",\"d8\"]}]}");
  
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[{\"filename\":\"b.txt\",\"checksum\":\"12345\"},{\"filename\":\"a.txt\",\"checksum\":\"e8b7be43\"},{\"filename\":\"d5\",\"checksum\":\"333221\"},{\"filename\":\"d7\",\"checksum\":\"4321\"}]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult == pullTarget);
  
  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[{\"checksum\":\"54321\",\"filename\":\"b (1).txt\",\"data\":\"\"},{\"checksum\":\"abcdef\",\"filename\":\"b.txt\",\"data\":\"\"},{\"filename\":\"d1\",\"checksum\":\"122333\",\"data\":\"\"},{\"filename\":\"d3\",\"checksum\":\"1234\",\"data\":\"\"},{\"checksum\":\"b9866403\",\"filename\":\"e.txt\",\"data\":\"\"}]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult == pushTarget);
}

void testBuildPushPullOneFileServer() {
  cout << "  Case where there is only one file and it's on the server" << endl;
  json src = json("{\"uniqueOnlyServer\":[{\"checksum\":\"e8b7be43\",\"filename\":\"a.txt\"}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[{\"filename\":\"a.txt\",\"checksum\":\"e8b7be43\"}]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult == pullTarget);
  
  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult == pushTarget);
}

void testBuildPushPullOneFileClient() {
  cout << "  Case where there is only one file and it's on the client" << endl;
  json src = json("{\"uniqueOnlyClient\":[{\"checksum\":\"b9866403\",\"filename\":\"e.txt\"}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult == pullTarget);
  
  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[{\"filename\":\"e.txt\",\"checksum\":\"b9866403\",\"data\":\"\"}]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult == pushTarget);
}

void testBuildPushPullDuplicatesServer() {
  cout << "  Case where a server has two copies each of two files" << endl;
  json src = json("{\"duplicateOnlyServer\":[{\"checksum\":\"122333\",\"filenames\":[\"file10\",\"file11\"]},{\"checksum\":\"333221\",\"filenames\":[\"file12\",\"file13\"]}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[{\"filename\":\"file10\",\"checksum\":\"122333\"},{\"filename\":\"file12\",\"checksum\":\"333221\"}]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult == pullTarget);

  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult == pushTarget);
}

void testBuildPushPullDuplicatesClient() {
  cout << "  Case where a client has two copies of the same file" << endl;
  json src = json("{\"duplicateOnlyClient\":[{\"checksum\":\"1234\",\"filenames\":[\"file5\",\"file6\"]}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult == pullTarget);

  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[{\"filename\":\"file5\",\"checksum\":\"1234\",\"data\":\"\"}]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult == pushTarget);
}

void testBuildPushPull() {
  cout << "Testing createPullRequestFromDiffJSON() and createPushRequestFromDiffJSON()" << endl;
  testBuildPushPullOneFileServer();
  testBuildPushPullOneFileClient();
  testBuildPushPullDuplicatesClient();
  testBuildPushPullDuplicatesServer();
  testBuildPushPullComplicated();
}*/
