#include "mm2_stream.h"
using namespace MM2;

int MM2::fseek  (Stream * stream, int position, seekWhence whence)  { return hook::StaticThunk<0x4C9F20>::Call<int>(stream, position, whence); }
int MM2::fgets  (char * buffer, int length, Stream * stream)        { return hook::StaticThunk<0x4C9F80>::Call<int>(buffer, length, stream); }

void MM2::fprintf(Stream * stream, char const * format, ...) {
    /* It's much easier to just rewrite the function since it's so simple */
    char buf[512];
    va_list va;
    va_start(va, format);
    _vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);

    stream->Write(buf, strlen(buf));
}

int MM2::fscanf(Stream * stream, char const * format, ...) {
    /*
    I think I need a shower after writing this...
    (Don't blame me, this is how Angel does it when calling sscanf!)
    */
    va_list va;
    va_start(va, format);
    int result = hook::StaticThunk<0x4C9FF0>::Call<int>(stream, format, *va, *(va + 1), *(va + 2), *(va + 3), *(va + 4), *(va + 5), *(va + 6), *(va + 7));
    va_end(va);

    return result;
}

/*
    Stream
*/

declfield(Stream::sm_DefaultOpenMethods)(0x5CED78);
declfield(Stream::sm_DefaultCreateMethods)(0x5CED7C);