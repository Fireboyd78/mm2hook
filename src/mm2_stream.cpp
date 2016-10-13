#include "mm2_stream.h"
using namespace MM2;

MM2FnHook<void> Stream::$DumpOpenFiles  ( NULL, NULL, 0x4C9970 );

MM2FnHook<Stream *> Stream::$Open$1     ( NULL, NULL, 0x4C99C0 );
MM2FnHook<Stream *> Stream::$Create$1   ( NULL, NULL, 0x4C9A00 );

MM2FnHook<Stream *> Stream::$Open$2     ( NULL, NULL, 0x4C9A40);
MM2FnHook<Stream *> Stream::$Create$2   ( NULL, NULL, 0x4C9A70);

MM2FnHook<int> Stream::$Read            ( NULL, NULL, 0x4C9AA0 );
MM2FnHook<int> Stream::$Write           ( NULL, NULL, 0x4C9BF0 );
MM2FnHook<int> Stream::$GetCh           ( NULL, NULL, 0x4C9D00 );
MM2FnHook<int> Stream::$PutCh           ( NULL, NULL, 0x4C9D30 );
MM2FnHook<int> Stream::$Seek            ( NULL, NULL, 0x4C9D60 );
MM2FnHook<int> Stream::$Tell            ( NULL, NULL, 0x4C9DB0 );
MM2FnHook<int> Stream::$Close           ( NULL, NULL, 0x4C9DC0 );
MM2FnHook<int> Stream::$Size            ( NULL, NULL, 0x4C9E00 );
MM2FnHook<int> Stream::$Flush           ( NULL, NULL, 0x4C9E60 );

MM2FnHook<void> $fprintf                ( NULL, NULL, 0x4C9ED0 );
MM2FnHook<int> $fseek                   ( NULL, NULL, 0x4C9F20 );
MM2FnHook<int> $fgets                   ( NULL, NULL, 0x4C9F80 );
MM2FnHook<int> $fscanf                  ( NULL, NULL, 0x4C9FF0 );

AGE_API NAKED void fprintf(Stream *stream, char const *format, ...) {
    JMP_PTR($fprintf);
};

AGE_API NAKED int fscanf(Stream *stream, char const *format, ...) {
    JMP_PTR($fscanf);
};

AGE_API int fseek(Stream *stream, int position, seekWhence whence) {
    return $fseek(stream, position, whence);
};

AGE_API int fgets(char *buffer, int length, Stream *stream) {
    return $fgets(buffer, length, stream);
};