/* 
 * \author vernkin
 *
 */

#ifndef _CMABASICTRAINER_H
#define	_CMABASICTRAINER_H

#include <string>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
#include <fstream>

#define TAG_SEP L'/'

#include "maxentmodel.hpp"
#include "CPPStringUtils.h"


using namespace std;
using namespace maxent;

namespace cma{

/** words, tags, i, rare_word, ret */
typedef void(*context_t)(vector<wstring>&, vector<wstring>&, size_t,
        bool, vector<wstring>&);



class TrainerData{

public:
    TrainerData(){
        rareFreq_ = 5;

        cutoff_ = 10;
        evCutoff_ = 1;

        get_context = 0;
    }


public:
    map<wstring, int> wordFreq_;
    map<wstring, int> featDict_;
    map<wstring, map<wstring, int> > tagDict_;
    /** use special feature for rare word with frequency &lt; RARE [default=5] */
    int rareFreq_;

    /** discard feature with frequency &lt; CUTOFF when training [default=5]*/
    int cutoff_;

    /** discard event with frequency &lt; CUTOFF when training [default=1]*/
    int evCutoff_;

    MaxentModel me;

    ofstream *training_data;

    /** words, tags, i, rare_word, ret */
    context_t get_context;
};



/**
 * split out word and pos/poc in s into two separate lists.<br>
 * Format: word1/tag1 word2/tag2 word3/tag3
 *
 * \param s the target string
 * \param words return vector to store words list
 * \return tags return value to store tag list
 */
void split_tag(const wstring& s, vector<wstring>& words, vector<wstring>& tags);

void token_wstring(const wstring& s, vector<wstring>& words);

void gather_feature(TrainerData* data, wstring& word, vector<wstring>& context,
        wstring& tag);

bool is_rare_word(TrainerData* data, wstring& word);

void add_event(TrainerData* data, wstring& word, vector<wstring>& context,
        wstring& tag);

void save_training_data(TrainerData* data, wstring& word,
        vector<wstring>& context, wstring& tag);

void add_heldout_event(TrainerData* data, wstring& word,
        vector<wstring>& context, wstring& tag);

void gather_word_freq(TrainerData* data, const char* file);

void get_chars(wstring& word, vector<wstring>& ret);

void extract_feature(TrainerData* data, const char* file,
        void (*func)(TrainerData*, wstring&, vector<wstring>&, wstring&));

void save_word_freq(TrainerData* data, const char* file);

void save_tag_dict(TrainerData* data, const char* file);

void load_tag_dict(map<wstring, map<wstring, int> > *tagDict, const char* file);

void save_features(TrainerData* data, const char* file);

void cutoff_feature(TrainerData* data, int cutoff, int rareCutoff);

/**
 * \param extractFile if set, save the training data to the extractFile and exit
 * \param iters how many iterations are required for training[default=15]
 * \param method the method of Maximum Model Parameters Estimation [default = lbfgs]
 * \param gaussian apply Gaussian penality when training [default=0.0]
 */
void train(TrainerData* data, const char* file, const string& destFile,
        const char* extractFile = 0, string method = "lbfgs",
        size_t iters = 15, float gaussian = 0.0f);

void fetchSegmentedFile(const char* inFile, const char* outFile,
        bool keepTag = true, bool keepSeparator = false);


}

#endif	/* _CMABASICTRAINER_H */

