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

void get_pos_zh_scontext(vector<wstring>& words, vector<wstring>& tags, size_t i,
        bool rareWord, vector<wstring>& context){
    wstring w = words[i];
    size_t n = words.size();

    if(rareWord){
        vector<wstring> prefixes;
        vector<wstring> suffixes;
        get_prefix_suffix(w, 2, prefixes, suffixes);
        for(vector<wstring>::iterator itr = prefixes.begin(); 
              itr != prefixes.end(); ++itr){
            context.push_back(L"prefix=" + *itr);
        }

        for(vector<wstring>::iterator itr = suffixes.begin();
              itr != suffixes.end(); ++itr){
            context.push_back(L"suffix=" + *itr);
        }

        if(isNumber(w))
            context.push_back(L"numeric");

        if(isUpperCase(w))
            context.push_back(L"uppercase");
        
        if(isHyphen(w))
            context.push_back(L"hyphen");
    }else{
        context.push_back(L"curword=" + w);
    }

    if(i > 0){
        context.push_back(L"word-1=" + words[i-1]);
        context.push_back(L"tag-1=" + tags[i-1]);
        if(i>1){
            context.push_back(L"word-2=" + words[i-2]);
            context.push_back(L"tag-1,2=" + tags[i-2] + L"," + tags[i-1]);
        }else{
            context.push_back(L"word-2=BOUNDARY");
            context.push_back(L"tag-1,2=BOUNDARY," + tags[i-1]);
        }
    }else{
        context.push_back(L"word-1=BOUNDARY");
        context.push_back(L"word-2=BOUNDARY");
        context.push_back(L"tag-1=BOUNDARY");
        context.push_back(L"tag-1,2=BOUNDARY,BOUNDARY");
    }

    if(i+1 < n){
        context.push_back(L"word+1=" + words[i+1]);
        if(i+2 < n)
            context.push_back(L"word+2=" + words[i+2]);
        else
            context.push_back(L"word+2=BOUNDARY");
    }else{
        context.push_back(L"word+1=BOUNDARY");
        context.push_back(L"word+2=BOUNDARY");
    }
}

void POSTagger::tag_word(vector<wstring>& words, int i, vector<wstring>& hist,
        vector<pair<wstring, double> >& ret){

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
    me.eval_all(evts, outcomes);

    if(exists){
        for(vector<pair<outcome_type, double> >::iterator itr = outcomes.begin();
                itr != outcomes.end(); ++itr){
            wstring tag = CPPStringUtils::from_utf8w(itr->first);
            if(tagDict_[words[i]][tag])
                ret.push_back(pair<wstring, double>(tag,itr->second));
        }
    }else{
        for(vector<pair<outcome_type, double> >::iterator itr = outcomes.begin();
                itr != outcomes.end(); ++itr){
            ret.push_back(pair<wstring, double>(
                  CPPStringUtils::from_utf8w(itr->first),itr->second));
        }
    }
}

int POSTagger::tag_sentence(vector<wstring>& words, int N){
    size_t n = words.size();

    return 0;
}

}
