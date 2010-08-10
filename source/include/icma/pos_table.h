/** \file pos_table.h
 * \brief POSTable stores the global table of part-of-speech tags
 * \author Jun Jiang
 * \version 0.1
 * \date Apr 02, 2009
 */

#ifndef CMA_POS_TABLE_H
#define CMA_POS_TABLE_H

#include <string>
#include <vector>
#include <map>
#include <icma/util/DCTrie.h>
#include <icma/util/StringArray.h>
#include <icma/util/VGenericArray.h>

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

    POSTable();

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
    inline int getCodeFromStr(const std::string& pos)
    {
        int ret = posMap_.search( pos.c_str() );
        if( ret > 0 )
            return ret;
        else
            return addPOS( pos );
    }

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

    /**
     * Special part-of-speech types.
     */
    enum POSType
    {
	POS_TYPE_NUMBER, ///< the POS type of numeral words, such as "123", "一百", etc
	POS_TYPE_DATE, ///< the POS type of temporal nouns such as "今天", "2009年", etc
	POS_TYPE_FOREIGN, ///< the POS type of foreign words such as "ISO", "ＣＨＩＮＡ", etc
        POS_TYPE_PUNCT, ///< the POS type of punctuations such as "，", "。", etc
        POS_TYPE_USER, ///< the POS type of user defined words
        POS_TYPE_MAX_NUM ///< the count of POS types
    };

    /**
     * Get the POS index code from the special POS type.
     * \param type the POS type
     * \return POS index code, -1 for non POS available
     */
    int getCodeFromType(POSType type) const;

    /**
     * Whether the posCode indicates that word is index word, if the posCode is invalid,
     * return true.
     * \param posCode the specific pos Code
     * \return true if posCode is the index POS
     */
    bool isIndexPOS( int posCode ) const;

    /**
     * Set the specific posCode whether is index
     * \param posCode the specific POS code
     * \param whether the index is posCode
     * \return whether set successfully, if false, the posCode doesn't exists
     */
    bool setIndexPOS( int posCode, bool isIndex );

    /**
	 * Reset all the pos index Information as defVal
	 * \return defVal default value to reset (default is true)
	 */
    void resetIndexPOSList(bool defVal = true);

    /**
	 * Set the POS index as true in the posList
	 * \param posList the list that allowed in the posList
	 * \return how many POS are set successfully (mainly because not exists).
	 */
	int setIndexPOSList( std::vector<std::string>& posList );

    /**
     * Get POSTable information, reserved Interface
     * \return POSTable information
     */
    const StringArray* getPOSTableInfo()
    {
    	return &posTable_;
    }

private:
    /** the instance of \e POSTable */
    static POSTable* instance_;

    /** the POS tag table */
    StringArray posTable_;

    /** Index array */
    PGenericArray<bool> indexedFlags_;

    /** the POS tag map type from case-insensitive string to index code */
    typedef DCTrie POSMap;

    /** the POS tag map */
    POSMap posMap_;

    /** the table from \e POSType to index code */
    std::vector<int> typeTable_;

    /** the map from string to \e POSType */
    typedef std::map<std::string, POSType> TypeMap;

    /** the instance of \e TypeMap */
    TypeMap typeMap_;
};

} // namespace cma

#endif // CMA_POS_TABLE_H
