
#include "CPPStringUtils.h"
#include "CMAPOCTagger.h"
#include "strutil.h"

#define _ME_STRICT_POC_MATCHED
#define EN_ASSERT

namespace cma{

const wstring _PuntStr = L"～！＠＃％＾＆×（）｀　｛｝［］：＂｜；＇＼＜＞？，．／。《》“”‘’＋－＝—．、~!@#%^&*()` {}[]:\"|;'\\<>?,./.<>""''+-=_.";

const wstring _LetterStr = L"ＱＷＥＲＴＹＵＩＯＰｑｗｅｒｔｙｕｉｏｐＡＳＤＦＧＨＪＫＬａｓｄｆｇｈｊｋｌＺＸＣＶＢＮＭｚｘｃｖｂｎｍQWERTYUIOPqwertyuiopASDFGHJKLasdfghjklZXCVBNMzxcvbnm";

const wstring _HyphenStr = L"-—－";

const wstring _DigitStr = L"０１２３４５６７８９0123456789";

map<string, uint8_t> POCs2c;

vector<string> POCVec;

map<string, CharType> TYPE_MAP;

set<string> HYPHEN_SET;

inline bool isPuntuation(const string& str){
    return TYPE_MAP[str] == CHAR_TYPE_PUNTUATION;
}

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context){
    context.push_back("curword=" + words[i]);

    /*
    if(isPuntuation(words[i])){
        context.push_back(L"puntuation");
    }*/

    size_t n = words.size();

    if(i>0){
        context.push_back("T-1=" + tags[i-1]);
        if(i>1){
            context.push_back("T-2=" + tags[i-2]);
            context.push_back("W-2,-1=" +words[i-2]+ "," + words[i-1]);
            if(i<n-1){
                context.push_back("W-1,+1=" + words[i-1] + "," + words[i+1]);
                context.push_back("W-1,0,+1=" + words[i-1] + "," + words[i] + "," + words[i+1]);
                if(i<n-2){
                    context.push_back("W+1,+2=" +words[i+1]+ "," + words[i+2]);
                    context.push_back("W-2,-1,+1,+2=" +words[i-2]+ "," +words[i-1]+ "," +words[i+1]+ "," +words[i+2]);
                }else{
                    context.push_back("W+1,+2=" +words[i+1]+ ",BOUNDARY");
                    context.push_back("W-2,-1,+1,+2=" +words[i-2]+ "," +words[i-1]+ "," +words[i+1]+ ",BOUNDARY");
                }
            }else{ // i==n-1
                context.push_back("W-1,+1=" +words[i-1]+ ",BOUNDARY");
                context.push_back("W-1,0,+1=" +words[i-1]+ "," +words[i]+ ",BOUNDARY");
                context.push_back("W+1,+2=BOUNDARY,BOUNDARY");
                context.push_back("W-2,-1,+1,+2=" +words[i-2]+ "," +words[i-1]+ ",BOUNDARY,BOUNDARY");
            }
        } //end i>1
        else{ // i==1
            context.push_back("T-2=BOUNDARY");
            context.push_back("W-2,-1=BOUNDARY," + words[i-1]);
            if(i<n-1){
                context.push_back("W-1,+1=" + words[i-1] + "," + words[i+1]);
                context.push_back("W-1,0,+1=" + words[i-1] + "," + words[i] + "," + words[i+1]);
                if(i<n-2){
                    context.push_back("W+1,+2=" +words[i+1]+ "," + words[i+2]);
                    context.push_back("W-2,-1,+1,+2=BOUNDARY," +words[i-1]+ "," +words[i+1]+ "," + words[i+2]);
                }else{
                    context.push_back("W+1,+2=" +words[i+1]+ ",BOUNDARY");
                    context.push_back("W-2,-1,+1,+2=BOUNDARY," +words[i-1]+ "," +words[i+1]+ ",BOUNDARY");
                }
            }else{ // i==n-1
                context.push_back("W-1,+1=" +words[i-1]+ ",BOUNDARY");
                context.push_back("W-1,0,+1=" +words[i-1] + "," + words[i]+ ",BOUNDARY");
                context.push_back("W+1,+2=BOUNDARY,BOUNDARY");
                context.push_back("W-2,-1,+1,+2=BOUNDARY," +words[i-1]+ ",BOUNDARY,BOUNDARY");
            }
        } //end i==1
    } // end if(i>0)
    else{ // i==0
        context.push_back("T-1=BOUNDARY");
        context.push_back("T-2=BOUNDARY");
        context.push_back("W-2,-1=BOUNDARY,BOUNDARY");
        if(i<n-1){
            context.push_back("W-1,+1=BOUNDARY," + words[i+1]);
            context.push_back("W-1,0,+1=BOUNDARY," +words[i] + "," + words[i+1]);
            if(i<n-2){
                context.push_back("W+1,+2=" + words[i+1] + "," + words[i+2]);
                context.push_back("W-2,-1,+1,+2=BOUNDARY,BOUNDARY," +words[i+1]+ "," +words[i+2]);
            }else{
                context.push_back("W+1,+2=" + words[i+1] + ",BOUNDARY");
                context.push_back("W-2,-1,+1,+2=BOUNDARY,BOUNDARY," +words[i+1]+ ",BOUNDARY");
            }
        }else{
            context.push_back("W-1,+1=BOUNDARY,BOUNDARY");
            context.push_back("W-1,0,+1=BOUNDARY," + words[i] + ",BOUNDARY");
            context.push_back("W+1,+2=BOUNDARY,BOUNDARY");
            context.push_back("W-2,-1,+1,+2=BOUNDARY,BOUNDARY,BOUNDARY,BOUNDARY");
        }
    } //end i==0
}

void poc_train(const char* file, const string& cateName,const char* extractFile,
        string method, size_t iters,float gaussian){
      TrainerData data;
      data.get_context = get_poc_zh_scontext;
      train(&data, file, cateName, extractFile, method, iters, gaussian);
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

void SegTagger::tag_word(vector<string>& words, int i, size_t N,
        vector<string>& hist, vector<POCTagUnit>& ret, double initScore){

    //cout<<hist.size()<<","<<i<<endl;
    assert(hist.size() == i);

    vector<string> context;
    bool exists = tagDict_.count(words[i]) > 0;
    get_context(words, hist, i, !exists, context);

    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(context, outcomes, true);
    #ifdef _ME_STRICT_POC_MATCHED
        string preTag = (i > 0) ? hist[i-1] : "I";
    #endif
    
    size_t origN = N;
    if(exists){
        //need to optimize
        for(vector<pair<outcome_type, double> >::iterator itr = outcomes.begin();
                itr != outcomes.end(); ++itr){
            
            if(tagDict_[words[i]][itr->first]){
                #ifdef _ME_STRICT_POC_MATCHED
                if(!is_matched_poc(preTag, itr->first))
                    continue;
                #endif
                ret.push_back(POCTagUnit(POCs2c[itr->first], itr->second*initScore, i));
                --N;
                //at most N tags
                if(!N) return;
            }
        }

        //add at least one tag
        if(origN != N)
            return;
    }
    
    for(vector<pair<outcome_type, double> >::iterator itr = outcomes.begin();
            itr != outcomes.end(); ++itr){
        #ifdef _ME_STRICT_POC_MATCHED
        //wstring tag = S2WS(itr->first);
        if(!is_matched_poc(preTag, itr->first))
            continue;
        #endif
        ret.push_back(POCTagUnit(POCs2c[itr->first], itr->second*initScore, i));
        --N;
        //at most N tags
        if(!N) return;
    }
    
}

void SegTagger::seg_sentence(string sentence, size_t N,
        vector<pair<vector<string>, double> >& segment){
    size_t n = sentence.length();

    vector<string> words;
    for(size_t i=0; i<n; ++i){
        //a segment is required here
        words.push_back(sentence.substr(i, 1));
    }

    vector<pair<vector<string>, double> > h0;
    pair<vector<string>, double> s;
    s.second = 1.0;
    h0.push_back(s);

    vector<POCTagUnit> candidates;

    for(size_t i=0; i<n; ++i){
        candidates.clear();
        #ifdef EN_ASSERT
            assert(h0.size() <= N);
        #endif
        size_t sz = h0.size();

        pair<vector<string>, double>* ptrKeeper[sz];
        for(size_t j=0; j<sz; ++j){
            pair<vector<string>, double>& pair = h0[j];
            ptrKeeper[j] = &h0[j];
            tag_word(words, i, N, pair.first, candidates, pair.second);

        }
        //cout<<i<<":"<<h1.size()<<endl;
        sort(candidates.begin(), candidates.end());

        h0.clear();
        size_t loop = min(sz, N);
        for(size_t j=0; j<sz; ++j){
            POCTagUnit& unit = candidates[j];
            if(unit.index == 1){
                
            }
        }

        //select the N-best
        
        /*
        h0.empty();
        size_t h0Size = min(N, h1.size());
        h0.insert(h0.begin(), h1.begin(), h1.begin() + h0Size);
         */
    }
    //cout<<"h0 size = "<<h0.size()<<endl;
    //construct the sentence
    #ifdef _ME_STRICT_POC_MATCHED
    for(vector<pair<vector<string>,double> >::iterator itr = h0.begin();
          itr != h0.end(); ++itr){
        vector<string> tags = itr->first;
        vector<string> seg;
        size_t begin = 0;
        for(size_t i = 0; i < n; ++i){
            if(tags[i] == "R"){
                seg.push_back(sentence.substr(begin, i-begin+1));
                begin = i + 1;
            }else if(tags[i] == "I"){
                assert(begin == i);
                seg.push_back(sentence.substr(i, 1));
                begin = i + 1;
            }else if(tags[i] == "L"){
                assert(begin == i);
            }else if(tags[i] == "M"){
                //do nothing
            }else{
                // unrecognize tag
                assert(0);
            }
        }

        if(begin != n)
            seg.push_back(sentence.substr(begin, n-begin)); //append the last segment
        segment.push_back(pair<vector<string>,double>(seg, itr->second));
    }
    #else
    for(vector<pair<vector<wstring>,double> >::iterator itr = h0.begin();
          itr != h0.end(); ++itr){
        vector<string> tags = itr->first;
        vector<string> seg;
        size_t begin = 0;
        for(size_t i = 0; i < n; ++i){
            if(tags[i] == "R"){
                seg.push_back(sentence.substr(begin, i-begin+1));
                begin = i + 1;
            }else if(tags[i] == "I"){
                //the previous word maym not end successfully
                if(begin < i)
                    seg.push_back(sentence.substr(begin, i - begin));
                seg.push_back(sentence.substr(i, 1));
                begin = i + 1;
            }else if(tags[i] == "L"){
                //the previous word maym not end successfully
                if(begin < i)
                    seg.push_back(sentence.substr(begin, i - begin));
                begin = i;
            }else if(tags[i] == "M"){
                //do nothing
            }else{
                // unrecognize tag
                assert(0);
            }
        }

        if(begin != n)
            seg.push_back(sentence.substr(begin, n-begin)); //append the last segment
        segment.push_back(pair<vector<string>,double>(seg, itr->second));
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

        vector<pair<vector<string>, double> > segment;
        seg_sentence(line, 2, segment);

        //print the best result
        vector<string>& best = segment[0].first;

        size_t n = best.size();
        for(size_t i=0; i<n; ++i){
            out<<best[i];
            if(i != n - 1)
                out<<" ";
            size_t n = best.size();
            for(size_t i=0; i<n; ++i){
                out<<best[i];
                if(i != n - 1)
                    out<<" ";
            }
        }
        out<<endl;
    }

    in.close();
    out.close();
}

void addToTypeMap(const wstring& str, CharType type){
    size_t len = str.length();
    for(size_t i=0; i<len; ++i){
        TYPE_MAP[T_UTF8(str.substr(i, 1))] = type;
    }
}

void SegTagger::initialize(){
    POCs2c["L"] = (uint8_t)0;
    POCs2c["M"] = (uint8_t)1;
    POCs2c["R"] = (uint8_t)2;
    POCs2c["I"] = (uint8_t)3;
    POCVec.push_back("L");
    POCVec.push_back("M");
    POCVec.push_back("R");
    POCVec.push_back("I");

    addToTypeMap(_PuntStr, CHAR_TYPE_PUNTUATION);
    addToTypeMap(_LetterStr, CHAR_TYPE_LETTER);
    addToTypeMap(_DigitStr, CHAR_TYPE_DIGIT);

    //update hyphen set
    size_t len = _HyphenStr.length();
    for(size_t i=0; i<len; ++i){
        HYPHEN_SET.insert(T_UTF8(_HyphenStr.substr(i, 1)));
    }
}

void SegTagger::appendWordTag(string& word, string& tag, int counter){
    tagDict_[word][tag] += counter;
}

}
