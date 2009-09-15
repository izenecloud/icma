/*
 * fetch_suspected_word.cc
 *
 *  Created on: Sep 11, 2009
 *      Author: vernkin
 */
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <set>

#include "VSynonym.h"
#include "CPPStringUtils.h"
#include "strutil.h"

using namespace std;

VTrieNode trieNode;

bool appendWordPOS(string& line, VTrie &trie){
    vector<string> tokens;
    TOKEN_STR(line, tokens);
    size_t n = tokens.size();
    if(!n){
        return false;
    }

    string word = tokens[0];
    replaceAll(word, "_", " ");

    trie.insert(word.data(), &trieNode);
    return true;
}

int loadDict(const char* file, VTrie &trie)
{
	ifstream in(file);
    if(!in)
    	return 0;
    string line;
    while(!in.eof()){
        //may be another way get line
        getline(in, line);
        if(line.empty())
            continue;
        appendWordPOS(line, trie);
    }
    in.close();
    return 1;
}

void anlysisLine(string& line, VTrie &trie, set<string>& curDictSet,
		set<string>& blackSet, set<string>& whiteSet)
{
	vector<string> src;
	TOKEN_STR(line, src);
	size_t n = src.size();
	if(!n)
		return;
	for (size_t i = 0; i < n - 1; ++i)
	{
		VTrieNode node;
		string buf;
		for(size_t j = i; j < n; ++j)
		{
			string& str = src[j];
			buf += str;
			size_t strLen = str.length();
			size_t kk = 0;
			for (; node.moreLong && kk < strLen; ++kk) {
				trie.find(str[kk], &node);
			}

			if(kk < strLen) // don't reach the end
				break;


			if(node.data > 0 && j > i)
			{
				blackSet.insert(buf);
				if(curDictSet.find(buf) != curDictSet.end()){
					whiteSet.insert(buf);
				}
			}

			if(!node.moreLong)
				break;
		}
	}
}

void collectCurrentWord(const char* file, set<string>& curDictSet)
{
	ifstream in(file);
	assert(in);

	string line;
	while(!in.eof())
	{
		getline(in, line);
		vector<string> src;
		TOKEN_STR(line, src);
		size_t n = src.size();
		for(size_t i=0; i<n; ++i)
			curDictSet.insert(src[i]);
	}

	in.close();
}

void printUsage()
{
	cout<<"dict_file gold_file black_out_file white_out_file"<<endl;
}

int main(int argc, char* argv[])
{
	if(argc < 4)
	{
		printUsage();
		exit(1);
	}

	trieNode.setData(1);

	VTrie trie;
	loadDict(argv[1], trie);
	cout<<"Trie size: "<<trie.size()<<endl;

	set<string> blackSet;
	set<string> whiteSet;
	set<string> curDictSet;
	collectCurrentWord(argv[2], curDictSet);

	ifstream in(argv[2]);
	assert(in);


	string line;
	while(!in.eof())
	{
		getline(in, line);
		anlysisLine(line, trie, curDictSet, blackSet, whiteSet);
	}

	in.close();

	ofstream bout(argv[3]);
	for(set<string>::iterator itr = blackSet.begin(); itr != blackSet.end(); ++itr)
		bout<<*itr<<endl;
	bout.close();

	ofstream wout(argv[4]);
	for(set<string>::iterator itr = whiteSet.begin(); itr != whiteSet.end(); ++itr)
		wout<<*itr<<endl;
	wout.close();

	cout<<"Output Black size "<<blackSet.size()<<endl;
	cout<<"Output White size "<<whiteSet.size()<<endl;
	cout<<"Done!"<<endl;

}
