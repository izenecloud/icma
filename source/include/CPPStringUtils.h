/* 
 * File:   CPPStringUtils.h
 * Author: Administrator
 *
 * Created on March 17, 2009, 5:39 PM
 */

#ifndef _CPPSTRINGUTILS_H
#define	_CPPSTRINGUTILS_H

#include <string>
#include <vector>
#include <string.h>
using namespace std;

/*
#define F_UTF8W(in) CPPStringUtils::from_utf8w(in)
#define T_UTF8(in) CPPStringUtils::to_utf8(in)
#define WS2S(in) CPPStringUtils::to_string(in)
#define S2WS(in) CPPStringUtils::to_wstring(in)

#define T_UTF8_VEC(in, ret) CPPStringUtils::separate_utf8_chars(in, ret)

extern const string UTF8_N;
*/
#define TOKEN_STR(in, ret) CPPStringUtils::token_string(in, ret)

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

        static void separate_utf8_chars(const string& source, vector<string>& ret);

        static std::string trim(std::string src);
        static std::wstring trim(std::wstring src);

        static std::string url_encode(std::string src);
        static std::wstring url_encode(std::wstring src);

        static std::string url_decode(std::string src);

        static std::string exchange_diacritic_chars_utf8(std::string src);
        static std::string tc2sc_utf8(std::string src);

        static std::wstring remove_html_content(std::wstring src, std::wstring startTag, std::wstring endTag);

        static void token_string(const string& s, vector<string>& words);

        static string encString(const char* toenc, const char* fromenc, string& in);

        /**
         * Change the encoding of a file from fromenc to toenc
         */
        static int encFile(const char* toEnc, const char* fromEnc,
                const char* inFile, const char* outFile);
};
#endif	/* _CPPSTRINGUTILS_H */

