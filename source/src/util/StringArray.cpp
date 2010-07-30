/**
 * \file StringArray.cpp
 * \brief 
 * \date Jul 30, 2010
 * \author Vernkin Chen
 */
#include <icma/util/StringArray.h>

using namespace std;

namespace cma
{

void StringArray::tokenize(
        const char* str,
        StringArray& ret,
        const string& delimeter
        )
{


    // simply set the string
    ret.setString( str );
    if( *str == 0 )
    {
        return;
    }

    char* curPtr = ret.data_;
    char* origCurPtr;

    const char* delimHead = delimeter.c_str();
    const char* delimPtr; // which location is match

    size_t delimLen = delimeter.length();

    bool separatorFlag = true;

    while( *curPtr != 0 )
    {
        //try to find delimeter
        if( *curPtr == *delimHead )
        {
            origCurPtr = curPtr;
            bool matched = true;
            for( delimPtr = delimHead + 1, ++curPtr;
                    *delimPtr != 0 && *curPtr != 0; ++delimPtr, ++curPtr )
            {
                if( *curPtr != *delimPtr )
                {
                    matched = false;
                    break;
                }
            }

            matched = matched && ( *delimPtr == 0 );

            if( matched == true )
            {
                memset( origCurPtr, 0x0, delimLen );
                separatorFlag = true;
                continue;
            }
            // if failed, move to original start
            curPtr = origCurPtr;
        }

        if( separatorFlag == true )
        {
            offset_t offset = (offset_t)( curPtr - ret.data_ );
            ret.offsetVec_.push_back( offset );
            separatorFlag = false;
        }
        ++curPtr;
    }

}

StringArray::StringArray( const char* str )
    : data_( NULL ),
    endPtr_( NULL ),
    dataLen_ ( 0 )
{
    if( str != NULL )
        setString( str );
}

StringArray::~StringArray()
{
    delete data_;
}

void StringArray::setString( const char* str )
{
    if( str == NULL )
    {
        initialize();
        return;
    }

    if( offsetVec_.empty() == false )
        offsetVec_.clear();
    if( data_ != NULL )
        delete data_;

    size_t strLen = strlen( str );
    if( strLen == 0 )
    {
        data_ = endPtr_ = NULL;
        dataLen_ = 0;
    }
    else
    {
        dataLen_ = strLen + 1;
        data_ = new char[ dataLen_ ];
        memcpy( data_, str, dataLen_ );
        endPtr_ = data_ + dataLen_;
    }
}

size_t StringArray::size()
{
    return offsetVec_.size();
}

void StringArray::reserve( size_t size )
{
    if( size < dataLen_ )
        return;
    char* tmp = new char[dataLen_];
    size_t usedLen = endPtr_ - data_;
    memcpy( tmp, data_, usedLen );
    delete data_;
    data_ = tmp;
    endPtr_ = data_ + usedLen;
}

void StringArray::append( const char* str, size_t len )
{
    if( len == 0 )
        len = strlen( str );
    ++len; // reserve tailing space
    ensureFreeLength( len );
    offset_t offset = endPtr_ - data_;
    offsetVec_.push_back( offset );
    memcpy( endPtr_, str, len );
    endPtr_ += len;
}

void StringArray::initialize()
{
    if( offsetVec_.empty() == false )
        offsetVec_.clear();
    if( data_ != NULL )
        delete data_;
    data_ = endPtr_ = NULL;
    dataLen_ = 0;
}

bool StringArray::contains( const char* str )
{
    size_t size = this->size();
    for( size_t i = 0; i < size; ++i )
    {
        if( strcmp( str, this->operator []( i ) ) == 0 )
            return true;
    }
    return false;
}

}
