/** \file cma_crf_factory.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 20, 2009
 */

#ifndef CMA_CRF_FACTORY_H
#define CMA_CRF_FACTORY_H

#include "cma_factory.h"

namespace cma
{

/**
 * CMA_CRF_Factory creates instances for Chinese morphological analysis.
 */
class CMA_CRF_Factory : public CMA_Factory
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

#endif // CMA_CRF_FACTORY_H
