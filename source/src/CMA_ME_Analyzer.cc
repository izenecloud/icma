
#include "VTrie.h"

#include "CMA_ME_Analyzer.h"
#include "strutil.h"
#include "pos_table.h"
#include "CPPStringUtils.h"
#include "CateStrTokenizer.h"

#include <fstream>
#include <iostream>

namespace cma {

    CMA_ME_Analyzer::CMA_ME_Analyzer() : knowledge_(0) {
        CateStrTokenizer::initialize();
    }

    CMA_ME_Analyzer::~CMA_ME_Analyzer() {

    }

    int CMA_ME_Analyzer::runWithSentence(Sentence& sentence) {
        int N = (int) getOption(OPTION_TYPE_NBEST);

        vector<pair<vector<string>, double> > segment;
        vector<vector<string> > pos;
        analysis(string(sentence.getString()), N, pos, segment, true);

        size_t size = segment.size();
        for (size_t i = 0; i < size; ++i) {
            MorphemeList list;
            vector<string>& segs = segment[i].first;
            vector<string>& poses = pos[i];
            size_t segSize = segs.size();
            for (size_t j = 0; j < segSize; ++j) {
                Morpheme morp;
                morp.lexicon_ = segs[j]; //TODO change the encoding
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

        //check the encoding
        string inFile(inFileName);
        string outFile(outFileName);
        bool createTmpFile = false;
        if(encoding_ != Knowledge::ENCODE_TYPE_UTF8){
            createTmpFile = true;
            inFile += ".utf8.tmp";
            outFile += ".utf8.tmp";
            ENC_FILE(ENCODING_VEC[Knowledge::ENCODE_TYPE_UTF8].data(), ENCODING_VEC[encoding_].data(),
                inFileName, inFile.data());
        }

        ifstream in(inFile.data());
        ofstream out(outFile.data());
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
            analysis(line, N, pos, segment, printPOS);

            if (printPOS) {
                vector<string>& best = segment[0].first;
                vector<string>& bestPOS = pos[0];
                size_t maxIndex = best.size() - 1;
                for (size_t i = 0; i < maxIndex; ++i) {
                    out << best[i] << "/" << bestPOS[i] << " ";
                }

                if (remains)
                    out << best[maxIndex] << "/" << bestPOS[maxIndex] << endl;
                else {
                    out << best[maxIndex] << "/" << bestPOS[maxIndex];
                    break;
                }
            } else {
                vector<string>& best = segment[0].first;
                size_t maxIndex = best.size() - 1;
                for (size_t i = 0; i < maxIndex; ++i) {
                    out << best[i] << " ";
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

        if(createTmpFile){
            //remove temporal utf8 input file
            remove(inFile.data());
            //change the encoding to the final encoding
            ENC_FILE(ENCODING_VEC[encoding_].data(), ENCODING_VEC[Knowledge::ENCODE_TYPE_UTF8].data(),
                outFile.data(), outFileName);
            //remove temporal utf8 output file
            remove(outFile.data());
        }

        return 1;
    }

    const char* CMA_ME_Analyzer::runWithString(const char* inStr) {
        bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

        string line(inStr);

        //FOR Test
        /*VTrieNode node;
        knowledge_->getTrie()->search(inStr, &node);
        cout<<"Exist "<<(node.data > 0)<<endl;
        */
        
        vector<pair<vector<string>, double> > segment;
        vector<vector<string> > pos;
        analysis(line, 1, pos, segment, printPOS);

        strBuf_.clear();
        if (printPOS) {
            vector<string>& best = segment.begin()->first;
            vector<string>& bestPOS = pos[0];
            size_t maxIndex = best.size() - 1;
            for (size_t i = 0; i < maxIndex; ++i) {
                strBuf_.append(best[i]).append("/").append(bestPOS[i]).append(" ");
            }
            strBuf_.append(best[maxIndex]).append("/").append(bestPOS[maxIndex]);
        } else {
            vector<string>& best = segment[0].first;
            size_t maxIndex = best.size() - 1;
            for (size_t i = 0; i < maxIndex; ++i) {
                strBuf_.append(best[i]).append(" ");
            }
            strBuf_.append(best[maxIndex]);
        }

        return strBuf_.c_str();
    }

    void CMA_ME_Analyzer::setKnowledge(Knowledge* pKnowledge) {
        knowledge_ = (CMA_ME_Knowledge*) pKnowledge;
        encoding_ = knowledge_->getEncodeType();
    }

    void combineRetWithTrie(VTrie *trie, vector<string>& src, vector<string>& dest) {
        
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
                    string buf = src[begin];
                    if(lastWordEnd < begin)
                        lastWordEnd = begin;
                    for(int k=begin+1; k<=lastWordEnd; ++k)
                        buf.append(src[k]);
                    dest.push_back(buf);
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
                        string buf = src[begin];

                        if(node.data > 0)
                            lastWordEnd = i;
                        else if(lastWordEnd < begin)
                            lastWordEnd = begin;

                        for(int k=begin+1; k<=lastWordEnd; ++k)
                            buf.append(src[k]);

                        dest.push_back(buf);
                        
                        begin = -1;
                        i = lastWordEnd;
                    }
                    node.init();
                }
            }
        } //end for

        if(begin >= 0){
            string buf = src[begin];
            for(int k=begin+1; k<n; ++k)
                buf.append(src[k]);
            dest.push_back(buf);
        }
    }

    void CMA_ME_Analyzer::analysis(const string& sentence, int N,
            vector<vector<string> >& posRet,
            vector<pair<vector<string>, double> >& segRet, bool tagPOS) {
        int segN = N > 1 ? N : 2;
        segRet.resize(N);

        vector<pair<vector<string>, double> > segment(N);

        SegTagger* segTagger = knowledge_->getSegTagger();
        //separate digits, letter and so on
        CateStrTokenizer ct(sentence);
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
                for (int i = 0; i < N; ++i) {
                    segment[i].first.push_back(ct.getSpecialStr());
                }
            }
        }

        //TODO: use the trie to combined words
        VTrie *trie = knowledge_->getTrie();
        for (int i = 0; i < N; ++i) {
            pair<vector<string>, double>& srcPair = segment[i];
            pair<vector<string>, double>& destPair = segRet[i];
            destPair.second = srcPair.second;
            combineRetWithTrie( trie, srcPair.first, destPair.first);
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
