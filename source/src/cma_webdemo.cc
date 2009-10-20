#include <platform.h>
#include <microhttpd.h>
#include <iostream>

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

struct file_connection_info_struct {
	int connectiontype;
	struct MHD_PostProcessor *postprocessor;
	FILE *fp;
	const char *answerstring;
	int answercode;
};

struct text_connection_info_struct {
	int connectiontype;
	char *answerstring;
	struct MHD_PostProcessor *postprocessor;
};

const char
		*askpage =
				"<html><body>\n\
                       Upload a file, please!<br>\n\
                       <form action=\"/filepost\" method=\"post\" enctype=\"multipart/form-data\">\n\
                       <input name=\"file\" type=\"file\">\n\
                       <input type=\"submit\" value=\" Send \"></form><br>\n\
						What’s your Favorite, Sir?<br>\
                       <form action=\"/textpost\" method=\"post\">\
                       <input name=\"textinput\" type=\"text\"\
                       <input type=\"submit\" value=\" Send \"></form>\
                       </body></html>";

const char *completepage =
		"<html><body>The upload has been completed.</body></html>";

const char *errorpage =
		"<html><body>This doesn’t seem to be right.</body></html>";

const char *servererrorpage =
		"<html><body>An internal server error has occured.</body></html>";

const char *fileexistspage =
		"<html><body>This file already exists.</body></html>";


const char *greatingpage =
		"<html><body><h1>Hello you like %s!</h1><br><br></body></html>";

/** Special Webpage */
const char *invalidurlpage =
		"<html><body>The Request URL <b>%s</b> is invalid. Back to <a href=\"/\">Home</a>.</body></html>";


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

	if (0 == strcmp(key, "textinput"))
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

		/*if( isFilePost )
		{
			connection_info_struct *con_info;
			con_info = (connection_info_struct*)malloc(sizeof(connection_info_struct));
			if (NULL == con_info)
				return MHD_NO;
			if (0 == strcmp(method, "POST"))
			{
				con_info->postprocessor = MHD_create_post_processor(connection,
						POSTBUFFERSIZE, iterate_post, (void *) con_info);
				if (NULL == con_info->postprocessor)
				{
					free(con_info);
					return MHD_NO;
				}
				con_info->connectiontype = POST;
				con_info->userinput = completepage;
			}
			else
				con_info->connectiontype = GET;
			*con_cls = (void *) con_info;
			return MHD_YES;
		}

		else if( isTextPost )
		{
			struct text_connection_info_struct *con_info;
			con_info = (text_connection_info_struct*)malloc(sizeof(struct text_connection_info_struct));
			if (NULL == con_info)
				return MHD_NO;
			con_info->answerstring = NULL;
			if (0 == strcmp(method, "POST")) {
				con_info->postprocessor = MHD_create_post_processor(connection,
						POSTBUFFERSIZE, iterate_post, (void *) con_info);
				if (NULL == con_info->postprocessor) {
					free(con_info);
					return MHD_NO;
				}
				con_info->connectiontype = POST;
			} else
				con_info->connectiontype = GET;
			*con_cls = (void *) con_info;
			return MHD_YES;
		}

		return MHD_YES;*/
	}

	if (0 == strcmp(method, "GET"))
	{
		//int ret;
		//char buffer[1024] = { 0 };
		//sprintf(buffer, askpage, nr_of_uploading_clients);
		return send_page(connection, askpage, MHD_HTTP_OK);
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
			return send_page(connection, con_info->userinput.c_str(), MHD_HTTP_OK);
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
