/*
 * \file io_util.cpp
 * \brief 
 * \date Jun 8, 2010
 * \author Vernkin Chen
 */
// for getLastModifiedTime
//#include <sys/stat.h>

#include <fstream>

#include "icma/util/io_util.h"

namespace cma
{

bool IOUtil::isFileExist( const char* path )
{
    std::ifstream fileIn( path );
    if( fileIn == NULL )
        return false;
    fileIn.close();
    return true;
}

/**
 * @brief get the last modified time of specific file, represented in seconds
 * FIXME only support linux/unix now
 */
long IOUtil::getFileLastModifiedTime( const char* path )
{
    /*
    struct stat attrib;   // create a file attribute structure
    stat( path , &attrib );  // get the attributes of specific file
    return static_cast<long>( attrib.st_mtime );
    */
    return 0;
}


}
