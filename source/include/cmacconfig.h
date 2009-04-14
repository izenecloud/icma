#ifndef _CMACCONFIG_H
#define	_CMACCONFIG_H

#define DEBUG

/** if set, output all the training data */
#define DEBUG_TRAINING

/** Enable assert statement */
#define EN_ASSERT

//#define _ME_STRICT_POC_MATCHED

#ifdef EN_ASSERT
    #include <assert.h>
#endif

#define TEST_SEG_CMD

#ifdef TEST_SEG_CMD
//Print the Trie Node Information
#define DEBUG_TRIE_MATCH
#endif

#include <iostream>

#endif	/* _CMACCONFIG_H */

