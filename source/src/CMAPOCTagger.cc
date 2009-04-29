#include <vector>

#include "CMAPOCTagger.h"
#include "CPPStringUtils.h"
#include "strutil.h"
#include "tokenizer.h"
#include "cma_ctype.h"

namespace cma{

string POC_EMPTY_STR = "";

#define POC_TAG_B 1
#define POC_TAG_E 2

#define POC_TAG_B_NAME "B"
#define POC_TAG_E_NAME "E"

#define DEFAULT_POC POC_TAG_B


 #ifdef USE_BE_TYPE_FEATURE
    #define POC_TEMPLATE_SIZE 11
 #else
    #define POC_TEMPLATE_SIZE 10
 #endif

bool POC_INIT_FLAG = false;

const string POC_BOUNDARY = "BOUNDARY";

#ifdef USE_BE_TYPE_FEATURE
string CharTypeArray[CHAR_TYPE_NUM];
#endif

namespace pocinner{

/**
 * Inner function to get the context of the POC
 */
inline void get_poc_zh_context_1(vector<string>& words, size_t index,
        vector<string>& context, CMA_CType *ctype){

    #ifdef EN_ASSERT
        assert(context.size() == POC_TEMPLATE_SIZE);
    #endif

    int n = (int)words.size();
    int offset = (int)index - 2;
    
    string wa[5]; //word array
    #ifdef USE_BE_TYPE_FEATURE
    CharType ta[5]; //type array
    CharType preType = CHAR_TYPE_INIT;
    if(offset > 0){
        preType = ctype->getCharType(words[offset-1].data(), preType, words[offset].data());
    }
    #endif

    for(int j = 0; j < 5; ++j, ++offset){
        if(offset >= 0 && offset < n){
            wa[j] = words[offset];
            #ifdef USE_BE_TYPE_FEATURE
            const char* lastP = 0;
            if( offset < n - 1){
                lastP = words[offset + 1].data();
            }
            preType = ctype->getCharType(wa[j].c_str(), preType, lastP);
            ta[j] = preType;
            #endif
        }else{
            wa[j] = POC_BOUNDARY;
            #ifdef USE_BE_TYPE_FEATURE
            ta[j] = CHAR_TYPE_INIT;
            #endif
        }
    }

    int k = -1;

     #ifdef USE_BE_TYPE_FEATURE
        if(ta[2] != CHAR_TYPE_OTHER){
            context.resize(5);
            context[++k] = "C0=" + wa[2];
            context[++k] = "C1=" + wa[3];
            context[++k] = "T-1=" + CharTypeArray[ta[1]];
            context[++k] = "T0=" + CharTypeArray[ta[2]];
            context[++k] = "T+1=" + CharTypeArray[ta[3]];
            return;
        }

    if(index > 0 && ta[1] != CHAR_TYPE_OTHER){
        context.resize(1);
        context[0] = "afEnt";
        return;
    }

     #endif
    
    // Cn , n ∈ [−2, 2]
    context[++k] = "C-2=" + wa[0];
    context[++k] = "C-1=" + wa[1];
    context[++k] = "C0=" + wa[2];
    context[++k] = "C1=" + wa[3];
    context[++k] = "C2=" + wa[4];

    //Cn Cn+1 , n ∈ [−2, 1]
    context[++k] = "C-2,-1=" + wa[0] + "," + wa[1];
    context[++k] = "C-1,0=" + wa[1] + "," + wa[2];
    context[++k] = "C0,1=" + wa[2] + "," + wa[3];
    context[++k] = "C1,2=" + wa[3] + "," + wa[4];

    //C−1 C1
    context[++k] = "C-1,1=" + wa[1] + "," + wa[3];

    #ifdef USE_BE_TYPE_FEATURE
    //Pu (C0 ) whether is punctuation

    /*context[++k] = "T-2,-1,0,1,2=" + ta[0] + "," + ta[1] +
            "," + ta[2] + "," + ta[3] + "," + ta[4];*/
    #endif //#ifdef USE_BE_TYPE_FEATURE
    
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

/**
 * The utility class to search the string one by one
 */
class StrBasedVTrie{
public:
    StrBasedVTrie( VTrie* pTrie ) : trie(pTrie){
    }

    /**
     * Reset all the status
     */
    void reset(){
        completeSearch = true;
        node.init();
    }

    /**
     * search the specific string
     *
     * \param p point to the specific string
     * \return true if have extending string or just the right string, here
     * string is the combination of the previous strings and p
     */
    bool search(const char *p){
        if(!completeSearch)
            return false;        
        
        while(node.moreLong && *p){
            trie->find(*p, &node);
            ++p;
        }

        completeSearch = !(*p) && (node.data > 0 || node.moreLong);
        return completeSearch;
    }

    /**
     * First reset then search
     */
    bool firstSearch(const char* p){
        reset();
        return search(p);
    }

public:
    VTrie* trie;

    /**
     * The VTrieNode
     */
    VTrieNode node;

    /**
     * true if the search over all the characters in the input string
     */
    bool completeSearch;
};

} //end namespace pocinner



/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_context(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context, CMA_CType *ctype){
    context.resize(POC_TEMPLATE_SIZE);
    pocinner::get_poc_zh_context_1(words, i, context, ctype);
}

void poc_train(const char* file, const string& cateName,
        Knowledge::EncodeType encType,
        string posDelimiter, const char* extractFile,
        string method, size_t iters,float gaussian){
      SegTagger::initialize();
      TrainerData data(get_poc_zh_context, encType, posDelimiter);
      train(&data, file, cateName, extractFile, method, iters, gaussian, false);
}


SegTagger::SegTagger(const string& cateName, VTrie* posTrie, double eScore){
    SegTagger::initialize();
    //cout<<"Load poc model"<<(cateName + ".model")<<endl;
    me.load(cateName + ".model");
    trie_ = posTrie;
    setEScore(eScore);
}

void SegTagger::tag_word(vector<string>& words, int index, size_t N, 
        uint8_t* tags, POCTagUnit* candidates, int& lastIndex, size_t& canSize,
        double initScore, int candidateNum){

    vector<string> context(POC_TEMPLATE_SIZE);

    pocinner::get_poc_zh_context_1(words, index, context, ctype_);

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



void SegTagger::seg_sentence(vector<string>& words, size_t N, size_t retSize,
        vector<pair<vector<string>, double> >& segment){
    size_t n = words.size();

    //h0, h1 and score don't need to initialize
    uint8_t _array1[N][n];
    uint8_t _array2[N][n];

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
        lastIndex = -1;
        canSize = 0;
        #ifdef EN_ASSERT
            assert(h0Size <= N);
        #endif

        for(size_t j=0; j<h0Size; ++j){
            tag_word(words, i, N, h0[j], candidates, lastIndex, canSize, scores[j], j);
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

    CMA_CType *cType = CMA_CType::instance(CMA_CType::getEncType(encType));
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

        vector<string> best;
        seg_sentence_best(words, best);

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


void SegTagger::seg_sentence_best(vector<string>& words, vector<string>& segment){
    size_t n = words.size();
    uint8_t pocRet[n];

    #ifndef USE_BE_TYPE_FEATURE
        pocinner::StrBasedVTrie strTrie(trie_);
    #endif

    for(size_t index=0; index<n; ++index){
         #ifndef USE_BE_TYPE_FEATURE
            const char* curPtr = words[ index ].c_str();
        #endif
            
        #ifdef DEBUG_POC_TAGGER
            cout<<"Check "<<index<<":"<<words[index]<<endl;
        #endif

        vector<string> context(POC_TEMPLATE_SIZE);
        pocinner::get_poc_zh_context_1(words, index, context, ctype_);

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
            pocRet[index] = POC_TAG_B;
            #ifndef USE_BE_TYPE_FEATURE
                strTrie.firstSearch(curPtr);
            #endif
            continue;
        }

        #ifdef USE_BE_TYPE_FEATURE
            pocRet[index] = POC_TAG_E;
        #else
            strTrie.search(curPtr);
            #ifdef DEBUG_POC_TAGGER
                cout<<"Search StrVTrie "<<strTrie.completeSearch<<endl;
            #endif

            if(tagEScore >= eScore_){
                pocRet[index] = POC_TAG_E;
                continue;
            }

            if(strTrie.completeSearch)
                pocRet[index] = POC_TAG_E;
            else{
                strTrie.firstSearch(curPtr);
                pocRet[index] = POC_TAG_B;
            }
        #endif

    }

    pocinner::combinePOCToWord(words, n, pocRet, segment);
}

void SegTagger::initialize(){
    if(POC_INIT_FLAG)
        return;
    POC_INIT_FLAG = true;

    #ifdef USE_BE_TYPE_FEATURE
    CharTypeArray[CHAR_TYPE_INIT] = "I";
    CharTypeArray[CHAR_TYPE_NUMBER] = "N";
    CharTypeArray[CHAR_TYPE_PUNC] = "P";
    CharTypeArray[CHAR_TYPE_SPACE] = "S";
    CharTypeArray[CHAR_TYPE_DATE] = "D";
    CharTypeArray[CHAR_TYPE_LETTER] = "L";
    CharTypeArray[CHAR_TYPE_OTHER] = "O";
    #endif
}

}
