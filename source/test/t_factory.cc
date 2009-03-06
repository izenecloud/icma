/** \file test_factory.cpp
 * Test to create an instance of CMA_Factory.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 23, 2009
 */

#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

#include <iostream>
#include <cassert>

using namespace std;
using namespace cma;

int main()
{
    CMA_Factory* factory = CMA_Factory::instance();

    // ensure only one instance exists
    assert(factory == CMA_Factory::instance());

    // create analyzer and knowledge
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // load files
    int result = knowledge->loadStatModel("model");
    if(result == 0)
    {
	cerr << "fail to load statistical model" << endl;
	exit(1);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);

    // analyze a sentence
    Sentence s;
    s.setString("abc");
    analyzer->runWithSentence(s);

    delete knowledge;
    delete analyzer;

    return 0;
}
