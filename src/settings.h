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

    char m_buffer[8192] = { NULL };
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

    template <typename TType>
    static TType GetProperty(const char *key, TType defaultValue = TType()) {
        bool (*getter)(const char *, TType &) = GetProperty;

        TType result = defaultValue;

        return getter(key, result),
            result;
    }

    template <>
    static char * GetProperty<char *>(const char *key, char *defaultValue) {
        char *result = defaultValue;
        char buffer[MAX_PATH] { NULL };

        if (GetProperty(key, buffer)) {
            int len = strlen(buffer);

            // allocate a new string buffer
            result = new char[len] { NULL };
            strncpy_s(result, len, buffer, len);
        }

        return result;
    }

    // is flag property present and enabled?
    // e.g. 'UseHackyOverride=1'
    static bool IsFlagEnabled(const char *key) {
        bool result = false;

        return GetProperty(key, result),
            result;
    }
};

class ConfigProperty {
private:
    const char *name;
    const char *arg;

    inline bool CanCheckArg();
public:
    explicit ConfigProperty(const char *propName);
    explicit ConfigProperty(const char *propName, const char *argName);

    bool Get();
    bool Get(char *value);
    bool Get(bool &value);
    bool Get(int &value);
    bool Get(float &value);
};

template <typename TType>
class ConfigValue : protected ConfigProperty {
private:
    TType value = TType();
    bool parsed = false;

    void CheckValue() {
        // parse once
        if (!parsed) {
            ConfigProperty::Get(value);
            parsed = true;
        }
    }
public:
    explicit ConfigValue(const char *propName)
        : ConfigProperty(propName) { }
    explicit ConfigValue(const char *propName, const char *argName)
        : ConfigProperty(propName, argName) { }

    explicit ConfigValue(const char *propName, TType defaultValue)
        : ConfigProperty(propName), value(defaultValue) { }
    explicit ConfigValue(const char *propName, const char *argName, TType defaultValue)
        : ConfigProperty(propName, argName), value(defaultValue) { }

    inline bool Get(TType &value) {
        return ConfigProperty::Get(value);
    }

    inline TType & Get() {
        return CheckValue(),
            value;
    }

    inline operator ConfigValue<TType> &()          = delete;
    inline operator ConfigValue<TType> &() const    = delete;

    inline operator TType &() {
        return CheckValue(),
            value;
    }

    inline bool operator==(const TType &rhs) const  { return value == rhs; }
    inline bool operator!=(const TType &rhs) const  { return value != rhs; }
};

template <>
class ConfigValue<char *> : protected ConfigProperty {
    static_assert(true, "ConfigValue for strings not implemented yet");
};