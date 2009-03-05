/** \file cma_factory.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#ifndef CMA_FACTORY_H
#define CMA_FACTORY_H

namespace cma
{

class Analyzer;
class Knowledge;

/**
 * CMA_Factory creates instances for Chinese morphological analysis.
 */
class CMA_Factory
{
public:
    /**
     * Create an instance of \e CMA_Factory.
     * \return the pointer to instance
     */
    static CMA_Factory* instance();

    /**
     * Create an instance of \e Analyzer.
     * \return the pointer to instance
     */
    virtual Analyzer* createAnalyzer() = 0;

    /**
     * Create an instance of \e Knowledge.
     * \return the pointer to instance
     */
    virtual Knowledge* createKnowledge() = 0;

protected:
    virtual ~CMA_Factory();

private:
    /** the instance of factory */
    static CMA_Factory* instance_;
};

} // namespace cma

#endif // CMA_FACTORY_H
