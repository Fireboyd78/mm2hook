#pragma once
#include <modules\core.h>

namespace MM2
{
    // Forward declarations
    class Stream;

    // External declarations

    // Class definitions

    class Stream {
    public:
        static hook::Type<coreFileMethods *> sm_DefaultOpenMethods;
        static hook::Type<coreFileMethods *> sm_DefaultCreateMethods;

        AGE_API static void     DumpOpenFiles   (void)                              { return hook::StaticThunk<0x4C9970>::Call<void>(); };

        AGE_API static Stream*  Open            (LPCSTR filename, bool readOnly)    { return hook::StaticThunk<0x4C99C0>::Call<Stream *>(filename, readOnly); };
        AGE_API static Stream*  Open            (LPCSTR filename, const coreFileMethods *fileMethods, bool readOnly)
                                                                                    { return hook::StaticThunk<0x4C9A40>::Call<Stream *>(filename, fileMethods, readOnly); };

        AGE_API static Stream*  Create          (LPCSTR filename)                   { return hook::StaticThunk<0x4C9A00>::Call<Stream *>(filename); };
        AGE_API static Stream*  Create          (LPCSTR filename, const coreFileMethods *fileMethods)
                                                                                    { return hook::StaticThunk<0x4C9A70>::Call<Stream *>(filename, fileMethods); };

        AGE_API int             Read            (LPVOID dstBuf, int size)           { return hook::Thunk<0x4C9AA0>::Call<int>(this, dstBuf, size); };
        AGE_API int             Write           (const LPVOID srcBuf, int size)     { return hook::Thunk<0x4C9BF0>::Call<int>(this, srcBuf, size); };
        AGE_API int             GetCh           (void)                              { return hook::Thunk<0x4C9D00>::Call<int>(this); };
        AGE_API int             PutCh           (unsigned char ch)                  { return hook::Thunk<0x4C9D30>::Call<int>(this, ch); };
        AGE_API int             Seek            (int offset)                        { return hook::Thunk<0x4C9D60>::Call<int>(this, offset); };
        AGE_API int             Tell            (void)                              { return hook::Thunk<0x4C9DB0>::Call<int>(this); };
        AGE_API int             Close           (void)                              { return hook::Thunk<0x4C9DC0>::Call<int>(this); };
        AGE_API int             Size            (void)                              { return hook::Thunk<0x4C9E00>::Call<int>(this); };
        AGE_API int             Flush           (void)                              { return hook::Thunk<0x4C9E60>::Call<int>(this); };

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<Stream>("Stream")
                .addFactory([](LPCSTR filename, bool createFile = false) {
                    auto stream = (createFile) ? Stream::Create(filename) : Stream::Open(filename, false);
                    return stream;
                }, LUA_ARGS(LPCSTR, _opt<bool>))
                .addStaticFunction("DumpOpenFiles", &Stream::DumpOpenFiles)

                .addStaticFunction("Open", static_cast<Stream * (*)(LPCSTR, bool)>(&Stream::Open))
                .addStaticFunction("Create", static_cast<Stream * (*)(LPCSTR)>(&Stream::Create))

                .addFunction("Read", &Stream::Read)
                .addFunction("Write", &Stream::Write)
                .addFunction("GetCh", &Stream::GetCh)
                .addFunction("PutCh", &Stream::PutCh)
                .addFunction("Seek", &Stream::Seek)
                .addFunction("Tell", &Stream::Tell)
                .addFunction("Close", &Stream::Close)
                .addFunction("Size", &Stream::Size)
                .addFunction("Flush", &Stream::Flush)
            .endClass();
        }
    };

    AGE_EXT int     fseek       (Stream *stream, int position, seekWhence whence);
    AGE_EXT int     fgets       (char *buffer, int length, Stream *stream);
    AGE_EXT void    fprintf     (Stream *stream, char const *format, ...);
    AGE_EXT int     fscanf      (Stream *stream, char const *format, ...);

    // Lua initialization

}