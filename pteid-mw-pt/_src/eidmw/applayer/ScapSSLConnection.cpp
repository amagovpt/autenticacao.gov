/*-****************************************************************************

 * Copyright (C) 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <string>

#include "ScapSSLConnection.h"
#include "Log.h"


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

	char * get_status_line(char * full_http_response) {
		char * ptr = full_http_response;
		size_t first_line_len = 0;
		while(*ptr && *ptr != '\r' && *ptr != '\n') {
			first_line_len++;
			ptr++;
		}

		if (first_line_len == 0 || strlen(full_http_response)== first_line_len) {
			return full_http_response;
		}

		char * status_line = (char *) calloc(first_line_len+1, sizeof(char));
		memcpy(status_line, full_http_response, first_line_len);

		return status_line;
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

		MWLOG(LEV_DEBUG, MOD_APL, "Server reply (size=%d): \n", bytes_read);
		if (bytes_read > 0) {
			char * http_status = get_status_line(buffer.buf);
			MWLOG(LEV_DEBUG, MOD_APL, "Server returned HTTP status: %s", http_status);
			if (http_status != buffer.buf) {
				free(http_status);
			}
		}

		return buffer.buf;
	}

}
