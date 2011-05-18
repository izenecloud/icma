/*
 * \file analysis_fmincover.cpp
 * \brief 
 * \date Jul 7, 2010
 * \author Vernkin Chen
 *
 * - Log
 *    -  2010.12.10 Zhongxia Li
 *       Merge unigram with segmentation which controlled by option parameter.
 *    -  2011.01.21 Zhongxia Li
 *       divide mixed digits and alpha (keep mixed string),
 *       divide SBC/DBC case for digits and alpha, and do unigram for SBC case.
 */
#include "icma/me/CMA_ME_Analyzer.h"
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
        StringVectorType& words,
        AnalOption& analOption
        )
{
 /*   string tmp;
    inner::assignStrVectorDomain( tmp, words,
            beginIdx, endIdxSt );
    cout << "divideNormalString "<<tmp<<endl;
*/

    StrBasedVTrie strTrie( trie );

    if( endIdxSt <= beginIdx )
        return;
    FMSizeType endIdx = (FMSizeType)endIdxSt;
    FMSizeType beginOffset = (FMSizeType)beginIdx;

    // Step 1: set the dictLen
    FMSizeType dictLenSize = endIdx - beginOffset;
    FMSizeType* dictLen = new FMSizeType[ dictLenSize ];
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
            out.push_back( dLIdx + beginOffset + 1 );
            ++dLIdx;
            continue;
        }

        //Check haveCross Status

        FMSizeType dLEndIdx = dLIdx + curLen;
        // Input is ABC, following cases haveCross are true
        //   a) AB/C and A/BC  ( cross segmentation )
        //   b) ABC  and AB/C  ( combine segmentation )  => should be: ABC and A/BC
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
        	if ( analOption.doUnigram ) {
        		FMSizeType uniIdx = dLIdx;
        		FMSizeType uniEndIdx = dLEndIdx - 1;
				for ( ; uniIdx <= uniEndIdx; uniIdx ++ ) {
					out.push_back( beginOffset + uniIdx );
					out.push_back( beginOffset + uniIdx + 1 );

					if ( uniIdx == dLIdx && !analOption.useMaxOffset ) {
		                out.push_back( dLIdx + beginOffset );
		                out.push_back( beginOffset + dLEndIdx );
					}
					else if ( uniIdx == uniEndIdx && analOption.useMaxOffset ) {
		                out.push_back( dLIdx + beginOffset );
		                out.push_back( beginOffset + dLEndIdx );
					}
				}
        	}
        	else {
        		out.push_back( dLIdx + beginOffset );
        		out.push_back( beginOffset + dLEndIdx );
        	}

            dLIdx = dLEndIdx;
            continue;
        }

        if ( analOption.noOverlap ) {
        	// ignore cross
        	// n-best (1)
        	for( advDLIdx = dLIdx; advDLIdx < dLEndIdx; )
        	{
        		curLen = dictLen[ advDLIdx ];
        		out.push_back( advDLIdx + beginOffset );
        		out.push_back( advDLIdx + beginOffset + curLen );

        		advDLIdx += curLen;
        	}
        }
        else {
			for( advDLIdx = dLIdx; advDLIdx < dLEndIdx; ++advDLIdx )
			{
				curLen = dictLen[ advDLIdx ];
				out.push_back( advDLIdx + beginOffset );
				out.push_back( advDLIdx + beginOffset + 1 );

				if( curLen == 1 )
					continue;

				out.push_back( advDLIdx + beginOffset );
				out.push_back( advDLIdx + beginOffset + curLen );
			}
        }

        dLIdx = dLEndIdx;
    }

    delete[] dictLen;
}

/**
 * \brief divide digits and letters, but keep whole string as a segment.
 * [Note] only do unigram for SBC case.
 */
void divideDigitLetterString(
        FMinCOutType& out,
        CharType* types,
        size_t beginIdx,
        size_t endIdx,
        StringVectorType& words,
        AnalOption& analOption
        )
{
    // whole word
	if ( analOption.noOverlap == false ) {
		out.push_back( beginIdx );
		out.push_back( endIdx );
	}

	// divide mixed digits and letters
	FMSizeType pre;
	FMSizeType cur = beginIdx;

	CharTypeExpand preType;
	CharTypeExpand curType = getExpandedCharType(words[cur], types[cur]);

	while ( cur < endIdx ) {
		pre = cur;
		preType = curType;

		cur ++;
		while ( cur < endIdx ) {
			curType = getExpandedCharType(words[cur], types[cur]);
			if ( curType == preType )
				cur ++;
			else
				break;
		}

		// if whole string is the same type, it has been added as a segment at the beginning.
		// if ( !(pre == beginIdx && cur == endIdx) )
		if ( pre != beginIdx || cur != endIdx ) {
			out.push_back(pre);
			out.push_back(cur);
		}
		else {
			if ( analOption.noOverlap ) {
				out.push_back( beginIdx );
				out.push_back( endIdx );
			}
		}

		// if merge unigram
		if ( analOption.doUnigram && (pre + 1 < cur) ) {
			if ( (CHAR_TYPE_DIGIT_SBC == preType)
					|| (CHAR_TYPE_LETTER_SBC == preType) )
			{
				FMSizeType uniIdx = pre;
				for ( ; uniIdx < cur; uniIdx ++ ) {
					out.push_back(uniIdx);
					out.push_back(uniIdx + 1);
				}
			}
		}
    }
}


void addFMinCString(
        FMinCOutType& out,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx,
        StringVectorType& words,
        CharType* types,
        AnalOption& analOption
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
        divideNormalString( out, trie, beginIdx, endIdx, words, analOption );
        return;
    }
    case CHAR_TYPE_DIGIT:
    case CHAR_TYPE_LETTER:
    {
    	divideDigitLetterString(out, types, beginIdx, endIdx, words, analOption );
        return;
    }

    case CHAR_TYPE_DATE:
    {
    	// whole string
		out.push_back( beginIdx );
		out.push_back( endIdx );

		// divide to 2 parts
		if ( maxIdx > beginIdx && !analOption.noOverlap) {
    		// pre digits (may be mixed with letters)
			divideDigitLetterString(out, types, beginIdx, maxIdx, words, analOption );
    		// date-char
    		out.push_back( maxIdx );
    		out.push_back( endIdx );
    	}
    	return;
    }

    case CHAR_TYPE_CHARDIGIT:
    {
        // check for start with part of string
        out.push_back( beginIdx );
        out.push_back( endIdx );

        if ( analOption.doUnigram && maxIdx > beginIdx ) {
        	FMSizeType uniIdx = beginIdx;
        	while ( uniIdx < endIdx ) {
        		out.push_back( uniIdx );
        		out.push_back( uniIdx + 1 );
        		++ uniIdx;
        	}
        }
        return;
    }

    case CHAR_TYPE_PUNC:
    {
        out.push_back( beginIdx );
        out.push_back( endIdx );
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
        size_t endIdx,
        AnalOption& analOption
        )
{
    out.clear();
    if( words.empty() == true )
        return;
    out.reserve( (size_t)( ( words.size() - beginIdx ) * 2.2 ) );

    // 1st. analysis by CharType
    size_t size = words.size();
    size_t curIdx = beginIdx + 1;
    size_t fsIdx = beginIdx; // fragment start index
    CharType t_1 = types[ curIdx - 1 ];

    //cout << words[beginIdx] << " " << types[beginIdx] << endl;

    while( curIdx < size )
    {
    	//cout << words[curIdx] << " " << types[curIdx] << endl;
        //cout << "#"<<curIdx<<", outsize = " << out.size() << ",last: " << ( out.empty() ? "" : *out.rbegin()) << endl;
        CharType t0 = types[ curIdx ];
        switch( t_1 )
        {
        case CHAR_TYPE_PUNC:
        {
            if( strcmp( words[ curIdx ], words[ curIdx - 1 ] ) != 0 ||
                    strcmp( words[ curIdx ], "." ) != 0 )
            {
                addFMinCString( out, trie, fsIdx, curIdx, words, types, analOption );
                fsIdx = curIdx;
            }
            break;
        }

        case CHAR_TYPE_DATE:
        {
            addFMinCString( out, trie, fsIdx, curIdx, words, types, analOption );
            fsIdx = curIdx;
            break;
        }

        case CHAR_TYPE_DIGIT:
        case CHAR_TYPE_LETTER:
        {
            if( t0 != CHAR_TYPE_DATE && t0 != CHAR_TYPE_DIGIT && t0 != CHAR_TYPE_LETTER )
            {
                addFMinCString( out, trie, fsIdx, curIdx, words, types, analOption );
                fsIdx = curIdx;
            }
            break;
        }

        default:
        {
            if( t0 != t_1 )
            {
                addFMinCString( out, trie, fsIdx, curIdx, words, types, analOption );
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
        addFMinCString( out, trie, fsIdx, curIdx, words, types, analOption );
    }

}

}

}
