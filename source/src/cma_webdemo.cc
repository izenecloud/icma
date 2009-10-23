#include <platform.h>
#include <microhttpd.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <boost/tokenizer.hpp>

#include "strutil.h"
#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

// Deamon required header files
#include <sys/types.h>
#include <sys/stat.h>
//#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>


//#define DEBUG_HTTP

#define USE_DEAMON

#define PORT            8380
#define POSTBUFFERSIZE  512
#define GET             0
#define POST            1

using namespace std;
using namespace cma;

Knowledge* knowledge = 0;

class ConnectionInfo {
public:
	ConnectionInfo() : tagPOS(false), nbest(1)
	{
	}

	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	string userinput;
	int answercode;
	bool tagPOS;
	unsigned int nbest;
};

string EMPTY_STR = "";

const string askpage ="\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
<head>\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
<title>iCMA' Demo</title>\n\
\n\
<style type=\"text/css\">\n\
	body {\n\
		margin:0px;\n\
		font-size:13px;\n\
		font-family:Arial, Helvetica, sans-serif;\n\
		}\n\
		\n\
	#container {\n\
		position:relative;\n\
		width:90%;\n\
		margin-left:5%;\n\
		margin-right:5%;\n\
		/*border:#666666 thin solid;*/\n\
	}\n\
	\n\
	#banner {\n\
		font-size:20px;\n\
		text-align:center;\n\
		border-bottom:#CCCCCC thin solid;\n\
		padding-top:15px;\n\
		padding-bottom:10px;\n\
		margin-bottom:5px;\n\
	}\n\
	\n\
	/** Content Area */\n\
	\n\
	#content {\n\
		width:100%;\n\
		margin-bottom:15px;\n\
	}\n\
	\n\
	#input {\n\
		float:left;\n\
		width:48%;\n\
		margin-right:1%;\n\
		height:inherit;\n\
	}\n\
	\n\
	#result {\n\
		float:right;\n\
		width:48%;\n\
		margin-left:1%;\n\
		height:inherit;\n\
	}\n\
	\n\
	.prompt {\n\
		margin-top:2px;\n\
		margin-bottom:2px;\n\
		color:blue;\n\
		text-align:left;\n\
		display:block;\n\
	}\n\
	\n\
	#userInput {\n\
		margin-top:2px;\n\
		margin-bottom:2px;\n\
		font-size:14px;\n\
		width:100%;\n\
	}\n\
	\n\
	#inputbtns {\n\
		text-align:center;\n\
		margin-bottom:5px;\n\
		width:100%;\n\
	}\n\
	\n\
	.inputbtn {\n\
		margin: 1px 3px 1px 3px;\n\
		font-size:14px;\n\
	} \n\
	\n\
	#uploadfilediv {\n\
		text-align:left;\n\
		margin-bottom:5px;\n\
		width:100%;\n\
	}\n\
	\n\
	#resultdisplay {\n\
		font-size:14px;\n\
		margin-top:3px;\n\
		margin-bottom:5px;\n\
	}\n\
	\n\
	#optionsdiv {\n\
		text-align:left;\n\
		margin-bottom:5px;\n\
		margin-top:2px;\n\
		width:100%;\n\
	}\n\
	\n\
	.optionprompt {\n\
		color:black;\n\
		margin-left:2px;\n\
		margin-right:2px;\n\
	}\n\
	\n\
	.optioninfo {\n\
		color:green;\n\
		margin-left:2px;\n\
	}\n\
	\n\
	.optionwarn {\n\
		color:red;\n\
		margin-left:2px;\n\
	}\n\
	\n\
	/** footer */\n\
	#footer { \n\
		border-top:#CCCCCC thin solid;\n\
		margin-top: 10px;\n\
		clear: both; \n\
		text-align: center;\n\
		color: #808080;\n\
		padding: 5px 0;\n\
	}\n\
\n\
	#footer a {\n\
		color: #C3593C;\n\
		background: inherit;\n\
	}\n\
	\n\
	/** POS Display */	\n\
	.rankSpan {\n\
		font-size:18px;\n\
		color:black;\n\
		font-weight:bold;\n\
		padding-right:2px;\n\
	}\n\
	.scoreSpan {\n\
		color:red;\n\
		font-size:20px;\n\
		padding-right:4px;\n\
	}\n\
	\n\
	.possame {\n\
		font-weight:bold;\n\
	}\n\
	\n\
	.pos {\n\
		color:Maroon;\n\
	}\n\
	\n\
	.sentence {\n\
		width:100%;\n\
		border:#000000 thin solid;\n\
		margin-bottom:15px;\n\
	}\n\
	\n\
	.oddresult {\n\
		width:100%;\n\
		display:block;\n\
		background-color:white;\n\
	}\n\
	\n\
	.evenresult {\n\
		width:100%;\n\
		display:block;\n\
		background-color:#CCCCCC;\n\
	}\n\
	\n\
</style>\n\
\n\
<script type=\"text/javascript\">\n\
\n\
//functions\n\
function getFirstEleByIdItr(ele, id){\n\
	var children = ele.childNodes;\n\
	for(var i=0;i<children.length;++i){		\n\
		var child = children[i];\n\
		if(child.id == id)\n\
			return child;\n\
		if(child.childNodes.length>0){\n\
			var ret = getFirstEleByIdItr(child,id);\n\
			if(ret != null)\n\
				return ret;\n\
		}\n\
	}\n\
	return null;\n\
}\n\
\n\
function findParentById(node, id)\n\
{\n\
	var parent = node.parentNode;\n\
	while(parent != undefined && parent != null)\n\
	{\n\
		if(parent.id == id)\n\
			return parent;\n\
		parent = parent.parentNode;\n\
	}\n\
	return null;\n\
}\n\
\n\
function resetDirectInput( ele )\n\
{\n\
	var father = findParentById( ele, \"input\" );\n\
	if( father == null )\n\
		return;\n\
	var userInput = getFirstEleByIdItr( father, \"userInput\" );\n\
	if( userInput != null )\n\
		userInput.value = \"\";\n\
}\n\
\n\
function isUnsignedInteger(s) {\n\
  return (s.toString().search(/^[0-9]+$/) == 0);\n\
}\n\
\n\
function onPOSTagChange( )\n\
{\n\
	var cb = document.getElementById( \"tagposcheckbox\" );\n\
	alert( cb.checked );\n\
}\n\
\n\
var tempNBest;\n\
\n\
function onNBestChange( ele )\n\
{\n\
	if( !isUnsignedInteger(ele.value) )\n\
	{\n\
		alert(\"N Best must be a positive integer!\");\n\
		tempNBest = ele;\n\
    	setTimeout(\"tempNBest.focus();\",1);\n\
		//ele.focus();\n\
		return false;\n\
	}\n\
	return true;\n\
}\n\
\n\
function onSubmit( form )\n\
{\n\
	var tagPOS = 0;\n\
	if( document.getElementById( \"tagposcheckbox\" ).checked )\n\
		tagPOS = 1;\n\
	var nbest = document.getElementById( \"nbesttext\" ).value;\n\
	\n\
	form.tagpos.value = tagPOS;\n\
	form.nbest.value = nbest;\n\
	\n\
	//alert(form+\",\"+form.tagpos+\",\"+form.tagpos.value+\",\"+form.nbest+\",\"+form.nbest.value);\n\
	return true;\n\
}\n\
\n\
</script>\n\
</head>\n\
\n\
<body>\n\
<div id=\"container\">\n\
	<div id=\"banner\">iCMA's Web Demo</div>\n\
	\n\
	<div id=\"content\">\n\
		<div id=\"input\">\n\
			<form method=\"post\" action=\"/textpost\" onsubmit=\"return onSubmit(this);\">\n\
			<span class=\"prompt\">Please input the Chinese Text:</span>\n\
			<textarea  rows=\"15\" wrap=\"soft\" name=\"userInput\" id=\"userInput\">%s</textarea>\n\
			<div id=\"inputbtns\">\n\
				<input type=\"hidden\" name=\"tagpos\" value=\"1\"/>\n\
				<input type=\"hidden\" name=\"nbest\" value=\"1\"/>\n\
				<input type=\"submit\" class=\"inputbtn\" value=\"Segment\"/>\n\
				<input type=\"button\" class=\"inputbtn\" value=\"Clear\" onclick=\"resetDirectInput(this);\"/>\n\
			</div>\n\
			</form>\n\
			<span class=\"prompt\"></span>\n\
			<span class=\"prompt\">Or upload a file with UTF-8 encoding:</span>\n\
			\n\
			<div id=\"uploadfilediv\">\n\
				<form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\" onsubmit=\"return onSubmit(this);\">\n\
					<input type=\"hidden\" name=\"tagpos\" value=\"1\"/>\n\
					<input type=\"hidden\" name=\"nbest\" value=\"1\"/>\n\
					<input name=\"file\" type=\"file\" size=\"35px\"/>\n\
					<input type=\"submit\" value=\" Upload \" class=\"inputbtn\"/>\n\
				</form>			\n\
			</div>\n\
			\n\
			<span class=\"prompt\">Options:</span>\n\
			<div id=\"optionsdiv\">\n\
				<span class=\"optionprompt\">POS Output:</span>\n\
				<input type=\"checkbox\" id=\"tagposcheckbox\" %s/>\n\
				<span class=\"optioninfo\">Whether tags POS (Part-Of-Speech).</span>\n\
			</div>\n\
			\n\
			<div id=\"optionsdiv\">\n\
				<span class=\"optionprompt\">N-Best:</span>\n\
				<input type=\"text\" size=\"5px\" id=\"nbesttext\" value=\"%d\" onblur=\"return onNBestChange(this);\"/>\n\
				<span class=\"optioninfo\">Display at most N possible results. The N-best results are for each line in the input separately.</span>\n\
				<span class=\"optionwarn\">If N &gt; 1, please don't input large text/file as it causes the browser to display too many contents at one time.</span>\n\
			</div>\n\
		</div>\n\
		\n\
		<div id=\"result\">\n\
			<span class=\"prompt\">The Segmentation Result is:&nbsp;&nbsp;&nbsp;<a href=\"/icma_pos.html\" target=\"_blank\">View iCMA's POS Tag Set</a></span>\n\
			<div id=\"resultdisplay\">%s</div>\n\
		</div>\n\
	\n\
	</form>\n\
	</div>\n\
	\n\
	<!-- footer -->\n\
	<div id=\"footer\">\n\
		<p><a href=\"http://www.izenesoft.com\" target=\"_blank\">Contact</a> | <a href=\"mailto:vernkin@gmail.com\">Advices</a> <br/>\n\
		&copy; Copyright 2009 iZENEsoft (Shanghai) Co., Ltd.</p>\n\
	</div>\n\
</div>\n\
</body>\n\
</html>\n\
				";

const string iCMA_POS_PAGE = "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
<head>\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
<title>POS Tag Sets of Basis and iJMA</title>\n\
</head>\n\
\n\
<style type=\"text/css\">\n\
<!--\n\
body {\n\
	width:94%;\n\
	margin-left:3%;\n\
	margin-right:3%;\n\
	font-size:15px;\n\
}\n\
\n\
table {\n\
	width:100%;\n\
	margin-bottom:20px;\n\
	margin-top:20px;\n\
	border:solid 1px;\n\
}\n\
\n\
table tr th, table tr td {\n\
	border:solid 1px;\n\
}\n\
\n\
.linkdiv {\n\
	width:100%;\n\
	text-align:center;\n\
	margin-top:5px;\n\
	margin-bottom:5px;\n\
}\n\
\n\
.linkdiv a {\n\
	padding-left:5px;\n\
	padding-right:5px;\n\
	font-size:16px;\n\
}\n\
\n\
-->\n\
</style>\n\
\n\
<body>\n\
<div class=\"linkdiv\">\n\
	<h1>iCMA's POS Tag Set</h1>\n\
</div>\n\
<table name=\"basis\">\n\
<tr><th colspan=\"4\">iCMA's POS Tag Set</th></tr>\n\
<tr><th>Tag</th><th>English Description</th><th>中文解释</th><th>Example</th></tr>\n\
<tr><td>A</td><td>Adjective.</td><td>形容词。</td><td>上古, 美丽</td></tr>\n\
<tr><td>AD</td><td>Adverbial adjective.</td><td>副形词直接作状语的形容词，形容词代码a和副词代码d并在一起。</td><td>不同，不够</td></tr>\n\
<tr><td>AG</td><td>Adjective morpheme.</td><td>形语素形容词性语素。形容词代码为a，语素代码ｇ前面置以A。</td><td>举，乔</td></tr>\n\
<tr><td>AN</td><td>Adjective with a Noun function.</td><td>名形词具有名词功能的形容词。形容词代码a和名词代码n并在一起。</td><td>亲昵，亮丽</td></tr>\n\
<tr><td>B</td><td>Distinguish words.</td><td>区别词:大型一次性短效,取汉字'别'的声母。</td><td>亲自，人为</td></tr>\n\
<tr><td>C</td><td>Conjunction.</td><td>连词。</td><td>和，以及</td></tr>\n\
<tr><td>D</td><td>Adverb.</td><td>副词。</td><td>仅仅，仍旧</td></tr>\n\
<tr><td>DG</td><td>Adverb Morpheme.</td><td>副语素是副词性语素。副词代码为d，语素代码ｇ前面置以D。</td><td>务，勉</td></tr>\n\
<tr><td>E</td><td>Exclamation.</td><td>叹词。</td><td>呀，啊</td></tr>\n\
<tr><td>F</td><td>Position.</td><td>方位词:后右下取汉字'方'。</td><td>（假期）期间，（本世纪）末</td></tr>\n\
<tr><td>G</td><td>Morpheme.</td><td>语素绝大多数语素都能作为合成词的'词根'，取汉字'根'的声母。</td><td>呱，咪</td></tr>\n\
<tr><td>H</td><td>Pre-access Component.</td><td>前接成分。</td><td>小（孩子），老（李）</td></tr>\n\
<tr><td>I</td><td>Idiom.</td><td>成语。</td><td>龙马精神，欣欣向荣</td></tr>\n\
<tr><td>J</td><td>Abbreviation.</td><td>简称略语取汉字'简'的声母。</td><td>中(国)，美（国）</td></tr>\n\
<tr><td>K</td><td>Post-access Component.</td><td>后接成分。</td><td>（欧美）式，（突发）性</td></tr>\n\
<tr><td>L</td><td>Temporal Idiom.</td><td>习语:有意思固定资产操作系统习用语尚未成为成语，有点'临时性'，取'临'的声母</td><td>突围而出，按捺不住</td></tr>\n\
<tr><td>M</td><td>Numeral.</td><td>数词。</td><td>三万一千，四百</td></tr>\n\
<tr><td>MQ</td><td>Quantifier.</td><td>数量词。</td><td>(三百)户，（五千）量</td></tr>\n\
<tr><td>NG</td><td>Noun Morpheme.</td><td>名语素为名词性语素。名词代码为n，语素代码ｇ前面置以N。</td><td>扇，技</td></tr>\n\
<tr><td>N</td><td>Noun.</td><td>名词。</td><td>桌子，花瓶</td></tr>\n\
<tr><td>NR</td><td>Person's Name.</td><td>人名名词为代码n和“人(ren)”的声母并在一起。</td><td>爱因斯坦，周恩来</td></tr>\n\
<tr><td>NS</td><td>Location's Name.</td><td>地名名词为代码n和处所词代码s并在一起。</td><td>上海，巴黎</td></tr>\n\
<tr><td>NT</td><td>Organization's Name.</td><td>机构团体'团'的声母为t，名词代码n和t并在一起。</td><td>联合国，北京奥组委</td></tr>\n\
<tr><td>NX</td><td>Foreign Characters.</td><td>外文字符英文或英文数字字符串。</td><td>Simple，Easy</td></tr>\n\
<tr><td>NZ</td><td>Proper Noun.</td><td>其他专名'专'的声母的第1个字母为z，名词代码n和z并在一起。</td><td>抄表器，抑制器</td></tr>\n\
<tr><td>O</td><td>Onomatopoeia.</td><td>拟声词。</td><td>扑通（一声），朗朗（上口）</td></tr>\n\
<tr><td>P</td><td>Preposition.</td><td>介词。</td><td>本着（公平的原则），来自（山东）</td></tr>\n\
<tr><td>Q </td><td>Quantity.</td><td>量词。</td><td>（三）杯，（一）枚</td></tr>\n\
<tr><td>R</td><td>Pronoun.</td><td>代词。</td><td>我，他</td></tr>\n\
<tr><td>S</td><td>Space.</td><td>处所词。</td><td>枯水区，柜中</td></tr>\n\
<tr><td>TG</td><td>Time Morpheme.</td><td>时语素时间词性语素。时间词代码为t,在语素的代码g前面置以T。</td><td>汉，清</td></tr>\n\
<tr><td>T</td><td>Time.</td><td>时间词。</td><td>去年，今天</td></tr>\n\
<tr><td>U</td><td>Auxiliary.</td><td>助词。</td><td>然，物</td></tr>\n\
<tr><td>VD</td><td>Adverbial Verb.</td><td>副动词直接作状语的动词。动词和副词的代码并在一起。</td><td>牵头，狡辩</td></tr>\n\
<tr><td>VG</td><td>Verb Morpheme.</td><td>动语素动词性语素。动词代码为v。在语素的代码g前面置以V。</td><td>狩，率</td></tr>\n\
<tr><td>VN</td><td>The Verb with a noun function.</td><td>名动词指具有名词功能的动词。动词和名词的代码并在一起。</td><td>玩乐，玩火</td></tr>\n\
<tr><td>V</td><td>Verb.</td><td>动词。</td><td>玩闹，环抱</td></tr>\n\
<tr><td>W</td><td>Punctuation.</td><td>标点符号。</td><td>【，？</td></tr>\n\
<tr><td>X</td><td>Non-morpheme Word.</td><td>非语素字只是一个符号，字母x通常用于代表未知数、符号。</td><td>瑷，疬</td></tr>\n\
<tr><td>Y</td><td>Modal.</td><td>语气词汉字“语”的声母。</td><td>的，着哪</td></tr>\n\
<tr><td>Z</td><td>Status Word.</td><td>状态词，取汉字'状'的声母的前一个字母。</td><td>酷热，甜甜</td></tr>\n\
<tr><td>un</td><td>Unknown.</td><td>未知。</td><td>滹，滷</td></tr>\n\
</table>\n\
\n\
</body>\n\
</html>\n\
";

const string a_nbest_result = "\
			<div class=\"%s\">\n\
				<span class=\"rankSpan\">%d:</span>\n\
				<span class=\"scoreSpan\">%.5f</span>\n\
%s\n\
			</div>\n\
";

const char* ODD_RESULT_ROW = "oddresult";
const char* EVEN_RESULT_ROW = "evenresult";

const string POS_DELIM = "<bold>/</bold>";
const string WORD_DELIM = " &nbsp; ";

const char *errorpage =
		"<html><body>This doesn’t seem to be right.</body></html>";

const char *servererrorpage =
		"<html><body>An internal server error has occured.</body></html>";

/** Special Webpage */
const char *invalidurlpage =
		"<html><body>The Request URL <b>%s</b> is invalid. Back to <a href=\"/\">Home</a>.</body></html>";

/**
 * Before invoke this method, global variable knowledge should be initialized
 */
void init_analyzer( Analyzer* ana )
{
	ana->setKnowledge( knowledge );
	ana->setOption(Analyzer::OPTION_TYPE_NBEST, 1);
	ana->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 1);
	ana->setPOSDelimiter( POS_DELIM.c_str() );
	ana->setWordDelimiter( WORD_DELIM.c_str() );
}

//Process the input first
typedef boost::tokenizer <boost::char_separator<char>,
	string::const_iterator, string> LineTokenizer;
const boost::char_separator<char> LINE_SEP("\n");

/**
 * Perform the segment
 */
void perform_segment( string& input, bool tagPOS, int nbest, string& ret)
{
	Analyzer* analyzer = CMA_Factory::instance()->createAnalyzer();
	init_analyzer( analyzer );
	analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, tagPOS ? 1 : 0);

	replaceAll( input, "\r", " " ); //support windows format
	LineTokenizer token(input, LINE_SEP);

	if( nbest <= 1 )
	{
		analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, 1 );
		for(LineTokenizer::const_iterator itr = token.begin(); itr != token.end(); ++itr){
			string line = *itr;
			trimSelf(line);
			const char* segRet = analyzer->runWithString( line.c_str());
			ret.append( segRet );
			ret.append("<BR>");
		}
		return;
	}

	// if nbest > 1
	analyzer->setOption( Analyzer::OPTION_TYPE_NBEST, nbest );
	for(LineTokenizer::const_iterator itr = token.begin(); itr != token.end(); ++itr)
	{
		// get a single line
		string line = *itr;
		trimSelf(line);
		if( line.empty() ) // ignore empty line
			continue;
		ret.append("<div class=\"sentence\">\n");

		Sentence sent( line.c_str() );
		analyzer->runWithSentence( sent );

		int resultNum = sent.getListSize();
		for( int i = 0; i < resultNum; ++i )
		{
			string ithSegResult;
			int ithResultCount = sent.getCount(i);
			for( int j = 0; j < ithResultCount; ++j )
			{
				if( tagPOS )
					ithSegResult += string(sent.getLexicon(i, j)) + POS_DELIM
							+ string(sent.getStrPOS(i, j)) + WORD_DELIM;
				else
					ithSegResult += string(sent.getLexicon(i, j)) + WORD_DELIM;
			}

			const char* rowType= (i % 2 == 0 ) ? ODD_RESULT_ROW : EVEN_RESULT_ROW;

			size_t bufLen = a_nbest_result.length() + ithSegResult.length() + 30;
			char *buf = new char[bufLen];
			memset( buf, 0x0, bufLen );
			sprintf( buf, a_nbest_result.c_str(), rowType, (i+1),
					sent.getScore(i), ithSegResult.c_str());

			ret.append(buf);
			delete[] buf;
		}

		ret.append("</div>\n"); //end <div class="sentence">
	}

}

int send_segment_page(struct MHD_Connection *connection, string& userinput,
		bool tagPOS, unsigned int nbest, int status_code)
{
	int ret;
	struct MHD_Response *response;

	string result;
	if( !userinput.empty() )
		perform_segment( userinput, tagPOS, nbest, result );
	size_t bufLen = askpage.length() + userinput.length() + result.length() + 50;
	char *buf = new char[ bufLen ];
	memset( buf, 0x0, bufLen );
	string tagPOSStr = tagPOS ? "checked" : "";
	int writeBytes = sprintf( buf, askpage.c_str(), userinput.c_str(),
			tagPOSStr.c_str(), nbest, result.c_str() );
#ifdef DEBUG_HTTP
	//cout<<"## send_segment_page userinput = "<<userinput<<" . "<<endl;
	//cout<<"## send_segment_page return "<<writeBytes<<" bytes "<<endl;
#endif

	response = MHD_create_response_from_data(strlen(buf), (void *) buf,
			MHD_NO, MHD_YES);

	if (!response)
		return MHD_NO;
	ret = MHD_queue_response(connection, status_code, response);
	MHD_destroy_response(response);
	delete[] buf;
	return ret;
}

int send_page(struct MHD_Connection *connection, const char *page,
		int status_code)
{
	int ret;
	struct MHD_Response *response;
	response = MHD_create_response_from_data(strlen(page), (void *) page,
			MHD_NO, MHD_YES);
	if (!response)
		return MHD_NO;
	ret = MHD_queue_response(connection, status_code, response);
	MHD_destroy_response(response);
	return ret;
}

int send_page(struct MHD_Connection *connection, string& page,
		int status_code)
{
	return send_page( connection, page.c_str(), status_code );
}

int iterate_post(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
		const char *filename, const char *content_type,
		const char *transfer_encoding, const char *data, uint64_t off,
		size_t size)
{
#ifdef DEBUG_HTTP
	cout<<"\n#### iterate_post #####"<<endl;
	if( key )
		cout<<"\t key = "<<key<<endl;
	if( filename )
		cout<<"\t filename = "<<filename<<endl;
	if( content_type )
		cout<<"\t content_type = "<<content_type<<endl;
	if( transfer_encoding )
		cout<<"\t transfer_encoding = "<<transfer_encoding<<endl;
	cout<<"\t size = "<<size<<endl;
#endif

	ConnectionInfo *con_info =
			(ConnectionInfo *) coninfo_cls;

	if (0 == strcmp(key, "userInput"))
	{
		con_info->userinput.append(data);
		return MHD_YES;
	}

	if (0 == strcmp(key, "file"))
	{
		con_info->userinput.append(data);
		return MHD_YES;
	}

	if (0 == strcmp(key, "tagpos")){
		con_info->tagPOS = ( 0 != atoi(data) );
		return MHD_YES;
	}

	if (0 == strcmp(key, "nbest")){
		int intVal = atoi(data);
		if( intVal < 1 )
			intVal = 1;
		con_info->nbest = intVal;
		return MHD_YES;
	}

	return MHD_YES;
}

/** Invoke by a request ended */
void request_completed(void *cls, struct MHD_Connection *connection,
		void **con_cls, enum MHD_RequestTerminationCode toe)
{
#ifdef DEBUG_HTTP
	cout<<"## request_completed begin ..."<<endl;
#endif

	if ( !con_cls || !*con_cls )
		return;

	ConnectionInfo *con_info = (ConnectionInfo *) *con_cls;

	if (con_info->connectiontype == POST)
	{
		if( NULL != con_info->postprocessor )
			MHD_destroy_post_processor(con_info->postprocessor);
	}

	delete con_info;
	*con_cls = NULL;

#ifdef DEBUG_HTTP
	cout<<"## request_completed end."<<endl;
#endif
}

int answer_to_connection(void *cls, struct MHD_Connection *connection,
		const char *url, const char *method, const char *version,
		const char *upload_data, size_t *upload_data_size, void **con_cls)
{
#ifdef DEBUG_HTTP
	cout<<"\n#### New Connection #####"<<endl;
	cout<<"\t url = "<<url<<endl;
	cout<<"\t method = "<<method<<endl;
	cout<<"\t version = "<<version<<endl;
	cout<<"\t upload_data_size = "<<upload_data_size<<endl;
#endif

	bool isFilePost = false;
	bool isTextPost = false;

	string lowerURL( url );
	toLowerSelf( lowerURL );

	if( lowerURL == "/icma_pos.html")
		return send_page(connection, iCMA_POS_PAGE.c_str(), MHD_HTTP_OK);
	else if( lowerURL == "/filepost")
		isFilePost = true;
	else if( lowerURL == "/textpost" )
		isTextPost = true;
	else if( lowerURL != "/favicon.ico" && lowerURL != "/" )
	{
		char buffer[1024] = { 0 };
		sprintf(buffer, invalidurlpage, url);
		return send_page(connection, buffer, MHD_HTTP_OK);
	}


	if (NULL == *con_cls)
	{
		ConnectionInfo *con_info = new ConnectionInfo;

		if ( 0 == strcmp(method, "POST") )
		{
			con_info->postprocessor = MHD_create_post_processor(connection,
									POSTBUFFERSIZE, iterate_post, (void *) con_info);
			if (NULL == con_info->postprocessor)
			{
				delete con_info;
				return MHD_NO;
			}
			con_info->connectiontype = POST;
			*con_cls = (void *) con_info;
			return MHD_YES;
		}
		else
		{
			con_info->connectiontype = GET;
			*con_cls = (void *) con_info;
			//don't return if connectiontype is GET
		}
	}

	if (0 == strcmp(method, "GET"))
	{
		//int ret;
		//char buffer[1024] = { 0 };
		//sprintf(buffer, askpage, nr_of_uploading_clients);
		return send_segment_page(connection, EMPTY_STR, true, 1, MHD_HTTP_OK);
	}

	if (0 == strcmp(method, "POST"))
	{
		ConnectionInfo *con_info = (ConnectionInfo*)(*con_cls);
		if (0 != *upload_data_size)
		{
			MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
			*upload_data_size = 0;
			return MHD_YES;
		}
		else
			return send_segment_page(connection, con_info->userinput,
					con_info->tagPOS, con_info->nbest, MHD_HTTP_OK);
	}

	return send_page(connection, errorpage, MHD_HTTP_BAD_REQUEST);
}

void run_webdemo( int port )
{
	cout<<"## To initialize iCMA..."<<endl;
	//initialize the iCMA
	const char* modelPath = "../db/icwb/utf8/";
	// create instances
	CMA_Factory* factory = CMA_Factory::instance();
	knowledge = factory->createKnowledge();
	knowledge->loadModel( "utf8", modelPath );



	cout<<"## To initialize HTTP Server in port "<<port<<" ..."<<endl;
	struct MHD_Daemon *daemon = 0;
	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED,
			request_completed, NULL, MHD_OPTION_END);
	if (NULL == daemon)
		return;
	cout<<"## The initialization is done!"<<endl;

#ifndef USE_DEAMON
	getchar();
	if(daemon)
		MHD_stop_daemon(daemon);
#endif
}

int main(int argc, char* argv[])
{
	int port = PORT;
	if( argc >= 2 )
	{
		port = atoi( argv[1] );
	}

#ifdef USE_DEAMON
	cout<<"## To become a daemon..."<<endl;
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
			exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0) {
			exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Open any logs here */

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir(".")) < 0) {
			/* Log the failure */
			exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* Daemon-specific initialization goes here */

	/* The Big Loop */
	run_webdemo( port );
	while(1)
	{
		sleep(60);
	} // end big loop
#else
	run_webdemo( port );
#endif



	exit(EXIT_SUCCESS);
	return 0;
}
