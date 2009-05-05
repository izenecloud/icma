/** \file analyzer.h
 * \brief Analyzer executes the Chinese morphological analysis.
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
 * \brief Analyzer executes the Chinese morphological analysis.
 *
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
     * \param sentence the instance containing the raw sentence string and also to save the analysis result
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
	OPTION_TYPE_POS_TAGGING, ///< the value zero for not to tag part-of-speech tags in the result of \e runWithSentence(), \e runWithString() and \e runWithStream(), which value is 1 defaultly.
	OPTION_TYPE_NBEST, ///< a positive value to set the number of candidate results of \e runWithSentence(), which value is 1 defaultly.
	OPTION_TYPE_NUM ///< the count of option types
    };

    /**
     * Set the option value for analysis.
     * \param nOption the option type
     * \param nValue the option value
     * \attention when \e nOption is \e OPTION_TYPE_NBEST, the invalid \e nValue less than 1 will take no effect.
     */
    void setOption(OptionType nOption, double nValue);

    /**
     * Get the option value.
     * \param nOption the option type
     * \return the option value
     */
    double getOption(OptionType nOption) const;

    /**
     * Set the delimiter between word and POS tag in the output result of \e runWithString() and \e runWithStream(), which delimiter is "/" defaultly so that the result would be "word/pos  word/pos  ...".
     * \param delimiter the delimiter between word and POS tag in the output result
     */
    void setPOSDelimiter(const char* delimiter);

    /**
     * Get the delimiter between word and POS tag in the output result of \e runWithString() and \e runWithStream(), which delimiter is "/" defaultly so that the result would be "word/pos  word/pos  ...".
     * \return the delimiter between word and POS tag in the output result
     */
    const char* getPOSDelimiter() const;

    /**
     * Set the delimiter between the pairs (word and POS tag) in the output result of \e runWithString() and \e runWithStream(), which delimiter is "  " (double-space) defaultly so that the result would be "word/pos  word/pos  ...".
     * \param delimiter the delimiter between the pairs (word and POS tag) in the output result
     */
    void setWordDelimiter(const char* delimiter);

    /**
     * Get the delimiter between the pairs (word and POS tag) in the output result of \e runWithString() and \e runWithStream(), which delimiter is "  " (double-space) defaultly so that the result would be "word/pos  word/pos  ...".
     * \return the delimiter between the pairs (word and POS tag) in the output result
     */
    const char* getWordDelimiter() const;

protected:
    /** option values */
    std::vector<double> options_;

    /** the delimiter between word and POS tag in the output result */
    const char* posDelimiter_;

    /** the delimiter between the pairs (word and POS tag) in the output result */
    const char* wordDelimiter_;
};

} // namespace cma

#endif // CMA_ANALYZER_H
