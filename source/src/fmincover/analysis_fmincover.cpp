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

namespace cma
{
namespace fmincover
{

namespace inner
{

inline void assignStrVectorDomain(
        string& out,
        vector<string>& words,
        size_t beginIdx,
        size_t endIdx
        )
{
    out = words[ beginIdx ];
    for( size_t i = beginIdx + 1; i < endIdx; ++i )
    {
        out += words[i];
    }
}

}

const string DefStr;

void divideNormalString(
        vector< string >& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdxSt,
        vector<string>& words
        )
{
 /*   string tmp;
    inner::assignStrVectorDomain( tmp, words,
            beginIdx, endIdxSt );
    cout << "divideNormalString "<<tmp<<endl;
*/
    typedef unsigned int FMSizeType;

    StrBasedVTrie strTrie( trie );
    vector< string >::iterator itr;

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
        string& curWord = words[ curIdx ];
        strTrie.firstSearch( curWord.c_str() );
        // if non word ( length > 1 ) begin with words[ curIdx ]
        if( strTrie.completeSearch == false
                || strTrie.node.moreLong == false )
        {
            dictLen[ curIdx - beginOffset ] = 1;
            continue;
        }

        // try to find words that length > 2, forword minimum matching
        FMSizeType advIdx = curIdx + 1; // advanced index to find longer word
        FMSizeType foundWordLastIdx = 0;
        for( ; advIdx < endIdx; ++advIdx )
        {
            strTrie.search( words[ advIdx ].c_str() );
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
            itr = out.insert( out.end(), DefStr );
            *itr = words[ dLIdx + beginOffset ];
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
            itr = out.insert( out.end(), DefStr );
            inner::assignStrVectorDomain( *itr, words,
                    dLIdx + beginOffset, dLEndIdx + beginOffset );
            dLIdx = dLEndIdx;
            continue;
        }

        for( advDLIdx = dLIdx; advDLIdx < dLEndIdx; ++advDLIdx )
        {
            curLen = dictLen[ advDLIdx ];
            itr = out.insert( out.end(), DefStr );
            *itr = words[ advDLIdx + beginOffset ];

            if( curLen == 1 )
                continue;

            itr = out.insert( out.end(), DefStr );
            inner::assignStrVectorDomain( *itr, words,
                    advDLIdx + beginOffset, advDLIdx + curLen + beginOffset );

        }

        dLIdx = dLEndIdx;
    }
}

void addFMinCString(
        vector< string >& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx,
        vector<string>& words,
        CharType* types
        )
{
/*    string tmp;
        inner::assignStrVectorDomain( tmp, words,
                beginIdx, endIdx );
        cout << "addFMinCString "<<tmp<<",type:"<<types[endIdx-1]<<endl;*/

    vector< string >::iterator itr;

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
        itr = out.insert( out.end(), DefStr );
        inner::assignStrVectorDomain( *itr, words, beginIdx, endIdx );
        return;
    }

    case CHAR_TYPE_DATE:
    case CHAR_TYPE_CHARDIGIT:
    {
        // check for start with part of string
        itr = out.insert( out.end(), DefStr );
        inner::assignStrVectorDomain( *itr, words, beginIdx, endIdx );
        return;
    }

    case CHAR_TYPE_PUNC:
    {
        itr = out.insert( out.end(), DefStr );
        inner::assignStrVectorDomain( *itr, words, beginIdx, endIdx );
        return;
    }

    case CHAR_TYPE_SPACE:
    case CHAR_TYPE_INIT:
    {
        cout<<"ignore space "<<endl;// do nothing
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
        vector< string >& out,
        vector< string >& words,
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
        //cout << "#"<<curIdx<<", outsize = " << out.size() << ",last: " << ( out.empty() ? "" : *out.rbegin()) << endl;
        CharType t0 = types[ curIdx ];
        switch( t_1 )
        {
        case CHAR_TYPE_PUNC:
        {
            if( words[ curIdx ] !=  words[ curIdx - 1 ] ||
                    words[ curIdx ] != "." )
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

        case CHAR_TYPE_NUMBER:
        case CHAR_TYPE_LETTER:
        {
            if( t0 != CHAR_TYPE_DATE && t0 != CHAR_TYPE_NUMBER && t0 != CHAR_TYPE_LETTER )
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
