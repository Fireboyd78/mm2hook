#pragma once
#include "mm2.h"

namespace MM2
{
    class Stream {
    protected:
        static MM2FnHook<void> $DumpOpenFiles;

        static MM2FnHook<Stream*> $Open;
        static MM2FnHook<Stream*> $Create;

        static MM2FnHook<int> $Read;
        static MM2FnHook<int> $Write;
        static MM2FnHook<int> $GetCh;
        static MM2FnHook<int> $PutCh;
        static MM2FnHook<int> $Seek;
        static MM2FnHook<int> $Tell;
        static MM2FnHook<int> $Close;
        static MM2FnHook<int> $Size;
        static MM2FnHook<int> $Flush;
    public:
        static void DumpOpenFiles(void) {
            $DumpOpenFiles();
        };

        static Stream* Open(LPCSTR filename, bool isZipFile) {
            return $Open(filename, isZipFile);
        };

        static Stream* Create(LPCSTR filename) {
            return $Create(filename);
        };

        int Read(LPVOID dstBuf, int size) {
            return $Read(this, dstBuf, size);
        };

        int Write(const LPVOID srcBuf, int size) {
            return $Write(this, srcBuf, size);
        };

        int GetCh(void) {
            return $GetCh(this);
        };

        int PutCh(unsigned char ch) {
            return $PutCh(this, ch);
        };

        int Seek(int offset) {
            return $Seek(this, offset);
        };

        int Tell(void) {
            return $Tell(this);
        };

        int Close(void) {
            return $Close(this);
        };

        int Size(void) {
            return $Size(this);
        };

        int Flush(void) {
            return $Flush(this);
        };
    };
}