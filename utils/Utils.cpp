#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include "Utils.h"

// Return true if file can be open for reading.
bool Utils::Files::isFileCanOpen(const std::string &filename){
    std::ifstream f(filename.c_str());
    return f.is_open();
}

// Return true if there are only digits in the string and there is at least one character, false otherwise.
bool Utils::Strings::hasOnlyDigits(std::string s) {
    return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}

// Trims leading whitespace
void Utils::Strings::ltrim(std::string &s){
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// Trims trailing whitespace
void Utils::Strings::rtrim(std::string &s){
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trims leading and trailing whitespace
void Utils::Strings::trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// Return a vector of words in the string separated by the delimiter.
// Leading and trailing white space is trimmed.
std::vector<std::string> Utils::Strings::split(std::string &s, char delimiter) {
    std::vector<std::string> strings;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        trim(token);
        strings.push_back(token);
        s.erase(0, pos + 1);
    }
    trim(s);
    strings.push_back(s);
    return strings;
}

