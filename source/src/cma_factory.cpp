/** \file cma_factory.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 20, 2009
 */

#include "cma_factory.h"
#include "cma_crf_factory.h"

namespace cma
{

CMA_Factory* CMA_Factory::instance_;

CMA_Factory* CMA_Factory::instance()
{
    if(instance_ == 0)
    {
	instance_ = new CMA_CRF_Factory;
    }

    return instance_;
}

CMA_Factory::~CMA_Factory()
{
}

} // namespace cma
