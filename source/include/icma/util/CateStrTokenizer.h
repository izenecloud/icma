/** \file CateStrTokenizer.h 
 * \brief Category String Tokenizier
 *
 * Segment the string based on the char types, line letters, digits and so on
 *
 * \author vernkin
 */

#ifndef _CATESTRTOKENIZER_H
#define	_CATESTRTOKENIZER_H

#include <map>
#include <set>
using namespace std;

#include "icma/cmacconfig.h"
#include "icma/util/CPPStringUtils.h"
#include "icma/util/tokenizer.h"

namespace cma{

/**
 * \brief Record the index and category of the specific index
 *
 * Record the index and category of the specific index
 */
struct CatePoint{
    /**
     * The index in the whole string
     */
    int index;

    /**
     * Three Type: Letter/Digit/Punctuation
     */
    CharType type;
};

/**
 * \brief Category String Tokenizier
 *
 * Segment the string based on the char types, line letters, digits and so on
 *
 * \author vernkin
 *
 */
class CateStrTokenizer{
public:

    /**
     * Create an instance
     */
    CateStrTokenizer(CTypeTokenizer* pCToken);

    /**
     * Move to next token
     * \return if false their is not any more token
     */
    bool next();


    /**
     * Whether is word sequence, the another possibility is sepecial string,.
     * Like letters, digits and so on
     * \return if true invoke getWordSeq(), else invoke 
     */
    bool isWordSeq();

    /**
     * Get the words vector
     *
     * \return the words vector
     */
    vector<string>& getWordSeq();

    /**
     * Get the type of the current token
     *
     * \return the type of the current token
     */
    CharType getType();

    /**
     * Get the special string if the current token is not with type CHAR_TYPE_OTHER
     */
    string& getSpecialStr();

private:
    /** Words Collection */
    vector<string> wordSeq_;

    /** The Type of the token */
    CharType tokenT_;

    /** Special String */
    string speStr_;
    /** Current Character, for internal perpose*/
    string _strBuf;

    /** pervious Character Type, for internal perpose*/
    CharType _preCharT;

    CTypeTokenizer* ctoken_;

    const CMA_CType* ctype_;

    /**
     * The pointer to the next string
     */
    const char* nextTokenPtr;

    bool init;
};
    
}

#endif	/* _CATESTRTOKENIZER_H */

