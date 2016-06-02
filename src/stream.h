#pragma once
#include "mm2.h"

class Stream {
public:
    static void DumpOpenFiles(void);

    static Stream* Open(LPCSTR filename, bool isZipFile);
    static Stream* Create(LPCSTR filename);

    int Read(THIS_ LPVOID dstBuf, int size);
    int Write(THIS_ const LPVOID srcBuf, int size);
    int GetCh(THIS_ void);
    int PutCh(THIS_ unsigned char ch);
    int Seek(THIS_ int offset);
    int Tell(THIS_ void);
    int Close(THIS_ void);
    int Size(THIS_ void);
    int Flush(THIS_ void);
};