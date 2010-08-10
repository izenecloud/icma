/**
 * \file t_speed.cpp
 * \brief 
 * \date Jul 30, 2010
 * \author Vernkin Chen
 */

#include "VTimer.h"
#include <fstream>
#include <iostream>

// for testTokenizer
#include <icma/util/CPPStringUtils.h>
#include <icma/util/StringArray.h>
#include <icma/util/VGenericArray.h>
#include <knowledge.h>
#include <icma/type/cma_ctype.h>

using namespace std;
using namespace cma;

namespace tokenizer
{
void testStringArrayTokenizer()
{
    do
    {
        cout << "Input ('x' to exit): ";
        string line;
        getline( cin, line );
        if( line == "x" || line == "X" )
            break;
        StringArray strArray;
        StringArray::tokenize( line.c_str(), strArray );
        cout << "size " << strArray.size() << ": ";
        for( size_t i = 0; i < strArray.size(); ++i )
            cout << strArray[ i ] << " / ";
        cout << endl;

    }while( true );
}

void testTokenizerSpeed( int argc, char** argv )
{
    const char* testFile = "../db/icwb/utf8/sys.dic";

    if( argc > 1 )
    {
        testFile = argv[1];
    }

    VTimer timer;

    {
        ifstream fin( testFile );
        string line;
        cout << "Test CPPStringUtils::token_string(in, ret) ..." << endl;
        timer.startWithTitle( "CPPStringUtils::token_string(in, ret)" );
        while( fin.eof() == false )
        {
            getline( fin, line );
            vector<string> vec;
            TOKEN_STR( line, vec );
        }
        timer.endAndPrint();
        fin.close();
    }

    {
        ifstream fin( testFile );
        string line;
        cout << "Test StringArray::tokenize(in, ret) ..." << endl;
        timer.startWithTitle( "StringArray::tokenize(in, ret)" );
        while( fin.eof() == false )
        {
            getline( fin, line );
            StringArray strArray;
            StringArray::tokenize( line.c_str(), strArray );
        }
        timer.endAndPrint();
        fin.close();
    }
}
}

namespace extracorCharacter
{
void extractCharacterByVector( const char* sentence, vector< string >& charOut, CMA_CType* ctype_ )
{
    static const string DefString;

    if( ctype_->getEncodeType() == Knowledge::ENCODE_TYPE_UTF8 )
    {
        const unsigned char *uc = (const unsigned char *)sentence;
        if( uc[0] == 0xEF && uc[1] == 0xBB && uc[2] == 0xBF )
            sentence += 3;
    }

    charOut.reserve( strlen(sentence) / 2 + 1 );
    CMA_CType::getByteCount_t getByteFunc = ctype_->getByteCountFun_;
    unsigned int len;
    const unsigned char *us = (const unsigned char *)sentence;
    vector< string >::iterator itr;
    while( ( len = getByteFunc( us ) ) > 0 )
    {
        itr = charOut.insert( charOut.end(), DefString );
        itr->reserve( len );
        itr->append( (const char*)us, len );
        us += len;
    }
}

void extractCharacterByStringArray( const char* sentence, StringArray& charOut, CMA_CType* ctype_ )
{
    if( ctype_->getEncodeType() == Knowledge::ENCODE_TYPE_UTF8 )
    {
        const unsigned char *uc = (const unsigned char *)sentence;
        if( uc[0] == 0xEF && uc[1] == 0xBB && uc[2] == 0xBF )
            sentence += 3;
    }
    CMA_CType::getByteCount_t getByteFunc = ctype_->getByteCountFun_;
    size_t strLen = strlen(sentence);
    charOut.reserve( strLen * 2 );
    charOut.reserveOffsetVec( strLen );
    unsigned int len;
    const unsigned char *us = (const unsigned char *)sentence;
    while( ( len = getByteFunc( us ) ) > 0 )
    {
        charOut.push_back( ( const char* )us, len );
        us += len;
    }
}

void testExtractCharacterByStringArray()
{
    CMA_CType* ctype_ = CMA_CType::instance( Knowledge::ENCODE_TYPE_UTF8 );
    do
    {
        cout << "Input ('x' to exit): ";
        string line;
        getline( cin, line );
        if( line == "x" || line == "X" )
            break;
        StringArray strArray;
        extractCharacterByStringArray( line.c_str(), strArray, ctype_ );
        strArray.print();
        cout << endl;

    }while( true );
}

void testExtractCharacter( int argc, char** argv )
{
    const char* testFile = "/home/vernkin/expdata/scd/cn_test.scd";

    if( argc > 1 )
    {
        testFile = argv[1];
    }

    cout << "===== testExtractCharacter" << endl;
    cout << "=== test file is " << testFile << endl;

    CMA_CType* ctype_ = CMA_CType::instance( Knowledge::ENCODE_TYPE_UTF8 );
    VTimer timer;

    {
        ifstream fin( testFile );
        string line;
        cout << "Test extractCharacterByVector ..." << endl;
        timer.startWithTitle( "extractCharacterByVector" );
        while( fin.eof() == false )
        {
            getline( fin, line );
            vector<string> vec;
            extractCharacterByVector( line.c_str(), vec, ctype_ );
        }
        timer.endAndPrint();
        fin.close();
    }

    {
        ifstream fin( testFile );
        string line;
        cout << "Test extractCharacterByStringArray ..." << endl;
        timer.startWithTitle( "extractCharacterByStringArray" );
        while( fin.eof() == false )
        {
            getline( fin, line );
            StringArray strArray;
            extractCharacterByStringArray( line.c_str(), strArray, ctype_ );
        }
        timer.endAndPrint();
        fin.close();
    }
}

}

namespace memalloc
{

void testMemAlloc()
{
    int times = 5000;
    size_t size = 2 * 1024 * 1024;
    size_t newSize = 8 * 1024 * 1024;

    char* records[ times ];

    cout << "===== testMemAlloc" << endl;

    VTimer timer;

    {
        cout << "Test new memalloc ..." << endl;
        timer.startWithTitle( "new memalloc" );
        for( int i = 0; i < times; ++i )
        {
            char* tmp = new char[size];
            tmp[size-1] = 'a';
            records[ i ] = new char[newSize];
            memcpy( records[i], tmp, size );
            delete tmp;
            delete records[i];
        }
        timer.endAndPrint();
    }

    /*for( int i = 0; i < times; ++i )
    {
        delete[] records[ i ];
    }*/

    {
        cout << "Test malloc memalloc ..." << endl;
        timer.startWithTitle( "malloc memalloc" );
        for( int i = 0; i < times; ++i )
        {
            records[ i ] = (char*)malloc(size);
            (records[ i ])[size-1] = 'a';
            records[ i ] = (char*)realloc(records[ i ],size);
            free( records[i] );
        }
        timer.endAndPrint();
    }

    /*for( int i = 0; i < times; ++i )
    {
        free(records[ i ]);
    }*/
}

}

namespace genericarray
{
void testGenericArray()
{
    VGenericArray<string> s1;
    s1.push_back( "acs" );
    s1.reserve( 30 );
    cout << "Shoule be 'acs': " << s1[0] << endl;

    PGenericArray<int> s2;
    s2.push_back( 222 );
    s2.reserve( 30 );
    cout << "Shoule be '222: " << s2[0] << endl;
}
}


int main( int argc, char** argv )
{
    //tokenizer::testStringArrayTokenizer();
    tokenizer::testTokenizerSpeed( argc, argv );

    //extracorCharacter::testExtractCharacterByStringArray();
    //extracorCharacter::testExtractCharacter( argc, argv );

    //memalloc::testMemAlloc();

    //genericarray::testGenericArray();

    return 0;
}
