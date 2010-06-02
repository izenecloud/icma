/*
 * \file word_bound.cc
 * \brief the class to check whether a word possibly exists
 * \date Jun 2, 2010
 * \author Vernkin Chen
 */

#include "word_bound.h"
#include "sentence.h"
#include "knowledge.h"
#include "analyzer.h"
#include "cma_factory.h"
#include "VTrie.h"

#include <iostream>
#include <fstream>
using namespace std;

#define WB_NAME_NUM 5

namespace cma
{

const string WB_PREFIX_N  = "1";
const string WB_SUFFIX_N = "2";
const string WB_PREFIXPOS_N = "3";
const string WB_SUFFIXPOS_N = "4";
const string WB_POSSEQUENCE_N = "5";
const string WB_POS_SEP = "_";

namespace wbinner
{


void construtWBNames( Sentence& sent, vector< string >& out )
{
    out.clear();
    if( sent.getListSize() < 1 )
        return;
    int bestIdx = sent.getOneBestIndex();
    int wordCount = sent.getCount( bestIdx );
    if( wordCount < 2 )
        return;

    out.resize( 5 );

    // prefix
    out[ 0 ] = WB_PREFIX_N + sent.getLexicon( bestIdx, 0 );

    // suffix
    out[ 1 ] = WB_SUFFIX_N + sent.getLexicon( bestIdx, wordCount - 1 );

    // prefix POS
    out[ 2 ] = WB_PREFIXPOS_N + sent.getLexicon( bestIdx, 0 ) + WB_POS_SEP + sent.getStrPOS( bestIdx, 0 );

    // suffix POS
    out[ 3 ] = WB_SUFFIXPOS_N + sent.getLexicon( bestIdx, wordCount - 1 ) + WB_POS_SEP + sent.getStrPOS( bestIdx, wordCount - 1 );

    // pos sequence
    out[ 4 ] = WB_POSSEQUENCE_N + sent.getStrPOS( bestIdx, 0 );
    for( int wIdx = 1; wIdx < wordCount; ++wIdx )
    {
        out[ 4 ] += WB_POS_SEP + sent.getStrPOS( bestIdx, wIdx );
    }
}

void trainWord( Sentence& sent, VTrie& trie )
{
    vector< string > names;
    construtWBNames( sent, names );
    if( names.empty() == true )
        return;

    VTrieNode node;
    node.data = 1;
    for( int i = 0; i < WB_NAME_NUM; ++i )
    {
        trie.insert( names[ i ].c_str(), &node );
    }
}

}

WordBound::WordBound()
    : minScore_( 3 )
{
    trie_ = new VTrie;
}

WordBound::~WordBound()
{
    delete trie_;
}

int WordBound::trainModel(
        const char* iCMAModelPath,
        const vector< string >& dicts,
        const char* wbModelFile
        )
{
    int ret = 0;
    CMA_Factory* factory = CMA_Factory::instance();
    Knowledge* knowledge = factory->createKnowledge();
    knowledge->loadModel( iCMAModelPath );
    Analyzer* analyzer = factory->createAnalyzer();
    analyzer->setKnowledge( knowledge );

    vector< string > disabledWords;

    VTrie trie;
    for( vector< string >::const_iterator dictItr = dicts.begin();
            dictItr != dicts.end(); ++dictItr )
    {
        ifstream fin( dictItr->c_str() );
        if( fin == NULL )
        {
            ++ret;
            cerr << "[Error][WordBound Training] Fail to open dictionary file: "
                 << *dictItr << "." << endl;
            continue;
        }

        string line;
        while( fin.eof() == false )
        {
            getline( fin, line );
            if( line.empty() == true )
                continue;
            size_t spaceIdx = line.find( ' ' );
            if( spaceIdx == line.npos )
                spaceIdx = line.length();
            line = line.substr( 0, spaceIdx );

            if( disabledWords.empty() == false )
            {
                knowledge->enableWords( disabledWords );
                disabledWords.clear();
            }

            disabledWords.push_back( line );
            knowledge->disableWords( disabledWords );

            Sentence sent;
            sent.setString( line.c_str() );
            analyzer->runWithSentence( sent );
            wbinner::trainWord( sent, trie );
            //cout << "Train line " << line << "." << endl;
        }

        fin.close();
    }
    trie.saveToFile( wbModelFile );

    delete analyzer;
    delete knowledge;
    return ret;
}

bool WordBound::loadModel( const char* wbModelFile )
{
    return trie_->loadFromFile( wbModelFile );
}

void WordBound::setMinScore( double score )
{
    minScore_ = score;
}

double WordBound::getMinScore()
{
    return minScore_;
}

bool WordBound::isPossibeWord( Sentence& sent )
{
    vector< string > names;
    wbinner::construtWBNames( sent, names );
    if( names.empty() == true )
        return true;

    double score = 0;
    VTrieNode node;
    for( int i = 0; i < WB_NAME_NUM; ++i )
    {
        trie_->search( names[ i ].c_str(), &node );
        score += node.data;
        cout << "Check " << names[ i ] << ": " << node.data << endl;
    }
    cout << "Score " << score << endl;
    return score >= minScore_;
}

}
