#include <platform.h>
#include <microhttpd.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include "cma_factory.h"
#include "analyzer.h"
#include "knowledge.h"
#include "sentence.h"

#define DEBUG_HTTP

#define PORT            9999
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
<style>\n\
	body {\n\
		margin:0px;\n\
		font-size:13px;\n\
		font-family:Arial, Helvetica, sans-serif;\n\
		}\n\
		\n\
	#container {\n\
		position:relative;\n\
		width:80%;\n\
		margin-left:10%;\n\
		margin-right:10%;\n\
		/*border:#666666 thin solid;*/\n\
	}\n\
	\n\
	#banner {\n\
		font-size:20px;\n\
		text-align:center;\n\
		border-bottom:grey thin solid;\n\
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
		width:49%;\n\
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
	/** footer */\n\
	#footer { \n\
		border-top:grey thin solid;\n\
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
	/** POS Display */\n\
	.showNbestDiv ol li{\n\
		font-size:15px;\n\
		color:black;\n\
	}\n\
	\n\
	.scoreSpan {\n\
		color:red;\n\
		font-size:20px;\n\
		padding-right:7px;\n\
	}\n\
	\n\
	.possame {\n\
		font-weight:bold;\n\
	}\n\
	\n\
	.pos {\n\
		color:Maroon;\n\
	}\n\
</style>\n\
\n\
<script>\n\
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
					<input name=\"file\" type=\"file\" size=\"35px\">\n\
					<input type=\"submit\" value=\" Upload \" class=\"inputbtn\">\n\
				</form>			\n\
			</div>\n\
			\n\
			<span class=\"prompt\">Options:</span>\n\
			<div id=\"optionsdiv\">\n\
				<span class=\"optionprompt\">POS Output:</span>\n\
				<input type=\"checkbox\" id=\"tagposcheckbox\" %s/>\n\
				<span class=\"optionprompt\">&nbsp;&nbsp;</span>\n\
				<span class=\"optionprompt\">N Best:</span>\n\
				<input type=\"text\" size=\"5px\" id=\"nbesttext\" value=\"%d\" onblur=\"return onNBestChange(this);\"/>\n\
			</div>\n\
		</div>\n\
		\n\
		<div id=\"result\">\n\
			<span class=\"prompt\">The Segmentation Result is:</span>\n\
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
	ana->setPOSDelimiter( "<bold>/</bold>" );
	ana->setWordDelimiter( "&nbsp;&nbsp;&nbsp;" );
}


int send_segment_page(struct MHD_Connection *connection, string& userinput,
		bool tagPOS, unsigned int nbest, int status_code)
{
	int ret;
	struct MHD_Response *response;

	Analyzer* analyzer = 0;

	string result;
	if( !userinput.empty() )
	{
		analyzer = CMA_Factory::instance()->createAnalyzer();
		init_analyzer( analyzer );
		analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, tagPOS ? 1 : 0);
		result = analyzer->runWithString( userinput.c_str()) ;
	}
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
	delete analyzer;
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
		con_info->tagPOS = ( 0 != strcmp(data, "0") );
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

	if (NULL == con_cls)
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

	if( strcmp(url, "/filepost") == 0 )
		isFilePost = true;
	else if( strcmp(url, "/textpost") == 0 )
		isTextPost = true;
	else if( strcmp(url, "/favicon.ico") != 0 && strcmp(url, "/") != 0 )
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

int main()
{
	cout<<"## To initialize iCMA..."<<endl;
	//initialize the iCMA
	const char* modelPath = "../db/icwb/utf8/";
	// create instances
	CMA_Factory* factory = CMA_Factory::instance();
	knowledge = factory->createKnowledge();
	knowledge->loadModel( "utf8", modelPath );

	cout<<"## To initialize HTTP Server..."<<endl;
	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED,
			request_completed, NULL, MHD_OPTION_END);
	if (NULL == daemon)
		return 1;
	cout<<"## The initialization is done!"<<endl;
	getchar();
	MHD_stop_daemon(daemon);
	return 0;
}
