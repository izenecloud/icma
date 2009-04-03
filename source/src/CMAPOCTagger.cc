#include "CPPStringUtils.h"
#include "CMAPOCTagger.h"
#include "strutil.h"

namespace cma{

/** Its order cannot be modified, is_matched_poc() use their order*/
const string _pocTagSeq = "LMRI";

string POC_EMPTY_STR = "";

map<string, uint8_t> POCs2c;

string POCArray[4];

bool POC_INIT_FLAG = false;

inline bool is_matched_poc(uint8_t preTag, uint8_t curTag){
    switch(preTag){
        case 3: // I
        case 2: // R
            return curTag == 0 || curTag == 3; // L or I
        case 0: // L
            return curTag == 2 || curTag == 1; // M or R
        case 1: // M
            return curTag == 2 || curTag == 1; // R or M
        default:
            cerr<<"Invalid POC tag "<<preTag<<endl;
            exit(1);
            return false;
    }
}

/**
 * \param tag_1 is tags[i-1]
 * \param tag_2 is tags[i-2]
 */
inline void get_poc_zh_context_1(vector<string>& words, string& tag_1, string& tag_2,
        size_t i, vector<string>& context){
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
    
    #ifdef EN_ASSERT
        assert(context.size() == 8);
    #endif
}

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_context(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context){
    get_poc_zh_context_1(words, tags[i-1], tags[i-2], i, context);
}

void poc_train(const char* file, const string& cateName,const char* extractFile,
        string method, size_t iters,float gaussian){
      TrainerData data;
      data.get_context = get_poc_zh_context;
      train(&data, file, cateName, extractFile, method, iters, gaussian, false);
}

void create_poc_meterial(const char* inFile, const char* outFile){
    ifstream in(inFile);
    ofstream out(outFile);
    string line;

    typedef boost::tokenizer <boost::char_separator<wchar_t>,
        wstring::const_iterator, wstring> POCTokenizer;

    while(!in.eof()){
        getline(in, line);
        trimSelf(line);
        if(line.length() == 0){
            out<<endl;
            continue;
        }

        wstring ws = F_UTF8W(line);
        POCTokenizer token(ws, boost::char_separator<wchar_t>(L" "));
        POCTokenizer::const_iterator itr = token.begin();
        if( itr == token.end() ){
            out<<endl;
            continue;
        }
        
        while(true){
            size_t pos = itr->find_first_of(TAG_SEP);
            if(pos == wstring::npos || pos == 0){
                cerr<<"The Format is word/tag, but not ("<<T_UTF8(*itr)<<")"<<endl;
                exit(1);
            }
            wstring word = itr->substr(0,pos);
            if(word.length() == 1){
                out<<T_UTF8(word)<<"/I";
            }else{
                out<<T_UTF8(word.substr(0, 1))<<"/L ";
                size_t lastIndex = word.length() - 1;
                for(size_t i=1; i<lastIndex; ++i)
                    out<<T_UTF8(word.substr(i, 1))<<"/M ";
                out<<T_UTF8(word.substr(lastIndex, 1))<<"/R";
            }

            ++itr;
            if(itr != token.end())
                out<<" ";
            else{
                out<<endl;
                break;
            }
        }
    }
    in.close();
    out.close();
}


/**
 *
 * \return true if the candidates is full
 */
inline void insertCandidate(uint8_t pocCode, int index, double score,
        POCTagUnit* candidates, int& lastIndex, int& canSize, size_t N){
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

void SegTagger::tag_word(vector<string>& words, int index, size_t N, 
        uint8_t* tags, POCTagUnit* candidates, int& lastIndex, int& canSize,
        double initScore, int candidateNum){

    vector<string> context(8);

    string& tag_1 = index > 0 ? POCArray[tags[index-1]] : POC_EMPTY_STR;
    string& tag_2 = index > 1 ? POCArray[tags[index-2]] : POC_EMPTY_STR;
    get_poc_zh_context_1(words, tag_1, tag_2, index, context);

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
    int canSize;

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
    //construct the sentence
    #ifdef _ME_STRICT_POC_MATCHED
    for(int k=0; k<h0Size; ++k){
        uint8_t* tags = h0[k];
        pair<vector<string>,double>& pair = segment[k];        
        pair.second = (pair.second > 0) ? (pair.second * scores[k]) : scores[k];
        vector<string>& seg = pair.first;
        string strBuf;
        for(size_t i = 0; i < n; ++i){
            if(tags[i] == 2){ // R
                seg.push_back(strBuf + words[i]);
                strBuf.clear();
            }else if(tags[i] == 3){ // I
                #ifdef EN_ASSERT
                assert(strBuf.empty());
                #endif
                seg.push_back(words[i]);
            }else if(tags[i] == 0){ // L
                #ifdef EN_ASSERT
                assert(strBuf.empty());
                #endif
                strBuf.append(words[i]);
            }else if(tags[i] == 1){ // M
                strBuf.append(words[i]);
            }else{
                // unrecognize tag
                cerr<<"Error POC: "<<(int)tags[i]<<" ("<<k<<","<<i<<")"<<endl;
                assert(0);
            }
        }

        //the remaining string
        if(strBuf.length() > 0)
            seg.push_back(strBuf);
    }
    #else
    for(int k=0; k<h0Size; ++k){
        uint8_t* tags = h0[k];
        pair<vector<string>,double>& pair = segment[k];
        pair.second = (pair.second > 0) ? (pair.second * scores[k]) : scores[k];
        vector<string>& seg = pair.first;
        string strBuf;
        for(size_t i = 0; i < n; ++i){
            if(tags[i] == 2){ // R
                seg.push_back(strBuf + words[i]);
                strBuf.clear();
            }else if(tags[i] == 3){ // I
                if(!strBuf.empty()){
                    seg.push_back(strBuf);
                    strBuf.clear();
                }
                seg.push_back(words[i]);
            }else if(tags[i] == 0){ // L
                if(!strBuf.empty()){
                    seg.push_back(strBuf);
                    strBuf.clear();
                }
                strBuf.append(words[i]);
            }else if(tags[i] == 1){ // M
                strBuf.append(words[i]);
            }else{
                // unrecognize tag
                cerr<<"Error POC: "<<(int)tags[i]<<" ("<<k<<","<<i<<")"<<endl;
                assert(0);
            }
        }

        //the remaining string
        if(strBuf.length() > 0)
            seg.push_back(strBuf);
    }
    #endif
    
}

void SegTagger::tag_file(const char* inFile, const char* outFile){
    ifstream in(inFile);
    ofstream out(outFile);

    string line;

    while(!in.eof()){
        getline(in, line);
        if(line.length() == 0){
            out<<endl;
            continue;
        }

        vector<pair<vector<string>, double> > segment(1);
        vector<string> words;
        T_UTF8_VEC(line, words);

        seg_sentence(words, 2, 1, segment);
        #ifdef EN_ASSERT
            assert(segment.size() == 1);
        #endif
        //print the best result
        vector<string>& best = segment.front().first;
        size_t maxIndex = best.size() - 1;
        for(size_t i=0; i<maxIndex; ++i){
            out<<best[i]<<" ";
        }
        out<<best[maxIndex]<<endl;
    }

    in.close();
    out.close();
}



void SegTagger::initialize(){
    if(POC_INIT_FLAG)
        return;
    POC_INIT_FLAG = true;

    assert(sizeof(POCArray) / sizeof(string) == _pocTagSeq.length());

    for(size_t i=0;i<_pocTagSeq.size(); ++i){
        POCs2c[_pocTagSeq.substr(i,1)] = (uint8_t)i;
        POCArray[i] = _pocTagSeq.substr(i,1);
    }
}

}
