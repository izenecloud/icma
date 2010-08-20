/** \file cma_wtype.h
 * \brief The class the identify the type of the word
 * \author Vernkin
 * \version 0.2
 * \date Mar 23, 2009
 */

#ifndef CMA_WTYPE_H
#define CMA_WTYPE_H

#include <string.h>

#include "icma/util/tokenizer.h"

namespace cma
{

class CMA_CType;

/**
 * \brief CMA_WType gives the word type information.
 *
 */
class CMA_WType
{
public:
    /**
     * Constructor.
     * \param ctype reference to the specific character encoding
     */
    CMA_WType(const CMA_CType* ctype);

    /**
     * Word type.
     */
    enum WordType
    {
        WORD_TYPE_NUMBER = 1, ///< number word, example: 632, 三百八十
        WORD_TYPE_DATE, ///< date word, example: 2009年, 二零零九年, 5月, 6日
        WORD_TYPE_LETTER, ///< letter word, example: Pentium4
        WORD_TYPE_PUNC,
        WORD_TYPE_OTHER ///< other word
    };

    /**
     * Get the word type.
     * \param word pointer to the word string
     * \return the word type.
     */
    WordType getWordType(const char* word);

    /**
	 * Get the word type.
	 * \param types types that are already knows
	 * \param beginIdx beginning index of the word ( inclusive )
	 * \param endIdx ending index of the word ( exclusive )
	 * \return the word type.
	 */
	WordType getWordType(CharType* types, size_t beginIdx, size_t endIdx );

private:
    /** character encoding */
    const CMA_CType* ctype_;

    /** to access a sequence of characters in specific encoding */
    CTypeTokenizer tokenizer_;
};

} // namespace cma

#endif // CMA_WTYPE_H
