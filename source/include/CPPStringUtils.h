/* 
 * File:   CPPStringUtils.h
 * Author: Administrator
 *
 * Created on March 17, 2009, 5:39 PM
 */

#ifndef _CPPSTRINGUTILS_H
#define	_CPPSTRINGUTILS_H

#include <string>
using namespace std;


#define F_UTF8W(in) CPPStringUtils::from_utf8w(in)
#define T_UTF8(in) CPPStringUtils::to_utf8(in)
#define WS2S(in) CPPStringUtils::to_string(in)
#define S2WS(in) CPPStringUtils::to_wstring(in)

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

