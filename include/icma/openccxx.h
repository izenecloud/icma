#ifndef _ICMA_OPENCCXX_H_
#define _ICMA_OPENCCXX_H_

#include "opencc_types.h"
#include <string>

namespace cma
{

class OpenCC
{
public:
    OpenCC(const std::string& home_dir);
    virtual ~OpenCC();

    int open(const char * config_file, const char* home_dir);

    long convert(const std::string &in, std::string &out, long length = -1);

    /**
    * Warning:
    * This method can be used only if wchar_t is encoded in UCS4 on your platform.
    */
    long convert(const std::wstring &in, std::wstring &out, long length = -1);

    opencc_error errno() const;
private:
    char* config_file;
    opencc_t od;
};

}


#endif /* _ICMA_OPENCCXX_H_ */
