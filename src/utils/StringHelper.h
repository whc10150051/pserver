//
// Created by madrus on 10.07.16.
//

#ifndef PSERVER_STRINGHELPER_H
#define PSERVER_STRINGHELPER_H

#include <string>
#include <vector>
#include <Poco/RegularExpression.h>
#include <Poco/JSON/Object.h>

/**
 * Utilities to deal with strings
 */
class StringHelper {
public:
    static std::vector<std::string> tokenize(const std::string& s, const std::string& separators);
    static std::vector<std::string> tokenize(const std::string& s, char separator);

    static std::string utf8ToUtf16be(const std::string& utf8String);

    static bool matchRegexp(const std::string& s, const Poco::RegularExpression& pattern, std::vector<std::string>& matches);
    static bool matchRegexp(const std::string& s, const Poco::RegularExpression& pattern);
    static std::string objectToString(Poco::JSON::Object::Ptr);
};

#endif //PSERVER_STRINGHELPER_H
