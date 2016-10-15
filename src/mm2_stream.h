#pragma once
#include "mm2_common.h"

namespace MM2
{
    // Forward declarations
    extern class Stream;

    typedef void (*EnumFilesCallback)(LPCSTR, bool, LPVOID);

    enum seekWhence {
        seek_begin,
        seek_current,
        seek_end,
    };

    /* most of the methods are optional */
    struct coreFileMethods {
        int (*open)(LPCSTR filename, bool readOnly);
        int (*create)(LPCSTR filename);
        int (*read)(int handle, LPVOID buffer, int length);
        int (*write)(int handle, const LPVOID buffer, int length);
        int (*seek)(int handle, int position, seekWhence whence);
        int (*close)(int handle);
        int (*enumFiles)(LPCSTR, EnumFilesCallback, LPVOID);
        int (*size)(int handle);
        int (*flush)(int handle); // usually set to null
    };

    AGE_API void fprintf(Stream *stream, char const *format, ...);
    AGE_API int fscanf(Stream *stream, char const *format, ...);
    AGE_API int fseek(Stream *stream, int position, seekWhence whence);
    AGE_API int fgets(char *buffer, int length, Stream *stream);

    class Stream {
    protected:
        static AGEHook<0x4C9970>::Func<void> $DumpOpenFiles;

        static AGEHook<0x4C99C0>::Func<Stream*> $Open$1;
        static AGEHook<0x4C9A00>::Func<Stream*> $Create$1;

        static AGEHook<0x4C9A40>::Func<Stream*> $Open$2;
        static AGEHook<0x4C9A70>::Func<Stream*> $Create$2;

        static AGEHook<0x4C9AA0>::Func<int> $Read;
        static AGEHook<0x4C9BF0>::Func<int> $Write;
        static AGEHook<0x4C9D00>::Func<int> $GetCh;
        static AGEHook<0x4C9D30>::Func<int> $PutCh;
        static AGEHook<0x4C9D60>::Func<int> $Seek;
        static AGEHook<0x4C9DB0>::Func<int> $Tell;
        static AGEHook<0x4C9DC0>::Func<int> $Close;
        static AGEHook<0x4C9E00>::Func<int> $Size;
        static AGEHook<0x4C9E60>::Func<int> $Flush;
    public:
        AGE_API static void DumpOpenFiles(void) {
            $DumpOpenFiles();
        };

        AGE_API static Stream* Open(LPCSTR filename, bool isZipFile) {
            return $Open$1(filename, isZipFile);
        };

        AGE_API static Stream* Open(LPCSTR filename, const coreFileMethods *fileMethods, bool isZipFile) {
            return $Open$2(filename, fileMethods, isZipFile);
        };

        AGE_API static Stream* Create(LPCSTR filename) {
            return $Create$1(filename);
        };

        AGE_API static Stream* Create(LPCSTR filename, const coreFileMethods *fileMethods) {
            return $Create$2(filename, fileMethods);
        };

        AGE_API int Read(LPVOID dstBuf, int size) {
            return $Read(this, dstBuf, size);
        };

        AGE_API int Write(const LPVOID srcBuf, int size) {
            return $Write(this, srcBuf, size);
        };

        AGE_API int GetCh(void) {
            return $GetCh(this);
        };

        AGE_API int PutCh(unsigned char ch) {
            return $PutCh(this, ch);
        };

        AGE_API int Seek(int offset) {
            return $Seek(this, offset);
        };

        AGE_API int Tell(void) {
            return $Tell(this);
        };

        AGE_API int Close(void) {
            return $Close(this);
        };

        AGE_API int Size(void) {
            return $Size(this);
        };

        AGE_API int Flush(void) {
            return $Flush(this);
        };
    };
}