

#include "wsman-client-api.h"

static char *endpoint = NULL;

int main(int argc, char** argv)
{
	int		sid, sid1;
	wsman_data_t	*data;
	char		*buf;
	int		size;
	char 		retval = 0;
	u_error_t 	*error = NULL;
	u_uri_t		*uri;


	u_option_entry_t opt[] = {
	{ "endpoint",	'u',	U_OPTION_ARG_STRING,	&endpoint,
		"Endpoint in form of a URL", "<uri>" },
	{ NULL }
	};


	u_option_context_t *opt_ctx;	
	opt_ctx = u_option_context_new("");
	u_option_context_set_ignore_unknown_options(opt_ctx, FALSE);
	u_option_context_add_main_entries(opt_ctx, opt, "adv api example");
	retval = u_option_context_parse(opt_ctx, &argc, &argv, &error);
	u_option_context_free(opt_ctx);

	if (error) {
		if (error->message)
		printf ("%s\n", error->message);
		u_error_free(error);
		return 1;
	}
	u_error_free(error);

	if (endpoint) {
		u_uri_parse((const char *)endpoint, &uri);
	}
	if (!endpoint || !uri) {
		fprintf(stderr, "endpoint option required\n");
		return 1;
	}

	sid = wsman_session_open(uri->host, uri->port, uri->path, uri->scheme,
				uri->user, uri->pwd);

	if (sid < 0) {
		printf("Open session failed\n");
		return 0;
	}

	sid1 = wsman_session_open(uri->host, uri->port, uri->path, uri->scheme,
				uri->user, uri->pwd);

	if (sid < 0) {
		printf("Open session failed\n");
		wsman_session_close(sid);
		return 0;
	}

	printf("\n******** Opened session id %d ********\n\n", sid);
	printf("******** Opened session id %d ********\n\n", sid1);

	data = wsman_session_do_action(sid, WSMAN_ACTION_IDENTIFY);

	if (!data->response) {
		printf("******** Identify failed - %s ********\n",
			data->fault_message);
		return 0;
	}
	ws_xml_dump_memory_node_tree(ws_xml_get_doc_root(data->response),
					&buf, &size);

	printf ("******** Identify response (id %d) ********\n%s\n", sid, buf);

	wsman_session_close(sid);

	printf("******** Closed session id %d ********\n\n", sid);

	data = wsman_session_do_action(sid1, WSMAN_ACTION_ENUMERATION);

	if (!data->response) {
		printf("******** Enumeration failed - %s ********\n",
			data->fault_message);
		return 0;
	}
	ws_xml_dump_memory_node_tree(ws_xml_get_doc_root(data->response),
					&buf, &size);
	printf("******** Enumeration response (id %d) ********\n%s\n", sid1,buf);

	wsman_session_close(sid1);

	printf("******** Closed session id %d ********\n\n", sid1);

	return 1;
}

