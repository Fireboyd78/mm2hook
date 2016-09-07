#include "stream.h"

MM2FnHook<void>     lpStream_DumpOpenFiles                  ( NULL, NULL, 0x4C9970 );
MM2FnHook<Stream*>  lpStream_Open                           ( NULL, NULL, 0x4C99C0 );
MM2FnHook<Stream*>  lpStream_Create                         ( NULL, NULL, 0x4C9A00 );
MM2FnHook<int>      lpStream_Read                           ( NULL, NULL, 0x4C9AA0 );
MM2FnHook<int>      lpStream_Write                          ( NULL, NULL, 0x4C9BF0 );
MM2FnHook<int>      lpStream_GetCh                          ( NULL, NULL, 0x4C9D00 );
MM2FnHook<int>      lpStream_PutCh                          ( NULL, NULL, 0x4C9D30 );
MM2FnHook<int>      lpStream_Seek                           ( NULL, NULL, 0x4C9D60 );
MM2FnHook<int>      lpStream_Tell                           ( NULL, NULL, 0x4C9DB0 );
MM2FnHook<int>      lpStream_Close                          ( NULL, NULL, 0x4C9DC0 );
MM2FnHook<int>      lpStream_Size                           ( NULL, NULL, 0x4C9E00 );
MM2FnHook<int>      lpStream_Flush                          ( NULL, NULL, 0x4C9E60 );

void Stream::DumpOpenFiles(void) {
    lpStream_DumpOpenFiles();
}

Stream * Stream::Open(LPCSTR filename, bool isZipFile) {
    return lpStream_Open(filename, isZipFile);
}

Stream * Stream::Create(LPCSTR filename) {
    return lpStream_Create(filename);
}

int Stream::Read(THIS_ LPVOID dstBuf, int size) {
    return lpStream_Read(this, dstBuf, size);
}

int Stream::Write(THIS_ const LPVOID srcBuf, int size) {
    return lpStream_Write(this, srcBuf, size);
}

int Stream::GetCh(THIS_ void) {
    return lpStream_GetCh(this);
}

int Stream::PutCh(THIS_ unsigned char ch) {
    return lpStream_PutCh(this, ch);
}

int Stream::Seek(THIS_ int offset) {
    return lpStream_Seek(this, offset);
}

int Stream::Tell(THIS_ void) {
    return lpStream_Tell(this);
}

int Stream::Close(THIS_ void) {
    return lpStream_Close(this);
}

int Stream::Size(THIS_ void) {
    return lpStream_Size(this);
}

int Stream::Flush(THIS_ void) {
    return lpStream_Flush(this);
}