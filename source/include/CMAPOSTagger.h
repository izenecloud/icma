/* 
 * File:   CMAPOSTagger.h
 * Author: vernkin
 *
 * Created on March 20, 2009, 10:46 AM
 */

#ifndef _CMAPOSTAGGER_H
#define	_CMAPOSTAGGER_H

#include "CMABasicTrainer.h"

namespace cma{

inline bool isNumber(wstring& word){
    for(size_t i=0; i<word.length(); ++i){
        if(word[i] < '0' || word[i] > '9')
            return false;
    }
    return true;
}

inline bool isHyphen(wstring& word){
    return word.length() == 1 && word[0] == '-';
}

inline bool isUpperCase(wstring& word){
    for(size_t i=0; i<word.length(); ++i){
        if(word[i] < 'A' || word[i] > 'Z')
            return false;
    }
    return true;
}

void get_prefix_suffix(wstring& word, size_t length, vector<wstring>& prefixes,
        vector<wstring>& suffixes);

/**
 * POS context type for POS (zh/chinese)
 */
void get_pos_zh_scontext(vector<wstring>& words, vector<wstring>& tags, size_t i,
        bool rareWord, vector<wstring>& context);

}
#endif	/* _CMAPOSTAGGER_H */

