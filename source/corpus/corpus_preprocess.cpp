/** \file corpus_preprocess.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 23, 2009
 */

#include "corpus_preprocess.h"

#include <set>
#include <map>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cassert>

using namespace std;

namespace cma
{

const string CorpusPreprocess::FILE_OPEN_ERROR("Error in opening file: ");

typedef set<string> StrVec_t;
typedef map<string, StrVec_t> Dict_t;

void CorpusPreprocess::extractDictionary(const char* corpusFile, const char* dictFile, Dict_Format format)
{
    // open corpusFile
    assert(corpusFile);
    ifstream from(corpusFile);
    if(!from) {
        throw runtime_error(FILE_OPEN_ERROR + corpusFile);
    }

    Dict_t dictionary;

    // read corpusFile into dictionary
    string line, word, pos;
    string::size_type i, j;
    while(getline(from, line)) {
        line = line.substr(0, line.find('\r'));
        if(!line.empty()) {
            i = 0;
            while(i != string::npos) {
                j = line.find('/', i);
                assert(j != string::npos);
                word = line.substr(i, j-i);

                i = j+1;
                j = line.find(' ', i);
                assert(j != i);
                pos = line.substr(i, j-i);

                switch(format) {
                    case DICT_FORMAT_WORDPOS:
                        dictionary[word].insert(pos);
                        break;

                    case DICT_FORMAT_POSWORD:
                        dictionary[pos].insert(word);
                        break;

                    default:
                        assert(false);
                        break;
                }

                i = line.find_first_not_of(' ', j);
            }
        }
    }

    // open dictFile
    assert(dictFile);
    ofstream to(dictFile);
    if(!to) {
        throw runtime_error(FILE_OPEN_ERROR + dictFile);
    }

    // write dictionary into dictFile
    for(Dict_t::const_iterator i=dictionary.begin(); i!=dictionary.end(); ++i) {
        to << i->first << " ";
        for(StrVec_t::const_iterator j=i->second.begin(); j!=i->second.end(); ++j) {
            to << *j << " ";
        }
        to << endl;
    }
}

CorpusPreprocess::~CorpusPreprocess()
{
}

} // namespace cma
