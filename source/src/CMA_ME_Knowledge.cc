/** \file CMA_ME_Knowledge.cc
 *
 * \brief The Knowledge class of the CMAC
 *
 * \author vernkin
 * \version 0.1
 * \date Feb 17, 2009
 */

#include <string>

#include "CMA_ME_Knowledge.h"
#include "CMABasicTrainer.h"
#include "strutil.h"

#include <assert.h>
using namespace std;

namespace cma{

inline bool fileExists(const char* path)
{
	ifstream posIn(path);
	if(posIn)
	{
		posIn.close();
		return true;
	}
	return false;
}

CMA_ME_Knowledge::CMA_ME_Knowledge()
		: segT_(0), posT_(0),vsynC_(0),trie_(new VTrie), posTable_(new POSTable){
}

CMA_ME_Knowledge::~CMA_ME_Knowledge(){
    delete segT_;
    delete posT_;
    delete vsynC_;
    delete trie_;
    delete posTable_;
}

int CMA_ME_Knowledge::loadPOSModel(const char* cateName){
    string cateStr(cateName);

    //load pos table first
    ifstream posFile((cateStr + ".pos").data());
    assert(posFile);
    string line;
    while(!posFile.eof()){
        getline(posFile, line);
        trimSelf(line);
        if(line.empty())
            continue;
        posTable_->addPOS(line);
    }

    assert(!posT_);
    posT_ = new POSTagger((cateStr + ".model").data(), trie_);

    map<string, string> configMap;
    loadConfig0((cateStr + ".config").data(), configMap, false);
    string ret = configMap["defaultPOS"];
    posT_->defaultPOS = ret.empty() ? "N" : ret;

    ret = configMap["numberPOS"];
    posT_->numberPOS = ret.empty() ? "M" : ret;

    ret = configMap["letterPOS"];
    posT_->letterPOS = ret.empty() ? "NX" : ret;

    ret = configMap["puncPOS"];
    posT_->puncPOS = ret.empty() ? "W" : ret;

    ret = configMap["datePOS"];
    posT_->datePOS = ret.empty() ? "T" : ret;

    return 1;
}

int CMA_ME_Knowledge::loadStatModel(const char* cateName){
	string cateStr(cateName);

	int ret = CMA_CType::instance( getEncodeType() )->loadConfiguration( (cateStr + ".xml" ).data() );
	if( !ret )
	{
		cerr<<" Fail to load poc.xml, please check that file! "<<endl;
		exit(1);
		return 0;
	}

	assert(!segT_);
    segT_ = new SegTagger(cateStr, trie_);

    //try to load black words here
    string blackWordFile = cateStr.substr(0, cateStr.length() - 3) + "blackwords";


    ifstream bwIn(blackWordFile.data());
    if(bwIn)
	{
		string line;
		while(!bwIn.eof()){
			getline(bwIn, line);
			trimSelf(line);
			if(line.length() <= 0)
				continue;
			blackWords_.insert(line);
		}
		bwIn.close();
	}

    return 1;
}

int CMA_ME_Knowledge::loadSynonymDictionary(const char* fileName){
    assert(!vsynC_);
    vsynC_ = new VSynonymContainer(fileName, " ", "_");
    return 1;
}

void CMA_ME_Knowledge::getSynonyms(const string& word, VSynonym& synonym){
    vsynC_->get_synonyms(word, synonym);
}

int CMA_ME_Knowledge::loadStopWordDict(const char* fileName){
    ifstream in(fileName);
    if(!in)
    	return 0;
    string line;
    while(!in.eof()){
        getline(in, line);
        trimSelf(line);
        if(line.length() <= 0)
            continue;
        stopWords_.insert(line);
    }
    in.close();
    return 1;
}

int CMA_ME_Knowledge::loadSystemDict_(const char* binFileName){
	FILE *in = fopen(binFileName, "r");
    if(!in)
    	return 0;
    string line;
    while(!feof(in)){
        //may be another way get line
        line = readEncryptLine(in);
        if(line.empty())
            continue;
        appendWordPOS(line);
    }
    fclose(in);
    return 1;
}

int CMA_ME_Knowledge::loadSystemDict(const char* binFileName){
    if(!trie_)
        trie_ = new VTrie();

    int curRet = loadSystemDict_(binFileName);
    int ret = curRet;
    string destFile(binFileName);
    while(curRet)
    {
    	std::ostringstream buffer;
    	buffer << destFile << "." << ret;
    	curRet = loadSystemDict_(buffer.str().data());
    	ret += curRet;
    }

    return ret;
}

int CMA_ME_Knowledge::loadUserDict_(const char* fileName){
	ifstream in(fileName);
    if(!in)
    	return 0;
    cout<<"Load User Dic "<<fileName<<endl;
    string line;
    while(!in.eof()){
        getline(in, line);
        if(line.empty())
        	continue;
        appendWordPOS(line);
    }

    in.close();
    return 1;
}

int CMA_ME_Knowledge::loadUserDict(const char* fileName){
    if(!trie_)
        trie_ = new VTrie();

    int curRet = loadUserDict_(fileName);
    int ret = curRet;
    string destFile(fileName);
    while(curRet)
    {
    	std::ostringstream buffer;
    	buffer << destFile << "." << ret;
    	curRet = loadUserDict_(buffer.str().data());
    	ret += curRet;
    }

    return ret;
}


int CMA_ME_Knowledge::encodeSystemDict(const char* txtFileName,
        const char* binFileName){
    ifstream in(txtFileName);
    FILE *out = fopen(binFileName, "w");
    string line;

    int seCode[] = {0x12, 0x34, 0x54, 0x27};

    while(!in.eof()){
        getline(in, line);
        int size = (int)line.size();
        if(!size)
            continue;
        char buf[size + 1];
        buf[size] = '\0';
        strncpy(buf, line.data(), size);    

        for(int i=0; i<size; ++i){
            if( i % 2 == 0)
                buf[i] += 2;
            else if( i % 3 == 0)
                buf[i] += 3;
            else if( i % 5 == 0)
                buf[i] += 5;
            else if( i % 7 == 0)
                buf[i] += 7;
            buf[i] ^= seCode[ i % 4 ];
        }

        for(int i=0; i<=size/2; i+=2){
            char tmp = buf[i];
            buf[i] = buf[ size - 1 - i ];
            buf[ size - 1 - i ] = tmp;
        }

        //swap the length bit
        fputc(size >> 8 & 0xff, out);
        fputc(size >> 24 & 0xff, out);
        fputc(size & 0xff, out);
        fputc(size >> 16 & 0xff, out);
        fputs(buf, out);
    }

    in.close();
    fclose(out);
    return 1;
}

int CMA_ME_Knowledge::loadModel(const char* encoding, const char* modelPath)
{
	//set encoding
	Knowledge::EncodeType encode = Knowledge::decodeEncodeType(encoding);
	assert(encode != Knowledge::ENCODE_TYPE_NUM);
	setEncodeType(encode);

	assert(modelPath);
	string path = formatModelPath(modelPath);

	// load the STAT model
	loadStatModel( ( path + "poc").data() );

	// load the POS model
	ifstream posIn( ( path + "pos.model").data() );
	if(posIn)
	{
		posIn.close();
		loadPOSModel( ( path + "pos" ).data() );
	}

	//TODO here have to change to load system dictionary
	// load the system dictionaries
	loadUserDict( ( path + "sys.dic").data() );

	// load the configuration
	loadConfig( ( path + "cma.config" ).data() );

	return 1;
}

bool CMA_ME_Knowledge::isSupportPOS() const
{
	return posT_;
}

SegTagger* CMA_ME_Knowledge::getSegTagger() const{
    return segT_;
}

POSTagger* CMA_ME_Knowledge::getPOSTagger() const{
    return posT_;
}

string CMA_ME_Knowledge::readEncryptLine(FILE *in){
    int seCode[] = {0x12, 0x34, 0x54, 0x27};
    int lenBuf[4];
    lenBuf[0] = fgetc(in);
    if(lenBuf[0] == -1)
        return "";
    for(int i=1; i<4; ++i){
        lenBuf[i] = fgetc(in);
    }
    int size = (lenBuf[0] << 8) + (lenBuf[1] << 24) + (lenBuf[2]) + (lenBuf[3] << 16);

    char buf[size + 1];
    buf[size] = '\0';
    fgets(buf, size + 1, in);

    for(int i=0; i<=size/2; i+=2){
        char tmp = buf[i];
        buf[i] = buf[ size - 1 - i ];
        buf[ size - 1 - i ] = tmp;
    }

    for(int i=0; i<size; ++i){
        buf[i] ^= seCode[ i % 4 ];
        if( i % 2 == 0)
            buf[i] -= 2;
        else if( i % 3 == 0)
            buf[i] -= 3;
        else if( i % 5 == 0)
            buf[i] -= 5;
        else if( i % 7 == 0)
            buf[i] -= 7;
    }

    return string(buf);
}

bool CMA_ME_Knowledge::isStopWord(const string& word){
    return stopWords_.find(word) != stopWords_.end();
}

VTrie* CMA_ME_Knowledge::getTrie(){
    return trie_;
}

int CMA_ME_Knowledge::loadConfig(const char* fileName)
{
    bool r = loadConfig0( fileName, sysConfig_, true);

    return r ? 1 : 0;
}

bool CMA_ME_Knowledge::appendWordPOS(string& line){
    vector<string> tokens;
    TOKEN_STR(line, tokens);
    size_t n = tokens.size();
    if(!n){
        return false;
    }
    string& word = tokens[0];
    if(blackWords_.find(word) != blackWords_.end())
    {
    	return false;
    }
    set<string>* posSet = 0;
    //try to search first
    VTrieNode node;
    trie_->search(word.data(), &node);
    //already exits
    if(node.data > 0){
        if(posT_)
			posSet = &(posT_->posVec_[node.data]);
    }else{

    	if(posT_)
    	{
			//get the right offset (offset 0 is reserved)
			node.data = (int)posT_->posVec_.size();
			//insert new key
			posT_->posVec_.push_back(set<string>());
			posSet = &(posT_->posVec_.back());

			trie_->insert(word.data(), &node);
    	}
    	else
    	{
    		node.data = 1;
    		trie_->insert(word.data(), &node);
    	}
    }

    if(posT_)
    {
		for(size_t i=1; i<n; ++i){
			posSet->insert(tokens[i]);
		}
    }


    return true;
}

const POSTable* CMA_ME_Knowledge::getPOSTable() const
{
	return posTable_;
}

bool CMA_ME_Knowledge::loadConfig0(const char *filename, map<string, string>& map,
		bool required) {
  std::ifstream ifs(filename);
  if(required)
	  assert(ifs);
  else if(!ifs)
	  return false;

  std::string line;
  CMA_CType* ctype = CMA_CType::instance( getEncodeType() );
  while (std::getline(ifs, line)) {
    if (!line.size() ||
        (line.size() && (line[0] == ';' || line[0] == '#'))) continue;

    size_t pos = line.find('=');
    assert(pos != std::string::npos && "format error: ");

    size_t s1, s2;
    for (s1 = pos+1; s1 < line.size() && isspace(line[s1]); s1++);
    for (s2 = pos-1; static_cast<long>(s2) >= 0 && isspace(line[s2]); s2--);

    string key   = trimLeft(line.substr(0, s2 + 1));

    string valueTmp = line.substr(s1, line.size() - s1);
    const char* valuePtr = valueTmp.data();
    int len = static_cast<int>( valueTmp.length() );

    //analysis the value
    string value;
    int i = 0;
    while( i < len )
    {
    	unsigned short bytes = ctype->getByteCount( valuePtr + i );
    	if( bytes == 0 )
    		break;

    	if(bytes == 1)
    	{
    		if(valuePtr[i] == '\\')
    		{
    			++i;

    			switch( valuePtr[i] )
    			{
    			case '\'':
					value += "\'";
					break;
    			case '\"':
					value += "\"";
					break;
    			case '\\':
					value += "\\";
					break;
    			case '0':
					value += "\0";
					break;
    			case 'a':
					value += "\a";
					break;
    			case 'b':
					value += "\b";
					break;
    			case 'f':
					value += "\f";
					break;
    			case 'n':
					value += "\n";
					break;
    			case 'r':
					value += "\r";
					break;
    			case 't':
					value += "\t";
					break;
    			case 'v':
					value += "v";
					break;
    			case ' ':
					value += " ";
					break;
    			default:
    				cerr<<"Unknown Escape Sequence "<<valuePtr[i]<<endl;
    				return 0;
    			}
    		}
    		else
    			value += valueTmp.substr(i, 1);
    	}
    	else
    	{
    		value += valueTmp.substr(i, bytes);
    	}
    	i += bytes;
    }

    map[key] = value;
    //cout<<"Get "<<key<<" = "<<value<<endl;
  }

  return true;
}

const string* CMA_ME_Knowledge::getSystemProperty( const string& key )
{
	map<string, string>::iterator itr = sysConfig_.find( key );
	if( itr == sysConfig_.end() )
		return 0;
	return &itr->second;
}

}
