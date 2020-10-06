#include "settings.h"

// datArgParser
#include "mm2_data.h"

#include <map>
#include <unordered_map>

using namespace MM2;

/*
    line_reader
*/

DWORD line_reader::fill_buffer() {
    DWORD length = 0;

    m_index = -1;

    if (!m_eof) {
        if (ReadFile(m_handle, &m_buffer, sizeof(m_buffer) - 1, &length, NULL) && (length > 0)) {
            m_buffer[length] = '\0';
            m_offset += length;
            m_index = 0;
        } else {
            m_eof = true;
        }
    }

    return length;
}

std::string line_reader::read_line() {
    std::string result;
    bool done = false;

    while (!done) {
        if (m_buffer[m_index] == '\0')
            fill_buffer();

        if (m_eof) {
            ++m_line;
            break;
        }

        const char* data = &m_buffer[m_index];
        int length = std::strcspn(data, "\n");
        m_index += length;

        bool done = false;

        if (data[length] == '\n') {
            ++m_index;
            ++m_line;

            if (length > 0 && data[length - 1] == '\r')
                --length;

            done = true;
        }

        result.append(data, length);
    }

    return result;
}

std::string line_reader::read_line(bool (*predicate)(std::string &)) {
    while (!eof()) {
        std::string result = read_line();

        if (predicate(result))
            return result;
    }

    return "";
}

/*
    HookConfig
*/

static HANDLE handle;

static bool isOpen;
static bool isLoaded;

static std::map<std::string, std::string> properties;

inline bool is_comment(char c) {
    return (c == '#' || c == ';');
}

inline bool is_whitespace(char c) {
    return (c == '\t' || c == ' ');
}

bool skip_comments(std::string &line) {
    return !is_comment(line[0]);
}

size_t skip_whitespace(std::string &line, size_t start = 0, size_t end = 0) {
    if (end == 0)
        end = line.length();

    for (size_t idx = start; idx < end; idx++) {
        char c = line[idx];

        if (!is_whitespace(c))
            return idx;
    }

    // no whitespace
    return 0;
}

std::string read_line(line_reader &reader) {
    std::string line = reader.read_line(skip_comments);

    // trim leading whitespace
    size_t trim = skip_whitespace(line);

    if (trim > 0)
        line = line.substr(trim);

    size_t length = line.length();

    // strip out inline comments
    for (size_t idx = 0; idx < length; idx++) {
        char c = line[idx];

        // stop at the first inline comment
        if (is_comment(c)) {
            line = line.substr(0, idx);
            break;
        }
    }

    return line;
}

std::string read_token(std::string &line, int start, int end) {
    // trim trailing whitespace
    while (end > start) {
        char c = line[end - 1]; // check the previous character

        if (!is_whitespace(c))
            break;

        --end;
    }

    return std::string(line, start, (end - start));
}

bool HookConfig::Read() {
    // don't do anything if it's closed
    if (!isOpen)
        return false;

    line_reader reader(handle);

    // reset properties
    properties.clear();

    while (!reader.eof()) {
        // read line (minus leading whitespace or inline comments)
        std::string line = read_line(reader);

        // skip empty lines
        if (line.empty())
            continue;

        // parse sections
        if (line[0] == '[') {
            size_t sIdx = 1;
            size_t sEnd = line.find_first_of(']');

            if (sEnd == -1) {
                LogFile::Format("**** config line %d malformed (section isn't closed) ****\n", reader.line());
                continue;
            }

            std::string section(line, sIdx, (sEnd - sIdx));

            LogFile::Format("**** config section '%s' on line %d ****\n", section.c_str(), reader.line());

            // we don't do anything with sections yet,
            // so just skip them and move on
            continue;
        }

        size_t end = line.length();

        std::string key = "";
        std::string val = "";

        // property name
        size_t pIdx = line.find_first_of('=');

        // -1 = not found, 0 = no property name
        if (pIdx < 1) {
            LogFile::Format("**** config line %d malformed (empty/invalid property) ****\n", reader.line());
            continue;
        }

        key = read_token(line, 0, pIdx);

        // property value (trim leading whitespace as needed)
        size_t vIdx = skip_whitespace(line, (pIdx + 1),  end);

        val = read_token(line, vIdx, end);

        // add key/value pair
        properties.insert(std::make_pair(key, val));
    }

    return true;
}

bool HookConfig::Initialize(const char *filename) {
    if (isOpen) {
        LogFile::WriteLine("**** attempted to initialize configuration twice! ****");
        return false;
    }

    LogFile::WriteLine("Opening configuration file...");
    handle = CreateFileA(filename,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    isOpen = (handle != INVALID_HANDLE_VALUE);

    if (isOpen) {
        // read in settings
        LogFile::WriteLine("Loading configuration file...");
        Read();
        isLoaded = true;
    }

    return isOpen;
}

void HookConfig::Close() {
    if (isOpen) {
        CloseHandle(handle);

        handle = nullptr;
        isOpen = false;
    }
}

bool HookConfig::GetProperty(const char *key, char *value, size_t buf_len) {
    if (!isLoaded)
        return false;

    auto prop = properties.find(key);

    if (prop != properties.end()) {
        if (value != nullptr)
            strcpy_s(value, buf_len, prop->second.c_str());

        return true;
    }

    return false;
}

bool HookConfig::GetProperty(const char *key, bool &value) {
    if (!isLoaded)
        return false;

    int result = 0;

    if (GetProperty(key, result)) {
        value = (result > 0);
        return true;
    }

    return false;
}

bool HookConfig::GetProperty(const char *key, int &value) {
    if (!isLoaded)
        return false;

    char buffer[128] { NULL };

    if (GetProperty(key, buffer, sizeof(buffer))) {
        value = atoi(buffer);
        return true;
    }

    return false;
}

bool HookConfig::GetProperty(const char *key, float &value) {
    if (!isLoaded)
        return false;

    char buffer[128] { NULL };

    if (GetProperty(key, buffer, sizeof(buffer))) {
        value = (float)atof(buffer);
        return true;
    }

    return false;
}

/*
    ConfigProperty
*/

ConfigProperty::ConfigProperty(const char * propName)
    : name(propName), arg(nullptr) {}

ConfigProperty::ConfigProperty(const char * propName, const char * argName)
    : name(propName), arg(argName) {}

bool ConfigProperty::CanCheckArg() {
    return ((arg != nullptr) && (datArgParser::Argc > 0));
}

bool ConfigProperty::Get() {
    if (CanCheckArg() && datArgParser::Get(arg))
        return true;

    return HookConfig::IsFlagEnabled(name);
}

bool ConfigProperty::Get(char *value, size_t buf_len) {
    if (CanCheckArg()) {
        const char* arg_value = nullptr;

        if (datArgParser::Get(arg, 0, &arg_value)) {
            strcpy_s(value, buf_len, arg_value);
            return true;
        }
    }

    return HookConfig::GetProperty(name, value, buf_len);
}

bool ConfigProperty::Get(bool &value) {
    int result = 0;

    if (Get(result)) {
        value = (result != 0);
        return true;
    }

    return false;
}

bool ConfigProperty::Get(int &value) {
    if (CanCheckArg() && datArgParser::Get(name, 0, &value))
        return true;

    return HookConfig::GetProperty(name, value);
}

bool ConfigProperty::Get(float &value) {
    if (CanCheckArg() && datArgParser::Get(name, 0, &value))
        return true;

    return HookConfig::GetProperty(name, value);
}
