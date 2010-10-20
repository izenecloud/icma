/**
 * \file t_option.cpp
 * \brief 
 * \date Oct 20, 2010
 * \author Vernkin Chen
 */
#include "icma/icma.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include <ctime>
#include <algorithm>
#include <string.h>
#include <stdlib.h>

using namespace std;
using namespace cma;

// returns a lower case version of the string
inline string toUpper (const string & s)
{
    string d (s);
    transform (d.begin (), d.end (), d.begin (), (int(*)(int)) toupper);
    return d;
}

enum ChineseAnalysisType
{
    maximum_entropy = 1,
    maximum_match = 2,
    minimum_match = 3,
};

void printUsage()
{
    cout << "Usage: ./t_option [ME] [Max_M] [Min_M] [R+] [R0] [R1] ... [R9]" << endl;
    cout << "\t Each option can appear at most time." << endl;
    cout << "\t All analysis type are included if no explict analysis type given." << endl;
    cout << "\t R+ is given if no R* given" << endl;
    cout << "Example: ./t_option ME Max_M Min_M R+ R0  to test R+ and R0 for " <<
            "all three analysis type" << endl;
    cout << endl;
}

/**
 * \return true when all element is whenIs
 */
bool setToDefWhenAllIs(bool* boolArray, int len, bool whenIs, bool def)
{
    for( int i = 0; i < len; ++i )
    {
        if( boolArray[i] != whenIs )
            return false;
    }

    for( int i = 0; i < len; ++i )
    {
        boolArray[i] = def;
    }

    return true;
}

int main(int argc, char** argv)
{
    // between 0 to nBestSize - 1 (included)
    const int nBestSize = 10;
    const int analyzerSize = 3;

    bool nBest[nBestSize] = { false };
    bool enabledAnalyzers[analyzerSize] = {false};
    Analyzer *analyzers[analyzerSize] = { NULL };
    string anayzerTitles[analyzerSize] = { "ME", "Max_M", "Min_M" };
    ChineseAnalysisType enumTypes[analyzerSize] = { maximum_entropy, maximum_match, minimum_match };

    printUsage();

    // parse options
    for( int i = 1; i < argc; ++i )
    {
        string op( argv[ i ] );
        op = toUpper(op);

        if( op == "ME" )
        {
            enabledAnalyzers[ 0 ] = true;
        }
        else if( op == "MAX_M" )
        {
            enabledAnalyzers[ 1 ] = true;
        }
        else if( op == "MIN_M" )
        {
            enabledAnalyzers[ 2 ] = true;
        }
        else if( op[0] == 'R' )
        {
            size_t opLen = op.length();
            if(opLen == 1)
            {
                cerr << "Invalid NBest Option: " << op << endl;
                exit(1);
            }
            string nStr = op.substr( 1, opLen - 1 );
            int n = 0;
            if( nStr == "+" )
            {
                n = 2;
            }
            else
            {
                n = atoi( nStr.c_str() );
                if( n == 0 && nStr != "0" )
                {
                    cerr << "Invalid NBest Option: " << op << endl;
                    exit(1);
                }
            }

            if( n < 0 || n >= nBestSize )
            {
                cerr << "Invalid NBest Range: " << nStr <<
                        ". Should be in [0, " << nBestSize << ")"<< endl;
                exit(1);
            }
            nBest[n] = true;
        }
        else
        {
            cerr << "Invalid Unknown Option: " << op << endl;
            exit(1);
        }
    }

    // If no analysis type given, enable all
    setToDefWhenAllIs( enabledAnalyzers, analyzerSize, false, true );

    // If no NBest given, enable R+
    if( setToDefWhenAllIs( nBest, nBestSize, false, false ) == true )
    {
        nBest[ 2 ] = true;
    }


    CMA_Factory* factory = CMA_Factory::instance();

    // Create Knowledge
    Knowledge* knowledge = factory->createKnowledge();
    // set default dictionary file
    const char* modelPath = "../db/icwb/utf8/";
    // when use ME, loadModel is true
    bool loadModel = enabledAnalyzers[ 0 ];
    knowledge->loadModel( modelPath, loadModel );

    // initialize analyzers
    for( int i = 0; i < analyzerSize; ++i )
    {
        if( enabledAnalyzers[ i ] == true )
        {
            analyzers[ i ] = factory->createAnalyzer();
            analyzers[ i ]->setKnowledge( knowledge );
            analyzers[ i ]->setOption( Analyzer::OPTION_ANALYSIS_TYPE, enumTypes[ i ] );
        }
    }

    // query
    string line;
    while( true )
    {
        cout << "Input ('x' to exit): " << endl;
        getline( cin, line );
        if( line.empty() == true )
            continue;
        if( line == "x" || line == "X" )
            break;

        // print output
        for( int anaIdx = 0; anaIdx < analyzerSize; ++anaIdx )
        {
            if( analyzers[ anaIdx ] == NULL )
                continue;
            cout << "########## Analysis Type " << anayzerTitles[ anaIdx ] << " ########### " << endl;
            Analyzer* analyzer = analyzers[ anaIdx ];
            Sentence s( line.c_str() );
            for( int nbIdx = 0; nbIdx < nBestSize; ++nbIdx )
            {
                if( nBest[ nbIdx ] == false )
                    continue;
                cout << "#### When NBest = " << nbIdx << endl;
                analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, nbIdx );
                analyzer->runWithSentence( s );

                cout << "n-best result ( " << s.getListSize() << " ):" << endl;
                for(int i=0; i<s.getListSize(); ++i)
                {
                    cout << "\t" << i << ": ";
                    for(int j=0; j<s.getCount(i); ++j)
                    {
                        const char* pLexicon = s.getLexicon(i, j);
                        cout << pLexicon << "/" << s.getStrPOS(i, j) << "#";
                        if( s.isIncrementedWordOffset() )
                            cout << j;
                        else
                            cout << s.getOffset( i, j );
                        cout << "  ";
                    }
                    cout << "\t" << s.getScore(i) << "\t#words: " << s.getCount(i) << endl;
                    cout << endl;
                }
            }

            cout << endl << endl;
        }
    }

    // release resource
    delete knowledge;
    for( int i = 0; i < analyzerSize; ++i )
        delete analyzers[ i ];
}
