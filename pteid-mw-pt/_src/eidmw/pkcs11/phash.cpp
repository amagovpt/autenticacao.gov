/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include "ByteArray.h"
#include "eidErrors.h"
#include "Hash.h"
#include "pteid_p11.h"
#include "phash.h"

using namespace eIDMW;

#define WHERE "hash_init()"
int hash_init(CK_MECHANISM_PTR pMechanism, void **pphashinfo, unsigned int *size)
{
   int ret = CKR_OK;
   CHash *oHash = new CHash();
   tHashAlgo algo;

   switch(pMechanism->mechanism)
      {
      case CKM_MD5:
      case CKM_MD5_RSA_PKCS:           algo = ALGO_MD5;        *size=16; break;
      case CKM_SHA_1:
      case CKM_SHA1_RSA_PKCS:          algo = ALGO_SHA1;       *size=20; break;
      case CKM_SHA256:
      case CKM_SHA256_RSA_PKCS:        algo = ALGO_SHA256;     *size=32; break;
      case CKM_SHA384:
      case CKM_SHA384_RSA_PKCS:        algo = ALGO_SHA384;     *size=48; break;
      case CKM_SHA512:
      case CKM_SHA512_RSA_PKCS:        algo = ALGO_SHA512;     *size=64; break;
      case CKM_RIPEMD160:
      case CKM_RIPEMD160_RSA_PKCS:     algo = ALGO_RIPEMD160;  *size=20; break;
      default:
         ret = CKR_MECHANISM_INVALID;
         goto cleanup;
      }

   oHash->Init(algo);

   *pphashinfo = oHash;
   return ret;

cleanup:
   delete oHash;
   return ret;
}
#undef WHERE



#define WHERE "hash_update()"
int hash_update(void *phashinfo, char *p, unsigned long l)
{
int ret = CKR_OK;
CHash *oHash = (CHash *) phashinfo;
CByteArray data((unsigned char*)p,l);

oHash->Update(data);

return (ret);
}
#undef WHERE


#define WHERE "hash_final()"
int hash_final(void *phashinfo, unsigned char *p, unsigned long *l)
{
int ret = CKR_OK;

if (phashinfo == NULL)
   return(CKR_FUNCTION_FAILED);

CHash *oHash = (CHash *) phashinfo;
CByteArray data;

data = oHash->GetHash();

memcpy(p, data.GetBytes(), data.Size());

*l = data.Size();

delete(oHash);

return (ret);
}
#undef WHERE
