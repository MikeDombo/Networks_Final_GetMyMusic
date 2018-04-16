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
void testBase64EncodingHappyString();
void testFilenameIncrement();
void testBuildDiffStruct();
void testBuildPushPull();

json buildDiffStruct(const std::map< std::string, std::set<std::string> > &clientMap, const std::set<std::string> &clientFilenameSet, const std::map< std::string, std::set< std::string> > &serverMap, const std::set<std::string> &serverFilenameSet);  // defined in Project4Client.cpp. Requires that the main() method therein be commented out
json buildPullRequestFromDiffStruct(json diffStruct);
json buildPushRequestFromDiffStruct(json diffStruct);

int main() {
  //testBase64Encoding();
  //testBase64EncodingHappyString();
  //testFilenameIncrement();
  //testBuildDiffStruct();
  testBuildPushPull();
  return 0;
}

void testBase64Encoding() {
  // 1. Equivalent conversions for ASCII string
  testBase64EncodingHappyString();
  
  // 2. Equivalent conversions for binary file
  cout << "Testing conversion of \"" << "blob.binary" << "\":" << endl;
  
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
  cout << "Testing conversion of \"" << simpleString << "\":" << endl;

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
  json myresult = buildDiffStruct(clientFiles, clientFilenameSet, serverFiles, serverFilenameSet);
  cout << "    Target output: " << target.stringify() << endl;
  cout << "    Actual output: " << myresult.stringify() << endl;
  assert(myresult.stringify() == target.stringify());
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

  json myresult = buildDiffStruct(clientFiles, clientFilenameSet, serverFiles, serverFilenameSet);
  cout << "    Target output: " << target.stringify() << endl;
  cout << "    Actual output: " << myresult.stringify() << endl;
  assert(myresult.stringify() == target.stringify());
}

void testBuildDiffStruct() {
  cout << "Testing buildDiffStruct()" << endl;
  testBuildDiffStructDuplicateBoth();
  testBuildDiffStructConflict();
}

void testBuildPushPullComplicated() {
  json src = json("{\"duplicateBothClientServer\":[{\"checksum\":\"6b9df6f\",\"clientFilenames\":[\"c.txt\"],\"serverFilenames\":[\"c.txt\"]}],\"uniqueOnlyClient\":[{\"checksum\":\"b9866403\",\"filename\":\"e.txt\"},{\"checksum\":\"abcdef\",\"filename\":\"b.txt\"},{\"checksum\":\"54321\",\"filename\":\"b (1).txt\"}],\"uniqueOnlyServer\":[{\"checksum\":\"e8b7be43\",\"filename\":\"a.txt\"},{\"checksum\":\"12345\",\"filename\":\"b.txt\"}],\"clientToServerConflicts\":[{\"clientFilename\":\"b.txt\",\"serverTargetFilename\":\"b (1).txt\"}],\"serverToClientConflicts\":[{\"serverFilename\":\"b.txt\",\"clientTargetFilename\":\"b (2).txt\"}]}");
  assert(false);
}

void testBuildPushPullOneFileServer() {
  cout << "  Case where there is only one file and it's on the server" << endl;
  json src = json("{\"uniqueOnlyServer\":[{\"checksum\":\"e8b7be43\",\"filename\":\"a.txt\"}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[{\"filename\":\"a.txt\",\"checksum\":\"e8b7be43\"}]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult.stringify() == pullTarget.stringify());
  
  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult.stringify() == pushTarget.stringify());
}

void testBuildPushPullOneFileClient() {
  cout << "  Case where there is only one file and it's on the client" << endl;
  json src = json("{\"uniqueOnlyClient\":[{\"checksum\":\"b9866403\",\"filename\":\"e.txt\"}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult.stringify() == pullTarget.stringify());
  
  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[{\"filename\":\"e.txt\",\"checksum\":\"b9866403\",\"data\":\"\"}]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult.stringify() == pushTarget.stringify());
}

void testBuildPushPullDuplicatesClient() {
  cout << "  Case where a client has two copies of the same file" << endl;
  json src = json("{\"duplicateOnlyClient\":[{\"checksum\":\"1234\",\"filenames\":[\"file5\",\"file6\"]}]}");
  json pullTarget = json("{\"version\":1,\"type\":\"pullRequest\",\"request\":[]}");
  json pullResult = buildPullRequestFromDiffStruct(src);
  cout << "    Target output (pull): " << pullTarget.stringify() << endl;
  cout << "    Actual output (pull): " << pullResult.stringify() << endl;
  assert(pullResult.stringify() == pullTarget.stringify());

  json pushTarget = json("{\"version\":1,\"type\":\"pushRequest\",\"request\":[{\"filename\":\"file5\",\"checksum\":\"1234\",\"data\":\"\"}]}");
  json pushResult = buildPushRequestFromDiffStruct(src);
  cout << "    Target output (push): " << pushTarget.stringify() << endl;
  cout << "    Actual output (push): " << pushResult.stringify() << endl;
  assert(pushResult.stringify() == pushTarget.stringify());
}

void testBuildPushPull() {
  cout << "Testing buildPullRequestFromDiffStruct() and buildPushRequestFromDiffStruct()" << endl;
  testBuildPushPullOneFileServer();
  testBuildPushPullOneFileClient();
  testBuildPushPullDuplicatesClient();
  testBuildPushPullComplicated();
}
  
