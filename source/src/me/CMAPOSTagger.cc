/** \file CMAPOSTagger.cc
 * \brief for the POS tagger using MaxEnt Model.
 *
 * \author vernkin
 *
 * Created on March 20, 2009, 10:46 AM
 */

#include <vector>

#include "VTrie.h"

#include "icma/me/CMAPOSTagger.h"
#include "icma/util/io_util.h"

#define ON_DEV

namespace cma{

string POS_BOUNDARY = "BoUnD";
const char* POS_BOUNDARY_CSTR = POS_BOUNDARY.c_str();

namespace posinner{

inline void get_pos_zh_scontext_1(vector<string>& words, string& tag_1,
        string& tag_2, int index, vector<string>& context,
        CMA_WType& wtype){
    CMA_WType::WordType type = wtype.getWordType(words[index].data());

    switch(type){
        case CMA_WType::WORD_TYPE_DATE:
            context.push_back("iD");
            return;
        case CMA_WType::WORD_TYPE_LETTER:
            context.push_back("iL");
            return;
        case CMA_WType::WORD_TYPE_NUMBER:
            context.push_back("iN");
            return;
        case CMA_WType::WORD_TYPE_PUNC:
            context.push_back("iP");
            return;
        default:
            break;
    }

    int n = (int)words.size();
    string wa[5]; //word array    
    wa[0] = index > 1 ? words[index-2] : POS_BOUNDARY;
    wa[1] = index > 0 ? words[index-1] : POS_BOUNDARY;
    wa[2] = words[index];
    wa[3] = (index < n - 1) ? words[index+1] : POS_BOUNDARY;
    wa[4] = (index < n - 2) ? words[index+2] : POS_BOUNDARY;

    /*
    context.push_back("CO=" + wa[2]);
    context.push_back("C-1=" + wa[1]);
    context.push_back("C-2=" + wa[0]);
    context.push_back("C1=" + wa[3]);

    context.push_back("C-2,-1=" + wa[0] + "," + wa[1]);
    context.push_back("C-1,0=" + wa[1] + "," + wa[2]);
    context.push_back("C0,1=" + wa[2] + "," + wa[3]);
    context.push_back("C-1,1=" + wa[1] + "," + wa[3]);

    context.push_back("T-1=" + tag_1);
    context.push_back("T-2,-1="+tag_2+","+tag_1);
    */
    //context.push_back("C2=" + wa[4]);


    //context.push_back("C-2=" + wa[0]);
    context.push_back("C-1=" + wa[1]);   
    context.push_back("CO=" + wa[2]);
    context.push_back("C1=" + wa[3]);
    //context.push_back("C2=" + wa[3]);

    context.push_back("C-2,-1=" + wa[0] + "," + wa[1]);
    context.push_back("C-1,0=" + wa[1] + "," + wa[2]);
    context.push_back("C0,1=" + wa[2] + "," + wa[3]);
    //context.push_back("C1,2=" + wa[3] + "," + wa[4]);
    
    context.push_back("C-1,1=" + wa[1] + "," + wa[3]);

    //context.push_back("C-2,-1,0=" + wa[0] + "," + wa[1] + "," + wa[2]);
    context.push_back("C-1,0,1=" + wa[1] + "," + wa[2] + "," + wa[3]);
    //context.push_back("C0,1,2=" + wa[2] + "," + wa[3] + "," + wa[4]);

    //context.push_back("T-1=" + tag_1);
    context.push_back("T-2,-1="+tag_2+","+tag_1);

}

inline void get_pos_zh_scontext_postagger(
        StringVectorType& words,
        const char* tag_1,
        const char* tag_2,
        int index,
        int endIdx,
        vector<string>& context
        )
{

    int n = endIdx;
    string wa[5]; //word array
    wa[0] = index > 1 ? words[index-2] : POS_BOUNDARY;
    wa[1] = index > 0 ? words[index-1] : POS_BOUNDARY;
    wa[2] = words[index];
    wa[3] = (index < n - 1) ? words[index+1] : POS_BOUNDARY;
    wa[4] = (index < n - 2) ? words[index+2] : POS_BOUNDARY;

    //context.push_back("C-2=" + wa[0]);
    context.push_back("C-1=" + wa[1]);
    context.push_back("CO=" + wa[2]);
    context.push_back("C1=" + wa[3]);
    //context.push_back("C2=" + wa[3]);

    context.push_back("C-2,-1=" + wa[0] + "," + wa[1]);
    context.push_back("C-1,0=" + wa[1] + "," + wa[2]);
    context.push_back("C0,1=" + wa[2] + "," + wa[3]);
    //context.push_back("C1,2=" + wa[3] + "," + wa[4]);

    context.push_back("C-1,1=" + wa[1] + "," + wa[3]);

    //context.push_back("C-2,-1,0=" + wa[0] + "," + wa[1] + "," + wa[2]);
    context.push_back("C-1,0,1=" + wa[1] + "," + wa[2] + "," + wa[3]);
    //context.push_back("C0,1,2=" + wa[2] + "," + wa[3] + "," + wa[4]);

    //context.push_back("T-1=" + tag_1);
    string tmp( "T-2,-1=" );
    tmp.append( tag_2 ).append( "," ).append( tag_1 );
    context.push_back( tmp );

}

} //end namespace posinner

void get_pos_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context, CMA_CType *ctype){
    string& tag_1 = i > 0 ? tags[i-1] : POS_BOUNDARY;
    string& tag_2 = i > 1 ? tags[i-2] : POS_BOUNDARY;
    CMA_WType wtype(ctype);
    posinner::get_pos_zh_scontext_1(words, tag_1, tag_2, (int)i, context, wtype);
}

void pos_train(const char* file, const string& modelPath, Knowledge::EncodeType encType,
        string posDelimiter, bool isLargeCorpus,
        const char* extractFile, string method, size_t iters,float gaussian){
      TrainerData data(get_pos_zh_scontext, encType, posDelimiter);
      if(isLargeCorpus){
    	  cout<<"[Info] As the corpus is large, some parameters are set larger."<<endl;
    	  data.rareFreq_ = 4;
    	  data.cutoff_ = 10;
    	  data.rareCutoff_ = 5;
      }
      train(&data, file, modelPath, extractFile, method, iters, gaussian, true);
}


inline void insertCandidate(string& pos, int index, double score,
        POSTagUnit* candidates, int& lastIndex, size_t& canSize, size_t N){
    if(canSize == 0 || (N==1 && score > candidates[lastIndex].score)){
        POSTagUnit& head = candidates[0];
        head.pos = pos;
        head.index = index;
        head.score = score;
        head.previous = -1;
        canSize = 1;
        lastIndex = 0;
        return;
    }

    //append to the end
    if(score <= candidates[lastIndex].score){
        #ifdef EN_ASSERT
        assert(canSize < N);
        #endif
        POSTagUnit& unit = candidates[canSize];
        unit.score = score;
        unit.index = index;
        unit.pos = pos;
        unit.previous = lastIndex;
        lastIndex = canSize;
        ++canSize;
        return;
    }

    //the index last to be compared
    int lIndex = lastIndex;
    //the current index;
    int cIndex = candidates[lIndex].previous;
    while(cIndex >= 0 && candidates[cIndex].score < score){
        lIndex = cIndex;
        cIndex = candidates[cIndex].previous;
    }

    int destIndex;
    //have to replace
    if(canSize == N){
        destIndex = lastIndex;
        //if not replace the last one, update the lastIndex
        if(lIndex != lastIndex){
            lastIndex = candidates[lastIndex].previous;
            candidates[lIndex].previous = destIndex;
        }
    }
    //there are more space
    else{
        destIndex = canSize;
        candidates[lIndex].previous = destIndex;
        ++canSize;
    }

    POSTagUnit& unit = candidates[destIndex];
    unit.score = score;
    unit.index = index;
    unit.pos = pos;
    unit.previous = cIndex;
}

POSTagger::POSTagger(const string& model, VTrie* pTrie, bool loadModel )
        : isInnerTrie_(false){
    if( loadModel )
    {
        me.load( model );
    }

    assert(pTrie);
    trie_ = pTrie;
    posVec_.reserve( 410000 );
    //reserved the location offset 0
    posVec_.push_back( POSUnitType() );
}

POSTagger::POSTagger(const string& model, const char* dictFile) : isInnerTrie_(true){
    me.load(model);

    trie_ = new VTrie();
    //reserved the location offset 0
    posVec_.push_back( POSUnitType() );
    ifstream in(dictFile);
    assert(in);
    string line;
    while(!in.eof()){
        getline(in, line);
        appendWordPOS(line);
    }
    in.close();
}

POSTagger::~POSTagger(){
    if(isInnerTrie_)
        delete trie_;
}

void POSTagger::tag_word(vector<string>& words, int index, size_t N,
        string* tags, POSTagUnit* candidates, int& lastIndex, size_t& canSize,
        double initScore, int candidateNum, CMA_WType& wtype){
    vector<string> context;

    VTrieNode node;
    trie_->search( words[index].data(), &node );

    bool exists = node.data > 0;
    string& tag_1 = index > 0 ? tags[index-1] : POS_BOUNDARY;
    string& tag_2 = index > 1 ? tags[index-2] : POS_BOUNDARY;
    posinner::get_pos_zh_scontext_1(words, tag_1, tag_2, index, context, wtype);

    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(context, outcomes, false);

    size_t outSize = outcomes.size();
    if(exists){
        POSUnitType& posSet = posVec_[node.data];
        for(size_t i=0; i<outSize; ++i){
            pair<outcome_type, double>& pair = outcomes[i];
            //whether exists such pos
            if( posSet.contains( pair.first.c_str() ) == true )
                continue;
            double score = pair.second * initScore;
            if(canSize >= N && score <= candidates[lastIndex].score)
                continue;
            insertCandidate(pair.first, candidateNum, score, candidates,
                    lastIndex, canSize, N);
        }

        if(canSize)
            return;
    }


    for(size_t i=0; i<outSize; ++i){
        pair<outcome_type, double>& pair = outcomes[i];
        double score = pair.second * initScore;
        if(canSize >= N && score <= candidates[lastIndex].score)
            continue;
        insertCandidate(pair.first, candidateNum, score, candidates,
                lastIndex, canSize, N);
    }
}

void POSTagger::tag_sentence(vector<string>& words, size_t N, size_t retSize,
            vector<pair<vector<string>, double> >& segment){
    size_t n = words.size();

    //h0, h1 and score don't need to initialize
    string** _array1 = new string*[N];
    string** _array2 = new string*[N];
    for(size_t i = 0; i < N; i++)
    {
        _array1[i] = new string[n];
        _array2[i] = new string[n];
    }

    // TODO : Need to check if following modification is right (by dohyun)
    string** h0 = _array1;
    string** h1 = _array2;

    string** hTmp;

    double* scores = new double[N];
    scores[0] = 1.0;

    size_t h0Size = 1;

    POSTagUnit* candidates = new POSTagUnit[N];
    //last index of candidates
    int lastIndex;
    //the size of the candidates
    size_t canSize;

    CMA_WType wtype(ctype_);

    for(size_t i=0; i<n; ++i){
        lastIndex = -1;
        canSize = 0;
        #ifdef EN_ASSERT
            assert(h0Size <= N);
        #endif

        for(size_t j=0; j<h0Size; ++j){
            tag_word(words, i, N, h0[j], candidates, lastIndex, canSize,
                  scores[j], j, wtype);
        }

        //generate the N-best
        for(size_t k=canSize-1; k>=0; --k){
            POSTagUnit& unit = candidates[lastIndex];
            lastIndex = unit.previous;

            for(size_t jj=0; jj<i; ++jj){
                h1[k][jj] = h0[unit.index][jj];
            }

            h1[k][i] = unit.pos;
            scores[k] = unit.score;
        }

        //swap h0 and h1
        hTmp = h0;
        h0 = h1;
        h1 = hTmp;

        h0Size = canSize;
    }
    if(retSize < h0Size)
        h0Size = retSize;

    //store the result
    for(size_t k=0; k<h0Size; ++k){
        pair<vector<string>,double>& pair = segment[k];
        pair.second = (pair.second > 0) ? (pair.second * scores[k]) : scores[k];
        vector<string>& seg = pair.first;
        string* tags = h0[k];
        for(size_t i = 0; i < n; ++i){
            seg.push_back(tags[i]);
        }
    }


    for(size_t i = 0; i < N; i++)
    {
        delete[] _array1[i], _array2[i];
        _array1[i] = 0;
        _array2[i] = 0;
    }
    delete[] _array1, _array2, scores, candidates;
}

void POSTagger::tag_file(const char* inFile, const char* outFile){
#ifndef ON_DEV
    ifstream in(inFile);
    ofstream out(outFile);

    string line;

    while(!in.eof()){
        getline(in, line);
        if(line.length() == 0){
            out<<endl;
            continue;
        }
        
        vector<string> words;
        TOKEN_STR(line, words);
        if(words.size() == 0){
            out<<endl;
            continue;
        }

        vector<string> posRet;
        tag_sentence_best(words, posRet, 0, words.size());

        size_t maxIndex = words.size() - 1;
        for(size_t i=0; i<maxIndex; ++i){
            out<<words[i] << "/"<< posRet[i]<<" ";
        }

        out<<words[maxIndex] << "/"<< posRet[maxIndex]<<endl;
    }

    in.close();
    out.close();
#endif
}

void POSTagger::tag_sentence_best(
        StringVectorType& words,
        PGenericArray<size_t>& segSeq,
        CharType* types,
        size_t wordBeginIdx,
        size_t wordEngIdx,
        size_t seqStartIdx,
        PGenericArray< const char* >& posRet
        )
{
    int word2SeqIdxOffset = (int)seqStartIdx - (int)wordBeginIdx * 2;
    posRet.reserve( posRet.usedLen() + wordEngIdx - wordBeginIdx );

    CMA_WType wtype(ctype_);
    vector<string> context;
    int posIndex = -1;

    for( size_t index = wordBeginIdx; index < wordEngIdx; ++index )
    {
        size_t seqIdx = index * 2 + word2SeqIdxOffset;
        size_t seqWordBeginIdx = segSeq[ seqIdx ];
        size_t seqWordEndIdx =  segSeq[ seqIdx + 1 ];

        //cout << "index = " << index << ", seqIdx = " << seqIdx << ", words = " << words[ index ] << endl;
        if( seqWordEndIdx <= seqWordBeginIdx )
            break;

        CMA_WType::WordType wordT = wtype.getWordType( types, seqWordBeginIdx, seqWordEndIdx );

        switch(wordT){
            case CMA_WType::WORD_TYPE_PUNC:
                posRet.push_back( puncPOS.c_str() );
                continue;
            case CMA_WType::WORD_TYPE_NUMBER:
                posRet.push_back( numberPOS.c_str() );
                continue;
            case CMA_WType::WORD_TYPE_LETTER:
                posRet.push_back( letterPOS.c_str() );
                continue;
            case CMA_WType::WORD_TYPE_DATE:
                posRet.push_back( datePOS.c_str() );
                continue;
            default:
                break;
        }

        VTrieNode node;
        trie_->search( words[ index ], &node );
        if( node.data < 0 )
        {
            posRet.push_back( defaultPOS.c_str() );
            continue;
        }

        POSUnitType& posSet = posVec_[node.data];
        if( posSet.empty() == true )
        {
            posRet.push_back( defaultPOS.c_str() );
            continue;
        }
        else if( posSet.size() == 1 )
        {
            posRet.push_back( posSet[ 0 ] );
            continue;
        }

        const char* pos = NULL;
        context.clear();
        const char* tag_1 = index > wordBeginIdx ? posRet[ index - 1 ] : POS_BOUNDARY_CSTR;
        const char* tag_2 = index > wordBeginIdx + 1 ? posRet[ index - 2 ] : POS_BOUNDARY_CSTR;
		
		
		//cout << tag_1 << ", ";
		//cout << tag_2 << endl;
        posinner::get_pos_zh_scontext_postagger(
                words, tag_1, tag_2, index, wordEngIdx, context );

        vector<pair<outcome_type, double> > outcomes;
        me.eval_all(context, outcomes, false);

        //find the best pos
        double bestScore = -1.0;
        size_t outSize = outcomes.size();

        for( size_t k=0; k<outSize; ++k )
        {
            pair<outcome_type, double>& pair = outcomes[k];
            if( pair.second > bestScore && ( posIndex = posSet.index( pair.first.c_str() ) >= 0 ) )
            {
                bestScore = pair.second;
                pos = posSet[ posIndex ];
            }
        }

        if( pos == NULL )
        {
            pos = defaultPOS.c_str();
        }
        posRet.push_back(pos);
    }
}

void POSTagger::quick_tag_sentence_best(
        StringVectorType& words,
        PGenericArray<size_t>& segSeq,
        CharType* types,
        size_t wordBeginIdx,
        size_t wordEngIdx,
        size_t seqStartIdx,
        PGenericArray< const char* >& posRet
        )
{
    int word2SeqIdxOffset = (int)seqStartIdx - (int)wordBeginIdx * 2;
    posRet.reserve( posRet.usedLen() + wordEngIdx - wordBeginIdx );

    CMA_WType wtype(ctype_);
    for( size_t index = wordBeginIdx; index < wordEngIdx; ++index )
    {
        size_t seqIdx = index * 2 + word2SeqIdxOffset;
        size_t seqWordBeginIdx = segSeq[ seqIdx ];
        size_t seqWordEndIdx =  segSeq[ seqIdx + 1 ];
        if( seqWordEndIdx <= seqWordBeginIdx )
            break;
        CMA_WType::WordType wordT = wtype.getWordType( types, seqWordBeginIdx, seqWordEndIdx );

        switch(wordT){
            case CMA_WType::WORD_TYPE_PUNC:
                posRet.push_back( puncPOS.c_str() );
                continue;
            case CMA_WType::WORD_TYPE_NUMBER:
                posRet.push_back( numberPOS.c_str() );
                continue;
            case CMA_WType::WORD_TYPE_LETTER:
                posRet.push_back( letterPOS.c_str() );
                continue;
            case CMA_WType::WORD_TYPE_DATE:
                posRet.push_back( datePOS.c_str() );
                continue;
            default:
                break;
        }

        const char* word = words[ index ];
        VTrieNode node;
        trie_->search( word, &node );
        if( node.data > 0 )
        {
            POSUnitType& posSet = posVec_[node.data];
            if( posSet.empty() == false )
            {
                posRet.push_back( posSet[ 0 ] );
                continue;
            }
        }

        posRet.push_back( defaultPOS.c_str() );
    }
}

bool POSTagger::appendWordPOS(string& line)
{
    StringArray tokens;
    StringArray::tokenize( line.c_str(), tokens );

    size_t n = tokens.size();
    if( n == 0 )
    {
        return false;
    }

    string word = tokens[0];
    replaceAll(word, "_", " ");

    POSUnitType* posSet = 0;
    //try to search first
    VTrieNode node;
    trie_->search(word.data(), &node);
    //already exits
    if(node.data > 0){
        posSet = &(posVec_[node.data]);
    }else
    {
        //get the right offset (offset 0 is reserved)
        node.data = (int)posVec_.size();
        //insert new key
        posVec_.push_back( POSUnitType() );
        posSet = &(posVec_.back());

        trie_->insert(word.data(), &node);
    }

    if( posSet->empty() == true )
    {
        tokens.removeHead();
        posSet->swap( tokens );
    }
    else
    {
        for( size_t i = 1; i < n; ++i )
        {
            const char* str = tokens[ i ];
            if( posSet->contains( str ) == false )
                posSet->push_back( tokens[ i ] ) ;
        }
    }

    return true;
}

}
