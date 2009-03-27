/* 
 * \author vernkin
 *
 */

#ifndef _ENCUTIL_H
#define	_ENCUTIL_H

#include <iconv.h>
#include <string>

using namespace std;

void encString(const char* toenc, const char* fromenc, string& in, string& out){
    iconv_t cd = iconv_open(toenc, fromenc);
    size_t bufSize = in.length() * 2;
    size_t origSize = in.length();
    char* data = (char*)in.data();
    char tmp[bufSize];
    char* c1 = tmp;

    iconv(cd, &data, &origSize, &c1, &bufSize);
    iconv_close(cd);
    out.append(tmp);
}

#endif	/* _ENCUTIL_H */

