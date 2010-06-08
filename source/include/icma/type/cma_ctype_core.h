/** \file cma_ctype_core.h
 * \brief Gives the logic of analysis character types.
 * \author Vernkin
 * \version 0.1
 * \date Mar 10, 2009
 */


#ifndef CMA_CTYPE_CORE_H_
#define CMA_CTYPE_CORE_H_

#include <vector>
#include <map>
#include <set>
#include <iostream>

using namespace std;

namespace cma
{

/**
 * \brief Character type.
 * [Warn] Don't set the value for following enum types
 * Character type.
 */
enum CharType
{
    CHAR_TYPE_INIT, ///< initial type, used to identify the type

    CHAR_TYPE_OTHER, ///< other character

    //special type have to put after CHAR_TYPE_OTHER
    CHAR_TYPE_NUMBER, ///< digit character
    CHAR_TYPE_CHARDIGIT, //< digit in hanzi format, like '一'
    CHAR_TYPE_PUNC, ///< puntuation character
    CHAR_TYPE_SPACE, ///< space character, like ' '
    CHAR_TYPE_DATE, ///< date character
    CHAR_TYPE_LETTER, ///< letter character
    CHAR_TYPE_NUM
};

typedef unsigned int CharValue;

/**
 * Get the Original Character
 * \param value represent in the CharValue type
 * \return the original character
 */
string getDecodeChar( CharValue value );

/**
 * \brief This class check the previous/next characters and their types to check whether
 * match the Condition.
 */
class Condition
{
public:
	Condition();

	~Condition();

	void init(CharType type, vector<CharType>& preTypes, vector<CharType>& noPreTypes,
				vector<CharType>& NextTypes, vector<CharType>& noNextTypes,
				set<CharValue>& nextChars, bool isEnd = false);

	CharType getType() const
	{
		return type_;
	}

	/**
	 * If next character doesn't exists, nextValue is 0 and nextType is CHAR_TYPE_OTHER
	 */
	bool match( CharType preType, CharValue nextValue, CharType nextType ) const;

	/**
	 * Whether the content are totally the same
	 */
	bool equals( Condition& other );

    friend ostream& operator << ( ostream& sout, const Condition& cond )
    {
        sout<<"type: "<<cond.type_<<", ";
    	sout<<"isPre: "<<cond.isSetPreTypes_;
        if(cond.isSetPreTypes_)
        {
        	sout<<" ( ";
        	for( int i = 0; i < CHAR_TYPE_NUM; ++i )
        		sout<<!cond.noPreTypes_[i]<<", ";
        	sout<<")";
        }

        sout<<", isNext: "<<cond.isSetNextTypes_;
        if(cond.isSetNextTypes_)
        {
        	sout<<" ( ";
        	for( int i = 0; i < CHAR_TYPE_NUM; ++i )
        		sout<<!cond.noNextTypes_[i]<<", ";
        	sout<<")";
        }

        const set<CharValue>& nextChars = cond.nextChars_;
        sout<<", nextCharSize: "<<nextChars.size();

        if( !nextChars.empty() )
		{
			sout<<" ( ";
			for( set<CharValue>::const_iterator itr = nextChars.begin();
					itr != nextChars.end(); ++itr )
				sout<<getDecodeChar( *itr )<<", ";
			sout<<")";
		}

        sout<< ", isEnd: " << cond.isEnd_;
        return sout;
    }

private:
	/**
	 * Set the preTypes_ and nextTypes_
	 */
	void setTypesArray(bool& isSegFlag, bool *noTypes, vector<CharType>& allowTypes,
			vector<CharType>& forbidTypes);

public:
	/** If match the condition, the type of the target character */
	CharType type_;

	/** Whether set the previous types */
	bool isSetPreTypes_;
	/** True indicates forbid and False indicates allow */
	bool noPreTypes_[CHAR_TYPE_NUM];

	/** Whether set the next types */
	bool isSetNextTypes_;
	/** True indicates forbid and False indicates allow */
	bool noNextTypes_[CHAR_TYPE_NUM];

	set<CharValue> nextChars_;

	bool isEnd_;
}; //end class Condition

/**
 * \brief all the conditions of a specific character
 * Each character has at most one CharConditions
 */
class CharConditions
{
public:

	CharConditions();

	void setBaseType( CharType baseType );

	/**
	 * If the Condition's type is CHAR_TYPE_OTHER, it would insert
	 * as the first element
	 *
	 * \return whether add successfully
	 */
	bool addCondition( Condition& newCondition );

	bool addConditions( vector< Condition >& vec );

	/**
	 * Get the Proper CharType, if not found, return defaultType
	 * \param defaultType if not match, return this type
	 */
	CharType match( CharType preType, CharValue nextValue, CharType nextType,
			CharType defaultType = CHAR_TYPE_INIT ) const;

public:
	/** basis CharType */
	CharType baseType_;

	/** list of conditions */
	vector<Condition> conditions_;

	/** size of the conditions */
	int conditionSize_;
};

}


#endif /* CMA_CTYPE_CORE_H_ */
