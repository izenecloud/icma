
#include <vector>

#include "CMABasicTrainer.h"

#include <iostream>
#include <stdlib.h>
using namespace std;

const boost::char_separator<wchar_t> UNIT_SEP(L" ");

namespace cma{

/**
 * split out word and pos/poc in s into two separate lists.<br>
 * Format: word1/tag1 word2/tag2 word3/tag3
 *
 * \param s the target string
 * \param words return vector to store words list
 * \return tags return value to store tag list
 */
void CMABasicTrainer::split_tag(const wstring& s, vector<wstring>& words,
        vector<wstring>& tags){
    typedef boost::tokenizer <boost::char_separator<wchar_t>,
        wstring::const_iterator, wstring> TagTokenizer;
    TagTokenizer token(s, UNIT_SEP);
    for(TagTokenizer::const_iterator itr = token.begin(); itr != token.end(); ++itr){
        size_t pos = itr->find_first_of(TAG_SEP);
        if(pos == wstring::npos || pos == 0){
            wcerr<<"The Format is word/tag, but not "<<*itr<<endl;
            exit(1);
        }
        words.push_back(itr->substr(0,pos));
        tags.push_back(itr->substr(pos+1, itr->length() - pos));
    }
}

void CMABasicTrainer::gather_feature(wstring& word, vector<wstring>& context,
        wstring& tag){
    //only collect tag dict for common words
    if(!is_rare_word(word))
        tagDict_[word][tag] += 1;
   
    for(vector<wstring>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        featDict_[itr->append(tag)] += 1;
    }
}

bool CMABasicTrainer::is_rare_word(wstring& word){
    return wordFreq_[word] > rareFreq_;
}

void CMABasicTrainer::add_event(wstring& word, vector<wstring>& context,
        wstring& tag){
    vector<string> evts;
    for(vector<wstring>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        if(featDict_.count(itr->append(tag)) > 0)
            evts.push_back(CPPStringUtils::to_utf8(*itr));
    }

    if(!evts.empty())
        me.add_event(evts, CPPStringUtils::to_utf8(tag));
}

void CMABasicTrainer::save_training_data(wstring& word, vector<wstring>& context,
        wstring& tag){
    vector<string> evts;
    for(vector<wstring>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        if(featDict_.count(itr->append(tag)) > 0)
            evts.push_back(CPPStringUtils::to_utf8(*itr));
    }

    if(!evts.empty()){
        (*training_data)<<CPPStringUtils::to_utf8(tag)<<" ";
        for(vector<string>::iterator itr = evts.begin(); itr != evts.end(); ++itr){
            (*training_data)<<CPPStringUtils::to_utf8(*itr)<<" ";
        }
        (*training_data)<<"\n";
    }
}

void CMABasicTrainer::add_heldout_event(wstring& word, 
        vector<wstring>& context, wstring& tag){
    vector<string> evts;
    for(vector<wstring>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        if(featDict_.count(itr->append(tag)) > 0)
            evts.push_back(CPPStringUtils::to_utf8(*itr));
    }

    if(!evts.empty())
        me.add_heldout_event(evts, CPPStringUtils::to_utf8(tag));
}


void CMABasicTrainer::gather_word_freq(const char* file){
    ifstream in(file);
    string line;

    while(!in.eof()){
        getline(in, line);
        vector<wstring> words;
        vector<wstring> tags;
        split_tag(CPPStringUtils::from_utf8w(line), words, tags);
        for(vector<wstring>::iterator itr = words.begin();
              itr != words.end(); ++itr){
            wordFreq_[*itr] += 1;
        }
    }


}


void CMABasicTrainer::get_chars(wstring& word, vector<wstring>& ret){
    for(size_t i=0; i<word.length(); i+=2){
        ret.push_back(word.substr(i, 2));
    }
}


void CMABasicTrainer::extract_feature(const char* file,
        void (*func)(wstring& ,vector<wstring>& ,wstring&)){
    ifstream in(file);
    string line;

    while(!in.eof()){
        getline(in, line);
        if(!line.size())
            continue;
        vector<wstring> words;
        vector<wstring> tags;
        split_tag(CPPStringUtils::from_utf8w(line), words, tags);

        for(size_t i=0; i<words.size(); ++i){
            vector<wstring> context;
            get_context(words, tags, i, wordFreq_[words[i]] < rareFreq_, context);
            func(words[i], context, tags[i]);
        }
    }
}

}
