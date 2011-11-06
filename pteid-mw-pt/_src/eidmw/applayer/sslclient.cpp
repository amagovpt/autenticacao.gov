/*
 * sslclient.cpp
 *
 *  Created on: 2011/10/25
 *      Author: Luis Medinas <luis.medinas@caixamagica.pt>
 *      based on Eric Rescorla OpenSSL Example Programs 20020110
 */

/*
 *  pteid-mw-ng is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  pteid-mw-ng is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include "sslcommon.h"
#include "sslclient.h"

int tcp_connect(char *host,int port)
{
	struct hostent *hp;
	struct sockaddr_in addr;
	int sock;

	if(!(hp=gethostbyname(host)))
		berr_exit((char *)"Couldn't resolve host");
	memset(&addr,0,sizeof(addr));
	addr.sin_addr=*(struct in_addr*)
    		  hp->h_addr_list[0];
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);

	if((sock=socket(AF_INET,SOCK_STREAM,
			IPPROTO_TCP))<0)
		err_exit((char *)"Couldn't create socket");
	if(connect(sock,(struct sockaddr *)&addr,
			sizeof(addr))<0)
		err_exit((char *)"Couldn't connect socket");

	return sock;
}

/* Check that the common name matches the
   host name*/
void check_cert(SSL *ssl, char *host)
{
	X509 *peer;
	char peer_CN[256];

	if(SSL_get_verify_result(ssl)!=X509_V_OK)
		berr_exit((char *)"Certificate doesn't verify");

	/*Check the cert chain. The chain length
      is automatically checked by OpenSSL when
      we set the verify depth in the ctx */

	/*Check the common name*/
	peer=SSL_get_peer_certificate(ssl);
	X509_NAME_get_text_by_NID
	(X509_get_subject_name(peer),
			NID_commonName, peer_CN, 256);
	if(strcasecmp(peer_CN,host))
		err_exit ((char *)"Common name doesn't match host name");
}
