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

int main() {
  testFilenameIncrement();
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
  string targetFname = filenameIncrement("file0", existingFilenames);
  assert(targetFname == "file0");

  cout << "  Check if a filename present without an extension gets a (1) afterward" << endl;
  targetFname = filenameIncrement("file", existingFilenames);
  assert(targetFname == "file (1)");
 
  cout << "  Check if a filename present with an extension gets a (1) before the ext." << endl;
  targetFname = filenameIncrement("file1.ext", existingFilenames);
  assert(targetFname == "file1 (1).ext");

  cout << "  Check if a filename present with n increments gets an (n+1) before the ext." << endl;
  targetFname = filenameIncrement("file2.ext", existingFilenames);
  assert(targetFname == "file2 (3).ext");
}
