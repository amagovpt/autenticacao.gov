/*
 * sslclient.h
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

#ifndef SSLCLIENT_H_
#define SSLCLIENT_H_

int tcp_connect(char *host,int port);
void check_cert(SSL *ssl,char *host);

#endif /* SSLCLIENT_H_ */
