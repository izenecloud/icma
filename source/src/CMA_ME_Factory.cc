#include "CMA_ME_Factory.h"

namespace cma{

Analyzer* CMA_ME_Factory::createAnalyzer(){
    return new CMA_ME_Analyzer;
}

Knowledge* CMA_ME_Factory::createKnowledge(){
    return new CMA_ME_Knowledge;
}

}
