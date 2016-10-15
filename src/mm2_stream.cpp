#include "mm2_stream.h"
using namespace MM2;

DECLARE_HOOK(Stream::$DumpOpenFiles);

DECLARE_HOOK(Stream::$Open$1);
DECLARE_HOOK(Stream::$Create$1);

DECLARE_HOOK(Stream::$Open$2);
DECLARE_HOOK(Stream::$Create$2);

DECLARE_HOOK(Stream::$Read);
DECLARE_HOOK(Stream::$Write);
DECLARE_HOOK(Stream::$GetCh);
DECLARE_HOOK(Stream::$PutCh);
DECLARE_HOOK(Stream::$Seek);
DECLARE_HOOK(Stream::$Tell);
DECLARE_HOOK(Stream::$Close);
DECLARE_HOOK(Stream::$Size);
DECLARE_HOOK(Stream::$Flush);

AGEHook<0x4C9ED0>::Func<void> $fprintf;
AGEHook<0x4C9F20>::Func<int> $fseek;
AGEHook<0x4C9F80>::Func<int> $fgets;
AGEHook<0x4C9FF0>::Func<int> $fscanf;

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