#ifndef CRC32_H
#define CRC32_H

class CRC32 {

public:
    CRC32();

    bool FileCRC(const char *sFileName, unsigned int *iOutCRC, size_t iBufferSize);

    unsigned int FileCRC(const char *sFileName);

    void PartialCRC(unsigned int *iCRC, const unsigned char *sData, size_t iDataLength);

private:
    unsigned int Reflect(unsigned int iReflect, const char cChar);

    unsigned int iTable[256]; // CRC lookup table array.
};

#endif
