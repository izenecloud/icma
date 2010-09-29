/**
 * \file StringArray.cpp
 * \brief 
 * \date Jul 30, 2010
 * \author Vernkin Chen
 */
#include <cstring>
#include <icma/util/StringArray.h>
using namespace std;

namespace cma
{

string DefDelimeter = " ";
string DefPrintDelimeter = ",";

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

StringArray::StringArray( const StringArray& other )
{
    initialize();
    swap( (StringArray&)other );
}

StringArray::~StringArray()
{
    delete[] data_;
}

void StringArray::setString( const char* str )
{
    if( str == NULL )
    {
        initialize();
        return;
    }

    offsetVec_.clear();
    if( data_ != NULL )
        delete[] data_;

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

void StringArray::reserve( size_t size )
{
    if( size < dataLen_ )
        return;
    if( data_ == NULL )
    {
        dataLen_ = size;
        data_ = new char[ dataLen_ ];
        endPtr_ = data_;
        return;
    }

    char* tmp = new char[ size ];
    size_t usedLen = endPtr_ - data_;
    memcpy( tmp, data_, usedLen );
    delete[] data_;
    data_ = tmp;
    endPtr_ = data_ + usedLen;
    dataLen_ = size;
}

void StringArray::push_back( const char* str, size_t len )
{
    if( len == 0 )
        len = strlen( str );
    size_t fullLen = len + 1; // reserve tailing space
    ensureFreeLength( fullLen );
    offset_t offset = endPtr_ - data_;
    offsetVec_.push_back( offset );
    memcpy( endPtr_, str, len );
    endPtr_[ len ] = 0;
    endPtr_ += fullLen;
}

void StringArray::initialize()
{
    offsetVec_.clear();
    if( data_ != NULL )
        delete[] data_;
    data_ = endPtr_ = NULL;
    dataLen_ = 0;
}

void StringArray::clear()
{
    offsetVec_.clear();
    endPtr_ = data_;
}

bool StringArray::contains( const char* str ) const
{
    size_t size = this->size();
    for( size_t i = 0; i < size; ++i )
    {
        if( strcmp( str, this->operator []( i ) ) == 0 )
            return true;
    }
    return false;
}

int StringArray::index( const char* str ) const
{
    int size = (int)this->size();
    for( int i = 0; i < size; ++i )
    {
        if( strcmp( str, this->operator []( i ) ) == 0 )
            return i;
    }
    return -1;
}

void StringArray::removeHead()
{
    if( offsetVec_.empty() == false )
        offsetVec_.removeHead();
}

void StringArray::swap( StringArray& other )
{
    offsetVec_.swap( other.offsetVec_ );
    char* tmp;

    tmp = other.data_;
    other.data_ = data_;
    data_ = tmp;

    tmp = other.endPtr_;
    other.endPtr_ = endPtr_;
    endPtr_ = tmp;

    size_t tmpSize = other.dataLen_;
    other.dataLen_ = dataLen_;
    dataLen_ = tmpSize;
}

void StringArray::ensureFreeLength( size_t extraLen )
{
    // reserve one zero byte
    size_t freeLen = dataLen_ - ( endPtr_ - data_ );
    if( freeLen >= extraLen )
        return;
    //cout << "freelen " << freeLen << " < extraLen " << extraLen << ", dataLen = " << dataLen_ << endl;
    size_t minNewLen = dataLen_ + extraLen - freeLen;
    size_t newLen = dataLen_;
    while( newLen < minNewLen )
        newLen = (size_t)( newLen * 1.5 ) + 1;
    reserve( newLen );
}

void StringArray::print( const std::string& delimeter, std::ostream& out )
{
    size_t s = size();
    if( s == 0 )
    {
        out << "[]";
        return;
    }

    out << "[";
    for( size_t i = 0; i < s - 1; ++i )
    {
        out << this->operator []( i ) << ",";
    }
    out << this->operator []( s - 1 ) << "]";
}

}
