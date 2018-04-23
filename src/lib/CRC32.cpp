#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CRC32.h"

CRC32::CRC32() {
    // 0x04C11DB7 is the official polynomial used by PKZip, WinZip and Ethernet.
    unsigned int iPolynomial = 0x04C11DB7;

    memset(&this->iTable, 0, sizeof(this->iTable));

    // 256 values representing ASCII character codes.
    for (int iCodes = 0; iCodes <= 0xFF; iCodes++) {
        this->iTable[iCodes] = this->Reflect(iCodes, 8) << 24;

        for (int iPos = 0; iPos < 8; iPos++) {
            this->iTable[iCodes] = (this->iTable[iCodes] << 1) ^ ((this->iTable[iCodes] & (1 << 31)) ? iPolynomial : 0);
        }

        this->iTable[iCodes] = this->Reflect(this->iTable[iCodes], 32);
    }
}

unsigned int CRC32::Reflect(unsigned int iReflect, const char cChar) {
    unsigned int iValue = 0;

    // Swap bit 0 for bit 7, bit 1 For bit 6, etc....
    for (int iPos = 1; iPos < (cChar + 1); iPos++) {
        if (iReflect & 1) {
            iValue |= (1 << (cChar - iPos));
        }
        iReflect >>= 1;
    }

    return iValue;
}


void CRC32::PartialCRC(unsigned int *iCRC, const unsigned char *sData, size_t iDataLength) {
    while (iDataLength-- > 0) {
        *iCRC = (*iCRC >> 8) ^ this->iTable[(*iCRC & 0xFF) ^ *sData++];
    }
}


bool CRC32::FileCRC(const char *sFileName, unsigned int *iOutCRC, size_t iBufferSize) {
    *iOutCRC = 0xffffffff; // Initialize the CRC.

    FILE *fSource = nullptr;
    unsigned char *sBuf = nullptr;
    size_t iBytesRead = 0;

    if ((fSource = fopen(sFileName, "rb")) == nullptr) {
        return false; // Failed to open file for read access
    }

    // Allocate memory for file buffering
    if (!(sBuf = (unsigned char *) malloc(iBufferSize))) {
        fclose(fSource);
        return false; // Out of memory.
    }

    while ((iBytesRead = fread(sBuf, sizeof(char), iBufferSize, fSource))) {
        this->PartialCRC(iOutCRC, sBuf, iBytesRead);
    }

    free(sBuf);
    fclose(fSource);
    *iOutCRC ^= 0xffffffff; // Finalize the CRC.

    return true;
}

unsigned int CRC32::FileCRC(const char *sFileName) {
    unsigned int iCRC;
    if (this->FileCRC(sFileName, &iCRC, 1048576)) {
        return iCRC;
    } else {
        return 0xffffffff;
    }
}
