/* 
 * Category String Tokenizier
 * \author vernkin
 *
 */

#ifndef _CATESTRTOKENIZER_H
#define	_CATESTRTOKENIZER_H

#include <map>
#include <set>
using namespace std;

#include "CPPStringUtils.h"
#include "cmacconfig.h"
#include "tokenizer.h"

namespace cma{

/**
 * Record the index and category of the specific index
 */
struct CatePoint{
    int index;
    /**
     * Three Type: Letter/Digit/Punctuation
     */
    CharType type;
};

class CateStrTokenizer{
public:

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

    vector<string>& getWordSeq();
  
    CharType getType();

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

