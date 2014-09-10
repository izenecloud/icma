/** \file cma_factory.cpp
 * \brief CMA_Factory creates instances for Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 20, 2009
 */

#include "icma/cma_factory.h"
#include "icma/me/CMA_ME_Factory.h"

namespace cma
{

CMA_Factory* CMA_Factory::instance_;

CMA_Factory* CMA_Factory::instance()
{
    if(instance_ == 0)
    {
        instance_ = new CMA_ME_Factory;
    }

    return instance_;
}

CMA_Factory::~CMA_Factory()
{
}

} // namespace cma
