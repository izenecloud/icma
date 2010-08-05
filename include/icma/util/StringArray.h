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
#include <VGenericArray.h>
#include <iostream>

namespace cma
{
extern std::string DefDelimeter;
extern std::string DefPrintDelimeter;

class StringArray
{
public:
    typedef size_t offset_t;

    static void tokenize(
            const char* str,
            StringArray& ret,
            const std::string& delimeter = DefDelimeter
            );

    StringArray( const char* str = NULL );

    StringArray( const StringArray& other );

    ~StringArray();

    void setString( const char* str );

    inline size_t size() const
    {
        return offsetVec_.size();
    }

    inline char* operator[]( int idx ) const
    {
        return data_ + offsetVec_[ idx ];
    }

    void reserve( size_t size );

    inline void reserveOffsetVec( size_t size )
    {
        offsetVec_.reserve( size );
    }

    void push_back( const char* str, size_t len = 0 );

    void initialize();

    void clear();

    bool contains( const char* str ) const;

    inline bool empty() const
    {
        return offsetVec_.empty();
    }

    void removeHead();

    void swap( StringArray& other );

    void print( const std::string& delimeter = DefPrintDelimeter, std::ostream& out = std::cout );

private:
    void ensureFreeLength( size_t extraLen );

private:
    char* data_;
    char* endPtr_;
    size_t dataLen_;
    VGenericArray<offset_t> offsetVec_;
};

}

#endif /* STRINGARRAY_H_ */
