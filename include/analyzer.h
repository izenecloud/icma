/** \file analyzer.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#ifndef CMA_ANALYZER_H
#define CMA_ANALYZER_H

#include <vector>

namespace cma
{

class Knowledge;
class Sentence;

/**
 * Analyzer executes the Chinese morphological analysis.
 * Typically, the usage is like below:
 *
 * // create instances
 * CMA_Factory* factory = CMA_Factory::instance();
 * Analyzer* analyzer = factory->createAnalyzer();
 * Knowledge* knowledge = factory->createKnowledge();
 *
 * // load dictionaries
 * knowledge->loadSystemDict("...");
 * knowledge->loadUserDict("...");
 * knowledge->loadStopWordDict("...");
 * knowledge->loadStatModel("...");
 *
 * // set knowledge
 * analyzer->setKnowledge(knowledge);
 *
 * // analyze a sentence
 * Sentence s;
 * s.setString("...");
 * analyzer->runWithSentence(s);
 * ...
 *
 * // analyze a paragraph
 * const char* result = analyzer->runWithString("...");
 * ...
 *
 * // analyze a file
 * analyzer->runWithStream("...", "...");
 *
 * delete knowledge;
 * delete analyzer;
 */
class Analyzer
{
public:
    Analyzer();
    virtual ~Analyzer();

    /**
     * Set the \e Knowledge for analysis.
     * \param pKnowledge the pointer of \e Knowledge
     */
    virtual void setKnowledge(Knowledge* pKnowledge) = 0;

    /**
     * Execute the morphological analysis based on a sentence.
     * \param sentence an instance of \e Sentence
     * \return 0 for fail, 1 for success
     * \pre the raw sentence string could be got by \e sentence.getString().
     * \post on successful return, the n-best results could be got by \e sentence.getListSize(), etc.
     */
    virtual int runWithSentence(Sentence& sentence) = 0;

    /**
     * Execute the morphological analysis based on a paragraph string.
     * \param inStr paragraph string
     * \return 0 for fail, otherwise a non-zero string pointer for the one-best result
     */
    virtual const char* runWithString(const char* inStr) = 0;

    /**
     * Execute the morphological analysis based on a file.
     * \param inFileName input file name
     * \param outFileName output file name
     * \return 0 for fail, 1 for success
     * \post on successful return, the one-best result is printed to file \e outFileName.
     */
    virtual int runWithStream(const char* inFileName, const char* outFileName) = 0;

    /**
     * Option type for analysis.
     */
    enum OptionType
    {
	OPTION_TYPE_POS_TAGGING, ///< non-zero value to print part-of-speech tags, which is the default value
	OPTION_TYPE_NUM ///< the count of option types
    };

    /**
     * Set the option value for analysis.
     * \param nOption the option type
     * \param nValue the option value
     */
    void setOption(OptionType nOption, double nValue);

    /**
     * Get the option value.
     * \param nOption the option type
     * \return the option value
     */
    double getOption(OptionType nOption) const;

private:
    /** option values */
    std::vector<double> options_;
};

} // namespace cma

#endif // CMA_ANALYZER_H