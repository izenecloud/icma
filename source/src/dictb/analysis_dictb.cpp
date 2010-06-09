/*
 * \file analysis_dictb.cpp
 * \brief 
 * \date Jun 8, 2010
 * \author Vernkin Chen
 */

#include "icma/dictb/analysis_dictb.h"
#include "icma/util/StrBasedVTrie.h"

#include <iostream>

using namespace std;

namespace cma
{
namespace dictb
{
DictBString DefDBS;

void DictBString::print( vector<string>* words,  ostream& stream )
{
    for( size_t i = beginIdx_; i < endIdx_; ++i )
    {
        stream << (*words)[ i ];
    }
    stream << "(";
    switch( fragType_ )
    {
    case NORMAL:
        stream << "NORMAL";
        break;
    case SINGLE:
        stream << "SINGLE";
        break;
    case DIGIT:
        stream << "DIGIT";
        break;
    case CHARDIGIT:
        stream << "CHARDIGIT";
        break;
    case LETTER:
        stream << "LETTER";
        break;
    case DATE:
        stream << "DATE";
        break;
    case PUNCTUATION:
        stream << "PUNC";
        break;
    case COMPOSITE:
        stream << "COMP";
        break;
    default:
        stream << "N/A";
        break;
    }

    stream << ")";
}

void divideNormalString(
        vector< DictBString >& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx,
        vector<string>* words
        )
{
    StrBasedVTrie strTrie( trie );
    size_t fsIdx = beginIdx; // fragment start index
    size_t wsIdx = beginIdx; // word start index
    size_t curIdx = beginIdx; // current index

    size_t fragEndIdx = fsIdx; // fragment end index

    vector< DictBString >::iterator itr;

    while( wsIdx < endIdx )
    {
        if( curIdx == endIdx )
        {
            itr = out.insert( out.end(), DefDBS );
            itr->beginIdx_ = fsIdx;
            itr->endIdx_ = endIdx;
            itr->fragType_ = DictBString::NORMAL;
            break;
        }

        if( wsIdx == curIdx )
        {
            strTrie.firstSearch( (*words)[ curIdx ].c_str() );
            ++curIdx;
            if( strTrie.completeSearch == false )
            {
                itr = out.insert( out.end(), DefDBS );
                itr->beginIdx_ = fsIdx;
                itr->endIdx_ = fsIdx + 1; // +1 is required
                itr->fragType_ = DictBString::NORMAL;
                fsIdx = wsIdx = curIdx;
            }

            continue;
        }

        strTrie.search( (*words)[ curIdx ].c_str() );
        if( strTrie.exists() == true )
        {
            if( curIdx > fragEndIdx )
                fragEndIdx = curIdx;
            ++curIdx;
            continue;
        }

        // move to next start
        ++wsIdx;
        if( wsIdx > fragEndIdx )
        {
            itr = out.insert( out.end(), DefDBS );
            itr->beginIdx_ = fsIdx;
            itr->endIdx_ = fragEndIdx + 1; // +1 is required
            itr->fragType_ = DictBString::NORMAL;
            fragEndIdx = fsIdx = wsIdx;
        }

        curIdx = wsIdx;
    }

}

void addDictBString(
        vector< DictBString >& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx,
        vector<string>* words,
        CharType* types
        )
{
    vector< DictBString >::iterator itr;

    size_t maxIdx = endIdx - 1;
    CharType lastType = types[ maxIdx ];

    switch( lastType )
    {

    case CHAR_TYPE_OTHER:
    {
        // divide into smaller normal boundary
        divideNormalString( out, trie, beginIdx, endIdx, words );
        return;
    }
    case CHAR_TYPE_NUMBER:
    case CHAR_TYPE_LETTER:
    {
        // check for the word begin with whole string
        itr = out.insert( out.end(), DefDBS );
        itr->beginIdx_ = beginIdx;
        itr->endIdx_ = endIdx;
        if( lastType == CHAR_TYPE_NUMBER )
            itr->fragType_ = DictBString::DIGIT;
        else
            itr->fragType_ = DictBString::LETTER;
        return;
    }

    case CHAR_TYPE_DATE:
    case CHAR_TYPE_CHARDIGIT:
    {
        // check for start with part of string
        itr = out.insert( out.end(), DefDBS );
        itr->beginIdx_ = beginIdx;
        itr->endIdx_ = endIdx;
        if( lastType == CHAR_TYPE_DATE )
            itr->fragType_ = DictBString::DATE;
        else
            itr->fragType_ = DictBString::CHARDIGIT;
        return;
    }

    case CHAR_TYPE_PUNC:
    {
        itr = out.insert( out.end(), DefDBS );
        itr->beginIdx_ = beginIdx;
        itr->endIdx_ = endIdx;
        itr->fragType_ = DictBString::PUNCTUATION;
        return;
    }

    case CHAR_TYPE_SPACE:
    case CHAR_TYPE_INIT:
    {
        // do nothing
        return;
    }
    default:
    {
        cerr << "[Warn] Forgot to handle the chartype " << (int)lastType << endl;
        return;
    }

    }

}


void parseDictBString(
        vector< DictBString >& out,
        vector<string>& words,
        CharType* types,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx
        )
{
    if( words.empty() == true )
        return;

    // 1st. analysis by CharType
    size_t size = words.size();
    size_t curIdx = beginIdx + 1;
    size_t fsIdx = beginIdx; // fragment start index
    CharType t_1 = types[ curIdx - 1 ];

    while( curIdx < size )
    {
        CharType t0 = types[ curIdx ];
        switch( t_1 )
        {
        case CHAR_TYPE_PUNC:
        {
            if( words[ curIdx ] !=  words[ curIdx - 1 ] ||
                    words[ curIdx ] != "." )
            {
                addDictBString( out, trie, fsIdx, curIdx, &words, types );
                fsIdx = curIdx;
            }
            break;
        }

        case CHAR_TYPE_DATE:
        {
            addDictBString( out, trie, fsIdx, curIdx, &words, types );
            fsIdx = curIdx;
            break;
        }

        case CHAR_TYPE_NUMBER:
        case CHAR_TYPE_LETTER:
        {
            if( t0 != CHAR_TYPE_DATE && t0 != CHAR_TYPE_NUMBER && t0 != CHAR_TYPE_LETTER )
            {
                addDictBString( out, trie, fsIdx, curIdx, &words, types );
                fsIdx = curIdx;
            }
            break;
        }

        default:
        {
            if( t0 != t_1 )
            {
                addDictBString( out, trie, fsIdx, curIdx, &words, types );
                fsIdx = curIdx;
            }
            break;
        }
        }
        t_1 = t0;
        ++curIdx;
    }

    if( fsIdx < curIdx )
        addDictBString( out, trie, fsIdx, curIdx, &words, types );
}

}

}
