//
// Created by Johny Appleseed on 3/6/18.
//

#include <algorithm>
#include <string>
#include <fstream>
#include "Utils.h"

bool Utils::Files::isFileExist(const std::string &filename){
    std::ifstream f(filename.c_str());
    return f.is_open();
}

bool Utils::Strings::hasOnlyDigits(std::string s) {
    return s.find_first_not_of("0123456789") == std::string::npos;
}

void Utils::Strings::ltrim(std::string &s){
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

void Utils::Strings::rtrim(std::string &s){
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}