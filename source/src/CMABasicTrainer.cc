
#include <set>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "CMABasicTrainer.h"
#include "strutil.h"
#include "tokenizer.h"
#include "cma_wtype.h"

using namespace std;

namespace cma{

typedef boost::tokenizer <boost::char_separator<char>,
    string::const_iterator, string> TagTokenizer;

const boost::char_separator<char> UNIT_SEP(" ");

/**
 * Here take posDelimiter as '/' <BR>
 * split out word and pos/poc in s into two separate lists.<br>
 * Format: word1/tag1 word2/tag2 word3/tag3
 *
 * \param s the target string
 * \param words return vector to store words list
 * \return tags return value to store tag list
 * \param posDelimiter the specific pos delimiter
 */
void split_tag(const string& s, vector<string>& words, vector<string>& tags,
        string posDelimiter){
    TagTokenizer token(s, UNIT_SEP);
    size_t posDeliLen = posDelimiter.length();
    for(TagTokenizer::const_iterator itr = token.begin(); itr != token.end(); ++itr){
        string tmp = *itr;
        trimSelf(tmp);
        if(tmp.empty())
            continue;
        size_t pos = tmp.find_last_of(posDelimiter);
        if(pos == string::npos || pos == 0 || pos == (*itr).length() - posDeliLen){
            cout<< "The Format is word" << posDelimiter << "tag, but not ("
                    <<*itr<<")"<<endl;
            exit(1);
        }
        words.push_back(itr->substr(0,pos));
        tags.push_back(itr->substr(pos + posDeliLen, itr->length() - pos));
    }
}

void gather_feature(TrainerData* data, string& word, vector<string>& context,
        string& tag){
    //only collect tag dict for common words
    if(!data->isRareWord(word))
        data->tagDict_[word][tag] += 1;
    
    for(vector<string>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        data->featDict_[*itr + "_" + tag] += 1;
    }
}

void add_event(TrainerData* data, string& word, vector<string>& context,
        string& tag){
    vector<string> evts;
    for(vector<string>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        if(data->featDict_.count(*itr + "_" + tag) > 0)
            evts.push_back(*itr);
    }

    if(!evts.empty())
        data->me.add_event(evts, tag);
}

void save_training_data(TrainerData* data, string& word, vector<string>& context,
        string& tag){
    vector<string> evts;
    for(vector<string>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        if(data->featDict_.count(*itr + "_" + tag) > 0)
            evts.push_back(*itr);
    }

    ofstream *training_data = data->training_data;

    if(!evts.empty()){
        (*training_data)<<tag<<" ";
        for(vector<string>::iterator itr = evts.begin(); itr != evts.end(); ++itr){
            (*training_data)<<*itr<<" ";
        }
        (*training_data)<<"\n";
    }
}

void add_heldout_event(TrainerData* data, string& word,
        vector<string>& context, string& tag){
    vector<string> evts;
    for(vector<string>::iterator itr = context.begin();
          itr != context.end(); ++itr){
        if(data->featDict_.count(*itr + "_" + tag) > 0)
            evts.push_back(*itr);
    }

    if(!evts.empty())
        data->me.add_heldout_event(evts, tag);
}


void gather_word_freq(TrainerData* data, const char* file){
    ifstream in(file);
    assert(in);
    string line;    
    while(!in.eof()){
        getline(in, line);
        vector<string> words;
        vector<string> tags;
        split_tag(line, words, tags, data->posDelimiter);
        for(vector<string>::iterator itr = words.begin();
              itr != words.end(); ++itr){
            data->wordFreq_[*itr] += 1;
        }
    }
    in.close();

}

void extract_feature(TrainerData* data, const char* file,
        void (*func)(TrainerData*, string& ,vector<string>& ,string&)){
    ifstream in(file);
    string line;

    while(!in.eof()){
        getline(in, line);
        if(!line.size())
            continue;
        vector<string> words;
        vector<string> tags;
        split_tag(line, words, tags, data->posDelimiter);

        for(size_t i=0; i<words.size(); ++i){
            vector<string> context;
            data->get_context(words, tags, i, data->wordFreq_[words[i]] < data->rareFreq_, 
                    context, data->ctype_);
            func(data, words[i], context, tags[i]);
        }
    }

    in.close();
}

void save_word_freq(TrainerData* data, const char* file){
    ofstream out(file);
    for(map<string, int>::iterator itr = data->wordFreq_.begin();
          itr != data->wordFreq_.end(); ++itr){
        out<<itr->first<<" "<<itr->second<<endl;
    }
    out.close();
}

void save_tag_dict(TrainerData* data, const char* file){
    CMA_WType wtype(data->ctype_);
    ofstream out(file);
    for(map<string, map<string, int> >::iterator itr = data->tagDict_.begin();
          itr != data->tagDict_.end(); ++itr){
        CMA_WType::WordType type = wtype.getWordType(itr->first.data());
        switch(type){
            //ignore the combined words
            case CMA_WType::WORD_TYPE_DATE:
            case CMA_WType::WORD_TYPE_LETTER:
            case CMA_WType::WORD_TYPE_NUMBER:
                continue;
            default:
                break;
        }
        out<<itr->first;
        map<string, int>& inner = itr->second;
        for(map<string, int>::iterator itr2 = inner.begin();
              itr2 != inner.end(); ++itr2){
            out<<" "<<itr2->first;
        }
        out<<endl;
    }
    out.close();
}

void save_pos_list(TrainerData* data, const char* file){
    set<string> posSet;
    ofstream out(file);
    for(map<string, map<string, int> >::iterator itr = data->tagDict_.begin();
          itr != data->tagDict_.end(); ++itr){
        map<string, int>& inner = itr->second;
        for(map<string, int>::iterator itr2 = inner.begin();
              itr2 != inner.end(); ++itr2){
            posSet.insert(itr2->first);
        }
    }

    for(set<string>::iterator itr = posSet.begin(); itr != posSet.end(); ++itr){
        out<<*itr<<endl;
    }
    out.close();
}

void load_tag_dict(map<string, map<string, int> > *tagDict, const char* file){
    ifstream in(file);
    assert(in);
    string line;
    while(!in.eof()){
        getline(in, line);
        string ws = line;
        TagTokenizer token(ws, UNIT_SEP);
        TagTokenizer::iterator itr = token.begin();
        if(itr == token.end())
            continue;
        string word = *itr++;
        while(itr != token.end()){
            string tag = *itr++;
            string count = *itr++;
            (*tagDict)[word][tag] = atoi(count.data());
        }
    }

    in.close();
}

void save_features(TrainerData* data, const char* file){
    ofstream out(file);
    for(map<string, int>::iterator itr = data->featDict_.begin();
          itr != data->featDict_.end(); ++itr){
        out<<itr->first<<" "<<itr->second<<endl;
    }
    out.close();
}

void cutoff_feature(TrainerData* data){
    int cutoff = data->cutoff_;
    int rareCutoff = data->rareCutoff_;
    map<string, int> tmp;
    for(map<string, int>::iterator itr = data->featDict_.begin();
          itr != data->featDict_.end(); ++itr){
        if((itr->first).find("C0=") != (itr->first).npos){
            if(itr->second >= rareCutoff)
                tmp[itr->first] = itr->second;
        }else if(itr->second >= cutoff){
            tmp[itr->first] = itr->second;
        }
    }
    data->featDict_ = tmp;
}

void train(TrainerData* data, const char* file, const string cateName,
        const char* extractFile, string method, size_t iters, float gaussian,
        bool isPOS){

    //First pass: gather word frequency information {{{
    cout<<"First pass: gather word frequency information"<<endl;
    gather_word_freq(data, file);
    #ifdef DEBUG_TRAINING
        string wordFreqFile = cateName + ".wordfreq";
        save_word_freq(data, wordFreqFile.data());
    #endif
    // }}}

    //Second pass: gather features and tag dict {{{
    cout<<"Second pass: gather features and tag dict to be used in tagger"<<endl;
    extract_feature(data, file, gather_feature);
    cutoff_feature(data);
    #ifdef DEBUG_TRAINING
        string featureFile = cateName + ".features";
        save_features(data, featureFile.data());
        string tagFile = cateName + ".dic";
        save_tag_dict(data, tagFile.data());
    #else
        if(isPOS){
            string sysDictFile = cateName + ".dic";
            save_tag_dict(data, sysDictFile.data());
        }
    #endif
        
    if(isPOS){
        string posFile = cateName + ".pos";
        save_pos_list(data, posFile.data());
    }

    
    // }}}

    if(extractFile){
        cout<<"Saving training data to "<<extractFile<<endl;
        data->training_data = new ofstream(extractFile);
        extract_feature(data, file, save_training_data);
        data->training_data->close();
        delete data->training_data;
        exit(0);
    }

    // Third pass:training ME model...{{{
    cout<<"Third pass:training ME model..."<<endl;
    data->me.begin_add_event();
    extract_feature(data, file, add_event);
    data->me.end_add_event(data->evCutoff_);

    data->me.train(iters, method, gaussian);
    cout<<"training finished"<<endl;
    string destFile = cateName + ".model";
    cout<<"saving tagger model to "<<destFile<<endl;
    data->me.save(destFile);
    // }}}
}

void create_poc_meterial(const char* inFile, const char* outFile,
        Knowledge::EncodeType type, string posDelimiter){
    ifstream in(inFile);
    ofstream out(outFile);
    string line;

    typedef boost::tokenizer <boost::char_separator<char>,
        string::const_iterator, string> POCTokenizer;

    CTypeTokenizer ctypeToken(CMA_CType::instance(type));

    size_t posDeliLen = posDelimiter.length();
    while(!in.eof()){
        getline(in, line);
        trimSelf(line);
        if(line.length() == 0){
            continue;
        }

        POCTokenizer token(line, boost::char_separator<char>(" "));
        POCTokenizer::const_iterator itr = token.begin();
        if( itr == token.end() ){
            out<<endl;
            continue;
        }
        
        while(true){
            size_t pos = itr->find_last_of(posDelimiter);
            if(pos == string::npos || pos == 0 ||
                    pos == (*itr).length() - posDeliLen){
                cerr<<"The Format is word" << posDelimiter <<"tag, but not ("
                        <<*itr<<")"<<endl;
                exit(1);
            }

            string word = itr->substr( 0 , pos);
            ctypeToken.assign(word.c_str());
            
            vector<string> charVec;
            const char* nextPtr;
            while((nextPtr = ctypeToken.next())){
                charVec.push_back(nextPtr);
            }

            size_t charLen = charVec.size();
            if(charLen > 1)
                out<<charVec[0]<<"/B ";
            else
                out<<charVec[0]<<"/B";
            
            for(size_t i=1; i<charLen; ++i){
                if(i == charLen - 1)
                    out<<charVec[i]<<"/E";
                else
                    out<<charVec[i]<<"/E ";
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


void fetchSegmentedFile(const char* inFile, const char* outFile, 
        string posDelimiter, bool keepTag, bool keepSeparator){
    ifstream in(inFile);
    ofstream out(outFile);

    string line;
    char delimChar = posDelimiter[0];
    while(!in.eof()){
        getline(in, line);

        bool inTag = false;
        for(string::iterator itr = line.begin(); itr != line.end(); ++itr){
            if(*itr == delimChar){
                inTag = true;
            }else if(*itr == ' '){
                inTag = false;
                if(keepSeparator)
                    out<<' ';
                continue;
            }

            if(inTag){
                if(keepTag)
                    out<<*itr;
            }else{
                out<<*itr;
            }
        }

        if(!in.eof())
            out<<endl;
    }
    in.close();
    out.close();
}

}
