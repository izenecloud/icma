
#include "CPPStringUtils.h"
#include "CMAPOCTagger.h"
#include "strutil.h"

namespace cma{

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_scontext(vector<wstring>& words, vector<wstring>& tags, size_t i,
        bool rareWord, vector<wstring>& context){
    context.push_back(L"curword=" + words[i]);

    size_t n = words.size();

    if(i>0){
        context.push_back(L"T-1=" + tags[i-1]);
        if(i>1){
            context.push_back(L"T-2=" + tags[i-2]);
            context.push_back(L"W-2,-1=" +words[i-2]+ L"," + words[i-1]);
            if(i<n-1){
                context.push_back(L"W-1,+1=" + words[i-1] + L"," + words[i+1]);
                context.push_back(L"W-1,0,+1=" + words[i-1] + L"," + words[i] + L"," + words[i+1]);
                if(i<n-2){
                    context.push_back(L"W+1,+2=" +words[i+1]+ L"," + words[i+2]);
                    context.push_back(L"W-2,-1,+1,+2=" +words[i-2]+ L"," +words[i-1]+ L"," +words[i+1]+ L"," +words[i+2]);
                }else{
                    context.push_back(L"W+1,+2=" +words[i+1]+ L",BOUNDARY");
                    context.push_back(L"W-2,-1,+1,+2=" +words[i-2]+ L"," +words[i-1]+ L"," +words[i+1]+ L",BOUNDARY");
                }
            }else{ // i==n-1
                context.push_back(L"W-1,+1=" +words[i-1]+ L",BOUNDARY");
                context.push_back(L"W-1,0,+1=" +words[i-1]+ L"," +words[i]+ L",BOUNDARY");
                context.push_back(L"W+1,+2=BOUNDARY,BOUNDARY");
                context.push_back(L"W-2,-1,+1,+2=" +words[i-2]+ L"," +words[i-1]+ L",BOUNDARY,BOUNDARY");
            }
        } //end i>1
        else{ // i==1
            context.push_back(L"T-2=BOUNDARY");
            context.push_back(L"W-2,-1=BOUNDARY," + words[i-1]);
            if(i<n-1){
                context.push_back(L"W-1,+1=" + words[i-1] + L"," + words[i+1]);
                context.push_back(L"W-1,0,+1=" + words[i-1] + L"," + words[i] + L"," + words[i+1]);
                if(i<n-2){
                    context.push_back(L"W+1,+2=" +words[i+1]+ L"," + words[i+2]);
                    context.push_back(L"W-2,-1,+1,+2=BOUNDARY," +words[i-1]+ L"," +words[i+1]+ L"," + words[i+2]);
                }else{
                    context.push_back(L"W+1,+2=" +words[i+1]+ L",BOUNDARY");
                    context.push_back(L"W-2,-1,+1,+2=BOUNDARY," +words[i-1]+ L"," +words[i+1]+ L",BOUNDARY");
                }
            }else{ // i==n-1
                context.push_back(L"W-1,+1=" +words[i-1]+ L",BOUNDARY");
                context.push_back(L"W-1,0,+1=" +words[i-1] + L"," + words[i]+ L",BOUNDARY");
                context.push_back(L"W+1,+2=BOUNDARY,BOUNDARY");
                context.push_back(L"W-2,-1,+1,+2=BOUNDARY," +words[i-1]+ L",BOUNDARY,BOUNDARY");
            }
        } //end i==1
    } // end if(i>0)
    else{ // i==0
        context.push_back(L"T-1=BOUNDARY");
        context.push_back(L"T-2=BOUNDARY");
        context.push_back(L"W-2,-1=BOUNDARY,BOUNDARY");
        if(i<n-1){
            context.push_back(L"W-1,+1=BOUNDARY," + words[i+1]);
            context.push_back(L"W-1,0,+1=BOUNDARY," +words[i] + L"," + words[i+1]);
            if(i<n-2){
                context.push_back(L"W+1,+2=" + words[i+1] + L"," + words[i+2]);
                context.push_back(L"W-2,-1,+1,+2=BOUNDARY,BOUNDARY," +words[i+1]+ L"," +words[i+2]);
            }else{
                context.push_back(L"W+1,+2=" + words[i+1] + L",BOUNDARY");
                context.push_back(L"W-2,-1,+1,+2=BOUNDARY,BOUNDARY," +words[i+1]+ L",BOUNDARY");
            }
        }else{
            context.push_back(L"W-1,+1=BOUNDARY,BOUNDARY");
            context.push_back(L"W-1,0,+1=BOUNDARY," + words[i] + L",BOUNDARY");
            context.push_back(L"W+1,+2=BOUNDARY,BOUNDARY");
            context.push_back(L"W-2,-1,+1,+2=BOUNDARY,BOUNDARY,BOUNDARY,BOUNDARY");
        }
    } //end i==0
}

void poc_train(const char* file, const string& destFile,const char* extractFile,
        string method, size_t iters,float gaussian){
      TrainerData data;
      data.get_context = get_poc_zh_scontext;
      train(&data, file, destFile, extractFile, method, iters, gaussian);
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

        wstring ws = CPPStringUtils::from_utf8w(line);
        POCTokenizer token(ws, boost::char_separator<wchar_t>(L" "));
        POCTokenizer::const_iterator itr = token.begin();
        if( itr == token.end() ){
            out<<endl;
            continue;
        }
        
        while(true){
            size_t pos = itr->find_first_of(TAG_SEP);
            if(pos == wstring::npos || pos == 0){
                cerr<<"The Format is word/tag, but not ("<<
                      CPPStringUtils::to_utf8(*itr)<<")"<<endl;
                exit(1);
            }
            wstring word = itr->substr(0,pos);
            if(word.length() == 1){
                out<<CPPStringUtils::to_utf8(word)<<"/I";
            }else{
                out<<CPPStringUtils::to_utf8(word.substr(0, 1))<<"/L ";
                size_t lastIndex = word.length() - 1;
                for(size_t i=1; i<lastIndex; ++i)
                    out<<CPPStringUtils::to_utf8(word.substr(i, 1))<<"/M ";
                out<<CPPStringUtils::to_utf8(word.substr(lastIndex, 1))<<"/R";
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

void SegTagger::tag_word(vector<wstring>& words, int i, size_t N,
        vector<wstring>& hist, vector<pair<wstring, double> >& ret){

    //cout<<hist.size()<<","<<i<<endl;
    assert(hist.size() == i);

    vector<wstring> context;
    bool exists = tagDict_.count(words[i]) > 0;
    get_context(words, hist, i, !exists, context);

    vector<string> evts;
    for(vector<wstring>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        evts.push_back(CPPStringUtils::to_utf8(*itr));
    }

    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(evts, outcomes, true);
    wstring preTag = (i > 0) ? hist[i-1] : L"R";

    size_t origN = N;
    if(exists){
        //need to optimize
        for(vector<pair<outcome_type, double> >::iterator itr = outcomes.begin();
                itr != outcomes.end(); ++itr){
            wstring tag = CPPStringUtils::from_utf8w(itr->first);
            if(tagDict_[words[i]][tag]){
                if(!is_matched_poc(preTag, tag))
                    continue;
                ret.push_back(pair<wstring, double>(tag,itr->second));
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
        wstring tag = CPPStringUtils::from_utf8w(itr->first);
        if(!is_matched_poc(preTag, tag))
            continue;
        ret.push_back(pair<wstring, double>(tag, itr->second));
        --N;
        //at most N tags
        if(!N) return;
    }
    
}

void SegTagger::advance(pair<vector<wstring>,double> tag, vector<wstring>& words,
        int i, size_t N, vector<pair<vector<wstring>,double> >& ret){
    vector<pair<wstring, double> > pos;
    //pos contains at most N elements
    tag_word(words, i, N, tag.first, pos);
    for(vector<pair<wstring, double> >::iterator itr = pos.begin();
          itr != pos.end(); ++itr){
        vector<wstring> tag0 = tag.first;
        tag0.push_back(itr->first);
        double score = itr->second * tag.second;
        ret.push_back(pair<vector<wstring>, double>(tag0, score));
        //cout<<"#"<<i<<", tag="<<CPPStringUtils::to_utf8(itr->first)<<",score="<<itr->second<<endl;
    }
}

void SegTagger::seg_sentence(wstring sentence, size_t N,
        vector<pair<vector<wstring>, double> >& segment){
    size_t n = sentence.length();

    vector<wstring> words;
    for(size_t i=0; i<n; ++i){
        words.push_back(sentence.substr(i, 1));
    }

    vector<pair<vector<wstring>, double> > h0;
    pair<vector<wstring>, double> s;
    s.second = 1.0;
    h0.push_back(s);

    for(size_t i=0; i<n; ++i){
        //cout<<"Beg "<<i<<","<<CPPStringUtils::to_string(words[i])<<endl;
        size_t sz = min(N, h0.size());
        vector<pair<vector<wstring>, double> > h1;
        for(size_t j=0; j<sz; ++j){
            vector<pair<vector<wstring>,double> > r;
            advance(h0.back(), words, i, N, r);
            //pop the last element
            h0.pop_back();
            for(vector<pair<vector<wstring>,double> >::iterator itr = r.begin();
                  itr != r.end(); ++itr){
                h1.push_back(*itr);
            }
        }
        //cout<<i<<":"<<h1.size()<<endl;
        sort(h1.begin(), h1.end(), cmpSDPOCPair);
        h0.empty();
        size_t h0Size = min(N, h1.size());
        h0.insert(h0.begin(), h1.begin(), h1.begin() + h0Size);
    }
    //cout<<"h0 size = "<<h0.size()<<endl;
    //construct the sentence
    for(vector<pair<vector<wstring>,double> >::iterator itr = h0.begin();
          itr != h0.end(); ++itr){
        vector<wstring> tags = itr->first;
        vector<wstring> seg;
        size_t begin = 0;
        for(size_t i = 0; i < n; ++i){
            if(tags[i] == L"R"){
                seg.push_back(sentence.substr(begin, i-begin+1));
                begin = i + 1;
            }else if(tags[i] == L"I"){
                assert(begin == i);
                seg.push_back(sentence.substr(i, 1));
                begin = i + 1;
            }else if(tags[i] == L"L"){
                assert(begin == i);
            }else if(tags[i] == L"M"){
                //do nothing
            }else{
                // unrecognize tag
                assert(0);
            }
        }

        if(begin != n)
            seg.push_back(sentence.substr(begin, n-begin)); //append the last segment
        segment.push_back(pair<vector<wstring>,double>(seg, itr->second));
    }
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
        wstring sentence = CPPStringUtils::from_utf8w(line);

        vector<pair<vector<wstring>, double> > segment;
        seg_sentence(sentence, 3, segment);

        //print the best result
        vector<wstring> best = segment[0].first;

        size_t n = best.size();
        for(size_t i=0; i<n; ++i){
            out<<CPPStringUtils::to_utf8(best[i]);
            if(i != n - 1)
                out<<" ";
        }
        out<<endl;
    }

    in.close();
    out.close();
}

}
