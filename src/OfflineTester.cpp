#include "Project4Common.h"
#include "lib/base64.h"
#include <assert.h>     /* assert */

void testBase64Encoding();
void testBase64EncodingHappyString();

int main() {
  testBase64Encoding();
  return 0;
}

void testBase64Encoding() {
  // 1. Equivalent conversions for ASCII string
  testBase64EncodingHappyString();
  
  // 2. Equivalent conversions for binary file
  std::cout << "Testing conversion of \"" << "blob.binary" << "\":" << std::endl;
  
  std::ifstream input("testServerDir/blob.binary", std::ios::binary);
  // copies all data into buffer
  std::vector<char> buffer((std::istreambuf_iterator<char>(input)),
                           std::istreambuf_iterator<char>());
  std::string strBuffer(buffer.begin(), buffer.end());
  std::string target;
  Base64::Encode(strBuffer, &target);
  std::cout << "    Target Output: \"" << target << "\"" << std::endl;

  std::string myresult = base64Encode(buffer);
  std::cout << "    Actual Output: \"" << myresult << "\"" << std::endl;
  assert(myresult == target);
}

void testBase64EncodingHappyString() {
  std::string simpleString = "lalala happy string";
  std::cout << "Testing conversion of \"" << simpleString << "\":" << std::endl;

  std::string target;
  Base64::Encode(simpleString, &target);
  std::cout << "    Target Output: \"" << target << "\"" << std::endl;
  
  std::istringstream input(simpleString);
  std::vector<char> buffer((std::istreambuf_iterator<char>(input)),
                           std::istreambuf_iterator<char>());
  std::string myresult = base64Encode(buffer);
  std::cout << "    Actual Output: \"" << myresult << "\"" << std::endl;
  assert(myresult == target);
}
