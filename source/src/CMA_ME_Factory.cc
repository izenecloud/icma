/** \file CMA_ME_Factory.cc
 * \brief CMA_ME_Factory creates instances for Chinese morphological analysis.
 * \author vernkin
 */
#include "CMA_ME_Factory.h"

namespace cma{

Analyzer* CMA_ME_Factory::createAnalyzer(){
    return new CMA_ME_Analyzer;
}

Knowledge* CMA_ME_Factory::createKnowledge(){
    return new CMA_ME_Knowledge;
}

}
