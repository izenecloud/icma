/*
 * \file t_wordbound.cpp
 * \brief 
 * \date Jun 2, 2010
 * \author Vernkin Chen
 */



#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"
#include "word_bound.h"


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
    const char* OPTIONS[] = { "--text", "--stream", "--train" };

    /** optional command option for dictionary path */
    const char* OPTION_DICT = "--dict";
}



Knowledge* knowledge = NULL;
Analyzer* analyzer = NULL;
WordBound* wordBound = NULL;
vector< string > disabledWords;

bool isPossibleWord( string& input )
{
    if( disabledWords.empty() == true )
    {
        knowledge->enableWords( disabledWords );
        disabledWords.clear();
    }

    disabledWords.push_back( input );
    knowledge->disableWords( disabledWords );

    Sentence sent( input.c_str() );
    analyzer->runWithSentence( sent );
    return wordBound->isPossibeWord( sent );
}


/**
 * Analyze a string.
 */
void testWithString()
{
    cout << "########## test method runWithString()" << endl;

    string line;

    do{
        cout << "input: ";
        getline(cin, line);
        if(line.empty())
            continue;
        cout << "result:" << isPossibleWord( line ) << endl << endl;
    }while( true );
}

/**
 * Analyze a stream.
 */
void testWithStream( const char* source, const char* dest )
{
    cout << "########## test method runWithStream()" << endl;

    ifstream in( source );
    if( in == NULL || in.fail() == true )
    {
        cerr << "Fail to open source file " << source << endl;
        return;
    }
    ofstream out( dest );
    if( out == NULL || out.fail() == true )
    {
        cerr << "Fail to open destination file " << dest << endl;
        return;
    }

    string line;
    while( in.eof() == false )
    {
        getline( in, line );
        if( line.empty() == true )
        {
            out << endl;
            continue;
        }
        out << line << ": " << isPossibleWord( line ) << endl;
    }

    in.close();
    out.close();
}

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t" << OPTIONS[0] << " [--dict DICT_PATH]" << endl;
    cerr << "  or:\t" << OPTIONS[1] << " INPUT OUTPUT [--dict DICT_PATH]" << endl;
    cerr << "  or:\t" << OPTIONS[2] << " [--dict DICT_PATH]" << endl;
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
        || (optionIndex == 0 && argc < 2) // command option "--text"
        || (optionIndex == 1 && argc < 4) // command option "--stream INPUT OUTPUT"
        || (optionIndex == 2 && argc < 2)) // command option "--train"
    {
        printUsage();
        exit(1);
    }

    clock_t stime = 0;
    if(optionIndex == 2)
    {
        stime = clock();
    }

    // set default dictionary file
    const char* modelPath = "../db/icwb/utf8/";

    switch(optionIndex)
    {
    case 0:
        // command option: "--text --dict DICT_PATH"
        if(argc > 3 && ! strcmp(argv[2], OPTION_DICT))
        {
            modelPath = argv[3];
        }
        break;

    case 1:
        // command option: "--stream INPUT OUTPUT --dict DICT_PATH"
        if(argc > 5 && ! strcmp(argv[4], OPTION_DICT))
        {
            modelPath = argv[5];
        }
        break;

    case 2:
        // command option: "--train --dict DICT_PATH"
        if(argc > 3 && ! strcmp(argv[2], OPTION_DICT))
        {
            modelPath = argv[3];
        }
        break;

    default:
        printUsage();
        exit(1);
    }

    //bool loadModel = true;


    // set encoding type from the dictionary path
    string modelPathStr(modelPath);
    if(modelPathStr[ modelPathStr.length() -  1] != '/' )
        modelPathStr += "/";
    modelPath = modelPathStr.c_str();
    string wbModelFile = modelPathStr + "wb.model";

    // training
    if( optionIndex == 2 )
    {
        cout << "########## test method training" << endl;
        vector< string > dicts;
        dicts.push_back( modelPathStr + "sys.dic" );
        //dicts.push_back( "/home/vernkin/sys.dic" );
        int ret = WordBound::trainModel( modelPath, dicts, wbModelFile.c_str() );
        clock_t etime = clock();
        double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
        cout << "Training output is " <<  wbModelFile << " and return value is "
                << ret << "." << endl;
        cout << "Training total time: " << dif << endl;

        exit(0);
    }

    // create instances
    CMA_Factory* factory = CMA_Factory::instance();
    knowledge = factory->createKnowledge();
    knowledge->loadModel( modelPath );
    analyzer = factory->createAnalyzer();
    analyzer->setKnowledge(knowledge);

    wordBound = new WordBound;
    if( wordBound->loadModel( wbModelFile.c_str() ) == false )
    {
        exit(1);
    }

    switch(optionIndex)
    {
    case 0:
        testWithString();
        break;

    case 1:
    {
        clock_t etime = clock();
        double dif = (double)(etime - stime) / CLOCKS_PER_SEC;
        cout << "Initializing time: " << dif << endl;

        testWithStream( argv[2], argv[3] );

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
    delete wordBound;

    return 0;
}
