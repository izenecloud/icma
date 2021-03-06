/**
 * \file bt_icma_core.cpp
 * \brief 
 * \date Aug 20, 2010
 * \author Vernkin Chen
 */

#include <boost/test/unit_test.hpp>

#include "icma/icma.h"

#include <string>
#include <vector>

using namespace cma;
using namespace std;

Knowledge* globalKnowledge = NULL;

void createKnowledgeAndAnalyzer(
        Knowledge** knowledge,
        Analyzer** analyzer,
        int analysisType = 3,
        const string dictPath = "../db/icwb/utf8/"
        )
{
    CMA_Factory* factory = CMA_Factory::instance();

    if( globalKnowledge == NULL )
    {
        globalKnowledge = factory->createKnowledge();
        globalKnowledge->loadModel( dictPath.c_str(), true );
    }
    *knowledge = globalKnowledge;

    *analyzer = factory->createAnalyzer();
    (*analyzer)->setKnowledge( *knowledge );
    (*analyzer)->setOption( Analyzer::OPTION_ANALYSIS_TYPE, analysisType );
}

BOOST_AUTO_TEST_SUITE(icma_core_test)

BOOST_AUTO_TEST_CASE(icma_basicapi)
{
    Knowledge* knowledge = NULL;
    Analyzer* analyzer = NULL;
    createKnowledgeAndAnalyzer( &knowledge, &analyzer, 3 );
    BOOST_CHECK( knowledge != NULL );
    BOOST_CHECK( analyzer != NULL );

    // test Knowledge API
    BOOST_CHECK( Knowledge::decodeEncodeType( "utf8" ) == Knowledge::ENCODE_TYPE_UTF8 );
    BOOST_CHECK( knowledge->isSupportPOS() == true );
    string word = "衣服";
    BOOST_CHECK( knowledge->isExistWord( word.c_str() ) == true );

    vector< string > words;
    words.push_back( word );
    knowledge->disableWords( words );
    BOOST_CHECK( knowledge->isExistWord( word.c_str() ) == false );
    knowledge->enableWords( words );
    BOOST_CHECK( knowledge->isExistWord( word.c_str() ) == true );


    // test Analyzer API
    analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, 3 );
    BOOST_CHECK( analyzer->getOption( Analyzer::OPTION_TYPE_NBEST) == 3 );

    int posCode;
    string posValue = "N";
    posCode = analyzer->getCodeFromStr( posValue );
    BOOST_CHECK( posCode >= 0 );
    string posRetValue = analyzer->getStrFromCode( posCode );
    BOOST_CHECK( posRetValue == posValue );

    delete analyzer;
    //delete knowledge;
}


// forwards minimum cover
BOOST_AUTO_TEST_CASE(icma_fminconver)
{
    Knowledge* knowledge = NULL;
    Analyzer* analyzer = NULL;
    createKnowledgeAndAnalyzer( &knowledge, &analyzer, 3 );
    BOOST_CHECK( knowledge != NULL );
    BOOST_CHECK( analyzer != NULL );

    // test run with string
    string input = "我和衣服的故事";
    string expected = "我/P  和/AG  和衣/V  衣/N  衣服/N  服/J  的/A  故事/N  ";
    const char* ret = analyzer->runWithString( input.c_str() );
    BOOST_CHECK( strcmp( ret, expected.c_str() ) == 0 );

    // test run with sentence
    analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, 3 );
    Sentence sent;
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 1 );
    BOOST_CHECK( sent.getCount( 0 ) == 8 );
    BOOST_CHECK( strcmp( sent.getLexicon( 0, 2 ), "和衣" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 0, 2 ), "V" ) == 0 );

    delete analyzer;
    //delete knowledge;
}

// forwards minimum cover with unigram
BOOST_AUTO_TEST_CASE(icma_fmincover_unigram)
{
	Knowledge* knowledge = NULL;
	Analyzer* analyzer = NULL;
	createKnowledgeAndAnalyzer( &knowledge, &analyzer, 4 );
    BOOST_CHECK( knowledge != NULL );
    BOOST_CHECK( analyzer != NULL );

    // test run with string
    string input = "我和衣服的故事";
    string expected = "我/P  和/AG  和衣/V  衣/N  衣服/N  服/J  的/A  故/C  故事/N  事/N  ";
    const char* ret = analyzer->runWithString( input.c_str() );
    BOOST_CHECK( strcmp( ret, expected.c_str() ) == 0 );

    // test run with sentence
    analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, 3 );
    Sentence sent;
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 1 );
    BOOST_CHECK( sent.getCount( 0 ) == 10 );
    BOOST_CHECK( strcmp( sent.getLexicon( 0, 2 ), "和衣" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 0, 2 ), "V" ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 0, 8 ), "故事" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 0, 8 ), "N" ) == 0 );

    delete analyzer;
}


// forwards maximum matching
BOOST_AUTO_TEST_CASE(icma_fmm)
{
    Knowledge* knowledge = NULL;
    Analyzer* analyzer = NULL;
    createKnowledgeAndAnalyzer( &knowledge, &analyzer, 2 );
    BOOST_CHECK( knowledge != NULL );
    BOOST_CHECK( analyzer != NULL );

    // test run with string
    string input = "我和衣服的故事";
    string expected = "我/P  和衣/V  服/J  的/A  故事/N  ";
    const char* ret = analyzer->runWithString( input.c_str() );
    BOOST_CHECK( strcmp( ret, expected.c_str() ) == 0 );

    // test run with sentence
    analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, 3 );
    Sentence sent;
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 1 );
    BOOST_CHECK( sent.getCount( 0 ) == 5 );
    BOOST_CHECK( strcmp( sent.getLexicon( 0, 2 ), "服" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 0, 2 ), "J" ) == 0 );

    delete analyzer;
    //delete knowledge;
}


// maximum entropy model
BOOST_AUTO_TEST_CASE(icma_mmmodel)
{
    Knowledge* knowledge = NULL;
    Analyzer* analyzer = NULL;
    createKnowledgeAndAnalyzer( &knowledge, &analyzer, 1 );
    BOOST_CHECK( knowledge != NULL );
    BOOST_CHECK( analyzer != NULL );

    // test run with string
    string input = "我和衣服的故事";
    string expected = "我/R  和/C  衣服/N  的/B  故事/N  ";
    const char* ret = analyzer->runWithString( input.c_str() );
    BOOST_CHECK( strcmp( ret, expected.c_str() ) == 0 );

    // test run with sentence
    analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, 3 );
    Sentence sent;
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 3 );
    BOOST_CHECK( sent.getCount( 0 ) == 5 );
    BOOST_CHECK( sent.getCount( 1 ) == 6 );
    BOOST_CHECK( sent.getCount( 1 ) == 6 );
    BOOST_CHECK( strcmp( sent.getLexicon( 0, 2 ), "衣服" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 0, 2 ), "N" ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 4 ), "故" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 1, 4 ), "D" ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 2, 2 ), "衣" ) == 0 );
    BOOST_CHECK( strcmp( sent.getStrPOS( 2, 2 ), "NG" ) == 0 );

    delete analyzer;
    //delete knowledge;
}

// maximum prefix in dictionary matching
BOOST_AUTO_TEST_CASE(icma_maxprefix)
{
    Knowledge* knowledge = NULL;
    Analyzer* analyzer = NULL;
    CMA_Factory* factory = CMA_Factory::instance();

    knowledge = factory->createKnowledge();
    analyzer = factory->createAnalyzer();
    BOOST_CHECK( knowledge != NULL );
    BOOST_CHECK( analyzer != NULL );

    knowledge->loadModel( "utf8", "../db/icwb/utf8/fmindex_dic/", false);
    analyzer->setOption( Analyzer::OPTION_ANALYSIS_TYPE, 100);
    analyzer->setOption( Analyzer::OPTION_TYPE_POS_TAGGING, 0);
    analyzer->setKnowledge( knowledge );

    // test run with string
    string input = "几十万";
    string dic_expected = "";
    string ret = analyzer->runWithString( input.c_str() );
    printf("result : %s\n", ret.c_str());
    BOOST_CHECK( ret.empty() );

    // test run with sentence
    Sentence sent;
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 2 );
    BOOST_CHECK( sent.getCount( 0 ) == 0 );
    BOOST_CHECK( sent.getCount( 1 ) == 2 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 0 ), "几十" ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 1 ), "十万" ) == 0 );
    //
    // test run with string
    input = "佳能";
    dic_expected = "佳能  ";
    ret = analyzer->runWithString( input.c_str() );
    printf("result : %s\n", ret.c_str());
    BOOST_CHECK( ret == dic_expected );

    // test run with sentence
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 2 );
    BOOST_CHECK( sent.getCount( 0 ) == 1 );
    BOOST_CHECK( sent.getCount( 1 ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 0, 0 ), "佳能" ) == 0 );

    // dictionary are all low-case, so upper-cash must not exist
    input = "佳能(CANON)CANON2";
    dic_expected = "佳能  ";
    ret = analyzer->runWithString( input.c_str() );
    printf("result : %s\n", ret.c_str());
    BOOST_CHECK( ret == dic_expected );

    // test run with sentence
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 2 );
    BOOST_CHECK( sent.getCount( 0 ) == 1 );
    BOOST_CHECK( sent.getCount( 1 ) == 2 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 0 ), "CANON" ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 1 ), "CANON2" ) == 0 );

    input = "(Test-non)Testnon佳能";
    dic_expected = "佳能  ";
    ret = analyzer->runWithString( input.c_str() );
    printf("result : %s\n", ret.c_str());
    BOOST_CHECK( ret == dic_expected );

    // test run with sentence
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 2 );
    BOOST_CHECK( sent.getCount( 0 ) == 1 );
    BOOST_CHECK( sent.getCount( 1 ) == 2 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 0 ), "Test-non" ) == 0 );
    BOOST_CHECK( strcmp( sent.getLexicon( 1, 1 ), "Testnon" ) == 0 );

    input = "佳能(canon)test non 单子不存在exist字典(6666) eos 60d单反套机(ef-s 18-135mm f/3.5-5.6is）单反镜头单子不存在";
    dic_expected = "佳能  canon  60d  单反  18-135mm  3.5-5.6  单反镜头  ";
    ret = analyzer->runWithString( input.c_str() );
    printf("result : %s\n", ret.c_str());
    BOOST_CHECK( ret == dic_expected );

    // test run with sentence
    sent.setString( input.c_str() );
    BOOST_CHECK( analyzer->runWithSentence( sent ) == 1 );
    BOOST_CHECK( sent.getListSize() == 2 );
    BOOST_CHECK( sent.getCount( 0 ) == 7 );
    BOOST_CHECK( sent.getCount( 1 ) == 18 );

    delete analyzer;
    delete knowledge;
}


BOOST_AUTO_TEST_SUITE_END()
