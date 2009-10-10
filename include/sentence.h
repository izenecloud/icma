/** \file sentence.h
 * \brief Sentence saves the results of Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#ifndef CMA_SENTENCE_H
#define CMA_SENTENCE_H

#include <string>
#include <vector>
#include <map>

namespace cma
{

/**
 * \brief a pair of lexicon string and its part-of-speech tag.
 *
 */
struct Morpheme
{
    /** the lexicon string value */
    std::string lexicon_;

    /** the index code of part-of-speech tag */
    int posCode_;

    /** the POS string value */
    std::string posStr_;

    /**
     * Constructor.
     * The lexicon string value is initialized with empty string,
     * and the index code of part-of-speech tag is initialized with -1, meaning that no part-of-speech tag is available.
     */
    Morpheme();

    /**
     * Constructor
     * \param posCode initial POS Code
     * \param posStr initial POS string value
     */
    Morpheme(std::string lexicon, int posCode, std::string& posStr);
};

/** A list of morphemes. */
typedef std::vector<Morpheme> MorphemeList;

/**
 * \brief Sentence saves the results of Chinese morphological analysis.
 *
 * Sentence saves the results of Chinese morphological analysis.
 * Typically, the usage is like below:
 *
 * Sentence s;
 * s.setString("...");
 *
 * Analyzer* analyzer = ...;
 * analyzer->runWithSentence(s);
 *
 * // get n-best results
 * for(int i=0; i<s.getListSize(); ++i)
 * {
 *	for(int j=0; j<s.getCount(i); ++j)
 *	{
 *	    const char* pLexicon = s.getLexicon(i, j);
 *	    const char* strPOS = s.getStrPOS(i, j);
 *	    ...
 *	}
 *	double score = s.getScore(i);
 *	...
 * }
 *
 * // get one-best result
 * int i= s.getOneBestIndex();
 * ...
 */
class Sentence
{
public:

	/**
	 * Default Constructor and do nothing
	 */
	Sentence();

	/**
	 * Constructor with initial raw sentence string
	 * \param pString value of the raw string
	 */
	Sentence(const char* pString);

    /**
     * Set the raw sentence string.
     * \param pString value of the raw string
     * \attention the previous analysis results will be removed.
     */
    void setString(const char* pString);

    /**
     * Get the raw sentence string.
     * \return value of the raw string
     */
    const char* getString(void) const;

    /**
     * Get the number of candidates of morphological analysis result.
     * \return the number of candidates
     */
    int getListSize(void) const;

    /**
     * Get the number of morphemes in candidate result \e nPos.
     * \param nPos candidate result index
     * \return the number of morphemes
     */
    int getCount(int nPos) const;

    /**
     * Get the string of morpheme \e nIdx in candidate result \e nPos.
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return morpheme string
     */
    const char* getLexicon(int nPos, int nIdx) const;

    /**
     * Get the POS index code of morpheme \e nIdx in candidate result \e nPos.
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return POS index code, -1 for non POS available
     */
    int getPOS(int nPos, int nIdx) const;

    /**
     * Get the POS string of morpheme \e nIdx in candidate result \e nPos.
     * \param nPos candidate result index
     * \param nIdx morpheme index
     * \return POS string, null pointer for non POS available
     */
    const char* getStrPOS(int nPos, int nIdx) const;

    /**
     * Get the MorphemeList of candidate result \e nPos.
     * \param nPos candidate result index
     * \return the MorphemeList
     */
    const MorphemeList* getMorphemeList(int nPos) const;

    /**
     * Get the score of candidate result \e nPos.
     * \param nPos candidate result index
     * \return the score value
     */
    double getScore(int nPos) const;

    /**
     * Set the score of candidate result \e nPos.
     * \param nPos candidate result index
     * \param nScore the score value
     */
    void setScore(int nPos, double nScore);

    /**
     * Get the index of the candidate result, which has the highest score.
     * \return candidate result index, -1 is returned if there is no candidate result.
     */
    int getOneBestIndex(void) const;

    /**
     * Add a candidate result of morphological analysis.
     * \param morphemeList the candidate result
     * \param score the score value of the candidate
     */
    void addList(const MorphemeList& morphemeList, double score = 0.0);

private:
    /** the raw sentence string */
    std::string raw_;

    /** the candidates list of morphological analysis result */
    std::vector<MorphemeList> candidates_;

    /** the scores list of candidates */
    std::vector<double> scores_;
};

} // namespace cma

#endif // CMA_SENTENCE_H
