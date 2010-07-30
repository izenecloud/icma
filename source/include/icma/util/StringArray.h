/**
 * \file StringArray.h
 * \brief 
 * \date Jul 30, 2010
 * \author Vernkin Chen
 */

#ifndef STRINGARRAY_H_
#define STRINGARRAY_H_

#include <string>
#include <vector>

namespace cma
{


class StringArray
{
public:
    typedef size_t offset_t;

    static void tokenize(
            const char* str,
            StringArray& ret,
            const std::string& delimeter = " "
            );

    StringArray( const char* str = NULL );

    ~StringArray();

    void setString( const char* str );

    size_t size();

    inline char* operator[]( int idx )
    {
        return data_ + offsetVec_[ idx ];
    }

    void reserve( size_t size );

    void append( const char* str, size_t len = 0 );

    void initialize();

private:
    void ensureFreeLength( size_t extraLen )
    {
        // reserve one zero byte
        ++extraLen;
        size_t freeLen = dataLen_ - ( endPtr_ - data_ );
        if( freeLen > extraLen )
            return;
        size_t minNewLen = dataLen_ + extraLen - freeLen;
        size_t newLen = dataLen_;
        while( newLen < minNewLen )
            newLen = (size_t)( newLen * 1.5 );
        reserve( newLen );
    }

private:
    char* data_;
    char* endPtr_;
    size_t dataLen_;
    std::vector<offset_t> offsetVec_;
};

}

#endif /* STRINGARRAY_H_ */