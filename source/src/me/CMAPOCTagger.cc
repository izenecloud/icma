/** \file CMAPOCTagger.cc
 * \brief for the POC tagger using MaxEnt Model.
 *
 * \author vernkin
 *
 * Created on March 24, 2009, 8:52 PM
 */

#include <vector>

#include "strutil.h"
#include "icma/me/CMAPOCTagger.h"
#include "icma/util/CPPStringUtils.h"
#include "icma/util/io_util.h"
#include "icma/util/StrBasedVTrie.h"
#include "icma/type/cma_wtype.h"

namespace cma{

#define POC_TAG_INIT 0
#define POC_TAG_B 1
#define POC_TAG_E 2

#define POC_TAG_B_NAME "B"
#define POC_TAG_E_NAME "E"

#define DEFAULT_POC POC_TAG_B

const string POC_BOUNDARY = "BoUnD";

string CharTypeArray[CHAR_TYPE_NUM];

namespace pocinner{

#define CONTEXT_CHAR_LEN 4

/**
 * Inner function to get the context of the POC, invoked by segmentation method
 */
inline void get_poc_zh_context_seg(vector<string>& words, CharType *types,
        size_t index, vector<string>& context, CMA_CType *ctype){

	CharType t_1 = index > 0 ? types[index - 1] : CHAR_TYPE_INIT;
	CharType t0 = types[ index ];
	if( index > 0 )
	{
		bool match = false;
		switch( t_1 )
		{
		case CHAR_TYPE_DATE:
		case CHAR_TYPE_PUNC:
			match = true;
			break;
		case CHAR_TYPE_OTHER:
			match = t0 != CHAR_TYPE_OTHER;
			break;
		default:
			break;
		}

		if( !match && t0 == CHAR_TYPE_OTHER && t_1 != CHAR_TYPE_OTHER )
		{
			match = true;
		}

		if(match)
		{
			#ifdef DEBUG_POC_TAGGER
				cout<<"[Context] Set afEnt."<<endl;
			#endif
			context.push_back("afEnt");
			return;
		}
	}

    size_t n = words.size();


    if( t0 != CHAR_TYPE_OTHER )
    {
    	CharType t1;
    	string c1;
		if( (index + 1) < n )
		{
			c1 = words[ index + 1 ];
			t1 = types[ index + 1 ];
		}
		else
			t1 = ctype->getDefaultEndType( t0 );

    	context.resize(4);
        //context[++k] = "C0=" + wa[2];
        context[0] = "C1=" + c1;
        context[1] = "T-1=" + CharTypeArray[ t_1 ];
        context[2] = "T0=" + CharTypeArray[ t0 ];
        context[3] = "T+1=" + CharTypeArray[ t1 ];

		#ifdef DEBUG_POC_TAGGER
        for(int jj=0;jj<4;++jj)
        	cout<<"[Context] Seg Special Context "<<jj<<" : "<<context[jj]<<endl;
		#endif

        return;
    }

    string c0 = words[ index ];
    string c_2, c_1, c1;

    if( index > 0 )
    	c_1 = words[ index - 1 ];
    if( index > 1 )
        c_2 = words[ index - 2 ];
    if( (index + 1) < n )
    	c1 = words[ index + 1 ];

    // normal feature set
    context.resize(8);

    // Cn , n ∈ [−2, 1]
    context[0] = "C-2=" + c_2;
    context[1] = "C-1=" + c_1;
    context[2] = "C0=" + c0;
    context[3] = "C1=" + c1;

    //Cn Cn+1 , n ∈ [−2, 0]
    context[4] = "C-2,-1=" + c_2 + "," + c_1;
    context[5] = "C-1,0=" + c_1 + "," + c0;
    context[6] = "C0,1=" + c0 + "," + c1;

    //C−1 C1
    context[7] = "C-1,1=" + c_1 + "," + c1;
}

/**
 * Inner function to get the context of the POC
 */
inline void get_poc_zh_context_1(vector<string>& words, CharType *types, 
        size_t index, vector<string>& context, CMA_CType *ctype){

    int n = (int)words.size();
    int offset = (int)index - 2;
    
    string wa[CONTEXT_CHAR_LEN]; //word array
    CharType ta[CONTEXT_CHAR_LEN]; //type array
    CharType preType = CHAR_TYPE_INIT;
    if(!types && offset > 0){
        preType = ctype->getCharType(words[offset-1].data(), preType, words[offset].data());
    }

    for(int j = 0; j < CONTEXT_CHAR_LEN; ++j, ++offset){
        if(offset >= 0 && offset < n){
            wa[j] = words[offset];
            if(types){
                ta[j] = preType = types[offset];
            }else{
                const char* lastP = 0;
                if( offset < n - 1){
                    lastP = words[offset + 1].data();
                }
                preType = ctype->getCharType(wa[j].c_str(), preType, lastP);
                ta[j] = preType;
            }
        }else{
            wa[j] = POC_BOUNDARY;
            if(preType == CHAR_TYPE_DIGIT || preType == CHAR_TYPE_LETTER
            		|| preType == CHAR_TYPE_CHARDIGIT)
            	ta[j] = preType;
            else
            	ta[j] = CHAR_TYPE_OTHER;
        }
    }

    int k = -1;

     if(index > 0 && (ta[1] == CHAR_TYPE_DATE || ta[1] == CHAR_TYPE_PUNC)){
        context.resize(1);
        context[0] = "afEnt";
        return;
     }
        
    if(ta[2] != CHAR_TYPE_OTHER){
        context.resize(4);
        //context[++k] = "C0=" + wa[2];
        context[++k] = "C1=" + wa[3];
        context[++k] = "T-1=" + CharTypeArray[ta[1]];
        context[++k] = "T0=" + CharTypeArray[ta[2]];
        context[++k] = "T+1=" + CharTypeArray[ta[3]];

		#ifdef DEBUG_POC_TAGGER
        for(int jj=0;jj<4;++jj)
        	cout<<"special context "<<jj<<" : "<<context[jj]<<endl;
		#endif

        return;
    }

    if(index > 0 && ta[1] != CHAR_TYPE_OTHER){
        context.resize(1);
        context[0] = "afEnt";
        return;
    }

    // normal feature set
    context.resize(8);

    // Cn , n ∈ [−2, 1]
    context[++k] = "C-2=" + wa[0];
    context[++k] = "C-1=" + wa[1];
    context[++k] = "C0=" + wa[2];
    context[++k] = "C1=" + wa[3];
    //context[++k] = "C2=" + wa[4];

    //Cn Cn+1 , n ∈ [−2, 0]
    context[++k] = "C-2,-1=" + wa[0] + "," + wa[1];
    context[++k] = "C-1,0=" + wa[1] + "," + wa[2];
    context[++k] = "C0,1=" + wa[2] + "," + wa[3];
    //context[++k] = "C1,2=" + wa[3] + "," + wa[4];

    //C−1 C1
    context[++k] = "C-1,1=" + wa[1] + "," + wa[3];
    
}

/**
 *
 * \return true if the candidates is full
 */
inline void insertCandidate(uint8_t pocCode, int index, double score,
        POCTagUnit* candidates, int& lastIndex, size_t& canSize, size_t N){
    if(canSize == 0 || (N==1 && score > candidates[lastIndex].score)){
        POCTagUnit& head = candidates[0];
        head.pocCode = pocCode;
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
        POCTagUnit& unit = candidates[canSize];
        unit.score = score;
        unit.index = index;
        unit.pocCode = pocCode;
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

    POCTagUnit& unit = candidates[destIndex];
    unit.score = score;
    unit.index = index;
    unit.pocCode = pocCode;
    unit.previous = cIndex;
}

/**
 * Combine the poc tags to words
 */
inline void combinePOCToWord(vector<string>& words, size_t n, uint8_t* tags,
        vector<string>& seg){
    string strBuf;

    for(size_t i = 0; i < n; ++i){
        if(tags[i] == POC_TAG_B){
            if(!strBuf.empty())
                seg.push_back(strBuf);
            strBuf = words[i];
        }
        else if(tags[i] == POC_TAG_E){
            strBuf.append(words[i]);
        }
        else{
            assert(false && "Invalid POC Code in the BE tag set");
        }
    }
    //the remaining string
    if(!strBuf.empty())
        seg.push_back(strBuf);
}

} //end namespace pocinner



/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_context(vector<string>& words, vector<string>& tags, size_t index,
        bool rareWord, vector<string>& context, CMA_CType *ctype){

    pocinner::get_poc_zh_context_1(words, 0, index, context, ctype);
}

void poc_train(const char* file, const string& modelPath, Knowledge::EncodeType encType,
        string pocDelimiter,  bool isLargeCorpus,
        const char* extractFile, string method, size_t iters,float gaussian)
{
      SegTagger::initialize();
      TrainerData data(get_poc_zh_context, encType, pocDelimiter);
      if(isLargeCorpus){
		  cout<<"[Info] As the corpus is large, some parameters are set larger."<<endl;
		  data.rareFreq_ = 4;
		  data.cutoff_ = 10;
		  data.rareCutoff_ = 5;
      }
      train(&data, file, modelPath, extractFile, method, iters, gaussian, false);
}


SegTagger::SegTagger(const string& cateName, VTrie* posTrie, double eScore)
{
    SegTagger::initialize();
    me.load(cateName + ".model");

    trie_ = posTrie;
    setEScore(eScore);
}

SegTagger::~SegTagger()
{
}

void SegTagger::tag_word(vector<string>& words, CharType* types,
        int index, size_t N, uint8_t* tags, POCTagUnit* candidates,
        int& lastIndex, size_t& canSize, double initScore, int candidateNum){

    vector<string> context;

    pocinner::get_poc_zh_context_seg(words, types, index, context, ctype_);

    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(context, outcomes, false);

    size_t outSize = outcomes.size();
    for(size_t i=0; i<outSize; ++i){
        pair<outcome_type, double>& pair = outcomes[i];
        double score = pair.second * initScore;
        if(canSize >= N && score <= candidates[lastIndex].score)
            continue;
        uint8_t pocCode = (pair.first == POC_TAG_B_NAME) ? POC_TAG_B : POC_TAG_E;
        pocinner::insertCandidate(pocCode, candidateNum, score, candidates,
                lastIndex, canSize, N);
    }
}

void SegTagger::preProcess(vector<string>& words, uint8_t* tags)
{
	CMA_WType wtype(ctype_);

	memset(tags, POC_TAG_INIT, words.size());
	tags[0] = POC_TAG_B;
	StrBasedVTrie strTrie(trie_);

	int size = words.size();
	int start = 0;
	while(start < size)
	{
		int idx = start;
		strTrie.firstSearch(words[idx++].data());
		int maxLastIdx = 0; //exclude the maxLastIdx itself
		while(strTrie.completeSearch && idx < size)
		{
			strTrie.search(words[idx++].data());
			if(strTrie.exists())
				maxLastIdx = idx;
		}

		#ifdef DEBUG_POC_TAGGER_TRIE
			cout<<"Loop A Circle, begin with " << words[start] <<": start="<<start
				<<",maxLastIdx="<<maxLastIdx<<endl;
		#endif

		// exists word begin with start index and length at least MIN_PRE_WORD_LEN
		if((maxLastIdx - start) >= MIN_PRE_WORD_LEN)
		{
			//check the wtype
			string init(words[start]);
			for(int i=start+1; i<maxLastIdx; ++i)
			{
				init.append(words[i]);
			}

			//here don't dealt with digits, letters, data and other entities
			if(wtype.getWordType(init.data()) == CMA_WType::WORD_TYPE_OTHER)
			{
				#ifdef DEBUG_POC_TAGGER
					cout<<"PreProcess combine "<<init<<",start="<<start<<
							",maxLastIdx="<<maxLastIdx<<endl;
				#endif
				tags[start] = POC_TAG_B;
				for( ++start; start < maxLastIdx; ++start)
					tags[start] = POC_TAG_E;
				tags[start] = POC_TAG_B; //here also finish reset start
			}
			else
			{
				++start; //simply increment
			}
		}
		else
		{
			++start; //simply increment
		}
	}
}


void SegTagger::seg_sentence(vector<string>& words, CharType* types,
        size_t N, size_t retSize,
        vector<pair<vector<string>, double> >& segment){
    size_t n = words.size();

    //h0, h1 and score don't need to initialize
    uint8_t _array1[N][n];
    uint8_t _array2[N][n];

    //pre-process
    preProcess(words, _array1[0]);
    memcpy(_array2[0], _array1[0], n);
    //initialize all the array
    for(size_t i=1; i<N; ++i)
    {
    	memcpy(_array1[i], _array1[0], n);
    	memcpy(_array2[i], _array1[0], n);
    }


    uint8_t (*h0)[n] = _array1;
    uint8_t (*h1)[n] = _array2;

    uint8_t (*hTmp)[n];

    double scores[N];
    scores[0] = 1.0;
    
    size_t h0Size = 1;

    POCTagUnit candidates[N];
    //last index of candidates
    int lastIndex;
    //the size of the candidates
    size_t canSize;

    for(size_t i=0; i<n; ++i){
        if(_array1[0][i] != POC_TAG_INIT )
        	continue;
    	lastIndex = -1;
        canSize = 0;
        #ifdef EN_ASSERT
            assert(h0Size <= N);
        #endif

        for(size_t j=0; j<h0Size; ++j){
            tag_word(words, types, i, N, h0[j], candidates, lastIndex,
                    canSize, scores[j], j);
        }
        
        //generate the N-best
        for(int k=canSize-1; k>=0; --k){
            POCTagUnit& unit = candidates[lastIndex];
            lastIndex = unit.previous;
            memcpy(h1[k], h0[unit.index], i);            
            h1[k][i] = unit.pocCode;
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
    if(segment.size() != h0Size)
		segment.resize(h0Size);
    for(size_t k=0; k<h0Size; ++k){
        uint8_t* tags = h0[k];
        pair<vector<string>,double>& pair = segment[k];
        pair.second = (pair.second > 0) ? (pair.second * scores[k]) : scores[k];
        pocinner::combinePOCToWord(words, n, tags, pair.first);
    }
}

void SegTagger::tag_file(const char* inFile, const char* outFile,
        string encType){
    ifstream in(inFile);
    ofstream out(outFile);

    string line;

    CMA_CType *cType = CMA_CType::instance(Knowledge::decodeEncodeType(encType.c_str()));
    CTypeTokenizer ctypeCt(cType);

    while(!in.eof()){
        getline(in, line);
        if(line.length() == 0){
            out<<endl;
            continue;
        }

        ctypeCt.assign(line.data());

        vector<string> words;
        const char* nextPtr;
        while((nextPtr = ctypeCt.next())){
            words.push_back(nextPtr);
        }

        if(words.empty()){
            out<<endl;
            continue;
        }

        int maxWordOff = (int)words.size() - 1;
        CharType types[maxWordOff + 1];
        CharType preType = CHAR_TYPE_INIT;
        for(int i=0; i<maxWordOff; ++i){
            types[i] = preType = ctype_->getCharType(words[i].data(),
                    preType, words[i+1].data());
        }

        types[maxWordOff] = ctype_->getCharType(words[maxWordOff].data(),
                    preType, 0);

        vector<string> best;
        seg_sentence_best(words, types, best);

        //print the result
        size_t maxIndex = best.size() - 1;
        for(size_t i=0; i<maxIndex; ++i){
            out<<best[i]<<" ";
        }
        out<<best[maxIndex]<<endl;
    }

    in.close();
    out.close();
}

#define BACK_FIX_END_TAG if(!strTrie.exists()){ \
		for(size_t i=index-1; i>lastExistIndex && types[i] == CHAR_TYPE_OTHER; --i) \
			pocRet[i] = POC_TAG_B;}

void SegTagger::seg_sentence_best(vector<string>& words, CharType* types,
        vector<string>& segment){
    size_t n = words.size();
    uint8_t pocRet[n];

    #ifdef USE_STRTRIE
        StrBasedVTrie strTrie(trie_);
        int wordLen = 0;
        preProcess(words, pocRet);
    #endif

    size_t lastExistIndex = 0;

    for(size_t index=0; index<n; ++index){
		#ifdef DEBUG_POC_TAGGER
            cout << "Check at " << index << ": " << words[index] << ", type = " <<
				  CharTypeArray[types[index]]<<endl;
        #endif

    	if( pocRet[index] != POC_TAG_INIT )
        {
        	if( pocRet[index] == POC_TAG_B && index < n -1 &&
        			pocRet[index + 1] == POC_TAG_INIT)
        	{
        		wordLen = 1;
        		lastExistIndex = index;
        		strTrie.firstSearch(words[ index ].c_str());
        	}

        	continue;
        }
    	#ifdef USE_STRTRIE
            const char* curPtr = words[ index ].c_str();
        #endif
            
        vector<string> context;
        pocinner::get_poc_zh_context_seg(words, types, index, context, ctype_);

        #ifdef DEBUG_POC_TAGGER
        /*int n = context.size();
        for(int ii=0; ii<n; ++ii){
            cout<<"Context "<<ii<<":"<<context[ii]<<endl;
        }*/
        #endif

        vector<pair<outcome_type, double> > outcomes;
        me.eval_all(context, outcomes, false);
        pair<outcome_type, double>& pair0 = outcomes[0];
        double tagEScore = (pair0.first == POC_TAG_E_NAME) ?
              pair0.second : ( 1 - pair0.second );

        #ifdef DEBUG_POC_TAGGER
            cout<<"tagEScore "<<tagEScore<<endl;
        #endif

        //no check if the POC tag is B
        if(tagEScore <= 0.5){
        	BACK_FIX_END_TAG
            pocRet[index] = POC_TAG_B;
        	lastExistIndex = index;
			#ifdef DEBUG_POC_TAGGER
				cout<<"BACK_FIX_END_TAG by tagEScore <= 0.5 [1]"<<endl;
			#endif
            wordLen = 1;
            #ifdef USE_STRTRIE
				strTrie.firstSearch(curPtr);
            #endif
            continue;
        }

        #ifndef USE_STRTRIE
            pocRet[index] = POC_TAG_E;
        #else
            strTrie.search(curPtr);
            if(strTrie.exists())
            	lastExistIndex = index;
            #ifdef DEBUG_POC_TAGGER
                cout<<"Search StrVTrie, finish: "<<strTrie.completeSearch<<
					", exists: "<<strTrie.exists()<<endl;
            #endif

            if(tagEScore >= eScore_ || types[index] != CHAR_TYPE_OTHER){
                ++wordLen;
            	if(types[index] != CHAR_TYPE_OTHER || wordLen < MAX_PROP_WORD_LEN || strTrie.completeSearch)
					pocRet[index] = POC_TAG_E;
            	else
            	{
					#ifdef DEBUG_POC_TAGGER
						cout<<"BACK_FIX_END_TAG when tagEScore >= eScore_ [2]"<<endl;
					#endif
            		BACK_FIX_END_TAG
            		pocRet[index] = POC_TAG_B;
            		lastExistIndex = index;
            		wordLen = 1;

            		strTrie.firstSearch(curPtr);
            	}
            	continue;
            }

            if(strTrie.completeSearch)
            {
                pocRet[index] = POC_TAG_E;
                ++wordLen;
            }
            else
            {
				#ifdef DEBUG_POC_TAGGER
					cout<<"BACK_FIX_END_TAG when cannot complete search [3]"<<endl;
				#endif
            	BACK_FIX_END_TAG

            	pocRet[index] = POC_TAG_B;
                lastExistIndex = index;
                wordLen = 1;

                strTrie.firstSearch(curPtr);
            }
        #endif

    }

    pocinner::combinePOCToWord(words, n, pocRet, segment);
}

#undef BACK_FIX_END_TAG

bool POC_INIT_FLAG = false;

void SegTagger::initialize(){
    if(POC_INIT_FLAG)
        return;
    POC_INIT_FLAG = true;

    CharTypeArray[CHAR_TYPE_INIT] = "I";
    CharTypeArray[CHAR_TYPE_DIGIT] = "N";
    // It is OK to use the same name as CHAR_TYPE_DIGIT
    CharTypeArray[CHAR_TYPE_CHARDIGIT] = "N";
    CharTypeArray[CHAR_TYPE_PUNC] = "P";
    CharTypeArray[CHAR_TYPE_SPACE] = "S";
    CharTypeArray[CHAR_TYPE_DATE] = "D";
    CharTypeArray[CHAR_TYPE_LETTER] = "L";
    CharTypeArray[CHAR_TYPE_OTHER] = "O";
}

}
