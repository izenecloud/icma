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

#include "icma/cmacconfig.h"
#include "icma/type/cma_ctype_core.h"

#include "VTrie.h"
#include "icma/util/VGenericArray.h"

namespace cma
{

//TODO this value should be updated if new spaces is added
#define SPACE_ARRAY_SIZE 15


class CMA_ME_Analyzer;

/**
 * \brief CMA_CType gives the character type information.
 *
 * CMA_CType gives the character type information.
 */
class CMA_CType
{
public:

    typedef unsigned int(*getByteCount_t)(
        const unsigned char*
        );

    friend class CMA_ME_Analyzer;

    CMA_CType(
            Knowledge::EncodeType type,
            getByteCount_t getByteCountFun
            );

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
     * Destructor
     */
    ~CMA_CType();

    /**
     * Get the byte count of the first character pointed by \e p, which character is in a specific encoding.
     * \param p pointer to the character string
     * \return the count of bytes.
     */
    unsigned int getByteCount(const char* p) const;


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
     * Get the base Character Type of Character P without any rules
     * \param p the input character
     * \return the base type of p, or CHAR_TYPE_OTHER for default
     */
    CharType getBaseType( const char* p ) const;

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
    bool isSpace( CharValue charVal ) const;

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

    /**
     * Get the specific string with associated encoding in the poc.xml
     * Except for utf-16, others encoding return the original form
     * \param p the input string
     * \return the target string
     */
    string getPOCXmlStr( const char* p) const;

    Knowledge::EncodeType getEncodeType()
    {
        return type_;
    }


protected:
	/** The encode type of the current cma_ctype class */
	Knowledge::EncodeType type_;

private:
	/** Character to types map */
	//map< CharValue, CharConditions > typeMap_;
	VTrie condKeys_;
	VGenericArray<CharConditions> condValues_;

	/** Spaces Set */
	CharValue spaceArray_[SPACE_ARRAY_SIZE];

	/** Sentence Separator Set */
	set<CharValue> senSepSet_;

public:
	getByteCount_t getByteCountFun_;

};

} // namespace cma

#endif // CMA_CTYPE_H
