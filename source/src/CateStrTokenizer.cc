#include "CateStrTokenizer.h"

namespace cma{

/*
const wstring _PuntStr = L"～！＠＃％＾＆×（）｀｛｝［］：＂｜；＇＼＜＞？，．／。《》“”‘’＋＝．、~!@#%^&*()`{}[]:\"|;'\\<>?,./.<>""''+=.";

const wstring _LetterStr = L"ＱＷＥＲＴＹＵＩＯＰｑｗｅｒｔｙｕｉｏｐＡＳＤＦＧＨＪＫＬａｓｄｆｇｈｊｋｌＺＸＣＶＢＮＭｚｘｃｖｂｎｍQWERTYUIOPqwertyuiopASDFGHJKLasdfghjklZXCVBNMzxcvbnm";

const wstring _HyphenStr = L"-—－_";

const wstring _SpaceStr = L" 　\n\r\f\t";

const wstring _DigitStr = L"０１２３４５６７８９0123456789";

typedef map<string, CharType> TYPE_MAP_T;
TYPE_MAP_T TYPE_MAP;

set<string> HYPHEN_SET;
 */
bool CST_INIT_FLAG = false;
 
/*
void addToTypeMap(const wstring& str, CharType type){
    size_t len = str.length();
    for(size_t i=0; i<len; ++i){
        TYPE_MAP[T_UTF8(str.substr(i, 1))] = type;
    }
}*/

void CateStrTokenizer::initialize(){
    if(CST_INIT_FLAG)
        return;
    CST_INIT_FLAG = true;
    /*addToTypeMap(_PuntStr, CHAR_TYPE_PUNC);
    addToTypeMap(_LetterStr, CHAR_TYPE_LETTER);
    addToTypeMap(_DigitStr, CHAR_TYPE_NUMBER);
    //Take The _HyphenStr As CHAR_TYPE_LETTER, but not CHAR_TYPE_HYPHEN
    addToTypeMap(_HyphenStr, CHAR_TYPE_LETTER);
    addToTypeMap(_SpaceStr, CHAR_TYPE_SPACE);*/
}

CateStrTokenizer::CateStrTokenizer(CTypeTokenizer* pCToken) :
      tokenT_(CHAR_TYPE_INIT), _preCharT(CHAR_TYPE_INIT),
      ctoken_(pCToken){
    ctype_ = ctoken_->getCType();
    nextTokenPtr = ctoken_->next();
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

    if(!nextTokenPtr){
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

    while(nextTokenPtr){
        const char* curChar = nextTokenPtr;
        nextTokenPtr = ctoken_->next();

        //find the CharType of the current string
        CharType curType = ctype_->getCharType(curChar);
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

        else if(_preCharT == CHAR_TYPE_NUMBER || _preCharT == CHAR_TYPE_LETTER
                    || _preCharT == CHAR_TYPE_HYPHEN)
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
                tokenT_ = CHAR_TYPE_NUMBER;
                _preCharT = curType;
                _strBuf = curChar;
                return true;
            }

            if(curType == CHAR_TYPE_OTHER){
                speStr_ = _strBuf;
                wordSeq_.push_back(curChar);
                tokenT_ = CHAR_TYPE_NUMBER;
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
