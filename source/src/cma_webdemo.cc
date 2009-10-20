#include <platform.h>
#include <microhttpd.h>
#include <iostream>
#include <stdio.h>
#include <string>

#define DEBUG_HTTP

#define PORT            9999
#define POSTBUFFERSIZE  512
#define GET             0
#define POST            1

using namespace std;

class connection_info_struct {
public:
	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	string userinput;
	int answercode;
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
	.userInput {\n\
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
</style>	\n\
</head>\n\
\n\
<body>\n\
<div id=\"container\">\n\
	<div id=\"banner\">iCMA's Web Demo</div>\n\
	\n\
	<div id=\"content\">\n\
		<div id=\"input\">\n\
			<form method=\"post\" action=\"/textpost\">\n\
			<span class=\"prompt\">Please input the Chinese Text:</span>\n\
			<textarea  rows=\"15\" wrap=\"soft\" name=\"userInput\" class=\"userInput\">%s</textarea>\n\
			<div id=\"inputbtns\">\n\
				<input type=\"submit\" class=\"inputbtn\" value=\"Segment\"/>\n\
				<input type=\"reset\" class=\"inputbtn\" value=\"Clear\"/>\n\
			</div>\n\
			</form>\n\
			<span class=\"prompt\"></span>\n\
			<span class=\"prompt\">Or upload a file with UTF-8 encoding:</span>\n\
			\n\
			<div id=\"uploadfilediv\">\n\
				<form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\">\n\
                       <input name=\"file\" type=\"file\" size=\"35px\">\n\
                       <input type=\"submit\" value=\" Upload \" class=\"inputbtn\">\n\
				</form>			\n\
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

int send_segment_page(struct MHD_Connection *connection, string& userinput,
		int status_code)
{
	int ret;
	struct MHD_Response *response;

	string result;
	if( !userinput.empty() )
	{
		//TODO perform segmentation
		result = userinput;
	}
	int bufLen = askpage.length() + userinput.length() + result.length();
	char *buf = new char[ bufLen ];
	memset( buf, 0x0, bufLen );
	int writeBytes = sprintf( buf, askpage.c_str(), userinput.c_str(), result.c_str() );
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
	send_page( connection, page.c_str(), status_code );
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

	connection_info_struct *con_info =
			(connection_info_struct *) coninfo_cls;

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

	connection_info_struct *con_info = (connection_info_struct *) *con_cls;

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
		connection_info_struct *con_info = new connection_info_struct;

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
		return send_segment_page(connection, EMPTY_STR, MHD_HTTP_OK);
	}

	if (0 == strcmp(method, "POST"))
	{
		connection_info_struct *con_info = (connection_info_struct*)(*con_cls);
		if (0 != *upload_data_size)
		{
			MHD_post_process(con_info->postprocessor, upload_data, *upload_data_size);
			*upload_data_size = 0;
			return MHD_YES;
		}
		else
			return send_segment_page(connection, con_info->userinput, MHD_HTTP_OK);
	}

	return send_page(connection, errorpage, MHD_HTTP_BAD_REQUEST);
}

int main()
{
	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_NOTIFY_COMPLETED,
			request_completed, NULL, MHD_OPTION_END);
	if (NULL == daemon)
		return 1;
	getchar();
	MHD_stop_daemon(daemon);
	return 0;
}
