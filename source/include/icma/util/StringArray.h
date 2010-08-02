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
#include <queue>
#include <icma/util/GenericArray.h>

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

    StringArray( const StringArray& other );

    ~StringArray();

    void setString( const char* str );

    inline size_t size()
    {
        return offsetVec_.size();
    }

    inline char* operator[]( int idx )
    {
        return data_ + offsetVec_[ idx ];
    }

    void reserve( size_t size );

    void push_back( const char* str, size_t len = 0 );

    void initialize();

    bool contains( const char* str );

    inline bool empty()
    {
        return offsetVec_.empty();
    }

    void removeHead();

    void swap( StringArray& other );

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
    GenericArray<offset_t> offsetVec_;
};

}

#endif /* STRINGARRAY_H_ */
