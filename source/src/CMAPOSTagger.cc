
#include "VTrie.h"

#include "CMAPOSTagger.h"

namespace cma{

string POS_EMPTY_STR = "";

const string DEFAULT_POS = "n";

void get_prefix_suffix(wstring& word, size_t length, vector<wstring>& prefixes,
        vector<wstring>& suffixes){
    size_t l = min(word.length(), length);
    for(size_t i=1; i<=l; ++i){
        prefixes.push_back(word.substr(0, i));
        suffixes.push_back(word.substr(word.length()-i, i));
    }
}

inline void get_pos_zh_scontext_1(vector<string>& words, string& tag_1,
        string& tag_2, size_t i, bool rareWord, vector<string>& context){
    string& w = words[i];
    size_t n = words.size();

    if(rareWord){
        if(isNumber(w))
            context.push_back("numeric");

        if(isUpperCase(w))
            context.push_back("uppercase");
        
        if(isHyphen(w))
            context.push_back("hyphen");
    }else{
        context.push_back("curword=" + w);
    }

    if(i > 0){
        context.push_back("word-1=" + words[i-1]);
        context.push_back("tag-1=" + tag_1);
        if(i>1){
            context.push_back("word-2=" + words[i-2]);
            context.push_back("tag-1,2=" + tag_2 + "," + tag_1);
        }else{
            context.push_back("word-2=BOUNDARY");
            context.push_back("tag-1,2=BOUNDARY," + tag_1);
        }
    }else{
        context.push_back("word-1=BOUNDARY");
        context.push_back("word-2=BOUNDARY");
        context.push_back("tag-1=BOUNDARY");
        context.push_back("tag-1,2=BOUNDARY,BOUNDARY");
    }

    if(i+1 < n){
        context.push_back("word+1=" + words[i+1]);
        if(i+2 < n)
            context.push_back("word+2=" + words[i+2]);
        else
            context.push_back("word+2=BOUNDARY");
    }else{
        context.push_back("word+1=BOUNDARY");
        context.push_back("word+2=BOUNDARY");
    }
}

void get_pos_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context){
    string& tag_1 = i > 0 ? tags[i-1] : POS_EMPTY_STR;
    string& tag_2 = i > 1 ? tags[i-2] : POS_EMPTY_STR;
    get_pos_zh_scontext_1(words, tag_1, tag_2, i, rareWord, context);
}

void pos_train(const char* file, const string& cateFile,const char* extractFile,
        string method, size_t iters,float gaussian){
      TrainerData data;
      data.get_context = get_pos_zh_scontext;
      train(&data, file, cateFile, extractFile, method, iters, gaussian, true);
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

void POSTagger::tag_word(vector<string>& words, int index, size_t N,
        string* tags, POSTagUnit* candidates, int& lastIndex, size_t& canSize,
        double initScore, int candidateNum){
    vector<string> context;

    VTrieNode node;
    trie_->search( words[index].data(), &node );

    bool exists = node.data > 0;
    string& tag_1 = index > 0 ? tags[index-1] : POS_EMPTY_STR;
    string& tag_2 = index > 1 ? tags[index-2] : POS_EMPTY_STR;
    get_pos_zh_scontext_1(words, tag_1, tag_2, index, !exists, context);

    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(context, outcomes, false);

    size_t outSize = outcomes.size();
    if(exists){
        set<string>& posSet = posVec_[node.data];
        for(size_t i=0; i<outSize; ++i){
            pair<outcome_type, double>& pair = outcomes[i];
            //whether exists such pos
            if(posSet.count(pair.first) <= 0)
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
    string _array1[N][n];
    string _array2[N][n];

    string (*h0)[n] = _array1;
    string (*h1)[n] = _array2;

    string (*hTmp)[n];

    double scores[N];
    scores[0] = 1.0;

    size_t h0Size = 1;

    POSTagUnit candidates[N];
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
}

void POSTagger::tag_file(const char* inFile, const char* outFile){
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

        vector<pair<vector<string>, double> > h0(1);
        tag_sentence(words, 2, 1, h0);

        //print the best result
        vector<string>& best = h0[0].first;

        size_t maxIndex = words.size();
        for(size_t i=0; i<maxIndex; ++i){
            out<<words[i] << "/"<< best[i]<<" ";
        }

        out<<words[maxIndex] << "/"<< best[maxIndex]<<endl;
    }

    in.close();
    out.close();
}

void POSTagger::tag_sentence_best(vector<string>& words, vector<string>& posRet){
    size_t n = words.size();
    for(size_t index=0; index<n; ++index){
        vector<string> context;
        VTrieNode node;
        trie_->search( words[index].data(), &node );
        bool exists = node.data > 0;
        string& tag_1 = index > 0 ? posRet[index-1] : POS_EMPTY_STR;
        string& tag_2 = index > 1 ? posRet[index-2] : POS_EMPTY_STR;
        get_pos_zh_scontext_1(words, tag_1, tag_2, index, !exists, context);
        if(exists){
            set<string>& posSet = posVec_[node.data];

            if(posSet.empty()){
                posRet.push_back(DEFAULT_POS);
                continue;
            }else if(posSet.size() == 1){
                posRet.push_back(*posSet.begin());
                continue;
            }

            vector<pair<outcome_type, double> > outcomes;
            me.eval_all(context, outcomes, false);

            //find the best pos
            double bestScore = -1.0;
            string bestPos;
            size_t outSize = outcomes.size();
            
            for(size_t k=0; k<outSize; ++k){
                pair<outcome_type, double>& pair = outcomes[k];
                if(pair.second > bestScore && (posSet.count(pair.first) > 0)){
                    bestScore = pair.second;
                    bestPos = pair.first;
                }
            }

            //no suitable pos found, insert the default POS n
            if(bestScore < 0)
                posRet.push_back(DEFAULT_POS);
            else
                posRet.push_back(bestPos);
        }else{
            string pos;
            me.eval(context, pos);
            posRet.push_back(pos);
        }
    }
}

bool POSTagger::appendWordPOS(string& line){
    vector<string> tokens;
    TOKEN_STR(line, tokens);
    size_t n = tokens.size();
    if(!n)
        return false;
    string word = tokens[0];
    replaceAll(word, "_", " ");

    set<string>* posSet = 0;
    //try to search first
    VTrieNode node;
    trie_->search(word.data(), &node);   
    //already exits
    if(node.data > 0){
        posSet = &(posVec_[node.data]);
    }else{
        //get the right offset (offset 0 is reserved)
        node.data = (int)posVec_.size();
        //insert new key
        posVec_.push_back(set<string>());
        trie_->insert(word.data(), &node);
        posSet = &(posVec_.back());
    }
    
    for(size_t i=1; i<n; ++i){
        posSet->insert(tokens[i]);
    }

    return true;
}

}
