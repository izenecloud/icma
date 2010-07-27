/** \file cma_ctype.cpp
 * \brief CMA_CType gives the character type information.
 * \author Jun Jiang, Vernkin
 * \version 0.2
 * \date Mar 10, 2009
 */
#include <assert.h>
#include <stdlib.h>

#ifdef USE_UTF_16
	#include <wiselib/ustring/UString.h>
#endif

#include <tinyxml.h>

#include "icma/type/cma_ctype.h"
#include "icma/util/tokenizer.h"
#include "strutil.h"

using namespace std;
using namespace cma::ticpp;

namespace cma
{

namespace ctypeinner
{

unsigned int getByteCountBig5(const unsigned char* uc)
{
    if(uc[0] == 0)
        return 0;

    if(uc[0] < 0x80)
        return 1; // encoding in ASCII

    return 2; // encoding in Big5
}

unsigned int getByteCountGB18030( const unsigned char* uc )
{
    if(uc[0] == 0)
        return 0;

    if(uc[0] <= 0x80) //0x80 is valid but unsigned
        return 1; // encoding in ASCII

    if(uc[0] <= 0xfe)
    {
        if(uc[1] >= 0x40 && uc[1] <= 0xfe && uc[1] != 0x7f)
            return 2;
        else if(uc[1] >= 0x30 && uc[1] <= 0x39)
        {
            if(uc[2] >= 0x81 && uc[2] <= 0xfe && uc[3] >= 0x30 && uc[3] <= 0x39)
                return 4;
            //assert(false && ("Invalid third and fourth bytes of gb18030: " +
            //      (int)uc[2] + "," + (int)uc[3] + ",str=" + p));
            return 1;
        }
        else
        {
            //assert(false && ("Invalid second byte of gb18030: " + (int)uc[1] + ",str=" + p));
            return 1;
        }

    }
    //assert(false && ("Invalid first byte of gb18030: " + (int)uc[0] + ",str=" + p));
    return 1;
}

unsigned int getByteCountGB2312( const unsigned char* uc )
{
    if(uc[0] == 0)
        return 0;

    if(uc[0] < 0x80)
        return 1; // encoding in ASCII

    return 2; // encoding in GB2312
}

unsigned int getByteCountUTF16( const unsigned char* uc )
{
    return 2;
}

unsigned int getByteCountUTF8( const unsigned char* uc )
{
    unsigned char val = uc[0];

    if( val == 0 )        // length   byte1     byte2     byte3    byte4
        return 0;
    else if( val < 0x80 ) //    1    0xxxxxxx
        return 1;
    else if( val < 0xE0 ) //    2    110yyyxx  10xxxxxx
        return 2;
    else if( val < 0xF0)  //    3    1110yyyy  10yyyyxx  10xxxxxx
        return 3;
    return 4;             //    4    11110zzz  10zzyyyy  10yyyyxx  10xxxxxx
}

int computeMinMod( set<CharValue> inputSet )
{
    int inputSize = (int)inputSet.size();
    vector<CharValue> inputVec(inputSize);
    int idx = 0;
    for( set<CharValue>::iterator itr = inputSet.begin(); itr != inputSet.end(); ++itr )
    {
        inputVec[ idx ] = *itr;
        ++idx;
    }
    int minMod = inputSet.size() * 2 + 1;
    CharValue *tmpArray = NULL;
    while( true )
    {
        //cout << "Try " << spaceArraySize_ << endl;
        delete[] tmpArray;
        tmpArray =  new CharValue[ minMod ];
        memset( tmpArray, 0x0, sizeof(CharValue) * minMod );
        bool failed = false;
        for( idx = 0; idx < inputSize; ++idx )
        {
            CharValue charVal = inputVec[ idx ];
            int destIdx = charVal % minMod;
            // already be used
            if( tmpArray[ destIdx ] != 0 )
            {
                failed = true;
                break;
            }
            tmpArray[ destIdx ] = charVal;
        }

        if( failed == false )
            break;
        minMod += 2;
    }
    delete[] tmpArray;

    return minMod;
}

}

const string DEFAULT_SPACE = " \t\n\x0B\f\r";

map< Knowledge::EncodeType, CMA_CType* > CTypeCache;

CMA_CType::CMA_CType(
        Knowledge::EncodeType type,
        getByteCount_t getByteCountFun
        )
    : type_( type ),
    getByteCountFun_ ( getByteCountFun )
{
}

CMA_CType* CMA_CType::instance(Knowledge::EncodeType type)
{
    map< Knowledge::EncodeType, CMA_CType* >::iterator itr
            = CTypeCache.find( type );
    if( itr != CTypeCache.end() )
        return itr->second;

    CMA_CType* ret = NULL;

    switch(type)
    {
    case Knowledge::ENCODE_TYPE_GB2312:
        ret = new CMA_CType( type, &ctypeinner::getByteCountGB2312 );
        break;

    case Knowledge::ENCODE_TYPE_BIG5:
        ret = new CMA_CType( type, &ctypeinner::getByteCountBig5 );
        break;

    case Knowledge::ENCODE_TYPE_GB18030:
        ret = new CMA_CType( type, &ctypeinner::getByteCountGB18030 );
        break;

    case Knowledge::ENCODE_TYPE_UTF8:
        ret = new CMA_CType( type, &ctypeinner::getByteCountUTF8 );
        break;

#ifdef USE_UTF_16
	case Knowledge::ENCODE_TYPE_UTF16:
	    ret = new CMA_CType( type, &ctypeinner::getByteCountUTF16 );
       break;
#endif

	default:
		assert(false && "unknown character encode type");
	    return 0;
    }

    CTypeCache[ type ] = ret;
    return ret;
}

CharType CMA_CType::getCharTypeByXmlName( const char* name, bool noDefault )
{
	if( strcmp( name, "other") == 0)
		return CHAR_TYPE_OTHER;
	else if( strcmp( name, "digit") == 0)
		return CHAR_TYPE_NUMBER;
	else if( strcmp( name, "chardigit") == 0)
		return CHAR_TYPE_CHARDIGIT;
	else if( strcmp( name, "letter") == 0)
		return CHAR_TYPE_LETTER;
	else if( strcmp( name, "punctuation") == 0)
		return CHAR_TYPE_PUNC ;
	else if( strcmp( name, "space") == 0)
		return CHAR_TYPE_SPACE;
	else if( strcmp( name, "date") == 0)
		return CHAR_TYPE_DATE;

	if( noDefault )
	{
		string error = "Invalid Xml Type Name: " + string(name);
		assert( false && error.data() );
	}
	return CHAR_TYPE_OTHER;
}


CMA_CType::~CMA_CType()
{
}

unsigned int CMA_CType::getByteCount(const char* p) const
{
    const unsigned char* uc = (const unsigned char*)p;
    return getByteCountFun_( uc );
}

bool CMA_CType::isPunct(const char* p) const{
    return getCharType(p, CHAR_TYPE_INIT, 0) == CHAR_TYPE_PUNC;
}

size_t CMA_CType::length( const char* p ) const
{
    size_t ret = 0;
    const unsigned char* uc = (const unsigned char*)p;
    while( uc )
    {
        unsigned int len = getByteCountFun_( uc );
        //len can be 0
        if(!len)
            return ret;
        uc += len;
        ++ret;
    }
    return ret;
}

unsigned int CMA_CType::getEncodeValue(const char* p) const
{
    const unsigned char* uc = (const unsigned char*)p;

	unsigned int bytes = getByteCountFun_( uc );
	if( bytes == 0 )
		return 0;

	unsigned int val = 0;
	switch(bytes)
	{
			case 1:
					val = uc[0];
					break;
			case 2:
					val = uc[0] << 8 | uc[1];
					break;
			case 3:
					val =  uc[0] << 16 | uc[1] << 8 | uc[2];
					break;
			case 4:
					val = uc[0] << 24 | uc[1] << 16 | uc[2] << 8 | uc[3] ;
					break;
			default:
					assert(false && "Cannot handle 'Character's length > 4'");
					return 0;
	}

	return val;
}

inline const char* getTinyXmlText( const TiXmlNode* textNode )
{
	return textNode->FirstChild()->ToText()->Value();
}

void loadCharsValue( const char* str, CTypeTokenizer& tokenizer, set<CharValue>& ret )
{
	const CMA_CType *ctype = tokenizer.getCType();
	const char* p = 0;
	string instr = ctype->getPOCXmlStr(str);
	tokenizer.assign( instr.c_str() );
	while( ( p = tokenizer.next() ) )
	{
		//cout<<"load char "<<p<<endl;
		ret.insert( ctype->getEncodeValue( p ) );
	}
}

/**
 * Ignore the CHAR_TYPE_OTHER
 */
void loadTypes( const char* typeStr, vector<CharType>& ret )
{
	vector<string> names;
	stringToVector( string(typeStr), names, ",", true );
	for( vector<string>::iterator itr = names.begin();
			itr != names.end(); ++itr )
	{
		if( itr->empty() )
			continue;
		CharType type = CMA_CType::getCharTypeByXmlName( itr->data(), true );
		if( type == CHAR_TYPE_OTHER )
		{
			cerr<<"[poc.xml Error] Unknown Type Name " << type << endl;
			exit(1);
		}
		ret.push_back( type );
	}
}

void loadEntityImpl( const char* text, CTypeTokenizer& tokenizer, CharType type,
		map< CharValue, CharConditions >& ret)
{
	const CMA_CType *ctype = tokenizer.getCType();

	string instr = ctype->getPOCXmlStr(text);
	tokenizer.assign( instr.c_str() );
	const char* p = 0;

	while( ( p = tokenizer.next() ) )
	{
		CharValue value = ctype->getEncodeValue( p );
		CharConditions& cc = ret[value];
		if( cc.baseType_ != CHAR_TYPE_INIT && cc.baseType_ != type)
		{
			cerr<<"[poc.xml Error] Character "<< p <<"'s base type is set."<<endl;
			exit(1);
		}
		cc.baseType_ = type;
	}
}


void loadEntity( const TiXmlNode* textNode, CTypeTokenizer& tokenizer, CharType type,
		map< CharValue, CharConditions >& ret)
{
	const char* text = getTinyXmlText( textNode );
	loadEntityImpl( text, tokenizer, type, ret);
}

void loadCondition(const TiXmlNode* condNode, CTypeTokenizer& tokenizer, Condition& ret)
{
	CharType type = CHAR_TYPE_INIT;
	vector<CharType> preTypes;
	vector<CharType> noPreTypes;
	vector<CharType> nextTypes;
	vector<CharType> noNextTypes;
	set<CharValue> nextChars;
	bool isEnd = false;

	for( const TiXmlNode* node = condNode->FirstChild(); node; node = node->NextSibling() )
	{
		const char* tag = node->Value();
		if( strcmp( tag, "type") == 0 )
			type = CMA_CType::getCharTypeByXmlName( getTinyXmlText( node ), true );
		else if( strcmp( tag, "pretype") == 0 )
			loadTypes( getTinyXmlText( node ), preTypes );
		else if( strcmp( tag, "nopretype") == 0 )
			loadTypes( getTinyXmlText( node ), noPreTypes );
		else if( strcmp( tag, "nexttype") == 0 )
			loadTypes( getTinyXmlText( node ), nextTypes );
		else if( strcmp( tag, "nonexttype") == 0 )
			loadTypes( getTinyXmlText( node ), noNextTypes );
		else if( strcmp( tag, "nextchar") == 0 )
			loadCharsValue( getTinyXmlText( node ), tokenizer, nextChars );
		else if( strcmp( tag, "end") == 0 )
		{
			isEnd = ( strcmp( getTinyXmlText( node ), "true") == 0 );
		}
		else
		{
			cerr<<"[poc.xml Warn] Ignore Condition Node tag " << tag << endl;
		}
	}

	ret.init( type, preTypes, noPreTypes, nextTypes, noNextTypes, nextChars, isEnd );
}

void loadRule(const TiXmlNode* ruleNode, CTypeTokenizer& tokenizer, map< CharValue, CharConditions >& ret)
{
	set<CharValue> values;
	vector<Condition > conds;
	//dealt the rule node
	for( const TiXmlNode* node = ruleNode->FirstChild(); node; node = node->NextSibling() )
	{
		const char* tag = node->Value();
		if( strcmp( tag, "char") == 0 )
			loadCharsValue( getTinyXmlText( node ), tokenizer, values );
		else if( strcmp( tag, "condition") == 0 )
		{
			Condition cond;
			loadCondition( node, tokenizer, cond );
			bool duplCond = false;
			for( size_t i = 0; i < conds.size(); ++i )
			{
				if( conds[i].equals( cond ))
				{
					duplCond = true;
					continue;
				}
			}

			if( !duplCond )
			{
				//cout<<"#Add new condition "<<cond<<endl;
				conds.push_back( cond );
			}
		}
	}

	//update the conditions
	for( set<CharValue>::iterator itr = values.begin(); itr != values.end(); ++itr)
	{
		CharConditions& charCond = ret[ *itr ];
		charCond.addConditions( conds );
		if( charCond.baseType_ == CHAR_TYPE_INIT )
			charCond.baseType_ = CHAR_TYPE_OTHER;
	}
}

int CMA_CType::loadConfiguration( const char* file )
{
    TiXmlDocument doc( file );
	bool loadOkay = doc.LoadFile( );
	if( !loadOkay )
	{
		cerr<< "[poc.xml Init Error] " << doc.ErrorDesc() << " for file "<< file << endl;
		return 0;
	}

	const TiXmlElement* root= doc.RootElement();

	CTypeTokenizer tokenizer( this );

	//load the entities
	const TiXmlNode* entityNode = root->FirstChild( "entities" );
	set< CharValue > spaceSet;

	for( const TiXmlNode* node = entityNode->FirstChild(); node; node = node->NextSibling() )
	{
		const char* tag = node->Value();
		if( strcmp( tag, "digit") == 0 )
			loadEntity( node, tokenizer, CHAR_TYPE_NUMBER, typeMap_ );
		else if( strcmp( tag, "chardigit") == 0 )
			loadEntity( node, tokenizer, CHAR_TYPE_CHARDIGIT, typeMap_ );
		else if( strcmp( tag, "letter") == 0 )
			loadEntity( node, tokenizer, CHAR_TYPE_LETTER, typeMap_ );
		else if( strcmp( tag, "punctuation") == 0 )
			loadEntity( node, tokenizer, CHAR_TYPE_PUNC, typeMap_ );
		else if( strcmp( tag, "space") == 0 )
		{
			string spaceValue =  getTinyXmlText( node );
			spaceValue += DEFAULT_SPACE;
			//loadEntityImpl( spaceValue.c_str(), tokenizer, CHAR_TYPE_SPACE, typeMap_ );
			loadCharsValue( spaceValue.c_str(), tokenizer, spaceSet );
		}
		else if( strcmp( tag, "sentenceseparator") == 0 )
			loadCharsValue( getTinyXmlText( node ), tokenizer, senSepSet_ );
		else if( node->ToComment() != 0 )
			continue;
		else
		{
			cerr<<"Unknown Entity Name: " << tag << endl;
			exit(1);
		}
	}

	// initialize space array
	memset( spaceArray_, 0x0, sizeof( spaceArray_ ) );
	for( set<CharValue>::iterator itr = spaceSet.begin();
	        itr != spaceSet.end(); ++itr )
	{
	    spaceArray_[ *itr % SPACE_ARRAY_SIZE ] = *itr;
	}


	// load the rules
	const TiXmlNode* rulesNode = root->FirstChild( "rules" );
	for( const TiXmlNode* node = rulesNode->FirstChild(); node; node = node->NextSibling() )
	{
		if( strcmp( node->Value(), "rule" ) != 0)
			continue;
		loadRule(node, tokenizer, typeMap_);
	}

	return 1;
}

CharType CMA_CType::getCharType(const char* p, CharType preType, const char* nextP) const
{
	if( isSpace( p ) )
        return CHAR_TYPE_SPACE;
    CharValue curV = getEncodeValue(p);
	map< CharValue, CharConditions >::const_iterator itr = typeMap_.find( curV );
	if( itr == typeMap_.end() )
		return CHAR_TYPE_OTHER;

	CharValue nextV = nextP ? getEncodeValue( nextP ) : 0;
	map< CharValue, CharConditions >::const_iterator itr2 = typeMap_.find( nextV );
	CharType nextType = (itr2 == typeMap_.end()) ? CHAR_TYPE_OTHER : itr2->second.baseType_;

	const CharConditions& charConds = itr->second;
	CharType matchedType = charConds.match(preType, nextV, nextType, charConds.baseType_);
	if( preType == CHAR_TYPE_LETTER && matchedType == CHAR_TYPE_NUMBER )
		return CHAR_TYPE_LETTER;
	return matchedType;
}

CharType CMA_CType::getBaseType( const char* p ) const
{
	CharValue curV = getEncodeValue(p);
	map< CharValue, CharConditions >::const_iterator itr = typeMap_.find( curV );
	if( itr == typeMap_.end() )
		return CHAR_TYPE_OTHER;
	return itr->second.baseType_;
}

bool CMA_CType::isSpace(const char* p) const
{
	CharValue charVal = getEncodeValue(p);
	int destIdx = charVal % SPACE_ARRAY_SIZE;
	return spaceArray_[ destIdx ] == charVal;
}

bool CMA_CType::isSentenceSeparator(const char* p) const
{
	CharValue curV = getEncodeValue(p);
	return senSepSet_.find(curV) != senSepSet_.end();
}

CharType CMA_CType::getDefaultEndType( CharType preType )
{
	if(preType == CHAR_TYPE_NUMBER || preType == CHAR_TYPE_LETTER
	            		|| preType == CHAR_TYPE_CHARDIGIT)
		return preType;
	return CHAR_TYPE_OTHER;
}

string CMA_CType::getPOCXmlStr( const char* p ) const
{
#ifdef USE_UTF_16
	if(type_ == Knowledge::ENCODE_TYPE_UTF16)
	{
		wiselib::UString ustr(p, wiselib::UString::UTF_8);
		string ret( (const char*)ustr.c_str(), ustr.length()*2 );
		return ret;
	}
#endif
	return string(p);
}

} // namespace cma
