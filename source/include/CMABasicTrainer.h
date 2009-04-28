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

#include "cmacconfig.h"
#include "maxentmodel.hpp"
#include "knowledge.h"
#include "cma_ctype.h"
#include "knowledge.h"

using namespace std;
using namespace maxent;

namespace cma{

/** words, tags, i, rare_word, ret */
typedef void(*context_t)(vector<string>&, vector<string>&, size_t,
        bool, vector<string>&, CMA_CType *ctype);

/**
 * The ckass to hold data required in the training process
 */
class TrainerData{

public:
    TrainerData(context_t pGet_context, Knowledge::EncodeType encType, 
            string pPosDelimiter = "/") : get_context(pGet_context),
            posDelimiter(pPosDelimiter){
        rareFreq_ = 1;

        cutoff_ = 10;
        evCutoff_ = 1;

        ctype_ = CMA_CType::instance(encType);
    }

    bool isRareWord(string& word){
        return wordFreq_[word] < rareFreq_;
    }

public:
    map<string, int> wordFreq_;
    map<string, int> featDict_;
    map<string, map<string, int> > tagDict_;
    /** use special feature for rare word with frequency &lt; RARE [default=2] */
    int rareFreq_;

    /** discard feature with frequency &lt; CUTOFF when training [default=5]*/
    int cutoff_;

    /** discard event with frequency &lt; evCutoff when training [default=1]*/
    int evCutoff_;

    MaxentModel me;

    ofstream *training_data;

    /** words, tags, i, rare_word, ret */
    context_t get_context;

    CMA_CType *ctype_;

    string posDelimiter;
};



/**
 * split out word and pos/poc in s into two separate lists.<br>
 * Format: word1/tag1 word2/tag2 word3/tag3
 *
 * \param s the target string
 * \param words return vector to store words list
 * \return tags return value to store tag list
 */
void split_tag(const string& s, vector<string>& words, vector<string>& tags,
        string posDelimiter);

/**
 * Gather the feature from the context
 * \param data the TrainerData
 * \param word the current word
 * \param context the context of the current word
 * \param tag the tag for the current word
 */
void gather_feature(TrainerData* data, string& word, vector<string>& context,
        string& tag);

/**
 * Add the event to the Maxent Model
 * \param data the TrainerData
 * \param word the current word
 * \param context the context for the current word
 * \param tag the tag for the current word
 */
void add_event(TrainerData* data, string& word, vector<string>& context,
        string& tag);

/**
 * Save the training data to the file
 * \param data the TrainerData
 * \param word the current word
 * \param context the context for the current word
 * \param tag the tag for the current word
 */
void save_training_data(TrainerData* data, string& word,
        vector<string>& context, string& tag);

/**
 * Add the heldout event to the Maxent model
 * \param word the current word
 * \param context the context for the current word
 * \param tag the tag for the current word
 */
void add_heldout_event(TrainerData* data, string& word,
        vector<string>& context, string& tag);

/**
 * Gather the word frequenty information
 * \param data the TrainerData
 * \param the input file
 */
void gather_word_freq(TrainerData* data, const char* file);

/**
 * Extract the features from the specific file
 * \param data the TrainerData
 * \param file the input file
 * \param func the function to be invoked after extracting the features
 */
void extract_feature(TrainerData* data, const char* file,
        void (*func)(TrainerData*, string&, vector<string>&, string&));

/**
 * Save the word frequency information to the file. <br>
 * With each line as: word frequency
 *
 * \param data the TrainerData
 * \param file the output file
 */
void save_word_freq(TrainerData* data, const char* file);

/**
 * Save the tag dictionary to the file <br>
 * With each line as: word tag1 tag2 ... tagN
 *
 * \param data the TrainerData
 * \param file the output file
 */
void save_tag_dict(TrainerData* data, const char* file);

/**
 * Load the tag dictionary from the file <br>
 * With each line as: word tag1 tag2 ... tagN
 *
 * \param tagDict the map to store the result
 * \param file the input file
 */
void load_tag_dict(map<string, map<string, int> > *tagDict, const char* file);

/**
 * Save the features to the file
 */
void save_features(TrainerData* data, const char* file);

/**
 * Cut off the features. The cutoff frequency is TrainerData.cutoff_
 * and the rareCutoff is TrainerData.rareFreq_
 *
 * \param data the TrainerData
 */
void cutoff_feature(TrainerData* data);

/**
 * \param extractFile if set, save the training data to the extractFile and exit
 * \param iters how many iterations are required for training[default=15]
 * \param method the method of Maximum Model Parameters Estimation [default = gis]
 * \param gaussian apply Gaussian penality when training [default=0.0]
 * \param isPOS if true, output the tag dictioanry
 */
void train(TrainerData* data, const char* file, const string cateFile,
        const char* extractFile = 0, string method = "gis",
        size_t iters = 15, float gaussian = 0.0f, bool isPOS = true);

/**
 * The input is AB/tag2 CDF/tag2 G/tag3 ... <br>
 * The output is A/L B/R C/L D/M F/M G/I <br>
 * L is Left; R is Rightl M is Middle; I is independent.
 *
 * The other tagset is <b>B</b> (Begin) and <b>E</b> (end)
 * \param inFile the input file
 * \param outFile the output file
 * \param type the encoding type
 * \param posDelimiter the demiliter for the pos, default is "/"
 */
void create_poc_meterial(const char* inFile, const char* outFile,
        Knowledge::EncodeType type, string posDelimiter = "/");

/**
 * fetch the segmented file to other format<br>
 * The input file with each line like (Take "/" as pos separator): <br>
 * word1/tag1 word2/tag2 word3/tag <br>
 * <br>
 * if keepSeparator is false and keepSeparator is true, the output is <br>
 * word1 word2 word3<br>
 * <br>
 * if keepSeparator is false and keepSeparator is false, the output is <br>
 * word1word2word3 (Without spaces bewteen words)<br>
 * <br>
 * \param inFile the input file
 * \param theFile the output file
 * \param posDelimiter the pos delimiter, default is "/"
 * \param keepTag whether keep the tag information
 * \param keepSeparator whether keep spaces between words
 */
void fetchSegmentedFile(const char* inFile, const char* outFile,
        string posDelimiter = "/",
        bool keepTag = true, bool keepSeparator = false);


}

#endif	/* _CMABASICTRAINER_H */

