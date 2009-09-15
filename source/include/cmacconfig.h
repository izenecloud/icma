/** \file cmacconfig.h
 * \brief shared configuration header file for the whole CMAC project
 * \author vernkin
 */

#ifndef _CMACCONFIG_H
#define	_CMACCONFIG_H

/** Whether in the Debug mode */
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

/** Whether use the string trie in the segmentation */
#define USE_STRTRIE

/** Whether run with stdin as input */
//#define TEST_SEG_CMD

#ifdef TEST_SEG_CMD
    #ifdef DEBUG
    //Print the Trie Node Information
    //#define DEBUG_TRIE_MATCH

    #endif
#endif

/** Print the detail in the SegTagger*/
//#define DEBUG_POC_TAGGER

#ifdef DEBUG_POC_TAGGER
	//#define DEBUG_POC_TAGGER_TRIE
#endif

#include <iostream>

#endif	/* _CMACCONFIG_H */

