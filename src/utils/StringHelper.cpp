//
// Created by madrus on 10.07.16.
//

#include "StringHelper.h"

#include "StringHelper.h"
#include <regex>
#include <Poco/StringTokenizer.h>
#include <Poco/UTF16Encoding.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/TextConverter.h>
#include <iomanip>

std::vector<std::string> StringHelper::tokenize(const std::string& s, const std::string& separators) {
    std::vector<std::string> result;
    Poco::StringTokenizer tokenizer(s, separators);
    for (const std::string& token : tokenizer) {
        result.push_back(token);
    }
    return result;
}

std::vector<std::string> StringHelper::tokenize(const std::string& s, char separator) {
    std::string separators(1, separator);
    return tokenize(s, separators);
}

bool StringHelper::matchRegexp(const std::string& s, const Poco::RegularExpression& pattern, std::vector<std::string>& matches) {
    Poco::RegularExpression::MatchVec matchResult;
    if(pattern.match(s, 0, matchResult)) {
        matches.clear();
        size_t matchesCount = matchResult.size();
        matches.reserve(matchesCount);
        for (size_t i = 0; i < matchesCount; ++i) {
            matches.push_back(s.substr(matchResult[i].offset, matchResult[i].length));
        }
        return true;
    }
    return false;
}

bool StringHelper::matchRegexp(const std::string& s, const Poco::RegularExpression& pattern) {
    Poco::RegularExpression::MatchVec matchResult;
    return pattern.match(s, 0, matchResult) == 0;
}

std::string StringHelper::utf8ToUtf16be(const std::string& utf8String) {
    std::string utf16String;
    Poco::UTF8Encoding utf8;
    Poco::UTF16Encoding utf16(Poco::UTF16Encoding::BIG_ENDIAN_BYTE_ORDER);
    Poco::TextConverter converter(utf8, utf16);
    converter.convert(utf8String, utf16String);
    return utf16String;
}

std::string StringHelper::objectToString(Poco::JSON::Object::Ptr object) {
    std::ostringstream os;
    object->stringify(os);
    return os.str();
}