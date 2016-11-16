#include <string>

#include "ScapSSLConnection.h"
#include "Log.h"

#ifdef WIN32
#define snprintf _snprintf
#endif


namespace eIDMW
{


	ScapSSLConnection::ScapSSLConnection(char * host, char *port)
	{
		bool insecure = true;
		std::string host_and_port = std::string(host) +":"+ port;

		/* initialise the OpenSSL library */
    	init_openssl();
    	m_host = host;

    	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: connecting to SCAP server: %s", host_and_port.c_str());

    	if ((m_ssl_connection = connect_encrypted((char *)host_and_port.c_str(), insecure)) == NULL)
    	{
    		fprintf(stderr, "ScapSSLConnection: error returned by connect_encrypted!\n");
    	}
            
	}

	#define HEADERS_BUFSIZE 1000

	char * ScapSSLConnection::postSoapRequest(char *endpoint, char *soapAction, char *soapBody)
	{
		int ret_channel = 0;
		char * request_template= "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/soap+xml; charset=utf-8\r\nSOAPAction: %s\r\nContent-Length: %d\r\n\r\n";
		char * server_response = (char *) malloc(REPLY_BUFSIZE);
		char * request_headers = (char *) calloc(HEADERS_BUFSIZE, sizeof(char));

		std::string soapRequest = "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\">";
		soapRequest +=  "<soapenv:Body>";
		soapRequest += soapBody;
		soapRequest += "</soapenv:Body>";
		soapRequest += "</soapenv:Envelope>";

		//Build the full request
		snprintf (request_headers, HEADERS_BUFSIZE, request_template, endpoint, m_host, soapAction,
			soapRequest.size());

		//fprintf(stderr, "DEBUG: Sending POST Headers: \n%s\n", request_headers);
		//fprintf(stderr, "DEBUG: Sending POST body: \n%s\n", soapRequest.c_str());
		ret_channel = write_to_stream(m_ssl_connection, request_headers);

		ret_channel = write_to_stream(m_ssl_connection, (char *)soapRequest.c_str());

		//Read response
		int bytes_read = read_from_stream(m_ssl_connection, server_response, REPLY_BUFSIZE);

		//Hack for chunked replies
		if (strstr(server_response, "chunked") != NULL)
		{
			fprintf(stderr, "ScapSSLConnection: reply is chunked, trying read_chunked_reply()\n");
			read_chunked_reply(m_ssl_connection, server_response, REPLY_BUFSIZE, true);

		}

		fprintf(stderr, "DEBUG: Server reply (size=%d): \n%s\n", bytes_read, server_response);

		return server_response;
	}

}
