/*
 * fetch_special_word.cc
 *
 *  Created on: Sep 18, 2009
 *      Author: vernkin
 */
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <set>

#include "VSynonym.h"
#include "icma/util/CPPStringUtils.h"
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

void anlysisLine(string& line, VTrie &trie, map<string,set<string> >& outputMap)
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
		string segBuf;
		for(size_t j = i; j < n; ++j)
		{
			string& str = src[j];
			buf += str;
			segBuf += str + "  ";
			size_t strLen = str.length();
			size_t kk = 0;
			for (; node.moreLong && kk < strLen; ++kk) {
				trie.find(str[kk], &node);
			}

			if(kk < strLen) // don't reach the end
				break;


			if(node.data > 0)
			{
				if(j > i)
					outputMap[buf].insert(segBuf.substr(0, segBuf.length()-2));
				else if(outputMap.find(buf) != outputMap.end())
					outputMap[buf].insert(buf);
			}

			if(!node.moreLong)
				break;
		}
	}
}

void loadCurrentWord(const char* file, VTrie &trie)
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
			trie.insert(src[i].data(), &trieNode);
	}

	in.close();
}

void printUsage()
{
	cout<<"dict_file gold_file out_file"<<endl;
}

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printUsage();
		exit(1);
	}

	trieNode.setData(1);

	VTrie trie;
	loadDict(argv[1], trie);
	loadCurrentWord(argv[2], trie);
	cout<<"Trie size: "<<trie.size()<<endl;


	map<string,set<string> > outputMap;

	ifstream in(argv[2]);
	assert(in);
	string line;
	while(!in.eof())
	{
		getline(in, line);
		anlysisLine(line, trie, outputMap);
	}

	in.close();

	ofstream bout(argv[3]);
	for(map<string,set<string> >::iterator itr = outputMap.begin(); itr != outputMap.end(); ++itr)
	{
		bout<<itr->first;
		set<string>& iSet = itr->second;
		for(set<string>::iterator inner = iSet.begin(); inner != iSet.end(); ++inner)
			bout<<"\t"<<*inner;
		bout<<endl;
	}
	bout.close();


	cout<<"Output size "<<outputMap.size()<<endl;
	cout<<"Done!"<<endl;

}
