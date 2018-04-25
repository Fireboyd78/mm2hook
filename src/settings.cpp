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
        if (SetFilePointer(m_handle, m_offset, NULL, FILE_BEGIN) != -1
            && ReadFile(m_handle, &m_buffer, sizeof(m_buffer), &length, NULL)) {
            if (length < sizeof(m_buffer))
                m_buffer[length] = NULL;

            // terminate after the last line-break (if needed)
            for (DWORD i = length; i > 0; i--) {
                // acceptable
                if (m_buffer[i] == '\0')
                    break;

                if (m_buffer[i - 1] == '\n') {
                    m_buffer[i] = '\0';

                    // adjust length
                    length = i;
                    break;
                }
            }

            m_offset += length;
            m_index = 0;
        } else {
            m_eof = true;
        }
    }

    return length;
}

std::string line_reader::read_line() {
    std::string result = "";

    // nothing to read
    if (m_index == -1 && fill_buffer() == 0)
        return result;

    int start = m_index;
    int end = -1;

    while (m_index < sizeof(m_buffer)) {
        int idx = m_index++;

        char c = m_buffer[idx];
        
        if (c == '\0' || (c == '\n')) {
            if (end == -1)
                end = idx;

            result = std::string(&m_buffer[start], (end - start));
            ++m_line;

            if (c == '\0')
                m_index = -1;

            break;
        } else if (c == '\r') {
            end = idx;
        }
    }

    return result;
}

std::string line_reader::read_line(bool (*predicate)(std::string &)) {
    std::string result = read_line();

    // no predicate == valid (cause there's nothing to check)
    bool valid = (predicate == nullptr);

    while (!valid) {
        if (predicate(result)) {
            valid = true;
        } else {
            result = read_line();

            if (m_index == -1)
                break;
        }
    }

    return result;
}

/*
    HookConfig
*/

static HANDLE handle;

static bool isOpen;
static bool isLoaded;

static std::map<std::string, std::string> properties;

bool skip_comments(std::string &line) {
    return (line[0] != '#' && line[0] != ';');
}

int skip_whitespace(std::string &line, int start, int end) {
    int result = 0;

    while (start < end) {
        char c = line[start];

        if (c != '\t' && c != ' ')
            break;

        ++result;
    }

    return result;
}

std::string read_token(std::string &line, int start, int end) {
    // trim trailing whitespace
    while (end > start) {
        char c = line[end - 1]; // check the previous character

        if (c != '\t' && c != ' ')
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
        std::string line = reader.read_line(skip_comments);

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

            //LogFile::Format("**** config section '%s' on line %d ****\n", section.c_str(), reader.line());

            // we don't do anything with sections yet,
            // so just skip them and move on
            continue;
        }

        std::string key = "";
        std::string val = "";

        size_t idx = line.find_first_of('=');

        if (idx == -1) {
            LogFile::Format("**** config line %d malformed (empty/invalid property) ****\n", reader.line());
            continue;
        }

        int start = skip_whitespace(line, 0, idx);

        key = read_token(line, start, idx);
        start = (idx + 1);

        for (idx = start; idx < line.length(); idx++) {
            char c = line[idx];

            // stop if we encounter an inline comment
            if (c == '#' || c == ';')
                break;
        }

        start += skip_whitespace(line, start, idx);

        val = read_token(line, start, idx);

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

bool HookConfig::GetProperty(const char *key, char *value) {
    if (!isLoaded)
        return false;

    auto prop = properties.find(key);

    if (prop != properties.end()) {
        if (value != nullptr)
            strcpy(value, prop->second.c_str());

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

    char buffer[512] { NULL };

    if (GetProperty(key, buffer)) {
        value = atoi(buffer);
        return true;
    }

    return false;
}

bool HookConfig::GetProperty(const char *key, float &value) {
    if (!isLoaded)
        return false;

    char buffer[512] { NULL };

    if (GetProperty(key, buffer)) {
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

bool ConfigProperty::Get() {
    if (arg != nullptr && datArgParser::Get(arg))
        return true;

    return HookConfig::IsFlagEnabled(name);
}

bool ConfigProperty::Get(char *value) {
    if (arg != nullptr && datArgParser::Get(arg, 0, reinterpret_cast<const char **>(value)))
        return true;

    return HookConfig::GetProperty(name, value);
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
    if (arg != nullptr && datArgParser::Get(name, 0, &value))
        return true;

    return HookConfig::GetProperty(name, value);
}

bool ConfigProperty::Get(float &value) {
    if (arg != nullptr && datArgParser::Get(name, 0, &value))
        return true;

    return HookConfig::GetProperty(name, value);
}
