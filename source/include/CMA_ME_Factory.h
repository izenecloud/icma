/* 
 * 
 * \author vernkin
 */

#ifndef _CMA_ME_FACTORY_H
#define	_CMA_ME_FACTORY_H

#include "cma_factory.h"
#include "CMA_ME_Analyzer.h"
#include "CMA_ME_Knowledge.h"

namespace cma
{

/**
 * \brief CMA_ME_Factory creates instances for Chinese morphological analysis.
 *
 * CMA_ME_Factory creates instances for Chinese morphological analysis.
 */
class CMA_ME_Factory : public CMA_Factory
{
public:
    /**
     * Create an instance of \e Analyzer.
     * \return the pointer to instance
     */
    virtual Analyzer* createAnalyzer();

    /**
     * Create an instance of \e Knowledge.
     * \return the pointer to instance
     */
    virtual Knowledge* createKnowledge();
};

} // namespace cma

#endif	/* _CMA_ME_FACTORY_H */

