/*
 * hellobrowser.c
 *
 *  Created on: Oct 16, 2009
 *      Author: vernkin
 */
#include <platform.h>
#include <microhttpd.h>
#include <iostream>
#define PORT 8888

using namespace std;

int answer_to_connection (void *cls, struct MHD_Connection *connection, const char *url,
                          const char *method, const char *version, const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
  const char *page = "<html><body>The MicroHttpd Works!</body></html>";

  struct MHD_Response *response;
  int ret;
  response = MHD_create_response_from_data (strlen (page),
                                            (void*) page, MHD_NO, MHD_NO);

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);
  return ret;
}

int main ()
{
  struct MHD_Daemon *daemon;
  daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                             &answer_to_connection, NULL, MHD_OPTION_END);
  if (NULL == daemon) return 1;

  cout<<"Type http://localhost:8888 in your web browser. If open, this test pass."<<endl;
  cout<<"Use Ctrl-C to end this process after testing."<<endl;
  getchar ();
  MHD_stop_daemon (daemon);
  return 0;

}
