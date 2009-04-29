
#include "VTrie.h"

#include "CMA_ME_Analyzer.h"
#include "strutil.h"
#include "pos_table.h"
#include "CPPStringUtils.h"
#include "CateStrTokenizer.h"
#include "tokenizer.h"

#include <fstream>
#include <iostream>

namespace cma {

    CMA_ME_Analyzer::CMA_ME_Analyzer() : knowledge_(0), ctype_(0) {
    }

    CMA_ME_Analyzer::~CMA_ME_Analyzer() {
        delete ctype_;
    }

    int CMA_ME_Analyzer::runWithSentence(Sentence& sentence) {
        int N = (int) getOption(OPTION_TYPE_NBEST);

        vector<pair<vector<string>, double> > segment;
        vector<vector<string> > pos;
        analysis(sentence.getString(), N, pos, segment, true);

        size_t size = segment.size();
        for (size_t i = 0; i < size; ++i) {
            MorphemeList list;
            vector<string>& segs = segment[i].first;
            vector<string>& poses = pos[i];
            size_t segSize = segs.size();
            for (size_t j = 0; j < segSize; ++j) {
                string& seg = segs[j];
                if(knowledge_->isStopWord(seg))
                    continue;
                Morpheme morp;
                morp.lexicon_ = seg; //TODO change the encoding
                morp.posCode_ = POSTable::instance()->getCodeFromStr(poses[j]);
                list.push_back(morp);
            }
            sentence.addList(list, segment[i].second);
        }

        return 1;
    }

    int CMA_ME_Analyzer::runWithStream(const char* inFileName, const char* outFileName) {
        int N = 1;
        bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

        ifstream in(inFileName);
        ofstream out(outFileName);
        string line;
        bool remains = !in.eof();
        while (remains) {
            getline(in, line);
            remains = !in.eof();
            if (!line.length()) {
                out << endl;
                continue;
            }
            vector<pair<vector<string>, double> > segment;
            vector<vector<string> > pos;
            analysis(line.data(), N, pos, segment, printPOS);

            if (printPOS) {
                vector<string>& best = segment[0].first;
                vector<string>& bestPOS = pos[0];
                size_t maxIndex = best.size() - 1;
                for (size_t i = 0; i < maxIndex; ++i) {
                    out << best[i] << posDelimiter_ << bestPOS[i] << wordDelimiter_;
                }

                if (remains)
                    out << best[maxIndex] << posDelimiter_ << bestPOS[maxIndex] << endl;
                else {
                    out << best[maxIndex] << posDelimiter_ << bestPOS[maxIndex];
                    break;
                }
            } else {
                vector<string>& best = segment[0].first;
                size_t maxIndex = best.size() - 1;
                for (size_t i = 0; i < maxIndex; ++i) {
                    out << best[i] << wordDelimiter_;
                }

                if (remains)
                    out << best[maxIndex] << endl;
                else {
                    out << best[maxIndex];
                    break;
                }
            }
        }

        in.close();
        out.close();

        return 1;
    }

    const char* CMA_ME_Analyzer::runWithString(const char* inStr) {
        bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;
      
        vector<pair<vector<string>, double> > segment;
        vector<vector<string> > pos;
        analysis(inStr, 1, pos, segment, printPOS);

        strBuf_.clear();
        if (printPOS) {
            vector<string>& best = segment.begin()->first;
            vector<string>& bestPOS = pos[0];
            size_t maxIndex = best.size() - 1;
            for (size_t i = 0; i < maxIndex; ++i) {
                strBuf_.append(best[i]).append(posDelimiter_).
                      append(bestPOS[i]).append(wordDelimiter_);
            }
            strBuf_.append(best[maxIndex]).append(posDelimiter_).
                  append(bestPOS[maxIndex]);
        } else {
            vector<string>& best = segment[0].first;
            size_t maxIndex = best.size() - 1;
            for (size_t i = 0; i < maxIndex; ++i) {
                strBuf_.append(best[i]).append(wordDelimiter_);
            }
            strBuf_.append(best[maxIndex]);
        }

        return strBuf_.c_str();
    }

    void CMA_ME_Analyzer::setKnowledge(Knowledge* pKnowledge) {
        knowledge_ = (CMA_ME_Knowledge*) pKnowledge;
        ctype_ = CMA_CType::instance(knowledge_->getEncodeType());
        if(knowledge_->getPOSTagger())
            knowledge_->getPOSTagger()->setCType(ctype_);
        if(knowledge_->getSegTagger())
            knowledge_->getSegTagger()->setCType(ctype_);
    }

namespace meanainner{

    /**
     * \param lastWordEnd include that index
     */
    inline void toCombine(VTrie *trie, CMA_CType* type, vector<string>& src,
            int begin, int lastWordEnd, vector<string>& dest){
        if(begin == lastWordEnd){
            dest.push_back(src[begin]);
            return;
        }
        bool isOK = true;
        //only have two word
        /*if((lastWordEnd - begin < 2) &&
                (type->length(src[begin].data()) + type->length(src[begin+1].data()) < 3)){
            VTrieNode node;
            trie->search(src[begin].data(), &node);
            if(node.data > 0){
                trie->search(src[begin + 1].data(), &node);
                isOK = node.data <= 0;
            }
        }*/

        if(isOK){
            string buf = src[begin];
            for(int k=begin+1; k<=lastWordEnd; ++k)
                buf.append(src[k]);
            dest.push_back(buf);
        }else{
            for(int k=begin; k<=lastWordEnd; ++k)
                dest.push_back(src[k]);
        }
    }

    void combineRetWithTrie(VTrie *trie, vector<string>& src, 
            vector<string>& dest, CMA_CType* type) {
        
        int begin = -1;
        int lastWordEnd = -1;

        int n = (int)src.size();
        VTrieNode node;
        for (int i = 0; i < n; ++i) {
            string& str = src[i];

            #ifdef DEBUG_TRIE_MATCH
            cout<<"Before Check begin:"<<begin<<node<<endl;
            #endif

            size_t strLen = str.length();
            size_t j = 0;
            for (; node.moreLong && j < strLen; ++j) {
                trie->find(str[j], &node);
                //cout<<"Ret "<<(int)(unsigned char)str[j]<<","<<node<<endl;
            }

            #ifdef DEBUG_TRIE_MATCH
            cout<<"Check str "<<str<<",isEnd:"<<(j == strLen)<<node<<endl;
            #endif

            //didnot reach the last bit
            if (j < strLen) {
                //no exist in the dictionary
                if (begin < 0)
                    dest.push_back(str);
                else {
                    if(lastWordEnd < begin)
                        lastWordEnd = begin;
                    toCombine(trie, type, src, begin, lastWordEnd, dest);
                    begin = -1;
                    //restart that node
                    i = lastWordEnd; //another ++ in the loop
                }
                node.init();
            } else {
                if (node.moreLong && (i < n - 1)) {
                    if(begin < 0)
                        begin = i;
                    if(node.data > 0)
                        lastWordEnd = i;
                } else {
                    if (begin < 0)
                        dest.push_back(str);
                    else {
                        if(node.data > 0)
                            lastWordEnd = i;
                        else if(lastWordEnd < begin)
                            lastWordEnd = begin;

                        toCombine(trie, type, src, begin, lastWordEnd, dest);
                        begin = -1;
                        i = lastWordEnd;
                    }
                    node.init();
                }
            }
        } //end for

        if(begin >= 0){
            toCombine(trie, type, src, begin, n-1, dest);
        }
    }

}

    void CMA_ME_Analyzer::analysis(const char* sentence, int N,
            vector<vector<string> >& posRet,
            vector<pair<vector<string>, double> >& segRet, bool tagPOS) {
        int segN = N;
        segRet.resize(N);

        vector<pair<vector<string>, double> > segment(N);

        SegTagger* segTagger = knowledge_->getSegTagger();


        CTypeTokenizer token(ctype_, sentence);

        #ifdef USE_BE_TYPE_FEATURE
        vector<string> words;
        const char* next = 0;
        while((next = token.next())){
            words.push_back(next);
        }
        if(N == 1)
            segTagger->seg_sentence_best(words, segment[0].first);
        else
            segTagger->seg_sentence(words, segN, N, segment);
        #else
        //separate digits, letter and so on
        CateStrTokenizer ct(&token);
        while (ct.next()) {
            if (ct.isWordSeq()) {
                vector<string>& words = ct.getWordSeq();
                if (words.size() > 1) {
                    if(N == 1)
                        segTagger->seg_sentence_best(words, segment[0].first);
                    else
                        segTagger->seg_sentence(words, segN, N, segment);
                    continue;
                }
                string& word = words.front();
                for (int i = 0; i < N; ++i) {
                    segment[i].first.push_back(word);
                }
            } else {
                #ifdef DEBUG_POC_TAGGER
                    cout<<"Get Speical String "<<ct.getSpecialStr()<<endl;
                #endif
                for (int i = 0; i < N; ++i) {
                    segment[i].first.push_back(ct.getSpecialStr());
                }
            }
        }
        #endif


        VTrie *trie = knowledge_->getTrie();
        for (int i = 0; i < N; ++i) {
            pair<vector<string>, double>& srcPair = segment[i];
            pair<vector<string>, double>& destPair = segRet[i];
            destPair.second = srcPair.second;
            meanainner::combineRetWithTrie( trie, srcPair.first, 
                    destPair.first, ctype_);
        }

        if(!tagPOS)
            return;

        posRet.resize(N);
        POSTagger* posTagger = knowledge_->getPOSTagger();       
        for (int i = 0; i < N; ++i) {
            posTagger->tag_sentence_best(segRet[i].first, posRet[i]);
        }


    }



}
