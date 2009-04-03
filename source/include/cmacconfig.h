#ifndef _CMACCONFIG_H
#define	_CMACCONFIG_H

#define DEBUG

/** if set, output all the training data */
#define DEBUG_TRAINING

/** Enable assert statement */
#define EN_ASSERT

#define _ME_STRICT_POC_MATCHED

#ifdef EN_ASSERT
    #include <assert.h>
#endif

#endif	/* _CMACCONFIG_H */

