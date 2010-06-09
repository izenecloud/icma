/*
 * \file io_util.h
 * \brief 
 * \date Jun 8, 2010
 * \author Vernkin Chen
 */

#ifndef IO_UTIL_H_
#define IO_UTIL_H_

namespace cma
{

class IOUtil
{
public:
    static bool isFileExist( const char* path );

    static long getFileLastModifiedTime( const char* path );
};

}

#endif /* IO_UTIL_H_ */
