/**
 * \file seg_scd.cpp
 * \brief 
 * \date Sep 1, 2010
 * \author Vernkin Chen
 */

#include "icma/icma.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

using namespace std;
using namespace cma;

/**
 * Print the test usage.
 */
void printUsage()
{
    cerr << "Usages:\t" << "./seg_scd INPUT_SCD OUTPUT_SCD" << endl;
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if( argc < 3 )
    {
        printUsage();
        exit(1);
    }

    const char* inFile = argv[ 1 ];
    const char* outFile = argv[ 2 ];

    ifstream fin( inFile );
    if( fin == NULL || fin.fail() == true )
    {
        cerr << "Fail to open input: " << inFile << endl;
        exit(1);
    }

    // initialize icma
    cout << "Initialize iCMA ... " << endl;
    const char* modelPath = "../db/icwb/utf8/";
    CMA_Factory* factory = CMA_Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    knowledge->loadModel( modelPath );
    analyzer->setKnowledge( knowledge );
    analyzer->setOption( Analyzer::OPTION_TYPE_POS_TAGGING, 0 );
    analyzer->setOption( Analyzer::OPTION_ANALYSIS_TYPE,  1 );

    cout << "Initilization done! " << endl;
    cout << "Begin segment SCD file ... " << endl;

    ofstream fout( outFile );
    string line;
    bool toSeg = false;
    while( fin.eof() == false )
    {
        getline( fin, line );
        if( line.empty() == true )
        {
            fout << endl;
            continue;
        }

        if( line[ 0 ] == '<' )
        {
            size_t endPOS = line.find( '>', 1 );
            if( endPOS != line.npos )
            {
                string tagName = line.substr( 1, endPOS - 1 );
                //cout << "tagName " << tagName << endl;
                if( tagName != "Title" && tagName != "Content" )
                {
                    fout << line << endl;
                    toSeg = false;
                    continue;
                }
                fout << "<" << tagName << ">";
                toSeg = true;
                line = line.substr( endPOS + 1 );
            }

        }

        if( toSeg == false )
        {
            fout << line << endl;
            continue;
        }

        fout << analyzer->runWithString( line.c_str() ) << endl;

    }


    fin.close();
    fout.close();
    delete analyzer;
    delete knowledge;
    cout << "All done! " << endl;
}
