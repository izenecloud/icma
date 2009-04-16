#include "CPPStringUtils.h"
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iconv.h>

const string UTF8_N = "utf8";

#define SIZE_BUFFER 512
#define SIZE_BUFFER_256 256

std::string CPPStringUtils::to_string(const std::wstring source)
{
	string dest = string();
	for (unsigned int i=0; i<source.length(); ++i)
		dest += (unsigned char) source[i];

	return dest;
}

std::wstring CPPStringUtils::to_wstring(const std::string source)
{
	wstring dest = wstring();
	for (unsigned int i=0; i<source.length(); ++i)
		dest += (unsigned char) source[i];

	return dest;
}

std::string CPPStringUtils::to_string(int source)
{
	string dest = string();
	bool negativ = (source<0);

	if ( source==0 )
		dest = '0';
	else
	{
		while ( source )
		{
			char digit = (source % 10) + '0';
			dest = digit + dest;
			source /= 10;
		}
	}

	if ( negativ )
		dest = '-'+ dest;

	return dest;
}

std::wstring CPPStringUtils::to_wstring(int source)
{
	wstring dest = wstring();
	bool negativ = (source<0);

	if ( source==0 )
		dest = L'0';
	else
	{
		while ( source )
		{
			wchar_t digit = (source % 10) + '0';
			dest = digit + dest;

			source /= 10;
		}
	}

	if ( negativ )
		dest = L'-' + dest;

	return dest;
}

std::string CPPStringUtils::to_utf8(const std::string source)
{
	// as the string itself only holds bytes it's not necessary to encode to more than two bytes
	string dest = string();
	int length = source.length();

	for(int i=0; i<length; i++)
	{
		unsigned char c = (unsigned char) source[i];
		if ( c<0x80 )
			dest += c;
		else {
			dest += (0xc0 | (c>>6));
			dest += (0x80 | (c&0x3f));
		}
	}

	return dest;
}

std::string CPPStringUtils::to_utf8(const std::wstring source)
{
	string dest = string();
	int length = source.length();

	for(int i=0; i<length; i++)
	{
		unsigned int c = (unsigned int) source[i];
		if ( c<0x00080 )
			dest += c;
		else if ( c<0x00800 )
		{
			dest += (0xc0 | (c>>6));
			dest += (0x80 | (c & 0x3f));
		}
		else if ( c<0x010000 )
		{
			dest += (0xe0 | (c>>12));
			dest += (0x80 | (c>>6 & 0x3f));
			dest += (0x80 | (c & 0x3f));
		}
		else {
			dest += (0xf0 | (c>>18));
			dest += (0x80 | (c>>12 & 0x3f));
			dest += (0x80 | (c>>6 & 0x3f));
			dest += (0x80 | (c & 0x3f));
		}
	}

	return dest;
}

std::string CPPStringUtils::from_utf8(const std::string source)
{
	string dest = string();
	int length = source.length();

	for(int i=0; i<length; i++)
	{
		unsigned char c1 = (unsigned char) source[i];
		if ( c1 < 0x80 ) {
			dest += c1;
		}
		else if ( (c1 & 0xe0)==0xc0 )
		{
			if ( i+1 < length )
			{
				i++;
				unsigned char c2 = (unsigned char) source[i];
				dest += ((c1<<6) | (c2 & 0x3f));
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf0)==0xe0 )
		{
			if ( i+2<length )
			{
				i += 2;
				dest += '#';
				dest += '#';
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf8)==0xf0 )
		{
			if ( i+3<length ) {
				i += 3;
				dest += '#';
				dest += '#';
				dest += '#';
			}
			else
				i = length-1;
		}
		else {
			// illegal coding, skip that char
			dest += '?';
		}
	}

	return dest;
}

std::wstring CPPStringUtils::from_utf8w(const std::string source)
{
	wstring dest = wstring();
	int length = source.length();

	for(int i=0; i<length; i++)
	{
		unsigned int c1 = (unsigned char) source[i];
		if ( c1<0x80 ) {
			dest += c1;
		}
		else if ( (c1 & 0xe0)==0xc0 )
		{
			if ( i+1 < length )
			{
				i++;
				unsigned int c2 = (unsigned char) source[i];

				dest += (((c1 & 0x1f)<<6) | (c2 & 0x3f));
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf0)==0xe0 )
		{
			if ( i+2<length )
			{
				i++;
				unsigned int c2 = (unsigned char) source[i];

				i++;
				unsigned int c3 = (unsigned char) source[i];

				dest += (((c1 & 0x0f)<<12) | ((c2 & 0x3f)<<6) | (c3 & 0x3f));
			}
			else
				i = length-1;
		}
		else if ( (c1 & 0xf8)==0xf0 )
		{
			if ( i+3<length )
			{
				i++;
				unsigned int c2 = (unsigned char) source[i];

				i++;
				unsigned int c3 = (unsigned char) source[i];

				i++;
				unsigned int c4 = (unsigned char) source[i];

				dest += (((c1 & 0x07)<<18) | ((c2 & 0x3f)<<12) | ((c3 & 0x3f)<<6) | (c4 & 0x3f));
			}
			else
				i = length-1;
		}
		else {
			// illegal coding, skip that char
			dest += '?';
		}
	}

	return dest;
}


void CPPStringUtils::separate_utf8_chars(const string& source,
        vector<string>& ret)
{
    int length = source.length();

    for(int i=0; i<length; i++){
        unsigned int c1 = (unsigned char) source[i];
        if ( c1<0x80 ) {
            ret.push_back(source.substr(i, 1));
        }
        else if ( (c1 & 0xe0)==0xc0 ){
            if ( i+1 < length ){
                ret.push_back(source.substr(i, 2));
                ++i;
            }
            else
                break;
        }
        else if ( (c1 & 0xf0)==0xe0 ){
            if ( i+2<length ){
                ret.push_back(source.substr(i, 3));
                i += 2;
            }
            else
                break;
        }
        else if ( (c1 & 0xf8)==0xf0 ){
            if ( i+3<length ){
                ret.push_back(source.substr(i, 4));
                i += 3;
            }
            else
                break;
        }
        else {
            // illegal coding, skip that char
            //ret.push_back("?");
        }
    }
}


std::string CPPStringUtils::trim(std::string src)
{
	if (src.empty() )
		return src;

	int start = 0;
	int length = src.length();
	while (start<length && src[start]<=0x20)
		start++;

	if ( start==length )
		return string();

	int end = length-1;
	while (end>0 && src[end]<=0x20 )
		end--;

	return src.substr(start, end-start+1);
}

std::wstring CPPStringUtils::trim(std::wstring src)
{
	if (src.empty() )
		return src;

	int start = 0;
	int length = src.length();
	while (start<length && src[start]<=0x20)
		start++;

	if ( start==length )
		return wstring();

	int end = length-1;
	while (end>0 && src[end]<=0x20 )
		end--;

	return src.substr(start, end-start+1);
}

std::string CPPStringUtils::url_encode(std::string src)
{
	if ( src.empty() )
		return src;

	string dst = string();
	int i=0;
	int length = src.length();
	while ( i<length )
	{
		unsigned char c = src[i];
		if ( c==' ' || c=='<' || c=='>' || c=='"' || c=='#' || c=='%' || c=='<' || c>0x7f )
		{
			dst += "%";

			unsigned char d = (c & 0xf0) >> 4;
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;

			d = (c & 0x0f);
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;
		}
		else
			dst += c;
		i++;
	}

	return dst;
}

std::wstring CPPStringUtils::url_encode(std::wstring src)
{
	if ( src.empty() )
		return src;

	wstring dst = wstring();
	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned int c = src[i];
		if ( c=='<' || c=='>' || c=='"' || c=='#' || c=='%' || c=='<' || c>0x7f )
		{
			dst += L"%";

			unsigned char d = (c & 0xf0) >> 4;
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;

			d = (c & 0x0f);
			if ( d<10 )
				dst += '0' + d;
			else
				dst += 'A' - 10 + d;
		}
		else
			dst += c;
		i++;
	}

	return dst;
}

std::string CPPStringUtils::url_decode(std::string src)
{
	if ( src.empty() )
		return src;

	string dst = string();

	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned int c = src[i++];
		if ( c=='%' && (i+1<length) )
		{
			int number = 0;

			unsigned char digit = (unsigned char) src[i++];
			digit = toupper(digit);
			if ( digit<='9' )
				digit -= 48;
			else
				digit -= 55;
			number = digit;

			digit = (unsigned char) src[i++];
			digit = toupper(digit);
			if ( digit<='9' )
				digit -= 48;
			else
				digit -= 55;

			number = number*16 + digit;

			dst += number;
		}
		else
			dst += c;
	}

	return dst;
}

const unsigned char diacriticExchangeTable[] =
{
	// this table contains the char code for any diacritic char
    // 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x5a, 0x00, // 0x80 - 0x8f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x7a, 0x59, // 0x90 - 0x9f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xa0 - 0xaf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xb0 - 0xbf
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, // 0xc0 - 0xcf
	0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0x00, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0x00, 0x00, // 0xd0 - 0xdf
	0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x00, 0x63, 0x65, 0x65, 0x65, 0x65, 0x69, 0x69, 0x69, 0x69, // 0xe0 - 0xef
	0x00, 0x6E, 0x6F, 0x6F, 0x6F, 0x6F, 0x6F, 0x00, 0x6f, 0x75, 0x75, 0x75, 0x75, 0x79, 0x00, 0x00, // 0xf0 - 0xff
};

std::string CPPStringUtils::exchange_diacritic_chars_utf8(std::string src)
{
	if ( src.empty() )
		return src;

	string dst = string();

	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned int c = src[i++];

		if ( (c&0xc0)==0xc0 )
		{
			int d = ((c&0x1f)<<6) + (((unsigned char) src[i]) & 0x3f);
			if ( d>=0x80 && d<=0xff )
			{
				unsigned char ex = diacriticExchangeTable[d-0x80];
				if ( ex )
				{
					dst += ex;
					i++;
					continue;
				}
			}

			dst += c;
			dst += src[i++];
		}
		else
			dst += c;
	}

	return dst;
}

#include "tc_sc.inc"

/*
 Converts characters in tradional chineses to simplified chineses. Because an index
 tables is used the convertion is rather fast.
 */
std::string CPPStringUtils::tc2sc_utf8(std::string src)
{
	if ( src.empty() )
		return src;

	string dst = string();

	int i = 0;
	int length = src.length();
	while ( i<length )
	{
		unsigned char c = src[i++];

		if ( ((c & 0xE0)==0xE0) && (c>=0xE4) && (c<=0xE9) && (i+1<length) ) // three byte encoding in utf-8 starts with 1110xxxx
		{
			bool ready = false;

			// only these are used int traditional chinese encoding
			unsigned char d = src[i];

			unsigned short idx = tc_secondCodePos[c-0xE4][d-0x80];
			if ( idx!=0xffff )
			{
				unsigned char e = src[i+1];
				unsigned char* second = (unsigned char*) &chars_tc[idx][1];

				while ( d==*second++ )
				{
					if ( *second<e )
						second += 4;
					else
					{
						if ( *second==e )
						{
							int pos = *(second+1) + *(second+2)*0x100;

							unsigned char* sc = (unsigned char*) chars_sc + (pos*5);

							dst += *sc++;
							dst += *sc++;
							dst += *sc++;
							i += 2;

							ready = true;
						}
						break;
					}
				}
			}

			if ( !ready )
			{
				dst += c;
				dst += src[i++];
				dst += src[i++];
			}
		}
		else
			dst += c;
	}

	return dst;
}

std::wstring CPPStringUtils::remove_html_content(std::wstring src, std::wstring startTag, std::wstring endTag)
{
	wstring dst;

	size_t start = 0;

	size_t pos = 0;
	while ( (pos=src.find(startTag, pos))!=string::npos )
	{
		dst += src.substr(start, pos-start);

		size_t end = src.find(endTag, pos);
		if ( end==string::npos )
			//unclosed commment
			break;

		start = end + 3;
		pos = start;
	}

	size_t length = src.length() - start;
	if ( length )
		dst += src.substr(start, length);

	return dst;
}

const boost::char_separator<char> CPUNIT_SEP(" ");

void CPPStringUtils::token_string(const string& s, vector<string>& words){
    typedef boost::tokenizer <boost::char_separator<char>,
        string::const_iterator, string> CPTokenizer;
    CPTokenizer token(s, CPUNIT_SEP);
    for(CPTokenizer::const_iterator itr = token.begin(); itr != token.end(); ++itr){
        words.push_back(*itr);
    }
}

string CPPStringUtils::encString(const char* toenc, const char* fromenc, string& in){
    iconv_t cd = iconv_open(toenc, fromenc);
    size_t bufSize = in.length() * 3;
    size_t origSize = in.length();
    char* data = (char*)in.data();
    char tmp[bufSize];
    char* c1 = tmp;

    iconv(cd, &data, &origSize, &c1, &bufSize);
    iconv_close(cd);
    return string(tmp);
}

int CodeConvert (const char* fcharset, const char* tcharset, char* inbuf, size_t inlen,
        char* outbuf, size_t outlen)
{

    iconv_t vIconv;
    char** vPin = &inbuf;
    char** vPout = &outbuf;

    if( fcharset == NULL || tcharset == NULL || inbuf == NULL
          || outbuf == NULL || strlen(inbuf) == 0 || inlen == 0 || outlen == 0)
    {
        return -1;
    }

    vIconv = iconv_open(tcharset, fcharset);
    if (vIconv == (iconv_t)(-1))
    {
        printf("iconv_open error!\n");
        return -2;
    }
    if (iconv(vIconv, vPin, &inlen, vPout, &outlen) == 0)
    {
        return -3;
    }

    iconv_close(vIconv);
    return 0;
}


int CPPStringUtils::encFile ( const char* toEnc, const char* fromEnc,
        const char* inFile, const char* outFile)
{

        FILE* vInFile = NULL;
        FILE* vOutFile = NULL;

        char vBuffer[SIZE_BUFFER + 1];
        char vInBuffer[SIZE_BUFFER + 1];
        char vOutBuffer[SIZE_BUFFER + 1];

        int inbuflen = 0;
        int i = 0;


        int infilelen = strlen(inFile);
        int outfilelen = strlen(outFile);

        memset(vBuffer, 0x00, sizeof(vBuffer));
        memset(vInBuffer, 0x00, sizeof(vInBuffer));
        memset(vOutBuffer, 0x00, sizeof(vOutBuffer));

        /* 判断参数 */
        if (
                fromEnc == NULL || toEnc == NULL
                || inFile == NULL || outFile == NULL
                || infilelen == 0 || infilelen > 255
                || outfilelen == 0 || outfilelen > 255
        ) {
                printf("parameter is error!\n");
                return -1;
        }

        assert((vInFile=fopen(inFile, "rb")));
        assert((vOutFile=fopen(outFile, "wb+")));

        /* 取文件长度 */
        fseek(vInFile, 0, SEEK_END);

        infilelen = ftell(vInFile);
        fseek(vInFile, 0, SEEK_SET);

        /* Read file (256Bytes) */
        while ((inbuflen=fread(vInBuffer, 1, SIZE_BUFFER_256, vInFile)) != 0)
        {
                i = inbuflen;
                while (1)
                {
                        if (i <= 0) break;
                        strncpy(vBuffer, vInBuffer, i);
                        if (CodeConvert(fromEnc, toEnc, vBuffer, i, vOutBuffer, SIZE_BUFFER) == 0)
                        {
                                fseek(vInFile, -(inbuflen-i), SEEK_CUR);
                                break;
                        }
                        i--;
                }
                if (i <= 0)
                {
                        printf("convert codeing failed!\n");
                        fclose(vInFile);
                        fclose(vOutFile);
                        return -1;
                }
                fwrite(vOutBuffer, 1, strlen(vOutBuffer), vOutFile);
                memset(vInBuffer, 0x00, sizeof(vInBuffer));
                memset(vOutBuffer, 0x00, sizeof(vOutBuffer));
        }

        fclose(vInFile);
        fclose(vOutFile);
        return 0;

}
