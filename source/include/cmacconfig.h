#ifndef _CMACCONFIG_H
#define	_CMACCONFIG_H

#define DEBUG

/** if set, output all the training data */
//#define DEBUG_TRAINING

/** Enable assert statement */
#define EN_ASSERT
#ifdef EN_ASSERT
    #include <assert.h>
#endif

/** Whether use POC new feature */
#define USE_POC_NEW_FEATURE

//#define TEST_SEG_CMD

#ifdef TEST_SEG_CMD
    //Print the Trie Node Information
    //#define DEBUG_TRIE_MATCH

    /** Print the detail in the SegTagger*/
    #define DEBUG_POC_TAGGER
#endif

//#define DEBUG_CATESTRTOKENIZER

#include <iostream>

#endif	/* _CMACCONFIG_H */
