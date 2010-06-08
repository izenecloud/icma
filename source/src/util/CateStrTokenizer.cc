/** \file CateStrTokenizer.cc
 * \brief Category String Tokenizier
 *
 * Segment the string based on the char types, line letters, digits and so on
 *
 * \author vernkin
 */
#include "icma/util/CateStrTokenizer.h"

namespace cma{

CateStrTokenizer::CateStrTokenizer(CTypeTokenizer* pCToken) :
      tokenT_(CHAR_TYPE_INIT), _preCharT(CHAR_TYPE_INIT),
      ctoken_(pCToken), nextTokenPtr(0), init(true){
    ctype_ = ctoken_->getCType();
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
    #ifdef DEBUG_CATESTRTOKENIZER
        cout<<"ct.next()"<<endl;
    #endif
        
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

    if(!init && !nextTokenPtr){
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

    //clear the words vector if neccessary
    if(tokenT_ == CHAR_TYPE_OTHER){
        #ifdef EN_ASSERT
            assert(!wordSeq_.empty());
        #endif
        wordSeq_.clear();
    }

    char curChar[5];
    while(init || nextTokenPtr){
        if(init){
            init = false;
            nextTokenPtr = ctoken_->next();
            if(!nextTokenPtr)
                return false;
            strcpy(curChar, nextTokenPtr);
            //curChar = nextTokenPtr;
            nextTokenPtr = ctoken_->next();
            
        }else{
            strcpy(curChar, nextTokenPtr);
            //curChar = nextTokenPtr;
            nextTokenPtr = ctoken_->next();
        }

        //find the CharType of the current string FIXME 0 is error here
        CharType curType = ctype_->getCharType(curChar, _preCharT, nextTokenPtr);

        #ifdef EN_ASSERT
            assert(_preCharT != CHAR_TYPE_PUNC);
            assert(_preCharT != CHAR_TYPE_SPACE);
            assert(_preCharT != CHAR_TYPE_DATE);
        #endif

        #ifdef DEBUG_CATESTRTOKENIZER
            cout<<"P "<<_preCharT<<", C "<<curType<<endl;
        #endif

        if(curType == _preCharT){
            if(curType == CHAR_TYPE_OTHER)
                wordSeq_.push_back(curChar);
            else
                _strBuf.append(curChar);
            continue;
        }


        switch(_preCharT){
        
            case CHAR_TYPE_INIT:
                switch(curType){
                    case CHAR_TYPE_SPACE:
                        continue;
                    case CHAR_TYPE_PUNC:
                        speStr_ = curChar;
                        tokenT_ = CHAR_TYPE_PUNC;
                        return true;
                    default:
                        _preCharT = curType;
                        if(curType == CHAR_TYPE_OTHER){
                            _strBuf.clear();
                            wordSeq_.push_back(curChar);
                        }
                        else
                            _strBuf = curChar;
                        continue;
                }

            case CHAR_TYPE_OTHER:
                #ifdef EN_ASSERT
                assert(_strBuf.empty());
                assert(!wordSeq_.empty());
                #endif

                //ignore the space append other type
                if(curType == CHAR_TYPE_SPACE){
                    continue;
                }

                //digit, punc, letter
                _strBuf = curChar;
                _preCharT = curType;
                tokenT_ = CHAR_TYPE_OTHER;
                return true;

            case CHAR_TYPE_NUMBER:
                switch(curType){
                    case CHAR_TYPE_LETTER:
                        _preCharT = CHAR_TYPE_LETTER;
                        _strBuf.append(curChar);
                        continue;
                    case CHAR_TYPE_DATE:
                        _preCharT = CHAR_TYPE_INIT;
                        tokenT_ = CHAR_TYPE_DATE;
                        speStr_ = _strBuf;
                        speStr_.append(curChar);
                        _strBuf.clear();
                        return true;
                    case CHAR_TYPE_SPACE:
                        speStr_ = _strBuf;
                        tokenT_ = _preCharT;
                        //a new start
                        _preCharT = CHAR_TYPE_INIT;
                        _strBuf.clear();
                        return true;
                    case CHAR_TYPE_PUNC:
                        speStr_ = _strBuf;
                        tokenT_ = CHAR_TYPE_NUMBER;
                        _preCharT = curType;
                        _strBuf = curChar;
                        return true;
                    case CHAR_TYPE_OTHER:
                        speStr_ = _strBuf;
                        wordSeq_.push_back(curChar);
                        tokenT_ = CHAR_TYPE_NUMBER;
                        _preCharT = curType;
                        _strBuf.clear();
                        return true;
                    default:
                        assert(false && "Not Supposed to arrive here");
                }

            case CHAR_TYPE_LETTER:
                switch(curType){
                    case CHAR_TYPE_NUMBER:
                        _strBuf.append(curChar);
                        continue;
                    case CHAR_TYPE_SPACE:
                        speStr_ = _strBuf;
                        tokenT_ = _preCharT;
                        //a new start
                        _preCharT = CHAR_TYPE_INIT;
                        _strBuf.clear();
                        return true;
                    case CHAR_TYPE_PUNC:
                        speStr_ = _strBuf;
                        tokenT_ = CHAR_TYPE_NUMBER;
                        _preCharT = curType;
                        _strBuf = curChar;
                        return true;
                    case CHAR_TYPE_OTHER:
                        speStr_ = _strBuf;
                        wordSeq_.push_back(curChar);
                        tokenT_ = CHAR_TYPE_NUMBER;
                        _preCharT = curType;
                        _strBuf.clear();
                        return true;
                    default:
                        assert(false && "Not Supposed to arrive here");
                }

            default:
                assert(false && "Not Supposed to arrive here");
            } //end outer switch
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
