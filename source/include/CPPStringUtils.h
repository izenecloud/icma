/* 
 * File:   CPPStringUtils.h
 * Author: Administrator
 *
 * Created on March 17, 2009, 5:39 PM
 */

#ifndef _CPPSTRINGUTILS_H
#define	_CPPSTRINGUTILS_H

using namespace std;

#include <string>

class CPPStringUtils {
public:
        static std::string to_string(const std::wstring source);
        static std::wstring to_wstring(const std::string source);

        static std::string to_string(int source);
        static std::wstring to_wstring(int source);

        static std::string to_utf8(const std::string source);
        static std::string to_utf8(const std::wstring source);
        static std::string from_utf8(const std::string source);
        static std::wstring from_utf8w(const std::string source);

        static std::string to_lower(std::string src);
        static std::wstring to_lower(std::wstring src);
        static std::string to_lower_utf8(std::string utf8_src);

        static std::string trim(std::string src);
        static std::wstring trim(std::wstring src);

        static std::string url_encode(std::string src);
        static std::wstring url_encode(std::wstring src);

        static std::string url_decode(std::string src);

        static std::string exchange_diacritic_chars_utf8(std::string src);
        static std::string tc2sc_utf8(std::string src);

        static std::wstring remove_html_content(std::wstring src, std::wstring startTag, std::wstring endTag);
};
#endif	/* _CPPSTRINGUTILS_H */

