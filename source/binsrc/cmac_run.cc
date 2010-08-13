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

// CMA_ME_Knowledge.h is just for test here, don't include it for other purpose
#include "icma/me/CMA_ME_Knowledge.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include <ctime>

#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace cma;

//#define DEBUG_FMM

namespace
{
    /** command options */
    const char* OPTIONS[] = {"--sentence", "--string", "--stream", "--sentstream"};

    /** optional command option for dictionary path */
    const char* OPTION_DICT = "--dict";
}

string testFilePrefix = "/home/vernkin/temp/cmac/icwb2/icwb2_test.";
string testFileSuffix;

Knowledge* knowledge;

vector<string> testSrcVec;
bool srcVecLoadError = false;

vector<string> disabledWords;

void loadSpecialSentence(string& inout)
{
	if( disabledWords.empty() == false )
	{
	    knowledge->enableWords( disabledWords );
	    disabledWords.clear();
	}

    if( inout.find( "##" ) == 0 )
	{
	    inout = inout.substr( 2 );
	    disabledWords.push_back( inout );
	    knowledge->disableWords( disabledWords );
	    return;
	}

    if( inout.find("@@") != 0 && inout.find("!!") != 0 )
	    return;

    if(!srcVecLoadError && testSrcVec.empty()){
		const char* testFile = ( testFilePrefix + testFileSuffix ).c_str();
		ifstream in(testFile);
		if(!in){
			cout<<"[Error] Cannot open test file "<<testFile<<endl;
			inout = "";
			srcVecLoadError = true;
			return;
		}

		string line;
		while(!in.eof())
		{
			getline(in, line);
			testSrcVec.push_back(line);
		}

		in.close();
	}

	if(inout.find("@@") == 0)
	{


		int lineNo = atoi(inout.substr(2).data());
		if(lineNo < 1 || lineNo > (int)testSrcVec.size())
		{
			cout<<"[Error] Valid Line Number is 1 to "<<testSrcVec.size()<<endl;
			inout = "";
			return;
		}

		inout = testSrcVec[lineNo-1];
		return;
	}

	if(inout.find("!!") == 0)
	{
		string dest = "";
		for(size_t i=2; i<inout.length(); ++i)
		{
			if(inout[i] != ' ')
				dest.append(inout.substr(i, 1));
		}

		if(dest.empty())
		{
			inout = "";
			return;
		}

		inout = "";
		int len = (int)testSrcVec.size();
		for( int i=0; i<len; ++i )
		{
			string& line = testSrcVec[i];
			if( line.find(dest) != line.npos )
			{
				cout<<"[Info] Founded at line: "<< ( i + 1 ) <<endl;
				inout.append(line);
			}
		}

		return;
	}
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
    	loadSpecialSentence(line);
    	if(line.empty())
    		continue;
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
            cout << "n-best result ( " << s.getListSize() << " ):" << endl;
            for(int i=0; i<s.getListSize(); ++i)
            {
            	cout << "\t" << i << ": ";
            	for(int j=0; j<s.getCount(i); ++j)
                {
                    const char* pLexicon = s.getLexicon(i, j);
                    cout << pLexicon << "/" << s.getStrPOS(i, j) << "(" << s.getPOS(i,j) << ")  ";
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
                cout << pLexicon << "/" << s.getStrPOS(i, j) << "(" << s.getPOS(i,j) << ")  ";
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
    	loadSpecialSentence(line);
    	if(line.empty())
    	    continue;
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

void testSentStream(Analyzer* analyzer, const char* source)
{
    string line;
    ifstream fin( source );
    Sentence sent;
    while( fin.eof() == false )
    {
        getline( fin, line );
        sent.setString( line.c_str() );
        analyzer->runWithSentence( sent );
    }
}

/**
 * Analyze a string.
 */
void testWithNGram(Analyzer* analyzer, string nGramValue)
{
	//analysis the nGramValue
	int first, second;
	size_t idx = nGramValue.find( '-' );
	if( idx == nGramValue.npos )
	{
		first = second = atoi( nGramValue.data() );
	}
	else
	{
		first = atoi( nGramValue.substr( 0, idx ).data() );
		second = atoi( nGramValue.substr( idx + 1 ).data() );
		if( second < first )
		{
			int tmp = first;
			first = second;
			second = tmp;
		}
	}

	vector<int> nArray;
	for( int i = first; i <= second; ++i )
		nArray.push_back( i );

	cout << "########## test method runWithString()" << endl;
    cout << "please input string ended with newline:" << endl;

    string line;
    while(getline(cin, line))
    {
    	loadSpecialSentence(line);
    	if(line.empty())
    	    continue;
    	vector<string> output;
    	analyzer->getNGramArrayResult( line.c_str(), nArray, output );
    	cout << endl << "result:" << endl;
    	for( vector<string>::iterator itr = output.begin();
    			itr != output.end(); ++itr )
    		cout<<*itr<<" , ";
    	cout << endl;
        cout << endl << "please input string ended with newline:" << endl;
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
    cerr << "  or:\t" << OPTIONS[3] << " INPUT [--dict DICT_PATH] " << endl;
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
	    || (optionIndex == 2 && argc < 4) // command option "--stream INPUT OUTPUT"
    	|| (optionIndex == 3 && argc < 3)) // command option "--ngram N-Gram"
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

    knowledge = factory->createKnowledge();

    // set default dictionary file
    const char* modelPath = "../db/icwb/utf8/";

    switch(optionIndex)
    {
    case 0:
        // set the number of N-best results,
        // if this function is not called, one-best analysis is performed defaultly on Analyzer::runWithSentence().
        analyzer->setOption(Analyzer::OPTION_TYPE_NBEST, atoi(argv[2]));

        // command option: "--sentence N-best --dict DICT_PATH"
        if(argc == 5 && ! strcmp(argv[3], OPTION_DICT))
        {
        	modelPath = argv[4];
        }
        break;

    case 1:
        // command option: "--string --dict DICT_PATH"
        if(argc == 4 && ! strcmp(argv[2], OPTION_DICT))
        {
        	modelPath = argv[3];
        }
        break;

    case 2:
        // command option: "--stream INPUT OUTPUT --dict DICT_PATH"
        if(argc == 6 && ! strcmp(argv[4], OPTION_DICT))
        {
        	modelPath = argv[5];
        }
        break;

    case 3:
        // command option: "--ngram N-Gram --dict DICT_PATH"
        if(argc == 5 && ! strcmp(argv[3], OPTION_DICT))
        {
        	modelPath = argv[4];
        }
        break;

    default:
        printUsage();
        exit(1);
    }

    int analysisType = 3;

    bool loadModel = ( analysisType == 1 );

    // set encoding type from the dictionary path
    string modelPathStr(modelPath);
    if(modelPathStr[ modelPathStr.length() -  1] != '/' )
    	modelPathStr += "/";
    size_t last = modelPathStr.find_last_of('/');
    size_t first = modelPathStr.find_last_of('/', last-1);
    string encodeStr = modelPathStr.substr(first+1, last-first-1);
    testFileSuffix = encodeStr;
    knowledge->loadModel( encodeStr.data(), modelPath, loadModel );
    size_t dicSize = ((CMA_ME_Knowledge*)knowledge)->getTrie()->size();
    printf("[Info] All Dictionaries' Size: %.2fm.\n", dicSize/1048576.0);
    //cout<<"[Info] All Dictionary Size: "<<(dicSize/1048576.0)<<"m"<<endl;
    // set knowledge
    analyzer->setKnowledge(knowledge);
//if(true) exit(0);
    // disable POS output for runWithStream
    if( optionIndex == 2 )
    	analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);

    analyzer->setOption( Analyzer::OPTION_ANALYSIS_TYPE, analysisType );
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

    case 3:
    {
        clock_t etime = clock();
        double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
        cout << "knowledge loading time: " << dif << endl;

        testSentStream(analyzer, argv[2]);

        dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
        cout << "sentence stream analysis time: " << dif << endl;

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
