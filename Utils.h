//
// Created by Johny Appleseed on 3/6/18.
//

#ifndef RIP_UTILS_H
#define RIP_UTILS_H

namespace Utils {
    class Files {
    public:
        static bool isFileExist(const std::string& filename);
    };
    class Strings {
    public:
        static bool hasOnlyDigits(std::string s);
        static void ltrim(std::string &s);
        static void rtrim(std::string &s);
    };
};


#endif //RIP_UTILS_H
