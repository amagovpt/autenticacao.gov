#ifndef __APL_PTEIDCERTS_H__
#define __APL_PTEIDCERTS_H__

namespace eIDMW
{


#pragma pack(push, certs, 1)

struct CERT_S
{
	unsigned char * cert_data;
	long cert_len;
};

#define CERT_S_ENTRY(certname) {&certname[0], sizeof(certname)}

#define CERTS_N 16

extern CERT_S PTEID_CERTS[];


#pragma pack(pop, certs)

}



#endif
