#pragma once
#include "common.h"
#include <string>

//
// Hook configuration
//

class line_reader {
    HANDLE m_handle = nullptr;
    DWORD m_offset = 0;
    
    int m_line = 0;
    int m_index = -1;
    
    bool m_eof = false;

    char m_buffer[4096] = { NULL };
protected:
    DWORD fill_buffer();

    void prepare_buffer() {
        if (m_index == -1)
            fill_buffer();
    }
public:
    explicit line_reader(HANDLE handle)
        : m_handle(handle) {
        // read first line
        fill_buffer();
    }

    int line() {
        return m_line;
    }

    bool eof() {
        return m_eof || (m_index == -1 && fill_buffer() == 0);
    }

    bool can_read() {
        return !m_eof && (m_index > -1) && (m_index < sizeof(m_buffer));
    }

    std::string read_line();
    std::string read_line(bool(*predicate)(std::string &));
};

class HookConfig {
public:
    static bool Initialize(const char *filename);

    // closes the config file
    static void Close();

    // reads from the config file (if open)
    static bool Read();

    static bool GetProperty(const char *key, char *value);
    static bool GetProperty(const char *key, bool &value);
    static bool GetProperty(const char *key, int &value);
    static bool GetProperty(const char *key, float &value);

    // is flag property present and enabled?
    // e.g. 'UseHackyOverride=1'
    static bool IsFlagEnabled(const char *key) {
        bool result = false;

        return GetProperty(key, result),
            result;
    }
};

