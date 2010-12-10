/** \file CMA_ME_Analyzer.cc
 * \brief Analyzer for the CMAC
 *
 * \author vernkin
 *
 */

#include <string>
#include <fstream>
#include <iostream>

#include "VTrie.h"
#include "strutil.h"

#include "icma/pos_table.h"
#include "icma/me/CMA_ME_Analyzer.h"
#include "icma/util/CPPStringUtils.h"
#include "icma/util/StrBasedVTrie.h"
#include "icma/util/CateStrTokenizer.h"
#include "icma/util/tokenizer.h"

#include "icma/fmincover/analysis_fmincover.h"

// used to disable some codes
#define ON_DEV

namespace cma {

namespace cmainner
{
/**
 * Whether the two Segmentation is the same
 */
inline bool isSameSegment(
        Sentence& sent,
        int idx1,
        int idx2,
        bool includePOS
        )
{
    if( sent.getCount( idx1 ) != sent.getCount( idx2 ) )
        return false;

    size_t size = sent.getCount( idx1 );
    for( size_t i = 0; i < size; ++i )
    {
        if( strcmp( sent.getLexicon( idx1, i ),
                sent.getLexicon( idx2, i ) ) != 0 )
            return false;
    }

    if( includePOS = false )
        return true;

    for( size_t i = 0; i < size; ++i )
    {
        if( strcmp( sent.getStrPOS( idx1, i ),
                sent.getStrPOS( idx2, i ) ) != 0 )
            return false;
    }

    return true;
}

inline void removeDuplicatedSegment(
        Sentence& sent,
        bool includePOS
        )
{
    int listSize = sent.getListSize();
    if( listSize <= 1 )
        return;

    vector< size_t > duplicatedList;
    for( int i = 1; i < listSize; ++i )
    {
        for( int j = i - 1; j >= 0; --j )
        {
            //cout << "Compare #" << i << " with #" << j <<endl;
            if( isSameSegment( sent, i, j, includePOS ) == true )
            {
                duplicatedList.push_back( i );
                break;
            }
        }
    }

    for( vector< size_t >::reverse_iterator itr = duplicatedList.rbegin();
            itr != duplicatedList.rend(); ++itr )
    {
        //TODO have remove some results here
        //segment.erase( ( segment.begin() + *itr ) );
        //pos.erase( ( pos.begin() + *itr ) );
    }
}
}

namespace fmincover
{
	bool g_doUnigram = false;
	bool g_useMaxOffset = false;
}

    CMA_ME_Analyzer::CMA_ME_Analyzer()
			: knowledge_(0), ctype_(0), posTable_(0),
			  analysis(&CMA_ME_Analyzer::analysis_mmmodel)
    {
    }

    CMA_ME_Analyzer::~CMA_ME_Analyzer() {
    }

    void CMA_ME_Analyzer::setOption(OptionType nOption, double nValue)
    {
        options_[nOption] = nValue;
        // check for specific setting
        if( nOption == OPTION_ANALYSIS_TYPE )
        {
            if( static_cast<int>(nValue) == 2 )
                analysis = &CMA_ME_Analyzer::analysis_fmm;
            else if( static_cast<int>(nValue) == 3 )
                analysis = &CMA_ME_Analyzer::analysis_fmincover;
            else if( static_cast<int>(nValue) == 4 ) {
            	analysis = &CMA_ME_Analyzer::analysis_fmincover;
            	fmincover::g_doUnigram = true;
            	fmincover::g_useMaxOffset = false;
            }
            else if( static_cast<int>(nValue) == 77 )
                analysis = &CMA_ME_Analyzer::analysis_pure_mmmodel;
            else
                analysis = &CMA_ME_Analyzer::analysis_mmmodel;
        }
    }

    int CMA_ME_Analyzer::runWithSentence(Sentence& sentence)
    {
        static const MorphemeList DefMorphemeList;
        static const Morpheme DefMorp;
        if( strlen( sentence.getString() ) == 0 )
        	return 1;
        int N = (int) getOption(OPTION_TYPE_NBEST);

        bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;


        (this->*analysis)(sentence.getString(), N, sentence, printPOS);

        size_t size = sentence.getListSize();
        sentence.candidates_.reserve( size );

        if( size <= 1 )
        {
			size_t posSize = sentence.getCount( 0 );
			sentence.addList( DefMorphemeList );
			MorphemeList& list = *sentence.getMorphemeList( 0 );
			list.insert( list.end(), posSize, DefMorp );

			if( printPOS == true )
			{
                for ( size_t j = 0; j < posSize; ++j )
                {
                    Morpheme& morp = list[ j ];
                    morp.posCode_ = posTable_->getCodeFromStr( sentence.getStrPOS( 0, j ) );
                    morp.isIndexed = posTable_->isIndexPOS( morp.posCode_ );
                }
			}

			return 1;
        }


        double totalScore = 0;
        //remove duplicate first
        cmainner::removeDuplicatedSegment( sentence, printPOS );

		for ( size_t i = 0; i < size; ++i )
		{
		    size_t posSize = sentence.getCount( i );
			sentence.addList( DefMorphemeList );
			MorphemeList& list = *sentence.getMorphemeList( i );
			list.insert( list.end(), posSize, DefMorp );

            if( printPOS == true )
            {
                for ( size_t j = 0; j < posSize; ++j )
                {
                    Morpheme& morp = list[ j ];
                    morp.posCode_ = posTable_->getCodeFromStr( sentence.getStrPOS( i, j ) );
                    morp.isIndexed = posTable_->isIndexPOS( morp.posCode_ );
                }
            }

			double score = sentence.getScore( i );
			totalScore += score;
		}

		for ( int j = 0; j < sentence.getListSize(); ++j )
		{
			sentence.setScore( j, sentence.getScore(j) / totalScore );
		}

        return 1;

    }

    int CMA_ME_Analyzer::runWithStream(const char* inFileName, const char* outFileName) {

    	assert(inFileName);
    	assert(outFileName);

    	ifstream in(inFileName);
		if(!in)
		{
			cerr<<"[Error] The input file "<<inFileName<<" not exists!"<<endl;
			return 0;
		}

		ofstream out(outFileName);
		if(!out)
		{
			cerr<<"[Error] The output file "<<outFileName<<" could not be created!"<<endl;
			return 0;
		}

        bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

        string line;
        Sentence sent;
        bool remains = !in.eof();
        while (remains) {
            getline(in, line);
            remains = !in.eof();
            if (!line.length()) {
                if( remains )
					out << endl;
                continue;
            }
            //cout << "#analysis " << line << endl;
            (this->*analysis)(line.data(), 1, sent, printPOS);

            if( sent.getListSize() > 0 )
            {
                if (printPOS)
                {
                    int maxIndex = sent.getCount( 0 );
                    for ( int i = 0; i < maxIndex; ++i )
                    {
                        const char* lexicon = sent.getLexicon( 0, i );
                        //if( knowledge_->isStopWord( lexicon ) )
                        //	continue;
                        out << lexicon << posDelimiter_ << sent.getStrPOS( 0, i ) << wordDelimiter_;
                    }

                    if (remains)
                        out << sentenceDelimiter_ << endl;
                    else
                        break;
                }
                else
                {
                    int maxIndex = sent.getCount( 0 );
                    for (int i = 0; i < maxIndex; ++i)
                    {
                        const char* lexicon = sent.getLexicon( 0, i );
                        //if( knowledge_->isStopWord( lexicon) )
                        //	continue;
                        out << lexicon << wordDelimiter_;
                    }

                    if (remains)
                        out << sentenceDelimiter_ << endl;
                    else
                        break;
                }
            }
            else if( remains )
            {
                out << endl;
            }
        }

        in.close();
        out.close();

        return 1;
    }

    const char* CMA_ME_Analyzer::runWithString(const char* inStr) {
    	strBuf_.clear();

    	if( strlen( inStr ) == 0 )
    	      return strBuf_.c_str();

    	bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;
      
        Sentence sent;
        (this->*analysis)(inStr, 1, sent, printPOS);

        if( sent.getListSize() > 0 )
        {
            if (printPOS)
            {
                size_t size = sent.getCount( 0 );
                for ( size_t i = 0; i < size; ++i )
                {
                    const char* lexicon = sent.getLexicon( 0, i );
                    //if( knowledge_->isStopWord( lexicon ) )
                    //	continue;
                    strBuf_.append( lexicon ).append( posDelimiter_ ).
                          append( sent.getStrPOS( 0, i ) ).append( wordDelimiter_ );
                }
            }
            else
            {
                size_t size = sent.getCount( 0 );
                for ( size_t i = 0; i < size; ++i )
                {
                    const char* lexicon = sent.getLexicon( 0, i );
                    //if( knowledge_->isStopWord( lexicon ) )
                    //  continue;
                    strBuf_.append( lexicon ).append( wordDelimiter_ );
                }
            }
        }
        return strBuf_.c_str();
    }

    void CMA_ME_Analyzer::getNGramResult( const char *inStr, int n, vector<string>& output )
    {
    	vector<vector<OneGramType> > oneGram;
    	splitToOneGram( inStr, oneGram );
    	getNGramResultImpl( oneGram, n, output );
    }

    void CMA_ME_Analyzer::getNGramArrayResult( const char *inStr, vector<int> nArray, vector<string>& output )
	{
    	vector<vector<OneGramType> > oneGram;
		splitToOneGram( inStr, oneGram );
		for( vector<int>::iterator itr = nArray.begin();
				itr != nArray.end(); ++itr)
			getNGramResultImpl( oneGram, *itr, output );
	}

    void CMA_ME_Analyzer::setKnowledge(Knowledge* pKnowledge) {
        knowledge_ = (CMA_ME_Knowledge*) pKnowledge;
        // close the POS output automatically
        if(!knowledge_->isSupportPOS())
        	setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
        posTable_ = knowledge_->getPOSTable();
        ctype_ = CMA_CType::instance(knowledge_->getEncodeType());
        encodeType_ = knowledge_->getEncodeType();
        if(knowledge_->getPOSTagger())
            knowledge_->getPOSTagger()->setCType(ctype_);
        if(knowledge_->getSegTagger())
            knowledge_->getSegTagger()->setCType(ctype_);

        const string* val = knowledge_->getSystemProperty( "pos_delimiter" );
        if( val )
        	setPOSDelimiter( val->data() );

        val = knowledge_->getSystemProperty( "word_delimiter" );
		if( val )
			setWordDelimiter( val->data() );

		val = knowledge_->getSystemProperty( "sentence_delimiter" );
		if( val )
			setSentenceDelimiter( val->data() );

    }

    void CMA_ME_Analyzer::splitSentence(const char* paragraph, std::vector<Sentence>& sentences)
    {
        if(! paragraph)
            return;

        Sentence result;
        string sentenceStr;
        CTypeTokenizer tokenizer(ctype_);
        tokenizer.assign(paragraph);
        for(const char* p=tokenizer.next(); p; p=tokenizer.next())
        {
            if(ctype_->isSentenceSeparator(p))
            {
                sentenceStr += p;

                result.setString(sentenceStr.c_str());
                sentences.push_back(result);

                sentenceStr.clear();
            }
            // white-space characters are also used as sentence separator,
            // but they are ignored in the sentence result
            else if(ctype_->isSpace(p))
            {
                if(! sentenceStr.empty())
                {
                    result.setString(sentenceStr.c_str());
                    sentences.push_back(result);

                    sentenceStr.clear();
                }
            }
            else
            {
                sentenceStr += p;
            }
        }

        // in case the last character is not space or sentence separator
        if(! sentenceStr.empty())
        {
            result.setString(sentenceStr.c_str());
            sentences.push_back(result);

            sentenceStr.clear();
        }
    }

namespace meanainner{

    /**
     * \param lastWordEnd include that index
     */
    inline void toCombine(VTrie *trie, CMA_CType* type, vector<string>& src,
            int begin, int lastWordEnd, vector<string>& dest){
        if(begin == lastWordEnd){
            string& str = src[begin];
            if(!type->isSpace(str.c_str()))
                dest.push_back(str);
            return;
        }
        bool isOK = true;
        //only have two word
        /*if((lastWordEnd - begin < 2) &&
                (type->length(src[begin].data()) + type->length(src[begin+1].data()) < 3)){
            VTrieNode node;
            trie->search(src[begin].data(), &node);
            if(node.data > 0){
                trie->search(src[begin + 1].data(), &node);
                isOK = node.data <= 0;
            }
        }*/

        if(isOK){
            string buf = src[begin];
            for(int k=begin+1; k<=lastWordEnd; ++k)
                buf.append(src[k]);
            dest.push_back(buf);
        }else{
            for(int k=begin; k<=lastWordEnd; ++k){
                string& str = src[k];
                if(!type->isSpace(str.c_str()))
                    dest.push_back(str);
            }
        }
    }

    void combineRetWithTrie(VTrie *trie, vector<string>& src, 
            vector<string>& dest, CMA_CType* type) {
        
        int begin = -1;
        int lastWordEnd = -1;

        int n = (int)src.size();
        VTrieNode node;
        for (int i = 0; i < n; ++i) {
            string& str = src[i];

            #ifdef DEBUG_TRIE_MATCH
            cout<<"Before Check begin:"<<begin<<node<<endl;
            #endif

            size_t strLen = str.length();
            size_t j = 0;
            for (; node.moreLong && j < strLen; ++j) {
                trie->find(str[j], &node);
                //cout<<"Ret "<<(int)(unsigned char)str[j]<<","<<node<<endl;
            }

            #ifdef DEBUG_TRIE_MATCH
            cout<<"Check str "<<str<<",isEnd:"<<(j == strLen)<<node<<endl;
            #endif

            //did not reach the last bit
            if (j < strLen) {
                //no exist in the dictionary
                if (begin < 0)
                {
                    if(!type->isSpace(str.c_str()))
                        dest.push_back(str);
                }
                 else {
                    if(lastWordEnd < begin)
                        lastWordEnd = begin;
                    toCombine(trie, type, src, begin, lastWordEnd, dest);
                    begin = -1;
                    //restart that node
                    i = lastWordEnd; //another ++ in the loop
                }
                node.init();
            } else {
                if (node.moreLong && (i < n - 1)) {
                    if(begin < 0)
                        begin = i;
                    if(node.data > 0)
                        lastWordEnd = i;
                } else {
                    if (begin < 0)
                    {
                        if(!type->isSpace(str.c_str()))
                            dest.push_back(str);
                    }
                    else {
                        if(node.data > 0)
                            lastWordEnd = i;
                        else if(lastWordEnd < begin)
                            lastWordEnd = begin;

                        toCombine(trie, type, src, begin, lastWordEnd, dest);
                        begin = -1;
                        i = lastWordEnd;
                    }
                    node.init();
                }
            }
        } //end for

        if(begin >= 0){
            toCombine(trie, type, src, begin, n-1, dest);
        }
    }

    void toCombine(
            PGenericArray<size_t>& segment,
            size_t begin,
            size_t lastWordEnd
            )
    {
        //cout << " to combine [" << begin << ", " << lastWordEnd << "]"<< endl;
        size_t lastIndex = lastWordEnd + 1; // included
        segment[ begin + 1 ] = segment[ lastIndex ];
        for( size_t cbIdx = begin + 3; cbIdx <= lastIndex; cbIdx += 2 )
        {
            segment[ cbIdx ] = 0;
        }
    }

    void rmEmptyUnits(
            PGenericArray<size_t>& segment,
            size_t beginIdx,
            size_t endIdx
            )
    {
        size_t idx = beginIdx + 1;
        for( ; idx < endIdx; idx += 2 )
        {
            if( segment[ idx ] == 0 )
                break;
        }

        // no empty unit found
        if( idx >= endIdx )
            return;
        size_t validIdx = idx + 2;
        while( true )
        {
            // find next non-empty unit
            while( validIdx < endIdx && segment[ validIdx ] == 0 )
                validIdx += 2;
            // if not found, exit
            if( validIdx >= endIdx )
                break;

            do
            {
                segment[ idx - 1 ] = segment[ validIdx - 1 ];
                segment[ idx ] = segment[ validIdx ];
                idx += 2;
                validIdx += 2;
            }
            while( validIdx < endIdx && segment[ validIdx ] != 0 );
        }
        // set last idx as empty
        segment[ idx ] = 0;
    }

    void combineRetWithTrie(
            VTrie* trie,
            StringVectorType& words,
            CharType* types,
            PGenericArray<size_t>& segment,
            size_t beginIdx,
            size_t endIdx
            )
    {
        StrBasedVTrie strTrie( trie );

        bool continueSch = false;
        bool schFlag = false;
        size_t begin = 0;
        size_t lastWordEnd = 0;

        for ( size_t i = beginIdx; i <= endIdx;  i += 2 )
        {
            if( i == endIdx )
            {
                if( continueSch == false || ( lastWordEnd + 2 ) >= endIdx )
                    break;
                if( begin < lastWordEnd )
                    toCombine( segment, begin, lastWordEnd );

                continueSch = false;
                i = lastWordEnd;
                continue;

                break;
            }

            size_t wordIdx = segment[ i ];
            if( continueSch == true )
                schFlag = strTrie.search( words[ wordIdx ] );
            else
                schFlag = strTrie.firstSearch( words[ wordIdx ] );

            if( schFlag == true && strTrie.node.moreLong == true )
            {
                size_t wordEndIdx = segment[ i + 1 ];
                for( ++wordIdx; wordIdx < wordEndIdx; ++wordIdx )
                {
                    schFlag = strTrie.search( words[ wordIdx ] );
                    if( schFlag == false )
                        break;
                }
            }

            //did not reach the last bit
            if ( schFlag == false )
            {
                //no exist in the dictionary
                if ( continueSch == false )
                {
                    if( types[ wordIdx ] == CHAR_TYPE_SPACE )
                        segment[ i + 1 ] = 0;
                    continue;
                }


                if( begin < lastWordEnd )
                    toCombine( segment, begin, lastWordEnd );

                continueSch = false;
                i = lastWordEnd; //another ++ in the loop
            }
            else if( strTrie.node.moreLong == false )
            {
                if( continueSch == false )
                    continue;

                if( begin < lastWordEnd )
                    toCombine( segment, begin, lastWordEnd );

                continueSch = false;
                i = lastWordEnd;
            }
            else if( continueSch == false )
            {
                continueSch = true;
                lastWordEnd = begin = i;
            }
            else if( strTrie.node.data > 0 )
            {
                lastWordEnd = i;
            }
        } //end for

        if( continueSch == true )
        {
            toCombine( segment, begin, endIdx - 2 );
        }

        // remove zero bytes
        rmEmptyUnits( segment, beginIdx, endIdx );
    }

}

    void CMA_ME_Analyzer::analysis_mmmodel(
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS
            )
    {
        static CandidateMeta DefCandidateMeta;

        if( N <= 0 )
            N = 20;

        // Initial Step 1: split as Chinese Character based
        StringVectorType words;
        extractCharacter( sentence, words );

        if( words.empty() == true )
            return;

        // Initial Step 2nd: set character types
        CharType* types = new CharType[ (int)words.size() ];
        setCharType( words, types );


        VGenericArray< CandidateMeta >& candMeta = ret.candMetas_;
        candMeta.clear();
        PGenericArray<size_t> segment;

        SegTagger* segTagger = knowledge_->getSegTagger();
        if( N == 1 )
        {
            segTagger->seg_sentence_best_with_me( words, types, segment );
            candMeta.push_back( DefCandidateMeta );
            candMeta[ 0 ].segOffset_ = 0;
            candMeta[ 0 ].score_ = 1.0;
        }
        else
        {
            segTagger->seg_sentence( words, types, N, N, segment, candMeta );
            N = candMeta.size();
        }

        size_t* offsetArray = new size_t[ N + 1 ];
        offsetArray[ 0 ] = 0;
        offsetArray[ N ] = segment.size();
        for( int i = 1; i < N; ++i )
            offsetArray[ i ] = candMeta[ i ].segOffset_;

/*
{
        for( int i = 0; i <= N; ++i )
        {
            cout << "offsetArray[ " << i << " ] = " << offsetArray[ i ] << endl;
        }
        cout << "analysis_mmmodel segment: " << endl;
        int oaidx = 0;
        for( size_t i = 0; i < segment.size(); i += 2 )
        {
            if( i == offsetArray[ oaidx ] )
            {
                cout << "--------" << endl;
                ++oaidx;
            }
            cout << "#" << i << ": " << segment[ i ] << " -> " << segment[ i + 1 ] << endl;
        }
}
*/

        // only combine the first result
        VTrie *trie = knowledge_->getTrie();
        meanainner::combineRetWithTrie( trie, words, types, segment,
                0, offsetArray[ 1 ] );
        ret.segment_.clear();
        for( int i = 0; i < N; ++i )
        {
            candMeta[ i ].segOffset_ = ret.segment_.size();
            createStringLexicon( words, segment, ret.segment_,
                    offsetArray[ i ], offsetArray[ i + 1 ] );
        }

/*
{
        for( int i = 0; i <= N; ++i )
        {
            cout << "offsetArray[ " << i << " ] = " << offsetArray[ i ] << endl;
        }
        cout << "analysis_mmmodel segment: " << endl;
        int oaidx = 0;
        for( size_t i = 0; i < segment.size(); i += 2 )
        {
            if( i == offsetArray[ oaidx ] )
            {
                cout << "--------" << endl;
                ++oaidx;
            }
            cout << "#" << i << ": " << segment[ i ] << " -> " << segment[ i + 1 ] << endl;
        }
}
*/

        if( tagPOS == false )
        {
            delete[] types;
            delete[] offsetArray;
            return;
        }

        ret.pos_.clear();
        ret.pos_.reserve( ret.segment_.size() );
        POSTagger* posTagger = knowledge_->getPOSTagger();       
        for ( int i = 0; i < N; ++i )
        {
            CandidateMeta& cm = candMeta[ i ];
            candMeta[ i ].posOffset_ = ret.pos_.size();
            posTagger->tag_sentence_best( ret.segment_, segment, types,
                    cm.segOffset_, cm.segOffset_ + ret.getCount( i ), offsetArray[ i ], ret.pos_ );
        }

        delete[] types;
        delete[] offsetArray;
    }

    void CMA_ME_Analyzer::analysis_pure_mmmodel(
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS
            )
    {
        static CandidateMeta DefCandidateMeta;

        // Initial Step 1: split as Chinese Character based
        StringVectorType words;
        extractCharacter( sentence, words );

        if( words.empty() == true )
            return;

        // Initial Step 2nd: set character types
        CharType* types = new CharType[ (int)words.size() ];
        setCharType( words, types );


        VGenericArray< CandidateMeta >& candMeta = ret.candMetas_;
        candMeta.clear();
        PGenericArray<size_t> segment;

        SegTagger* segTagger = knowledge_->getSegTagger();
        if( N == 1 )
        {
            segTagger->seg_sentence_best_with_me( words, types, segment );
            candMeta.push_back( DefCandidateMeta );
            candMeta[ 0 ].segOffset_ = 0;
            candMeta[ 0 ].score_ = 1.0;
        }
        else
        {
            segTagger->seg_sentence( words, types, N, N, segment, candMeta );
            N = candMeta.size();
        }

        size_t* offsetArray = new size_t[ N + 1 ];
        offsetArray[ 0 ] = 0;
        offsetArray[ N ] = segment.size();
        for( int i = 1; i < N; ++i )
            offsetArray[ i ] = candMeta[ i ].segOffset_;


        // only combine the first result
        ret.segment_.clear();
        for( int i = 0; i < N; ++i )
        {
            candMeta[ i ].segOffset_ = ret.segment_.size();
            createStringLexicon( words, segment, ret.segment_,
                    offsetArray[ i ], offsetArray[ i + 1 ] );
        }

/*
        for( int i = 0; i <= N; ++i )
        {
            cout << "offsetArray[ " << i << " ] = " << offsetArray[ i ] << endl;
        }
        cout << "analysis_mmmodel segment: " << endl;
        int oaidx = 0;
        for( size_t i = 0; i < segment.size(); i += 2 )
        {
            if( i == offsetArray[ oaidx ] )
            {
                cout << "--------" << endl;
                ++oaidx;
            }
            cout << "#" << i << ": " << segment[ i ] << " -> " << segment[ i + 1 ] << endl;
        }
*/

        if( tagPOS == false )
        {
            delete[] types;
            delete[] offsetArray;
            return;
        }

        ret.pos_.clear();
        ret.pos_.reserve( ret.segment_.size() );
        POSTagger* posTagger = knowledge_->getPOSTagger();
        for ( int i = 0; i < N; ++i )
        {
            CandidateMeta& cm = candMeta[ i ];
            candMeta[ i ].posOffset_ = ret.pos_.size();
            posTagger->tag_sentence_best( ret.segment_, segment, types,
                    cm.segOffset_, cm.segOffset_ + ret.getCount( i ), offsetArray[ i ], ret.pos_ );
        }

        delete[] types;
        delete[] offsetArray;
    }

    void CMA_ME_Analyzer::analysis_fmm(
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS
            )
    {
        static CandidateMeta DefCandidateMeta;

        // Initial Step 1: split as Chinese Character based
        StringVectorType words;
        extractCharacter( sentence, words );

        if( words.empty() == true )
            return;

        size_t wordSize = words.size();
        // Initial Step 2nd: set character types
        CharType* types = new CharType[ (int)wordSize ];
        setCharType( words, types );

        ret.candMetas_.clear();
        ret.candMetas_.push_back( DefCandidateMeta );
        ret.candMetas_[ 0 ].segOffset_ = 0;
        ret.candMetas_[ 0 ].score_ = 1.0;


        PGenericArray<size_t> bestSegSeq;
        bestSegSeq.reserve( wordSize * 2 );
        for( size_t i = 0; i < wordSize; ++i )
        {
            bestSegSeq.push_back( i );
            bestSegSeq.push_back( i + 1 );
        }


        VTrie *trie = knowledge_->getTrie();
        meanainner::combineRetWithTrie( trie, words, types,
                bestSegSeq, 0, bestSegSeq.size() );

        /*
        for( size_t i = 0; i < wordSize; ++i )
        {
            cout << bestSegSeq[ i * 2 ] << " -> " << bestSegSeq[ i * 2 + 1 ] << endl;
        }
        */

        // convert to string lexicon
        ret.segment_.clear();
        createStringLexicon( words, bestSegSeq, ret.segment_, 0, bestSegSeq.size() );


        if( tagPOS == false )
        {
            delete[] types;
            return;
        }

        ret.candMetas_[ 0 ].posOffset_ = 0;
        ret.pos_.clear();
        knowledge_->getPOSTagger()->quick_tag_sentence_best(
                ret.segment_, bestSegSeq, types, 0, ret.segment_.size(), 0, ret.pos_ );
        delete[] types;
    }

    void CMA_ME_Analyzer::analysis_dictb(
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS
            )
    {
#ifndef ON_DEV
        // Initial Step 1: split as Chinese Character based
        vector<string> words;
        extractCharacter( sentence, words );

        if( words.empty() == true )
            return;

        // Initial Step 2nd: set character types
        CharType types[ (int)words.size() ];
        setCharType( words, types );

        int segN = N;
        SegRetType segment( segN );
        //SegTagger* segTagger = knowledge_->getSegTagger();

        // Segment 1st. Perform special characters



        N = segment.size();
        segRet.resize(N);

        VTrie *trie = knowledge_->getTrie();
        //TODO, only combine the first result
        for (int i = 0; i < N; ++i) {
            pair<vector<string>, double>& srcPair = segment[i];
            pair<vector<string>, double>& destPair = segRet[i];
            destPair.second = srcPair.second;
            if(i < 1){
                meanainner::combineRetWithTrie( trie, srcPair.first,
                        destPair.first, ctype_);
            }
            else{
                size_t srcSize = srcPair.first.size();
                for(size_t si = 0; si < srcSize; ++si){
                    string& val = srcPair.first[si];
                    if(!ctype_->isSpace(val.c_str()))
                        destPair.first.push_back(val);
                }
            }
        }

        if(!tagPOS)
            return;

        posRet.resize(N);
        POSTagger* posTagger = knowledge_->getPOSTagger();
        for (int i = 0; i < N; ++i) {
            posTagger->tag_sentence_best(segRet[i].first, posRet[i], 0,
                segRet[i].first.size());
        }

#endif
    }


    void CMA_ME_Analyzer::analysis_fmincover(
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS
            )
    {
        static CandidateMeta DefCandidateMeta;

        ret.setIncrementedWordOffset(false);

        // Initial Step 1: split as Chinese Character based
        StringVectorType words;
        extractCharacter( sentence, words );

        if( words.empty() == true )
            return;

        // Initial Step 2nd: set character types
        CharType* types = new CharType[ (int)words.size() ];
        setCharType( words, types );

        ret.candMetas_.clear();
        ret.candMetas_.push_back( DefCandidateMeta );
        ret.candMetas_[ 0 ].segOffset_ = 0;
        ret.candMetas_[ 0 ].score_ = 1.0;

        PGenericArray<size_t> bestSegSeq;

        VTrie *trie = knowledge_->getTrie();
        fmincover::parseFMinCoverString(
                bestSegSeq, words, types, trie, 0, words.size() );

        // convert to string lexicon
        ret.segment_.clear();
        createStringLexicon( words, bestSegSeq, ret.segment_, 0, bestSegSeq.size() );

        ret.candMetas_[ 0 ].wdOffset_ = 0;
        ret.wordOffset_.clear();
        ret.wordOffset_.reserve( ret.segment_.size() );
        createWordOffset( bestSegSeq, 0, bestSegSeq.size(), ret.wordOffset_ );


        if( tagPOS == false )
        {
            delete[] types;
            return;
        }

        ret.candMetas_[ 0 ].posOffset_ = 0;
        ret.pos_.clear();
        knowledge_->getPOSTagger()->quick_tag_sentence_best(
                ret.segment_, bestSegSeq, types, 0, ret.segment_.size(), 0, ret.pos_ );

        delete[] types;
    }

    void CMA_ME_Analyzer::splitToOneGram( const char* sentence, vector<vector<OneGramType> >& output )
    {
    	if( encodeType_ == Knowledge::ENCODE_TYPE_UTF8 )
		{
    		const unsigned char *uc = (const unsigned char *)sentence;
			if( uc[0] == 0xEF && uc[1] == 0xBB && uc[2] == 0xBF )
				sentence += 3;
		}

    	CTypeTokenizer token(ctype_, sentence);

    	const char* next = 0;

    	output.push_back( vector<OneGramType>() );
    	vector<OneGramType> *curFragment = &output[ output.size() - 1 ];

    	while( (next = token.next()) ){
    		CharType curType = ctype_->getBaseType( next );
    		switch( curType )
    		{
    		case CHAR_TYPE_DIGIT:
    		case CHAR_TYPE_LETTER:
    		case CHAR_TYPE_PUNC:
    			if( !curFragment->empty() )
				{
					output.push_back( vector<OneGramType>() );
					curFragment = &output[ output.size() - 1 ];
				}
				continue;
    		case CHAR_TYPE_SPACE:
    			continue;
    		default:
    			;//do nothing
    		}

			curFragment->push_back( next );
		}
    }

    void CMA_ME_Analyzer::getNGramResultImpl( const vector<vector<OneGramType> >& oneGram, const int n,
    		vector<string>& output )
    {
    	if( n < 1 )
    		return;
    	size_t fragSize = oneGram.size();
    	for(size_t fidx = 0; fidx < fragSize; ++fidx)
    	{
    		const vector<OneGramType>& curFragment = oneGram[ fidx ];
    		int size = static_cast<int>(curFragment.size());
			int lastIdx = size - n + 1;
			//cout<<"size="<<size<<",n="<<n<<",lastIdx="<<lastIdx<<endl;
			for( int i = 0; i < lastIdx; ++i )
			{
				string buf = curFragment[i];
				for( int j = 1; j < n; ++j )
					buf += curFragment[ i + j ];

				output.push_back( buf );
			}
    	}
    }

    void CMA_ME_Analyzer::resetIndexPOSList( bool defVal )
    {
    	posTable_->resetIndexPOSList( defVal );
    }

    int CMA_ME_Analyzer::setIndexPOSList( std::vector<std::string>& posList )
    {
    	return posTable_->setIndexPOSList( posList );
    }

    int CMA_ME_Analyzer::getCodeFromStr( const string& pos ) const
    {
    	return posTable_->getCodeFromStr( pos );
    }

    const char* CMA_ME_Analyzer::getStrFromCode( int index ) const
    {
    	return posTable_->getStrFromCode( index );
    }

    int CMA_ME_Analyzer::getPOSTagSetSize() const
    {
    	return posTable_->size();
    }

    void CMA_ME_Analyzer::extractCharacter( const char* sentence, StringVectorType& charOut )
    {
        static const string DefString;

        if( encodeType_ == Knowledge::ENCODE_TYPE_UTF8 )
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

    void CMA_ME_Analyzer::setCharType( StringVectorType& charIn, CharType* types )
    {
        int maxWordOff = (int)charIn.size() - 1;
        CharType preType = CHAR_TYPE_INIT;
        const char* curChar = charIn[ 0 ];
        const char* nextChar;
        for( int i = 0; i < maxWordOff; ++i )
        {
            nextChar = charIn[ i + 1 ];
            types[i] = preType = ctype_->getCharType(
                    curChar, preType, nextChar );
            curChar = nextChar;
        }
        types[ maxWordOff ] = ctype_->getCharType( curChar, preType, 0 );
    }

    void CMA_ME_Analyzer::createStringLexicon(
            StringVectorType& words,
            PGenericArray<size_t>& segSeq,
            StringVectorType& out,
            size_t beginIdx,
            size_t endIdx
            )
    {
        size_t curFreeLen = out.freeLen();
        size_t segSeqSize = ( endIdx - beginIdx ) / 2;
        size_t minLen = 0;
        size_t* segSeqItr = &segSeq[ beginIdx ];
        // minLen is used collect character number now
        for( size_t i = beginIdx; i < endIdx; i += 2 )
        {
            size_t seqStartIdx = *segSeqItr;
            ++segSeqItr;
            size_t seqEndIdx = *segSeqItr;
            ++segSeqItr;
            if( seqStartIdx >= seqEndIdx )
                break;
            minLen += seqEndIdx - seqStartIdx;
        }
        // convert to bytes, it will wasted some bytes
        minLen = minLen * 4 + segSeqSize + 10;
        if( minLen > curFreeLen )
        {
            out.reserve( out.dataLen_ + minLen - curFreeLen );
        }
        out.offsetVec_.reserve( out.size() + (size_t)(segSeqSize / 2) );

        // Converting integer to string
        char* outPtr = out.endPtr_;
        segSeqItr = &segSeq[ beginIdx ];
        for( size_t i = beginIdx; i < endIdx; i += 2 )
        {
            size_t seqStartIdx = *segSeqItr;
            ++segSeqItr;
            size_t seqEndIdx = *segSeqItr;
            ++segSeqItr;
            if( seqStartIdx >= seqEndIdx )
                break;
            out.offsetVec_.push_back( outPtr - out.data_ );
            for( size_t wordIdx = seqStartIdx; wordIdx < seqEndIdx; ++wordIdx )
            {
                const char* wordPtr = words[ wordIdx ];
                while( *wordPtr != 0 )
                {
                    *outPtr = *wordPtr;
                    ++outPtr;
                    ++wordPtr;
                }
            }

            *outPtr = 0;
            ++outPtr;
        }

        out.endPtr_ = outPtr;
    }


    void CMA_ME_Analyzer::createWordOffset(
            PGenericArray<size_t>& segSeq,
            size_t beginIdx,
            size_t endIdx,
            PGenericArray<size_t>& out
            )
    {
        if( beginIdx >= endIdx )
            return;
        size_t lastIdx = segSeq[ beginIdx ];
        size_t wordOffset = 0;
        for( size_t i = beginIdx + 1; i < endIdx; i += 2 )
        {
            size_t seqStartIdx = segSeq[ i - 1 ];
            size_t seqEndIdx = segSeq[ i ];
            if( seqStartIdx >= seqEndIdx )
                break;

            if( seqStartIdx > lastIdx )
            {
                ++wordOffset;
                lastIdx = seqStartIdx;
            }

            out.push_back( wordOffset );
        }
    }

}
