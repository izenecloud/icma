/*
 * \file analysis_fmincover.cpp
 * \brief 
 * \date Jul 7, 2010
 * \author Vernkin Chen
 */

#include "icma/fmincover/analysis_fmincover.h"
#include "icma/util/StrBasedVTrie.h"

#include <iostream>

using namespace std;

#define ON_DEV

namespace cma
{
namespace fmincover
{

namespace inner
{

inline void assignStrVectorDomain(
        string& out,
        StringVectorType& words,
        size_t beginIdx,
        size_t endIdx
        )
{
    out = words[ beginIdx ];
    for( size_t i = beginIdx + 1; i < endIdx; ++i )
    {
        out.append( words[i] );
    }
}

}

const string DefStr;

void divideNormalString(
        FMinCOutType& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdxSt,
        StringVectorType& words
        )
{
 /*   string tmp;
    inner::assignStrVectorDomain( tmp, words,
            beginIdx, endIdxSt );
    cout << "divideNormalString "<<tmp<<endl;
*/
    typedef unsigned int FMSizeType;

    StrBasedVTrie strTrie( trie );

    if( endIdxSt <= beginIdx )
        return;
    FMSizeType endIdx = (FMSizeType)endIdxSt;
    FMSizeType beginOffset = (FMSizeType)beginIdx;

    // Step 1: set the dictLen
    FMSizeType dictLenSize = endIdx - beginOffset;
    FMSizeType dictLen[ dictLenSize ];
    FMSizeType maxOffset = endIdx - 1;
    dictLen[ dictLenSize - 1 ] = 1;

    for( FMSizeType curIdx = beginOffset; curIdx < maxOffset; ++curIdx )
    {
        const char* curWord = words[ curIdx ];
        strTrie.firstSearch( curWord );
        // if non word ( length > 1 ) begin with words[ curIdx ]
        if( strTrie.completeSearch == false
                || strTrie.node.moreLong == false )
        {
            dictLen[ curIdx - beginOffset ] = 1;
            continue;
        }

        // try to find words that length > 2, forwards minimum matching
        FMSizeType advIdx = curIdx + 1; // advanced index to find longer word
        FMSizeType foundWordLastIdx = 0;
        for( ; advIdx < endIdx; ++advIdx )
        {
            strTrie.search( words[ advIdx ] );
            if( strTrie.completeSearch == false )
                break;
            if( strTrie.exists() == true )
            {
                foundWordLastIdx = advIdx;
                break;
            }
        }

        // non word ( length > 1 ) begin with words[ curIdx ]
        if( foundWordLastIdx == 0 )
        {
            dictLen[ curIdx - beginOffset ] = 1;
            continue;
        }
        dictLen[ curIdx - beginOffset ] = foundWordLastIdx - curIdx + 1;
    }


    FMSizeType dLIdx = 0;
    while( dLIdx < dictLenSize )
    {
        FMSizeType curLen = dictLen[ dLIdx ];
        if( curLen == 1 )
        {
            out.push_back( dLIdx + beginOffset );
            out.push_back( 1 );
            ++dLIdx;
            continue;
        }

        //Check haveCross Status

        FMSizeType dLEndIdx = dLIdx + curLen;
        // Input is ABC, following cases haveCross are true
        //   a) AB/C and A/BC  ( cross segmentation )
        //   b) ABC  and AB/C  ( combine segmentation )
        bool haveCross = false;
        FMSizeType advDLIdx = dLIdx + 1;
        for( ; advDLIdx < dLEndIdx; ++advDLIdx )
        {
            FMSizeType advLen = dictLen[ advDLIdx ];
            if( advLen == 1 )
                continue;
            haveCross = true;
            FMSizeType newDLEndIdx = advDLIdx + advLen;
            if( newDLEndIdx > dLEndIdx )
                dLEndIdx = newDLEndIdx;
        }

        if( haveCross == false )
        {
            out.push_back( dLIdx + beginOffset );
            out.push_back( dLEndIdx - dLIdx );
            dLIdx = dLEndIdx;
            continue;
        }

        for( advDLIdx = dLIdx; advDLIdx < dLEndIdx; ++advDLIdx )
        {
            curLen = dictLen[ advDLIdx ];
            out.push_back( advDLIdx + beginOffset );
            out.push_back( 1 );

            if( curLen == 1 )
                continue;

            out.push_back( advDLIdx + beginOffset );
            out.push_back( curLen );
        }

        dLIdx = dLEndIdx;
    }

}

void addFMinCString(
        FMinCOutType& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx,
        StringVectorType& words,
        CharType* types
        )
{
/*    string tmp;
        inner::assignStrVectorDomain( tmp, words,
                beginIdx, endIdx );
        cout << "addFMinCString "<<tmp<<",type:"<<types[endIdx-1]<<endl;*/

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
    case CHAR_TYPE_DIGIT:
    case CHAR_TYPE_LETTER:
    {
        // check for the word begin with whole string
        out.push_back( beginIdx );
        out.push_back( endIdx - beginIdx );
        return;
    }

    case CHAR_TYPE_DATE:
    case CHAR_TYPE_CHARDIGIT:
    {
        // check for start with part of string
        out.push_back( beginIdx );
        out.push_back( endIdx - beginIdx );
        return;
    }

    case CHAR_TYPE_PUNC:
    {
        out.push_back( beginIdx );
        out.push_back( endIdx - beginIdx );
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


void parseFMinCoverString(
        FMinCOutType& out,
        StringVectorType& words,
        CharType* types,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx
        )
{
    out.clear();
    if( words.empty() == true )
        return;
    out.reserve( (size_t)( words.size() * 2.2 ) );

    // 1st. analysis by CharType
    size_t size = words.size();
    size_t curIdx = beginIdx + 1;
    size_t fsIdx = beginIdx; // fragment start index
    CharType t_1 = types[ curIdx - 1 ];

    while( curIdx < size )
    {
        //cout << "#"<<curIdx<<", outsize = " << out.size() << ",last: " << ( out.empty() ? "" : *out.rbegin()) << endl;
        CharType t0 = types[ curIdx ];
        switch( t_1 )
        {
        case CHAR_TYPE_PUNC:
        {
            if( strcmp( words[ curIdx ], words[ curIdx - 1 ] ) != 0 ||
                    strcmp( words[ curIdx ], "." ) != 0 )
            {
                addFMinCString( out, trie, fsIdx, curIdx, words, types );
                fsIdx = curIdx;
            }
            break;
        }

        case CHAR_TYPE_DATE:
        {
            addFMinCString( out, trie, fsIdx, curIdx, words, types );
            fsIdx = curIdx;
            break;
        }

        case CHAR_TYPE_DIGIT:
        case CHAR_TYPE_LETTER:
        {
            if( t0 != CHAR_TYPE_DATE && t0 != CHAR_TYPE_DIGIT && t0 != CHAR_TYPE_LETTER )
            {
                addFMinCString( out, trie, fsIdx, curIdx, words, types );
                fsIdx = curIdx;
            }
            break;
        }

        default:
        {
            if( t0 != t_1 )
            {
                addFMinCString( out, trie, fsIdx, curIdx, words, types );
                fsIdx = curIdx;
            }
            break;
        }
        }
        t_1 = t0;
        ++curIdx;
    }

    if( fsIdx < curIdx )
    {
        addFMinCString( out, trie, fsIdx, curIdx, words, types );
    }

}

}

}
