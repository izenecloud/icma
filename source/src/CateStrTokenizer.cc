#include "CateStrTokenizer.h"

namespace cma{

const wstring _PuntStr = L"～！＠＃％＾＆×（）｀｛｝［］：＂｜；＇＼＜＞？，．／。《》“”‘’＋＝．、~!@#%^&*()`{}[]:\"|;'\\<>?,./.<>""''+=_.";

const wstring _LetterStr = L"ＱＷＥＲＴＹＵＩＯＰｑｗｅｒｔｙｕｉｏｐＡＳＤＦＧＨＪＫＬａｓｄｆｇｈｊｋｌＺＸＣＶＢＮＭｚｘｃｖｂｎｍQWERTYUIOPqwertyuiopASDFGHJKLasdfghjklZXCVBNMzxcvbnm";

const wstring _HyphenStr = L"-—－";

const wstring _SpaceStr = L" 　";

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

CateStrTokenizer::CateStrTokenizer(string& sentence) :
      sen_(sentence), senIdx_(0), senLen_((int)sentence.length()){

}

string& CateStrTokenizer::getSpecialStr(){
   return curStr_;
}

CharType CateStrTokenizer::getType(){
    return curType_;
}

vector<string>& CateStrTokenizer::getWordSeq(){
    return wordSeq_;
}

bool CateStrTokenizer::isWordSeq(){
    return !wordSeq_.empty();
}

void getNextChar(string& source, int length, int& i, string& ret){
    unsigned int c1 = (unsigned int)sen[i];
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
            i += 3;
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
    if(senIdx_ == senLen_)
        return false;
    //clear the word seq
    if(!wordSeq_.empty())
        wordSeq_.clear();

    while(curIdx_ < senLen_){
        _curChar.clear();
        getNextChar(sen_, len_, curIdx_, _curChar);
        //cannot read any character (after ignore the invalid one)
        if(_curChar.empty()){
            #ifdef EN_ASSERT
                assert(curIdx_ == (len_ - 1));
            #endif
            return false;
        }

        //find the CharType of the current string
        TYPE_MAP_T::iterator typeItr = TYPE_MAP.find(curChar);
        CharType type = (typeItr == TYPE_MAP.end()) ? CHAR_TYPE_OTHER : *typeItr;

    }

}

}
