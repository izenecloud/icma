#ifndef _CMACCONFIG_H
#define	_CMACCONFIG_H

#define DEBUG

#ifdef DEBUG
/** if set, output all the training data */
//#define DEBUG_TRAINING
/** DEBUG CateStrTokenizer */
//#define DEBUG_CATESTRTOKENIZER
#endif

/** Enable assert statement */
#define EN_ASSERT
#ifdef EN_ASSERT
    #include <assert.h>
#endif

/** if set, use punctuaion and character type feature */
#define USE_BE_TYPE_FEATURE

#define USE_STRTRIE

//#define TEST_SEG_CMD

#ifdef TEST_SEG_CMD
    #ifdef DEBUG
    //Print the Trie Node Information
    //#define DEBUG_TRIE_MATCH

    /** Print the detail in the SegTagger*/
    #define DEBUG_POC_TAGGER
    #endif
#endif

#include <iostream>

#endif	/* _CMACCONFIG_H */

