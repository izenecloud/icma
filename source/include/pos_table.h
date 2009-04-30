/** \file pos_table.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Apr 02, 2009
 */

#ifndef CMA_POS_TABLE_H
#define CMA_POS_TABLE_H

#include <string>
#include <vector>
#include <map>

namespace cma
{

/**
 * \brief Case-insensitive string compare.
 *
 * Case-insensitive string compare.
 */
class Nocase
{
public:
    /**
     * Compare string case-insensitively.
     * \param x the first string
     * \param y the second string
     * \return true if \e x is lexicographically less than \e y, not taking case into account
     */
    bool operator() (const std::string& x, const std::string& y) const;
};

/**
 * \brief POSTable stores the global table of part-of-speech tags
 *
 * POSTable stores the global table of part-of-speech tags, each tag includes a string value and its index code.
 * Note that the string value of part-of-speech tag is case-insensitive.
 * For example, 'Ng' equals to 'ng'.
 */
class POSTable
{
public:
    /**
     * Create an instance of \e POSTable.
     * \return the pointer to instance
     */
    static POSTable* instance();

    /**
     * Add the POS string to the global part-of-speech table.
     * \param pos the POS string
     * \return the index code of the POS string added, if the POS string has
     * been added before, its index code previously added is returned.
     * \attention Note that the POS string is case-insensitively.
     */
    int addPOS(const std::string& pos);

    /**
     * Get the POS index code from the POS string in the global part-of-speech table.
     * \param pos the POS string
     * \return POS index code, -1 for non POS available
     */
    int getCodeFromStr(const std::string& pos) const;

    /**
     * Get the POS string from the POS index code in the global part-of-speech table.
     * \param index the POS index code
     * \return POS string, null pointer for non POS available
     */
    const char* getStrFromCode(int index) const;

    /**
     * Get the number of POS tags in the global part-of-speech table.
     * \return the number of POS tags
     */
    int size() const;

protected:
    POSTable();

private:
    /** the instance of \e POSTable */
    static POSTable* instance_;

    /** the POS tag table */
    std::vector<std::string> posTable_;

    /** the POS tag map type from case-insensitive string to index code */
    typedef std::map<std::string, int, Nocase> POSMap;

    /** the POS tag map */
    POSMap posMap_;
};

} // namespace cma

#endif // CMA_POS_TABLE_H
