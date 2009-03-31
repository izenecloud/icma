
#include <string>

#include "CMAPOSTagger.h"

namespace cma{



void get_prefix_suffix(wstring& word, size_t length, vector<wstring>& prefixes,
        vector<wstring>& suffixes){
    size_t l = min(word.length(), length);
    for(size_t i=1; i<=l; ++i){
        prefixes.push_back(word.substr(0, i));
        suffixes.push_back(word.substr(word.length()-i, i));
    }
}

void get_pos_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context){
    string& w = words[i];
    size_t n = words.size();

    if(rareWord){
        /*vector<wstring> prefixes;
        vector<wstring> suffixes;
        get_prefix_suffix(w, 2, prefixes, suffixes);
        for(vector<wstring>::iterator itr = prefixes.begin(); 
              itr != prefixes.end(); ++itr){
            context.push_back(L"prefix=" + *itr);
        }

        for(vector<wstring>::iterator itr = suffixes.begin();
              itr != suffixes.end(); ++itr){
            context.push_back(L"suffix=" + *itr);
        }*/

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
        context.push_back("tag-1=" + tags[i-1]);
        if(i>1){
            context.push_back("word-2=" + words[i-2]);
            context.push_back("tag-1,2=" + tags[i-2] + "," + tags[i-1]);
        }else{
            context.push_back("word-2=BOUNDARY");
            context.push_back("tag-1,2=BOUNDARY," + tags[i-1]);
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

void pos_train(const char* file, const string& cateFile,const char* extractFile,
        string method, size_t iters,float gaussian){
      TrainerData data;
      data.get_context = get_pos_zh_scontext;
      train(&data, file, cateFile, extractFile, method, iters, gaussian);
}


void POSTagger::tag_word(vector<string>& words, int i, size_t N, 
        vector<string>& hist, vector<pair<string, double> >& ret){

    //cout<<hist.size()<<","<<i<<endl;
    assert(hist.size() == i);

    vector<string> context;
    bool exists = tagDict_.count(words[i]) > 0;
    get_context(words, hist, i, !exists, context);

    if(N == 1){
        string tag;
        double score = me.eval(context, tag);
        ret.push_back(pair<string, double>(tag, score));
        return;
    }


    vector<pair<outcome_type, double> > outcomes;
    me.eval_all(context, outcomes, true);

    size_t origN = N;
    if(exists){
        //need to optimize 
        for(vector<pair<outcome_type, double> >::iterator itr = outcomes.begin();
                itr != outcomes.end(); ++itr){
            string& tag = itr->first;
            if(tagDict_[words[i]][tag]){
                ret.push_back(pair<string, double>(tag,itr->second));
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
        ret.push_back(pair<string, double>(itr->first, itr->second));
        --N;
        //at most N tags
        if(!N) return;
    }
}

void POSTagger::advance(pair<vector<string>,double> tag, vector<string>& words,
        int i, size_t N, vector<pair<vector<string>,double> >& ret){
    vector<pair<string, double> > pos;
    //pos contains at most N elements
    tag_word(words, i, N, tag.first, pos);
    for(vector<pair<string, double> >::iterator itr = pos.begin();
          itr != pos.end(); ++itr){
        vector<string> tag0 = tag.first;
        tag0.push_back(itr->first);
        double score = itr->second * tag.second;
        ret.push_back(pair<vector<string>, double>(tag0, score));
    }
}

void POSTagger::tag_sentence(vector<string>& words, size_t N,
        vector<pair<vector<string>, double> >& h0){
    size_t n = words.size();
    pair<vector<string>, double> s;
    s.second = 1.0;
    h0.push_back(s);

    for(size_t i=0; i<n; ++i){
        size_t sz = min(N, h0.size());
        vector<pair<vector<string>, double> > h1;
        for(size_t j=0; j<sz; ++j){
            vector<pair<vector<string>,double> > r;
            advance(h0.back(), words, i, N, r);
            //pop the last element
            h0.pop_back();
            for(vector<pair<vector<string>,double> >::iterator itr = r.begin();
                  itr != r.end(); ++itr){
                h1.push_back(*itr);
            }
        }

        sort(h1.begin(), h1.end(), cmpSDPair);
        h0.empty();
        size_t h0Size = min(N, h1.size());
        h0.insert(h0.begin(), h1.begin(), h1.begin() + h0Size);

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

        vector<pair<vector<string>, double> > h0;
        tag_sentence(words, 5, h0);

        //print the best result
        vector<string>& best = h0[0].first;

        size_t n = words.size();
        for(size_t i=0; i<n; ++i){
            out<<words[i] << "/"<< best[i];
            if(i != n - 1)
                out<<" ";
        }

        out<<endl;
    }

    in.close();
    out.close();
}

}
