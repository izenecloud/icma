/** \file cma_ctype.h
 * \brief CMA_CType gives the character type information.
 * \author Vernkin
 * \version 0.1
 * \date Mar 10, 2009
 */

#ifndef CMA_CTYPE_H
#define CMA_CTYPE_H

#include <string>

using std::string;

#include "knowledge.h" // Knowledge::EncodeType
#include "cmacconfig.h"

#include "cma_ctype_core.h"

namespace cma
{

/**
 * \brief CMA_CType gives the character type information.
 *
 * CMA_CType gives the character type information.
 */
class CMA_CType
{
public:
    /**
     * Create an instance of \e CMA_CType based on the character encode type.
     * \param type the character encode type
     * \return the pointer to instance
     */
    static CMA_CType* instance(Knowledge::EncodeType type);

    /**
     * The naming in the poc.xml, include digit, chardigit, space, letter, punctuation
     * \param noDefault if true, an assert error will occur
     * \return if not found, return CHAR_TYPE_OTHER
     */
    static CharType getCharTypeByXmlName( const char* name, bool noDefault = false );

    /**
     * Destrucor
     */
    virtual ~CMA_CType();

    /**
     * Get the byte count of the first character pointed by \e p, which
     * character is in a specific encoding.
     * \param p pointer to the character string
     * \return true for punctuation, false for non punctuation.
     */
    virtual unsigned int getByteCount(const char* p) const = 0;

    /**
     * Get the character type.
     * \param p pointer to the string to be checked
     * \param preType the chartype of the previous character
     * \param nextP the pointer of the next character, it can be 0
     * \return the character type.
     */
    CharType getCharType(const char* p, CharType preType,
            const char* nextP) const;

    /**
     * Whether the p is a punctuation
     * \param p pointer to the string to be checked
     * \return true if the p is the punctuation
     */
    bool isPunct(const char* p) const;

    /**
     * Get the number of the characters in the p
     * \param p pointer to the string to be checked
     * \return the number of the characters in the p
     */
    size_t length(const char* p) const;

    /**
     * Get the encoding value in unsigned int type
     *
     * param p the character with pointer p
     * \return unsigned int value of p, 0 if error occurs
     */
    unsigned int getEncodeValue(const char* p) const;

    /**
     * Check whether is white-space character.
     * White-space characters are " \t\n\v\f\r", and also space character in specific encoding.
     * \param p pointer to the character string
     * \return true for white-space character, false for non white-space character.
     */
    bool isSpace(const char* p) const;

    /**
     * Check whether is a seperator of sentence.
     * \param p pointer to the character string
     * \return true for separator, false for non separator.
     */
    bool isSentenceSeparator(const char* p) const;

	/**
	 * Load the poc.xml
	 * \return 1 if load successfully
	 */
	int loadConfiguration( const char* file);

	/**
	 * Invoke this function when the last word not exists
	 * \param preType type of the previous character
	 * \return the default next type
	 */
	CharType getDefaultEndType( CharType preType );

private:
	/** Character to types map */
	map< CharValue, CharConditions > typeMap_;

	/** Spaces Set */
	set<CharValue> spaceSet_;

	/** Sentence Separator Set */
	set<CharValue> senSepSet_;

};

} // namespace cma

#endif // CMA_CTYPE_H
