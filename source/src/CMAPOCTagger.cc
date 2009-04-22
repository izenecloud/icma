#include <vector>

#include "CMAPOCTagger.h"
#include "CPPStringUtils.h"
#include "strutil.h"
#include "tokenizer.h"
#include "cma_ctype.h"

namespace cma{

string POC_EMPTY_STR = "";

map<string, uint8_t> POCs2c;

#ifdef USE_BE_TAG_SET
    #define POC_TAG_B 1
    #define POC_TAG_E 2

    string POCArray[3];

    #define DEFAULT_POC POC_TAG_B

    /** if set, use punctuaion and character type feature */
    //#define USE_BE_TYPE_FEATURE

#else
    #ifdef USE_POC_TRIE
        #define POC_TAG_L 1
        #define POC_TAG_M 2
        #define POC_TAG_R 4
        #define POC_TAG_I 8
        string POCArray[9];
    #else
        #define POC_TAG_L 0
        #define POC_TAG_M 1
        #define POC_TAG_R 2
        #define POC_TAG_I 3
        string POCArray[4];
    #endif

    #define DEFAULT_POC POC_TAG_I
#endif

#ifdef USE_POC_NEW_FEATURE
     #ifdef USE_BE_TYPE_FEATURE
        #define POC_TEMPLATE_SIZE 12
     #else
        #define POC_TEMPLATE_SIZE 10
     #endif
#else
    #define POC_TEMPLATE_SIZE 8
#endif

bool POC_INIT_FLAG = false;

const string POC_BOUNDARY = "BOUNDARY";

string CharTypeArray[CHAR_TYPE_NUM];

#ifndef USE_BE_TAG_SET
inline bool is_matched_poc(uint8_t preTag, uint8_t curTag){
    switch(preTag){
        case POC_TAG_I: // I
        case POC_TAG_R: // R
            return curTag == POC_TAG_L || curTag == POC_TAG_I; // L or I
        case POC_TAG_L: // L
            return curTag == POC_TAG_R || curTag == POC_TAG_M; // R or M
        case POC_TAG_M: // M
            return curTag == POC_TAG_R || curTag == POC_TAG_M; // R or M
        default:
            cerr<<"Invalid POC tag (POC_TRIE) "<<preTag<<endl;
            exit(1);
            return false;
    }
}
#endif

/**
 * \param tag_1 is tags[i-1]
 * \param tag_2 is tags[i-2]
 */
inline void get_poc_zh_context_1(vector<string>& words, string& tag_1, string& tag_2,
        size_t i, vector<string>& context, CMA_CType *ctype){

    #ifdef EN_ASSERT
        assert(context.size() == POC_TEMPLATE_SIZE);
    #endif

    #ifdef USE_POC_NEW_FEATURE       
    string wa[5]; //word array
    #ifdef USE_BE_TYPE_FEATURE
    string ta[5]; //type array
    #endif

    size_t n = words.size();
    size_t offset = i - 2;
    for(size_t j = 0; j < 5; ++j, ++offset){
        if(offset >= 0 && offset < n){
            wa[j] = words[offset];
            #ifdef USE_BE_TYPE_FEATURE
            ta[j] = CharTypeArray[ctype->getCharType(wa[j].c_str())];
            #endif
        }else{
            wa[j] = POC_BOUNDARY;
            #ifdef USE_BE_TYPE_FEATURE
            ta[j] = POC_BOUNDARY;
            #endif
        }
    }

    int k = -1;
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
    if( ta[2] == CharTypeArray[CHAR_TYPE_PUNC] )
        context[++k] = "punctuation=yes";
    else
        context[++k] = "punctuation=no";

    context[++k] = "T-2,-1,0,1,2=" + ta[0] + "," + ta[1] +
            "," + ta[2] + "," + ta[3] + "," + ta[4];
    #endif //#ifdef USE_BE_TYPE_FEATURE

    #else

    int k = -1;
    string& word_0 = words[i];
    context[++k] = "curword=" + word_0;

    size_t n = words.size();

    if(i>0){
        context[++k] = "T-1=" + tag_1;
        string& word_1 = words[i-1];
        if(i>1){
            context[++k] = "T-2=" + tag_2;
            string& word_2 = words[i-2];
            context[++k] = "W-2,-1=" +word_2+ "," + word_1;
            if(i<n-1){
                string& word_a1 = words[i+1];
                context[++k] = "W-1,+1=" + word_1 + "," + word_a1;
                context[++k] = "W-1,0,+1=" + word_1 + "," + word_0 + "," + word_a1;
                if(i<n-2){
                    string& word_a2 = words[i+2];
                    context[++k] = "W+1,+2=" +word_a1+ "," + word_a2;
                    context[++k] = "W-2,-1,+1,+2=" + word_2+ "," +word_1+ "," +word_a1+ "," +word_a2;
                }else{
                    context[++k] = "W+1,+2=" +word_a1+ ",BOUNDARY";
                    context[++k] = "W-2,-1,+1,+2=" +word_2+ "," +word_1+ "," +word_a1+ ",BOUNDARY";
                }
            }else{ // i==n-1
                context[++k] = "W-1,+1=" +word_1+ ",BOUNDARY";
                context[++k] = "W-1,0,+1=" +word_1+ "," +word_0+ ",BOUNDARY";
                context[++k] = "W+1,+2=BOUNDARY,BOUNDARY";
                context[++k] = "W-2,-1,+1,+2=" +word_2+ "," +word_1+ ",BOUNDARY,BOUNDARY";
            }
        } //end i>1
        else{ // i==1
            context[++k] = "T-2=BOUNDARY";
            context[++k] = "W-2,-1=BOUNDARY," + word_1;
            if(i<n-1){
                string& word_a1 = words[i+1];
                context[++k] = "W-1,+1=" + word_1 + "," + word_a1;
                context[++k] = "W-1,0,+1=" + word_1 + "," + word_0 + "," + word_a1;
                if(i<n-2){
                    string& word_a2 = words[i+2];
                    context[++k] = "W+1,+2=" +word_a1+ "," + word_a2;
                    context[++k] = "W-2,-1,+1,+2=BOUNDARY," +word_1+ "," +word_a1+ "," + word_a2;
                }else{
                    context[++k] = "W+1,+2=" +word_a1+ ",BOUNDARY";
                    context[++k] = "W-2,-1,+1,+2=BOUNDARY," +word_1+ "," +word_a1+ ",BOUNDARY";
                }
            }else{ // i==n-1
                context[++k] = "W-1,+1=" +word_1+ ",BOUNDARY";
                context[++k] = "W-1,0,+1=" +word_1 + "," + word_0+ ",BOUNDARY";
                context[++k] = "W+1,+2=BOUNDARY,BOUNDARY";
                context[++k] = "W-2,-1,+1,+2=BOUNDARY," +word_1+ ",BOUNDARY,BOUNDARY";
            }
        } //end i==1
    } // end if(i>0)
    else{ // i==0
        context[++k] = "T-1=BOUNDARY";
        context[++k] = "T-2=BOUNDARY";
        context[++k] = "W-2,-1=BOUNDARY,BOUNDARY";
        if(i<n-1){
            string& word_a1 = words[i+1];
            context[++k] = "W-1,+1=BOUNDARY," + words[i+1];
            context[++k] = "W-1,0,+1=BOUNDARY," +word_0 + "," + word_a1;
            if(i<n-2){
                string& word_a2 = words[i+2];
                context[++k] = "W+1,+2=" + word_a1 + "," + word_a2;
                context[++k] = "W-2,-1,+1,+2=BOUNDARY,BOUNDARY," +word_a1+ "," +word_a2;
            }else{
                context[++k] = "W+1,+2=" + word_a1 + ",BOUNDARY";
                context[++k] = "W-2,-1,+1,+2=BOUNDARY,BOUNDARY," +word_a1+ ",BOUNDARY";
            }
        }else{
            context[++k] = "W-1,+1=BOUNDARY,BOUNDARY";
            context[++k] = "W-1,0,+1=BOUNDARY," + word_0 + ",BOUNDARY";
            context[++k] = "W+1,+2=BOUNDARY,BOUNDARY";
            context[++k] = "W-2,-1,+1,+2=BOUNDARY,BOUNDARY,BOUNDARY,BOUNDARY";
        }
    } //end i==0
    #endif
}

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_context(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context, CMA_CType *ctype){
    context.resize(POC_TEMPLATE_SIZE);
    get_poc_zh_context_1(words, tags[i-1], tags[i-2], i, context, ctype);
}

void poc_train(const char* file, const string& cateName,
        Knowledge::EncodeType encType, const char* extractFile,
        string method, size_t iters,float gaussian){
      TrainerData data(get_poc_zh_context, encType);
      train(&data, file, cateName, extractFile, method, iters, gaussian, false);
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

SegTagger::SegTagger(const string& cateName){
    SegTagger::initialize();
    //cout<<"Load poc model"<<(cateName + ".model")<<endl;
    me.load(cateName + ".model");

    #ifdef USE_POC_TRIE
    ifstream in((cateName+".dic").data());
    assert(in);
    string line;
    while(!in.eof()){
        getline(in, line);
        appendWordPOC(line);
    }
    in.close();
    #endif
}

void SegTagger::tag_word(vector<string>& words, int index, size_t N, 
        uint8_t* tags, POCTagUnit* candidates, int& lastIndex, size_t& canSize,
        double initScore, int candidateNum){

    vector<string> context(POC_TEMPLATE_SIZE);

    string& tag_1 = index > 0 ? POCArray[tags[index-1]] : POC_EMPTY_STR;
    string& tag_2 = index > 1 ? POCArray[tags[index-2]] : POC_EMPTY_STR;
    get_poc_zh_context_1(words, tag_1, tag_2, index, context, ctype_);

    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(context, outcomes, false);
    #ifdef _ME_STRICT_POC_MATCHED
        uint8_t preTag = (index > 0) ? tags[index-1] : 3;
    #endif

    size_t outSize = outcomes.size();
    for(size_t i=0; i<outSize; ++i){
        pair<outcome_type, double>& pair = outcomes[i];
        double score = pair.second * initScore;
        if(canSize >= N && score <= candidates[lastIndex].score)
            continue;
        uint8_t pocCode = POCs2c[pair.first];
        #ifdef _ME_STRICT_POC_MATCHED
        if(!is_matched_poc(preTag, pocCode))
            continue;
        #endif
        insertCandidate(pocCode, candidateNum, score, candidates,
                lastIndex, canSize, N);
    }
}

inline void combinePOCToWord(vector<string>& words, size_t n, uint8_t* tags,
        vector<string>& seg){
    string strBuf;

    #ifdef USE_BE_TAG_SET
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
    #else
        #ifdef _ME_STRICT_POC_MATCHED
            for(size_t i = 0; i < n; ++i){
                if(tags[i] == POC_TAG_R){ // R
                    seg.push_back(strBuf + words[i]);
                    strBuf.clear();
                }else if(tags[i] == POC_TAG_I){ // I
                    #ifdef EN_ASSERT
                    assert(strBuf.empty());
                    #endif
                    seg.push_back(words[i]);
                }else if(tags[i] == POC_TAG_L){ // L
                    #ifdef EN_ASSERT
                    assert(strBuf.empty());
                    #endif
                    strBuf.append(words[i]);
                }else if(tags[i] == POC_TAG_M){ // M
                    strBuf.append(words[i]);
                }else{
                    // unrecognize tag
                    cerr<<"Error POC (MATCHED): "<<(int)tags[i]<<" ("<<k<<","<<i<<")"<<endl;
                    assert(0);
                }
            }

        #else
            for(size_t i = 0; i < n; ++i){
                if(tags[i] == POC_TAG_R){ // R
                    seg.push_back(strBuf + words[i]);
                    strBuf.clear();
                }else if(tags[i] == POC_TAG_I){ // I
                    if(!strBuf.empty()){
                        seg.push_back(strBuf);
                        strBuf.clear();
                    }
                    seg.push_back(words[i]);
                }else if(tags[i] == POC_TAG_L){ // L
                    if(!strBuf.empty()){
                        seg.push_back(strBuf);
                        strBuf.clear();
                    }
                    strBuf.append(words[i]);
                }else if(tags[i] == POC_TAG_M){ // M
                    strBuf.append(words[i]);
                }else{
                    // unrecognize tag
                    cerr<<"Error POC: "<<(int)tags[i]<<"("<<i<<")"<<endl;
                    assert(0);
                }
            }
        #endif
    #endif //end #ifdef USE_BE_TAG_SET
    //the remaining string
    if(!strBuf.empty())
        seg.push_back(strBuf);
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
        combinePOCToWord(words, n, tags, pair.first);
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

    #ifdef USE_BE_TAG_SET
    CharType preType = CHAR_TYPE_INIT;
    #endif
    for(size_t index=0; index<n; ++index){
        #ifdef DEBUG_POC_TAGGER
        cout<<"Check "<<index<<":"<<words[index]<<endl;
        #endif

        #ifdef USE_BE_TAG_SET
        CharType curType = ctype_->getCharType(words[index].c_str(), preType);

        if(curType == CHAR_TYPE_PUNC){
            pocRet[index] = POC_TAG_B;
            preType = curType;
            continue;
        }

        switch(preType){          
            case CHAR_TYPE_PUNC:
                pocRet[index] = POC_TAG_B;
                preType = curType;
                continue;

            case CHAR_TYPE_NUMBER:
                switch(curType){
                    case CHAR_TYPE_NUMBER:
                        pocRet[index] = POC_TAG_E;
                        continue;
                    case CHAR_TYPE_LETTER:
                    case CHAR_TYPE_HYPHEN:
                        pocRet[index] = POC_TAG_E;
                        preType = CHAR_TYPE_LETTER;
                        continue;
                    //may be unit is OK here
                    case CHAR_TYPE_DATE:
                        pocRet[index] = POC_TAG_E;
                        preType = CHAR_TYPE_INIT;
                        continue;
                    default:
                        break;
                        /*pocRet[index] = POC_TAG_B;
                        preType = curType;
                        continue;*/
                }

            case CHAR_TYPE_DATE:
                pocRet[index] = POC_TAG_B;
                preType = curType;
                continue;

            case CHAR_TYPE_LETTER:
            case CHAR_TYPE_HYPHEN:
                switch(curType){
                    case CHAR_TYPE_NUMBER:
                    case CHAR_TYPE_LETTER:
                    case CHAR_TYPE_HYPHEN:
                        pocRet[index] = POC_TAG_E;
                        preType = CHAR_TYPE_NUMBER;
                        continue;
                    default:
                        pocRet[index] = POC_TAG_B;
                        preType = curType;
                        continue;
                }

            case CHAR_TYPE_INIT:
                pocRet[index] = POC_TAG_B;
                preType = curType;
                continue;

            case CHAR_TYPE_OTHER:
                switch(curType){
                    case CHAR_TYPE_NUMBER:
                    case CHAR_TYPE_LETTER:
                    case CHAR_TYPE_HYPHEN:
                    case CHAR_TYPE_PUNC:
                        pocRet[index] = POC_TAG_B;
                        preType = curType;
                        continue;
                    default:
                        break;
                }

            default:
                break;
        }

        preType = curType;

        #endif


        vector<string> context(POC_TEMPLATE_SIZE);

        #ifdef USE_POC_TRIE
        VTrieNode node;
        trie_.search( words[index].data(), &node );
        int data = node.data;
        #endif

        string& tag_1 = index > 0 ? POCArray[pocRet[index-1]] : POC_EMPTY_STR;
        string& tag_2 = index > 1 ? POCArray[pocRet[index-2]] : POC_EMPTY_STR;
        get_poc_zh_context_1(words, tag_1, tag_2, index, context, ctype_);

        #ifdef USE_POC_TRIE
        if(data > 0){

            #ifdef USE_BE_TAG_SET
            if(data == POC_TAG_B || data == POC_TAG_E){
                pocRet[index] = data;
                continue;
            }

            #else
            if(data == POC_TAG_I || data == POC_TAG_L ||
                    data == POC_TAG_M || data == POC_TAG_R){
                pocRet[index] = data;
                continue;
            }
            #endif

            vector<pair<outcome_type, double> > outcomes;
            me.eval_all(context, outcomes, false);

            //find the best pos
            double bestScore = -1.0;
            uint8_t bestPoc;
            size_t outSize = outcomes.size();

            for(size_t k=0; k<outSize; ++k){
                pair<outcome_type, double>& pair = outcomes[k];
                #ifdef DEBUG_POC_TAGGER
                    cout<<"Eval(E) "<<pair.first<<"="<<pair.second<<endl;
                #endif
                if(pair.second > bestScore){
                    uint8_t pocCode = POCs2c[pair.first];
                    if(data & pocCode){
                        bestScore = pair.second;
                        bestPoc = pocCode;
                    }
                }
            }

            //no suitable pos found, insert the default POS n
            if(bestScore <= 0)
                pocRet[index] = DEFAULT_POC;
            else
                pocRet[index] = bestPoc;
        }else{
        #endif
            string poc;
            if(me.eval(context, poc) > 0.0)
                pocRet[index] = POCs2c[poc];
            else{
                vector<pair<outcome_type, double> > outcomes;
                me.eval_all(context, outcomes, false);

                //find the best pos
                double bestScore = -1.0;
                size_t outSize = outcomes.size();

                for(size_t k=0; k<outSize; ++k){
                    pair<outcome_type, double>& pair = outcomes[k];
                    #ifdef DEBUG_POC_TAGGER
                        cout<<"Eval(N) "<<pair.first<<"="<<pair.second<<endl;
                    #endif
                    if(pair.second > bestScore){
                        bestScore = pair.second;
                        poc = pair.first;
                    }
                }

                //no suitable pos found, insert the default POS n
                if(bestScore <= 0)
                    pocRet[index] = DEFAULT_POC;
                else
                    pocRet[index] = POCs2c[poc];
            }
        #ifdef USE_POC_TRIE
        } //end if(exists) / else
        #endif
    }

    combinePOCToWord(words, n, pocRet, segment);
}

bool SegTagger::appendWordPOC(const string& line){
    #ifdef USE_POC_TRIE
    vector<string> tokens;
    TOKEN_STR(line, tokens);
    size_t n = tokens.size();
    //at least have the word(character) and one POC tag
    if(n < 2)
        return false;
    string word = tokens[0];

    //try to search first, if found, also init node.data
    VTrieNode node;
    trie_.search(word.data(), &node);

    for(size_t i=1; i<n; ++i){
        string& tag = tokens[i];
        node.data |= POCs2c[tag];
    }
    trie_.insert(word.data(), &node);
    #endif
    
    return true;
}

void SegTagger::initialize(){
    if(POC_INIT_FLAG)
        return;
    POC_INIT_FLAG = true;

    CharTypeArray[CHAR_TYPE_INIT] = "I";
    CharTypeArray[CHAR_TYPE_NUMBER] = "N";
    CharTypeArray[CHAR_TYPE_PUNC] = "P";
    CharTypeArray[CHAR_TYPE_HYPHEN] = "H";
    CharTypeArray[CHAR_TYPE_SPACE] = "S";
    CharTypeArray[CHAR_TYPE_DATE] = "D";
    CharTypeArray[CHAR_TYPE_LETTER] = "L";
    CharTypeArray[CHAR_TYPE_OTHER] = "O";

    #ifdef USE_BE_TAG_SET
        POCs2c["B"] = (uint8_t)POC_TAG_B;
        POCArray[POC_TAG_B] = "B";

        POCs2c["E"] = (uint8_t)POC_TAG_E;
        POCArray[POC_TAG_E] = "E";
    #else
        POCs2c["L"] = (uint8_t)POC_TAG_L;
        POCArray[POC_TAG_L] = "L";

        POCs2c["R"] = (uint8_t)POC_TAG_R;
        POCArray[POC_TAG_R] = "R";

        POCs2c["M"] = (uint8_t)POC_TAG_M;
        POCArray[POC_TAG_M] = "M";

        POCs2c["I"] = (uint8_t)POC_TAG_I;
        POCArray[POC_TAG_I] = "I";
    #endif
}

}
