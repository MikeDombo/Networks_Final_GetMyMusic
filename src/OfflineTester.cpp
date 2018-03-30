#include "Project4Common.h"
#include "lib/base64.h"
#include <assert.h>     /* assert */
#include <iomanip> /* for formatted printing to cout */
void testBase64Decoding();
void printReverseMap();

void testBase64Encoding();
void testBase64EncodingHappyString();
void testBase64EncodingBinaryFile();

int main() {
  testBase64Decoding();
  //testBase64Encoding();
  return 0;
}

void testBase64Decoding() {
  printReverseMap();
  std::string simpleStringTarget = "lalala happy string";
  std::string decodingSrc;
  Base64::Encode(simpleStringTarget, &decodingSrc);

  std::vector<char> myResult = base64Decode(decodingSrc);
  std::string resultString(myResult.begin(), myResult.end());
  std::cout << resultString << std::endl;
  assert(resultString == simpleStringTarget);
}

void printReverseMap() {
  uint8_t reverseMap[256];
  memset(reverseMap, 0, 256);
  assert(BASE64_CHARS.size() == 64);
  std::cout << "---------------------------------------------------------------" << std::endl;
  for (uint8_t i=0; i < 64; ++i) {
    reverseMap[(uint8_t) BASE64_CHARS[i]] = i;
  }
  for (uint16_t i=0; i < 256; ++i) {
    std::cout << std::setw(2) << std::dec << (int) reverseMap[i] << ", ";
    if ((i+1) % 16 == 0) {
      std::cout << std::endl;
    }
  }

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "  These should look like the Base64 subset of the ASCII table  " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;
  
  for (uint16_t i=0; i < 256; ++i) {
    char printout;
    if (reverseMap[i] > 0) {
      printout = (char) i;
    } else {
      printout = '0';
    }
    std::cout << std::setw(2) << printout << ", ";
    if ((i+1) % 16 == 0) {
      std::cout << std::endl;
    }
  }
  std::cout << "---------------------------------------------------------------" << std::endl;
}
    

void testBase64Encoding() {
  // 1. Equivalent conversions for ASCII string
  testBase64EncodingHappyString();
  
  // 2. Equivalent conversions for binary file
  testBase64EncodingBinaryFile();
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

void testBase64EncodingBinaryFile() {
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
