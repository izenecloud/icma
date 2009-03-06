/** \file test_corpus_preprocess.cpp
 * Test to extract word and part-of-speech from corpus.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 23, 2009
 */

#include "corpus_preprocess.h"

#include <iostream>

using namespace std;
using namespace cma;

int main(int argc, char* argv[])
{
    if(argc < 3) {
    	cerr << "usage: ./a corpus dict [wordpos, posword]" << endl;
    	exit(1);
    }

    CorpusPreprocess::Dict_Format format = CorpusPreprocess::DICT_FORMAT_WORDPOS;
    if(argc > 3) {
        string formatStr(argv[3]);
        if(formatStr == "wordpos")
            format = CorpusPreprocess::DICT_FORMAT_WORDPOS;
        else if(formatStr == "posword")
            format = CorpusPreprocess::DICT_FORMAT_POSWORD;
        else {
            cerr << "format must be a string in [wordpos, posword]" << endl;
            exit(1);
        }
    }

    CorpusPreprocess::extractDictionary(argv[1], argv[2], format);
    return 0;
}
