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

class CMABasicTrainer{

public:

    CMABasicTrainer(){
        rareFreq_ = 5;
        rareFreq_ = 5;
    }

    /**
     * split out word and pos/poc in s into two separate lists.<br>
     * Format: word1/tag1 word2/tag2 word3/tag3
     *
     * \param s the target string
     * \param words return vector to store words list
     * \return tags return value to store tag list
     */
    void split_tag(const wstring& s, vector<wstring>& words, vector<wstring>& tags);

    void gather_feature(wstring& word, vector<wstring>& context, wstring& tag);

    bool is_rare_word(wstring& word);

    void add_event(wstring& word, vector<wstring>& context, wstring& tag);

    void save_training_data(wstring& word, vector<wstring>& context, wstring& tag);

    void add_heldout_event(wstring& word, vector<wstring>& context, wstring& tag);

    void gather_word_freq(const char* file);

    void get_chars(wstring& word, vector<wstring>& ret);

    void extract_feature(const char* file,
            void (*func)(wstring&, vector<wstring>&, wstring&));

private:
    map<wstring, int> wordFreq_;
    map<wstring, int> featDict_;
    map<wstring, map<wstring, int> > tagDict_;
    int rareFreq_;
    int rareWord_;

    MaxentModel me;

    ofstream *training_data;

protected:
    /** words, tags, i, rare_word, ret */
    void (*get_context)(vector<wstring>&, vector<wstring>&, int, bool, wstring&);
};

}

#endif	/* _CMABASICTRAINER_H */

