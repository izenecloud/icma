/** \file t_cmac_run.cpp
 * Test JMA in Japanese word segmentation and POS tagging analysis.
 * Below is the usage examples:
 * The "DICT_PATH" in below examples is the dictionary path, which is "../db/ctb/ctbc" defautly.
 * \code
 * To analyze a sentence from standard input and print 5 n-best results to standard output.
 * $ ./cmac_run --sentence 5 [--dict DICT_PATH]
 * 
 * To analyze a paragraph string from standard input and print the one-best result to standard output.
 * $ ./cmac_run --string [--dict DICT_PATH]
 *
 * To exit the loop in the above examples, please press CTRL-C.
 *
 * To analyze the raw input file "INPUT", and print the one-best result to "OUTPUT".
 * (the example of "INPUT" file could be available as "../db/test/asahi_test_raw_eucjp.txt")
 * $ ./cmac_run --stream INPUT OUTPUT [--dict DICT_PATH]
 * \endcode
 * 
 * \author Vernkin
 * \version 0.1
 * \date April 2, 2009
 */

#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include <ctime>

#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace cma;

namespace
{
    /** command options */
    const char* OPTIONS[] = {"--sentence", "--string", "--stream"};

    /** optional command option for dictionary path */
    const char* OPTION_DICT = "--dict";
}

/**
 * Analyze a sentence.
 */
void testWithSentence(Analyzer* analyzer)
{
    cout << "########## test method runWithSentence()" << endl;
    
    Sentence s;
    string line;
    cout << "please input sentence ended with newline:" << endl;
    while(getline(cin, line))
    {
        s.setString(line.c_str());

        int result = analyzer->runWithSentence(s);
        if(result != 1)
        {
            cerr << "fail in Analyzer::runWithSentence()" << endl;
            exit(1);
        }

        // get n-best results
        if(s.getListSize() == 0)
        {
            cout << "no n-best result exists." << endl;
        }
        else
        {
            cout << "n-best result:" << endl;
            for(int i=0; i<s.getListSize(); ++i)
            {
                for(int j=0; j<s.getCount(i); ++j)
                {
                    if(j == 0)
                        cout << "\t" << i << ": ";

                    const char* pLexicon = s.getLexicon(i, j);
                    cout << pLexicon << "/" << s.getStrPOS(i, j) << "  ";
                }
                cout << "\t" << s.getScore(i) << "\t#words: " << s.getCount(i) << endl;
            }
            cout << endl;
        }

        // get one-best result
        int i= s.getOneBestIndex();
        if(i == -1)
        {
            cout << "no one-best result exists." << endl;
        }
        else
        {
            cout << "one-best result:" << endl;
            for(int j=0; j<s.getCount(i); ++j)
            {
                if(j == 0)
                    cout << "\t" << i << ": ";

                const char* pLexicon = s.getLexicon(i, j);
                cout << pLexicon << "/" << s.getStrPOS(i, j) << "  ";
            }
            cout << "\t" << s.getScore(i) << endl;
        }

        cout << endl << "please input sentence ended with newline:" << endl;
    }
}

/**
 * Analyze a string.
 */
void testWithString(Analyzer* analyzer)
{
    cout << "########## test method runWithString()" << endl;
    cout << "please input string ended with newline:" << endl;

    string line;
    while(getline(cin, line))
    {
        cout << endl << "result:" << endl << analyzer->runWithString(line.c_str()) << endl;
        cout << endl << "please input string ended with newline:" << endl;
    }
}

/**
 * Analyze a stream.
 */
void testWithStream(Analyzer* analyzer, const char* source, const char* dest)
{
    cout << "########## test method runWithStream()" << endl;

    int result = analyzer->runWithStream(source, dest);
    if(result == 1)
    {
        cout << "succeed in runWithStream() from " << source << " to " << dest << endl;
    }
    else
    {
        cerr << "fail in runWithStream() from " << source << " to " << dest << endl;
        exit(1);
    }
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t" << OPTIONS[0] << " N-best [--dict DICT_PATH]" << endl;
    cerr << "  or:\t" << OPTIONS[1] << " [--dict DICT_PATH]" << endl;
    cerr << "  or:\t" << OPTIONS[2] << " INPUT OUTPUT [--dict DICT_PATH]" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printUsage();
        exit(1);
    }

    unsigned int optionIndex = 0;
    unsigned int optionSize = sizeof(OPTIONS) / sizeof(OPTIONS[0]);
    for(; optionIndex<optionSize; ++optionIndex)
    {
        if(! strcmp(argv[1], OPTIONS[optionIndex]))
            break;
    }

    // check argument
    if((optionIndex == optionSize)
	    || (optionIndex == 0 && argc < 3) // command option "--sentence N-best"
	    || (optionIndex == 2 && argc < 4)) // command option "--stream INPUT OUTPUT"
    {
        printUsage();
        exit(1);
    }

    // time evaluation for command option "--stream INPUT OUTPUT"
    clock_t stime = 0;
    if(optionIndex == 2)
    {
        stime = clock();
    }
    
    // create instances
    CMA_Factory* factory = CMA_Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();

    // no POS output
    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);

    Knowledge* knowledge = factory->createKnowledge();

    // set default dictionary file
    const char* sysdict = 0;
#if defined(_WIN32) && !defined(__CYGWIN__)
    sysdict = "../../db/icwb/gb18030/icwbc";
#else
    sysdict = "../db/icwb/gb18030/icwbc";
#endif

    switch(optionIndex)
    {
    case 0:
        // set the number of N-best results,
        // if this function is not called, one-best analysis is performed defaultly on Analyzer::runWithSentence().
        analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, atoi(argv[2]));

        // command option: "--sentence N-best --dict DICT_PATH"
        if(argc == 5 && ! strcmp(argv[3], OPTION_DICT))
        {
            sysdict = argv[4];
        }
        break;

    case 1:
        // command option: "--string --dict DICT_PATH"
        if(argc == 4 && ! strcmp(argv[2], OPTION_DICT))
        {
            sysdict = argv[3];
        }
        break;

    case 2:
        // command option: "--stream INPUT OUTPUT --dict DICT_PATH"
        if(argc == 6 && ! strcmp(argv[4], OPTION_DICT))
        {
            sysdict = argv[5];
        }
        break;

    default:
        printUsage();
        exit(1);
    }

    // load dictioanry files
    string cate = sysdict;
    string poc_mate = cate + "-poc";
    knowledge->loadStatModel(poc_mate.data());

    //check whether exist pos model
    ifstream posIn((cate + ".model").data());
    if(posIn)
    {
    	posIn.close();
    	knowledge->loadPOSModel(cate.data());
    }
    else
    {
    	cout<<"Cannot found POS model, ignore POS output "<<endl;
    	analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
    }

    knowledge->loadUserDict((cate+".dic").data());

    knowledge->setEncodeType(Knowledge::ENCODE_TYPE_GB18030);

    // set encoding type from the dictionary path
    string sysdictStr(sysdict);
    size_t last = sysdictStr.find_last_of('/');
    size_t first = sysdictStr.find_last_of('/', last-1);
    string encodeStr = sysdictStr.substr(first+1, last-first-1);
    Knowledge::EncodeType encode = Knowledge::decodeEncodeType(encodeStr.c_str());
    if(encode != Knowledge::ENCODE_TYPE_NUM)
    {
        cout << "set encoding type: " << encodeStr << endl;
        knowledge->setEncodeType(encode);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);



    switch(optionIndex)
    {
    case 0:
        testWithSentence(analyzer);
        break;

    case 1:
        testWithString(analyzer);
        break;

    case 2:
    {
        clock_t etime = clock();
        double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
        cout << "knowledge loading time: " << dif << endl;

        testWithStream(analyzer, argv[2], argv[3]);

        dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
        cout << "stream analysis time: " << dif << endl;

        dif = (double)(clock() - stime) / CLOCKS_PER_SEC;
        cout << "total time: " << dif << endl;
        break;
    }

    default:
        printUsage();
        exit(1);
    }

    // destroy instances
    delete knowledge;
    delete analyzer;

    return 0;
}
