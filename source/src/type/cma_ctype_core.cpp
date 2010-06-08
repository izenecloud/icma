/*
 * cma_ctype_core.cc
 *
 *  Created on: Sep 23, 2009
 *      Author: vernkin
 */

#include "icma/type/cma_ctype_core.h"
#include "tokenizer.h"

namespace cma
{

const int BYTES_OFFSET[] = {24, 16, 8, 0};

string getDecodeChar( CharValue value )
{
	char buf[5];
	int idx = 0;
	for( int i = 0; i < 4; ++i )
	{
		char c = (char)(unsigned short)( (value >> BYTES_OFFSET[i]) & 0x0FF );
		if( !idx && !c )
			continue;
		buf[ idx++ ] = c;
	}

	buf[idx] = '\0';
	return string(buf);
}

Condition::Condition()
{
	//empty constructor
}

Condition::~Condition()
{
	//cout<<"Condition Destructor: "<<*this<<endl;
}

void Condition::init(CharType type, vector<CharType>& preTypes, vector<CharType>& noPreTypes,
			vector<CharType>& nextTypes, vector<CharType>& noNextTypes,
			set<CharValue>& nextChars, bool isEnd)
{
	type_ = type;
	isEnd_ = isEnd;
	nextChars_ = nextChars;
	setTypesArray( isSetPreTypes_, noPreTypes_, preTypes, noPreTypes );
	setTypesArray( isSetNextTypes_, noNextTypes_, nextTypes, noNextTypes );
}

void Condition::setTypesArray( bool& isSegFlag, bool *noTypes, vector<CharType>& allowTypes,
		vector<CharType>& forbidTypes)
{
	if( forbidTypes.empty() && allowTypes.empty() )
	{
		isSegFlag = false;
		return;
	}

	isSegFlag = true;
	if( forbidTypes.empty() )
	{
		//initialized as are all forbid
		for( int i = 0; i < CHAR_TYPE_NUM; ++i )
			noTypes[i] = true;
		//only set the allow list
		for( vector<CharType>::iterator itr = allowTypes.begin();
				itr !=  allowTypes.end(); ++itr )
			noTypes[*itr] = false;
		return;
	}

	if( allowTypes.empty() )
	{
		//initialized as are all allowed
		for( int i = 0; i < CHAR_TYPE_NUM; ++i )
			noTypes[i] = false;
		//only set the allow list
		for( vector<CharType>::iterator itr = forbidTypes.begin();
				itr !=  forbidTypes.end(); ++itr )
			noTypes[*itr] = true;
		return;
	}

	// there are not empty, set the allowTypes first, then forbidTypes
	for( int i = 0; i < CHAR_TYPE_NUM; ++i )
		noTypes[i] = true;
	//only set the allow list
	for( vector<CharType>::iterator itr = allowTypes.begin();
			itr !=  allowTypes.end(); ++itr )
		noTypes[*itr] = false;
	for( vector<CharType>::iterator itr = forbidTypes.begin();
			itr !=  forbidTypes.end(); ++itr )
		noTypes[*itr] = true;
}

bool Condition::match( CharType preType, CharValue nextValue, CharType nextType ) const
{
	//cout<<"match "<<*this<<",preType="<<preType<<",nextValue="<<nextValue<<",nextType="<<nextType<<endl;
	if( ( isSetPreTypes_ && noPreTypes_[preType] ) ||
			( isSetNextTypes_ && noNextTypes_[nextType] ) )
	{
		//cout<<"match in if"<<endl;
		return false;
	}
	if( !nextChars_.empty() )
	{
		//cout<<"match in nextChars"<<endl;
		return nextChars_.find( nextValue ) != nextChars_.end();
	}
	//cout<<"match end"<<endl;
	return true;
}

bool Condition::equals( Condition& other )
{
	if( type_ != other.type_ )
		return false;

	if( isSetPreTypes_ != other.isSetPreTypes_ )
		return false;
	if( isSetPreTypes_ )
	{
		for( int i = 0; i < CHAR_TYPE_NUM; ++i )
			if( noPreTypes_[i] != other.noPreTypes_[i] )
				return false;
	}

	if( isSetNextTypes_ != other.isSetNextTypes_ )
			return false;
	if( isSetNextTypes_ )
	{
		for( int i = 0; i < CHAR_TYPE_NUM; ++i )
			if( noNextTypes_[i] != other.noNextTypes_[i] )
				return false;
	}

	if( nextChars_.size() != other.nextChars_.size() )
		return false;
	if( !nextChars_.empty() )
	{
		set<CharValue>& otherSet = other.nextChars_;
		for(set<CharValue>::iterator itr = nextChars_.begin();
				itr != nextChars_.end(); ++itr )
		{
			if( otherSet.find(*itr) == otherSet.end() )
				return false;
		}
	}

	return true;
}

CharConditions::CharConditions() :
	baseType_(CHAR_TYPE_INIT), conditionSize_(0)
{

}

void CharConditions::setBaseType( CharType baseType )
{
	baseType_ = baseType;
}

bool CharConditions::addCondition( Condition& newCondition )
{
	// Ignore duplicate Condition
	for( int i = 0; i < conditionSize_; ++i )
	{
		if( newCondition.equals( conditions_[i] ) )
			return false;
	}

	// add the CHAR_TYPE_OTHER at the begin of the conditions
	switch( newCondition.getType() )
	{
	case CHAR_TYPE_OTHER:
		conditions_.insert( conditions_.begin(), newCondition );
		break;

	case CHAR_TYPE_DATE:
		conditions_.push_back( newCondition );
		break;

	default:
		// before the date
		vector<Condition>::iterator itr = conditions_.begin();
		while( true )
		{
			if( itr == conditions_.end() || itr->type_ == CHAR_TYPE_DATE )
			{
				conditions_.insert( itr, newCondition );
				break;
			}
			++itr;
		}
		break;
	}
	++conditionSize_;
	return true;
}

bool CharConditions::addConditions( vector< Condition >& vec )
{
	for( size_t i = 0; i < vec.size(); ++i )
		addCondition( vec[i] );
	return true;
}

CharType CharConditions::match( CharType preType, CharValue nextValue, CharType nextType,
		CharType defaultType ) const
{
	for( int i = 0; i < conditionSize_; ++i )
	{
		const Condition& cond = conditions_[i];
		if( cond.match( preType, nextValue, nextType ) )
			return cond.type_;
	}

	return defaultType;
}

}
