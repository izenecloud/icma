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

namespace cma{

enum CharType
{
    CHAR_TYPE_DIGIT, ///< digit character
    CHAR_TYPE_PUNC, ///< puntuation character
    CHAR_TYPE_HYPHEN, ///< hythen string, like -
    CHAR_TYPE_SPACE, ///< space string, like ' '
    CHAR_TYPE_LETTER, ///< letter character
    CHAR_TYPE_OTHER ///< other character
};

extern map<string, CharType> TYPE_MAP;

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

    CateStrTokenizer(string& sentence);

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

    /**
     * Initialize all the map and set
     */
    static void initialize();

private:
    /** Words Collection */
    vector<string> wordSeq_;

    /** Current Type */
    CharType curType_;

    /** Current String */
    string curStr_;

    /** sentence */
    string sen_;

    /** the length of the sentence */
    int senLen_;

    /** the last index of the sen_ */
    int senIdx_;

    /** Current Character, for internal perpose*/
    string _curChar;

    /** Current Character Type, for internal perpose*/
    CharType _curCharT;
};
    
}

#endif	/* _CATESTRTOKENIZER_H */

