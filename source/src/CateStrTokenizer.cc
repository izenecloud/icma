#include "CateStrTokenizer.h"

namespace cma{

const wstring _PuntStr = L"～！＠＃％＾＆×（）｀｛｝［］：＂｜；＇＼＜＞？，．／。《》“”‘’＋＝．、~!@#%^&*()`{}[]:\"|;'\\<>?,./.<>""''+=.";

const wstring _LetterStr = L"ＱＷＥＲＴＹＵＩＯＰｑｗｅｒｔｙｕｉｏｐＡＳＤＦＧＨＪＫＬａｓｄｆｇｈｊｋｌＺＸＣＶＢＮＭｚｘｃｖｂｎｍQWERTYUIOPqwertyuiopASDFGHJKLasdfghjklZXCVBNMzxcvbnm";

const wstring _HyphenStr = L"-—－_";

const wstring _SpaceStr = L" 　\n\r\f\t";

const wstring _DigitStr = L"０１２３４５６７８９0123456789";

typedef map<string, CharType> TYPE_MAP_T;
TYPE_MAP_T TYPE_MAP;

set<string> HYPHEN_SET;

bool CST_INIT_FLAG = false;

void addToTypeMap(const wstring& str, CharType type){
    size_t len = str.length();
    for(size_t i=0; i<len; ++i){
        TYPE_MAP[T_UTF8(str.substr(i, 1))] = type;
    }
}

void CateStrTokenizer::initialize(){
    if(CST_INIT_FLAG)
        return;
    CST_INIT_FLAG = true;
    addToTypeMap(_PuntStr, CHAR_TYPE_PUNC);
    addToTypeMap(_LetterStr, CHAR_TYPE_LETTER);
    addToTypeMap(_DigitStr, CHAR_TYPE_DIGIT);
    addToTypeMap(_HyphenStr, CHAR_TYPE_HYPHEN);
    addToTypeMap(_SpaceStr, CHAR_TYPE_SPACE);
}

CateStrTokenizer::CateStrTokenizer(const string& sentence) :
      sen_(sentence), senIdx_(0), senLen_((int)sentence.length()),
      _preCharT(CHAR_TYPE_INIT), tokenT_(CHAR_TYPE_INIT){

}

string& CateStrTokenizer::getSpecialStr(){
   return speStr_;
}

CharType CateStrTokenizer::getType(){
    return tokenT_;
}

vector<string>& CateStrTokenizer::getWordSeq(){
    return wordSeq_;
}

bool CateStrTokenizer::isWordSeq(){
    return tokenT_ == CHAR_TYPE_OTHER;
}

void getNextChar(string& source, int length, int& i, string& ret){
    unsigned int c1 = (unsigned int)source[i];
    if ( c1<0x80 ) {
        ret = source.substr(i, 1);
        ++i;
    }
    else if ( (c1 & 0xe0)==0xc0 ){
        if ( i+1 < length ){
            ret = source.substr(i, 2);
            i += 2;
        }
        else
            i = length;
    }
    else if ( (c1 & 0xf0)==0xe0 ){
        if ( i+2<length ){
            ret = source.substr(i, 3);
            i += 3;
        }
        else
            i = length;
    }
    else if ( (c1 & 0xf8)==0xf0 ){
        if ( i+3<length ){
            ret = source.substr(i, 4);
            i += 4;
        }
        else
            i = length;
    }
    else {
        // illegal coding, skip that char
        ++i;
        if(i < length)
            getNextChar(source, length, i, ret);
    }
}

bool CateStrTokenizer::next(){
    //only punctuation contains at most one character
    if(_preCharT == CHAR_TYPE_PUNC){
        if(tokenT_ == CHAR_TYPE_OTHER)
            wordSeq_.clear();
        _preCharT = CHAR_TYPE_INIT;
        tokenT_ = CHAR_TYPE_PUNC;
        speStr_ = _strBuf;
        _strBuf.clear();
        return true;
    }

    if(senIdx_ == senLen_){
        //cannot be space type
        #ifdef EN_ASSERT
            assert(_preCharT != CHAR_TYPE_SPACE);
        #endif
        if(_preCharT == CHAR_TYPE_INIT)
            return false;
        if(_preCharT == CHAR_TYPE_OTHER){
            tokenT_ = CHAR_TYPE_OTHER;
            _preCharT = CHAR_TYPE_INIT;
            return true;
        }
        //else
        tokenT_ = _preCharT;
        _preCharT = CHAR_TYPE_INIT;
        speStr_ = _strBuf;
        //_strBuf.clear();
        return true;
    }

    if(tokenT_ == CHAR_TYPE_OTHER){
        #ifdef EN_ASSERT
            assert(!wordSeq_.empty());
        #endif
        wordSeq_.clear();
    }

    while(senIdx_ < senLen_){
        string curChar;
        getNextChar(sen_, senLen_, senIdx_, curChar);
        //cannot read any character (after ignore the invalid one)
        if(curChar.empty()){
            #ifdef EN_ASSERT
                assert(senIdx_ == senLen_);
            #endif
           //try to get ending one
            return next();
        }

        //find the CharType of the current string
        TYPE_MAP_T::iterator typeItr = TYPE_MAP.find(curChar);
        CharType curType = (typeItr == TYPE_MAP.end()) ? CHAR_TYPE_OTHER : typeItr->second;
        #ifdef EN_ASSERT
            assert(_preCharT != CHAR_TYPE_PUNC);
            assert(_preCharT != CHAR_TYPE_SPACE);
        #endif

        if(curType == _preCharT){
            if(curType == CHAR_TYPE_OTHER)
                wordSeq_.push_back(curChar);
            else
                _strBuf.append(curChar);
            continue;
        }


        if(_preCharT == CHAR_TYPE_INIT)
        {
            #ifdef EN_ASSERT
                assert(_strBuf.empty());
            #endif
            //ignore the leading space
            if(curType == CHAR_TYPE_SPACE){
                continue;
            }

            if(curType == CHAR_TYPE_PUNC){
                speStr_ = curChar;
                tokenT_ = CHAR_TYPE_PUNC;
                return true;
            }

            _preCharT = curType;
            if(curType == CHAR_TYPE_OTHER)
                wordSeq_.push_back(curChar);
            else
                _strBuf = curChar;
        }

        else if(_preCharT == CHAR_TYPE_OTHER){
            #ifdef EN_ASSERT
                assert(_strBuf.empty());
                assert(!wordSeq_.empty());
            #endif

            //ignore the space append other type
            if(curType == CHAR_TYPE_SPACE){
                continue;
            }

            //digit, hyphen, punc, letter
            _strBuf = curChar;
            tokenT_ = CHAR_TYPE_OTHER;
            _preCharT = curType;
            return true;
        }

        else if(_preCharT == CHAR_TYPE_DIGIT || _preCharT == CHAR_TYPE_HYPHEN
                    || _preCharT == CHAR_TYPE_LETTER)
        {
            if(curType == CHAR_TYPE_SPACE){
                speStr_ = _strBuf;
                tokenT_ = _preCharT;
                //a new start
                _preCharT = CHAR_TYPE_INIT;
                _strBuf.clear();
                return true;
            }

            if(curType == CHAR_TYPE_PUNC){
                speStr_ = _strBuf;
                tokenT_ = CHAR_TYPE_DIGIT;
                _preCharT = curType;
                _strBuf = curChar;
                return true;
            }

            if(curType == CHAR_TYPE_OTHER){
                speStr_ = _strBuf;
                wordSeq_.push_back(curChar);
                tokenT_ = CHAR_TYPE_DIGIT;
                _preCharT = curType;
                _strBuf.clear();
                return true;
            }
            
            //improve a level
            _preCharT = CHAR_TYPE_LETTER;
            _strBuf.append(curChar);
            continue;
        }
    } //end while

    tokenT_ =  _preCharT;
    _preCharT = CHAR_TYPE_INIT;
    if(tokenT_ != CHAR_TYPE_OTHER){
        speStr_ = _strBuf;
        _strBuf.clear();
    }
    return true;
}

}
