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
		std::string host_and_port = std::string(host) +":"+ port;

		/* initialise the OpenSSL library */
    	init_openssl();
    	m_host = host;

    	MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection: connecting to SCAP server: %s", host_and_port.c_str());

    	connect_encrypted((char *)host_and_port.c_str());

    	if (m_ssl_connection == NULL)
        {
        	MWLOG(LEV_ERROR, MOD_APL, "Error connecting to SCAP SSL Server!", host_and_port.c_str());
    	}
            
	}

	#define HEADERS_BUFSIZE 1000

	char * ScapSSLConnection::postSoapRequest(char *endpoint, char *soapAction, char *soapBody)
	{
		int ret_channel = 0;
		char * request_template= "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/soap+xml; charset=utf-8\r\nSOAPAction: %s\r\nContent-Length: %d\r\n\r\n";
		char * server_response = (char *) malloc(SCAP_REPLY_BUFSIZE);
		char request_headers[HEADERS_BUFSIZE];

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

		NetworkBuffer buffer;
		buffer.buf = server_response;
		buffer.buf_size = SCAP_REPLY_BUFSIZE;

		//Read response
		int bytes_read = read_from_stream(m_ssl_connection, &buffer);

		//Hack for chunked replies
		if (strstr(buffer.buf, "Transfer-Encoding: chunked") != NULL)
		{
			MWLOG(LEV_DEBUG, MOD_APL, "ScapSSLConnection: server response is chunked, trying read_chunked_reply()");
			read_chunked_reply(m_ssl_connection, &buffer, true);

		}

		MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: Server reply (size=%d): \n%s\n", bytes_read, buffer.buf);

		return buffer.buf;
	}

}
