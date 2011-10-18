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
#ifdef _WIN32
  #include <windows.h>
#else
  #define MAX_PATH 260
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h> 
#include <time.h>

#include "p11.h"
#include "log.h"
#include "util.h"

/******************************************************************************
 *
 * Macros
 *
 ******************************************************************************/
//#define CLEANUP(x) { err = (x); goto cleanup; }

#ifndef MAX_PATH
#define MAX_PATH 128
#endif

#ifndef WIN32
#define strcpy_s(a,b,c)         strcpy((a),(c))
#define sprintf_s(a,b,c,d,e,f,g,h,i)    sprintf((a),(c),(d),(e),(f),(g),(h),(i))
#endif

/******************************************************************************
 *
 * Globals
 *
 ******************************************************************************/
#ifdef DEBUG
unsigned int g_uiLogLevel          = LOG_LEVEL_SPY;
#else
unsigned int g_uiLogLevel          = LOG_LEVEL_WARNING;
#endif

void *logmutex = NULL;
char g_szLogFile[MAX_PATH];

/******************************************************************************
 *
 * log_init
 *
 ******************************************************************************/
void log_init(char *pszLogFile, unsigned int uiLogLevel)
{
  FILE          *fp = NULL;

  util_init_lock(&logmutex);
  util_lock(logmutex);

  strcpy_s(g_szLogFile,sizeof(g_szLogFile), pszLogFile);
  g_uiLogLevel = uiLogLevel;

  //this will empty the logfile automatically
#ifdef WIN32
  if ((fopen_s(&fp,g_szLogFile, "a")) == 0)
#else
  if ((fp = fopen(g_szLogFile, "w")) != NULL)
#endif
     fclose(fp);

  util_unlock(logmutex);
}

/******************************************************************************
 *
 * log_trace
 *
 ******************************************************************************/
void log_trace(const char *where, const char *string,... )
{
  int           ret;
  static char   buf[0x4000];    
  va_list       args;
  FILE          *fp = NULL;
  time_t        ltime;  
  struct tm     stime;
  char          asctime[21];
  unsigned int  level = g_uiLogLevel & 0x0F;

   // evaluate debug level
  if (string[1] == ':')
  {
    switch (string[0])
    {
      case 'I':
        if (level  < LOG_LEVEL_INFO) 
          return;
        break;

      case 'S':
        if (level  < LOG_LEVEL_SPY) 
          return;
        break;

      case 'W':
        if (level < LOG_LEVEL_WARNING) 
          return;
        break;

      case 'E':
        break;

      default:
        return;
    }

    //string+=2;
  }
  else
  {
#ifndef DEBUG            
   // return;    => restore
#endif
  }

  util_lock(logmutex);

#ifdef WIN32
  if ((fopen_s(&fp, g_szLogFile, "a")) != 0)
#else
  if ((fp = fopen(g_szLogFile, "a")) == NULL)
#endif
    CLEANUP(0);
  
   va_start(args, string);                                       // get args from param-string     
#ifdef WIN32
    _vsnprintf_s(buf, sizeof(buf), sizeof(buf), string, args);     // convert to string    
#else
  vsnprintf(buf, sizeof(buf), string, args);    // convert to string    
#endif
   va_end(args);                                                               // free arguments

#ifdef DEBUG
  printf("%s %s\n", where, buf);
#endif

  time(&ltime);
#ifdef WIN32
	localtime_s(&stime, &ltime );
#else
	stime = *(localtime(&ltime ));
#endif
  
  sprintf_s(asctime,sizeof(asctime), "%02d.%02d.%04d %02d:%02d:%02d", 
                    stime.tm_mday,
                    stime.tm_mon+1,
                    stime.tm_year+1900,
                    stime.tm_hour,
                    stime.tm_min,
                    stime.tm_sec); 
#ifdef WIN32
  fprintf(fp, "%d %d %19s %-26s | %s\n",GetCurrentProcessId(), GetCurrentThreadId(), asctime, where, buf);
#else
  fprintf(fp, "%19s %-26s | %s\n", asctime, where, buf);
#endif
  fclose(fp);

cleanup:
  util_unlock(logmutex);
  return;
}

/******************************************************************************
 *
 * log_xtrace
 *
 ******************************************************************************/
void log_xtrace(const char *where, char *string,void *data,int len)
{
  static  char  hex[]="0123456789abcdef";
  int           a;
  char          *x;
  char          *dt;
  int           adr=0;
  char          buff1[40];
  char          buff2[20];  

  FILE          *fp = NULL;
  time_t        ltime;  
  struct tm     stime;
  char          asctime[21];
  unsigned int  level = g_uiLogLevel & 0x0F;
  
  // evaluate debug level
  if (string != NULL && string[1] == ':')
  {
    switch (string[0])
    {
      case 'I':
        if (level  < LOG_LEVEL_INFO) 
          return;
        break;

      case 'W':
        if (level < LOG_LEVEL_WARNING) 
          return;
        break;

      case 'E':
        break;

      default:        
        return;
    }

    string += 2;
  }
  else
  {
#ifndef DEBUG            
   // return;
#endif
  }
  
  util_lock(logmutex);
#ifdef DEBUG
  _log_xtrace(string, data, len);
#endif

#ifdef WIN32
  if ((fopen_s(&fp,g_szLogFile, "a")) != 0)
#else
  if ((fp = fopen(g_szLogFile, "a")) == NULL)
#endif
    goto cleanup;
        
   time(&ltime);
#ifdef WIN32
	localtime_s(&stime, &ltime );
#else
	stime = *(localtime(&ltime ));
#endif
   
  sprintf_s(asctime,sizeof(asctime), "%02d.%02d.%04d %02d:%02d:%02d", 
                    stime.tm_mday,
                    stime.tm_mon+1,
                    stime.tm_year+1900,
                    stime.tm_hour,
                    stime.tm_min,
                    stime.tm_sec); 
  
if (where)
   {
   if(string != NULL) 
      fprintf(fp, "%19s | %-26s | %s\n", asctime, where, string);
   else
      fprintf(fp, "%19s | %-26s | \n", asctime, where);    
   }
else
   {
   if(string != NULL) 
      fprintf(fp, "%s\n", string);
   }

  dt=(char *)data;
  
  while(len>0) 
      {
      x=buff1;
      for(a=0; a<16 && a<len; a++) 
        {
        if((a&3)==0) *x++ = ' ';
        if((a&7)==0) *x++ = ' ';
        *x++ = hex[(dt[a]>>4)&15];
        *x++ = hex[dt[a]&15];
        }
      *x=0;

      x=buff2;
      for(a=0; a<16 && a<len; a++) 
      {
      *x++ = (dt[a]>' ' && dt[a]<0x7f) ? dt[a] : ' ';
      }
      *x=0;

      fprintf(fp, "%-6x | %-38s |%-16s\n", adr, buff1, buff2);
      len-=16;
      dt+=16;
      adr+=16;
      }

  fclose(fp);

cleanup:
  util_unlock(logmutex);
  return;
}


/******************************************************************************
 *
 * _log_xtrace
 *
 ******************************************************************************/
void _log_xtrace(char *text, void *data, int l_data)
{
  int     a;
  char    *x;
  char    *dt;
  int     adr=0;
  char    buff1[40];
  char    buff2[20];
  static char hex[]="0123456789abcdef";

//  if(text) 
//    printf("%s:\n",text);

  dt=(char *)data;

  while(l_data>0) 
   {
   x=buff1;
   for(a=0;a<16 && a<l_data;a++) 
     {
     if((a&3)==0) *x++ = ' ';
     if((a&7)==0) *x++ = ' ';
     *x++ = hex[(dt[a]>>4)&15];
     *x++ = hex[dt[a]&15];
     }
   *x=0;
          
   x=buff2;
   for(a=0;a<16 && a<l_data;a++) 
     {
     *x++ = (dt[a]>' ' && dt[a]<0x7f) ? dt[a] : ' ';
     }
   *x=0;

//   printf(" %6x | %-38s | %-16s\n",adr,buff1,buff2);
   l_data-=16;
   dt+=16;
   adr+=16;
   }
}

void log_template(const char *string, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG count)
{
unsigned int  level = g_uiLogLevel & 0x0F;

  // evaluate debug level
  if (string[1] == ':')
  {
    switch (string[0])
      {
      case 'I':
        if (level  < LOG_LEVEL_INFO) 
          return;
        break;

      case 'S':
        if (level  < LOG_LEVEL_SPY) 
          return;
        break;

      case 'W':
        if (level < LOG_LEVEL_WARNING) 
          return;
        break;

      case 'E':
        break;

      default:
        return;
      }
  }
  //else
  //   return;

//log template only in INFO mode
//  if (level  < LOG_LEVEL_INFO) 
//     return;

  log_trace(string, "size = %d", count);

  if ((pTemplate == NULL) || (count == 0))
     return;

  while(count--)
     {
     log_attr(pTemplate++);
     }
}

void map_log_info(CK_ULONG type, const char **attrtype, int *logtype)
{
const P11_MAP_TYPE *p = &P11_ATTR_TYPES[0];

while (p->ctype)
   {
   if (p->ultype == type)
      {
      *attrtype = p->ctype;
      *logtype = p->logtype;
      return;
      }
   p++;
   }
*attrtype = NULL;
}



const char* get_type_string(CK_ULONG type, CK_ULONG subtype)
{
   const P11_MAP_TYPE *map = NULL;
   const P11_MAP_TYPE *p = NULL;
   static char stype[100];

   switch (type)
      {
      case CKA_CLASS:
         map = &P11_CLASS_TYPES[0];
         break;
      case CKA_CERTIFICATE_TYPE:
         map = &P11_CERTIFICATE_TYPES[0];
         break;
      case CKA_KEY_TYPE:
         map = &P11_KEY_TYPES[0];
         break;
      default:
         //convert to string
#ifdef WIN32
         sprintf_s(stype,sizeof(stype), "??? (%08X, %08X)", type, subtype);
#else
         sprintf(stype, "??? (%08lX, %08lX)", type, subtype);
#endif
         return (stype);
//       return (NULL);
      }

   p = map;
   while (p->ctype)
      {
      if (subtype == p->ultype)
         return (p->ctype);
      p++;
      }

   //convert to string
#ifdef WIN32
   sprintf_s(stype,sizeof(stype), "??? (%08X)", subtype);
#else
   sprintf(stype, "??? (%08lX)", subtype);
#endif
   return (stype);
}


void log_attr(CK_ATTRIBUTE_PTR pAttr)
{
  const char *ctype = NULL;
  int       logtype = 0;
  FILE      *fp = NULL;
  char      string[129];
  const char *s;
  int       len = 0;
  CK_ULONG  ul = 0;
  CK_BBOOL	b;

  if (pAttr == NULL)
     return;

  util_lock(logmutex);

#ifdef WIN32
  if ((fopen_s(&fp,g_szLogFile, "a")) != 0)
#else
  if ((fp = fopen(g_szLogFile, "a")) == NULL)
#endif
     goto cleanup;

  map_log_info(pAttr->type, &ctype, &logtype);

  //log attribute type
  if (ctype)
     fprintf(fp, "\nAttribute type : %s\n", ctype); 
  else
     fprintf(fp, "\nAttribute type : ??? (0x%0lx)\n", pAttr->type); 

  //log value
  if (pAttr->pValue == NULL)
     {
     fprintf(fp, "Attribute Value: NULL\n");
     goto cleanup;
     }

  switch (logtype)
     {
     case T_TYPE:
        if (pAttr->ulValueLen != sizeof(CK_ULONG))
           {
           fprintf(fp, "Attribute Value: INVALID size for Value (CK_ULONG)\n)");
           break;
           }
        memcpy(&ul, (CK_ULONG*) pAttr->pValue, sizeof(CK_ULONG));
        s = get_type_string(pAttr->type, ul);
        fprintf(fp, "Attribute Value: %s\n", s);
        break;

     case T_BOOL:
        if (pAttr->ulValueLen != sizeof(CK_BBOOL))
           {
           fprintf(fp, "Attribute Value: INVALID size for Value (CK_BBOOL)\n)");
           break;
           }
        memcpy(&b, pAttr->pValue, sizeof(CK_BBOOL));
        fprintf(fp, b == CK_TRUE ? "Attribute Value: TRUE\n":"Value: FALSE\n"); 
        break;

     case T_STRING:
        len = pAttr->ulValueLen <= 128 ? pAttr->ulValueLen:128;
        memcpy(string, pAttr->pValue, len);
        string[len]=0;
        fprintf(fp, "Attribute Value: %s\n", string); 
        break;

     case T_UL:
        if (pAttr->ulValueLen != sizeof(CK_ULONG))
           {
           fprintf(fp, "Attribute Value: INVALID size for CK_ULONG\n)");
           break;
           }
         
        memcpy(&ul, (CK_ULONG*) pAttr->pValue, sizeof(CK_ULONG));
        fprintf(fp, "Attribute Value: 0x%lx\n",  ul); 
        break;

     default: ;
        /* if length < sizeof (CK_ULONG) => print as ULONG */
        if (pAttr->ulValueLen <= sizeof(CK_ULONG))
           {
           memcpy(&ul, pAttr->pValue, pAttr->ulValueLen);
           fprintf(fp, "Attribute Value: 0x%lx\n",  ul); 
           }
        else
           {
           fclose(fp);
           fp = NULL;
           log_xtrace(0, "Attribute Value: ", pAttr->pValue, pAttr->ulValueLen);
           }
     }
 
cleanup:
  util_unlock(logmutex);
  if (fp)
     fclose(fp);
}



char* log_map_error(int err)
{
static char cerr[20];
switch(err)
   {
   case CKR_CANCEL:                    return("CKR_CANCEL"); break;
   case CKR_HOST_MEMORY:               return("CKR_HOST_MEMORY"); break;
   case CKR_SLOT_ID_INVALID:           return("CKR_SLOT_ID_INVALID"); break;
   case CKR_GENERAL_ERROR:             return("CKR_GENERAL_ERROR"); break;
   case CKR_FUNCTION_FAILED:           return("CKR_FUNCTION_FAILED"); break;
   case CKR_ARGUMENTS_BAD:             return("CKR_ARGUMENTS_BAD"); break;
   case CKR_NO_EVENT:                  return("CKR_NO_EVENT"); break;
// case CKR_NEED_TO_CREATE_THREADS:    return("CKR_NEED_TO_CREATE_THREADS"); break;
   case CKR_CANT_LOCK:                 return("CKR_CANT_LOCK"); break;
   case CKR_ATTRIBUTE_READ_ONLY:       return("CKR_ATTRIBUTE_READ_ONLY"); break;
   case CKR_ATTRIBUTE_SENSITIVE:       return("CKR_ATTRIBUTE_SENSITIVE"); break;
   case CKR_ATTRIBUTE_TYPE_INVALID:    return("CKR_ATTRIBUTE_TYPE_INVALID"); break;
   case CKR_ATTRIBUTE_VALUE_INVALID:   return("CKR_ATTRIBUTE_VALUE_INVALID"); break;
// case CKR_DATA_INVALID:              return("CKR_DATA_INVALID"); break;
// case CKR_DATA_LEN_RANGE:            return("CKR_DATA_LEN_RANGE"); break;
   case CKR_DEVICE_ERROR:              return("CKR_DEVICE_ERROR"); break;
   case CKR_DEVICE_MEMORY:             return("CKR_DEVICE_MEMORY"); break;
   case CKR_DEVICE_REMOVED:            return("CKR_DEVICE_REMOVED"); break;
// case CKR_ENCRYPTED_DATA_INVALID:    return("CKR_ENCRYPTED_DATA_INVALID"); break;
// case CKR_ENCRYPTED_DATA_LEN_RANGE:  return("CKR_ENCRYPTED_DATA_LEN_RANGE"); break;
   case CKR_FUNCTION_CANCELED:         return("CKR_FUNCTION_CANCELED"); break;
   case CKR_FUNCTION_NOT_PARALLEL:     return("CKR_FUNCTION_NOT_PARALLEL"); break;
   case CKR_FUNCTION_NOT_SUPPORTED:    return("CKR_FUNCTION_NOT_SUPPORTED"); break;
   case CKR_KEY_HANDLE_INVALID:        return("CKR_KEY_HANDLE_INVALID"); break;
   case CKR_KEY_SIZE_RANGE:            return("CKR_KEY_SIZE_RANGE"); break;
   case CKR_KEY_TYPE_INCONSISTENT:     return("CKR_KEY_TYPE_INCONSISTENT"); break;
// case CKR_KEY_NOT_NEEDED:            return("CKR_KEY_NOT_NEEDED"); break;
// case CKR_KEY_CHANGED:               return("CKR_KEY_CHANGED"); break;
   case CKR_KEY_NEEDED:                return("CKR_KEY_NEEDED"); break;
// case CKR_KEY_INDIGESTIBLE:          return("CKR_KEY_INDIGESTIBLE"); break;
   case CKR_KEY_FUNCTION_NOT_PERMITTED:return("CKR_KEY_FUNCTION_NOT_PERMITTED"); break;
// case CKR_KEY_NOT_WRAPPABLE:         return("CKR_KEY_NOT_WRAPPABLE"); break;
   case CKR_KEY_UNEXTRACTABLE:         return("CKR_KEY_UNEXTRACTABLE"); break;
   case CKR_MECHANISM_INVALID:         return("CKR_MECHANISM_INVALID"); break;
   case CKR_MECHANISM_PARAM_INVALID:   return("CKR_MECHANISM_PARAM_INVALID"); break;
   case CKR_OBJECT_HANDLE_INVALID:     return("CKR_OBJECT_HANDLE_INVALID"); break;
   case CKR_OPERATION_ACTIVE:          return("CKR_OPERATION_ACTIVE"); break;
   case CKR_OPERATION_NOT_INITIALIZED: return("CKR_OPERATION_NOT_INITIALIZED"); break;
   case CKR_PIN_INCORRECT:             return("CKR_PIN_INCORRECT"); break;
   case CKR_PIN_INVALID:               return("CKR_PIN_INVALID"); break;
   case CKR_PIN_LEN_RANGE:             return("CKR_PIN_LEN_RANGE"); break;
// case CKR_PIN_EXPIRED:               return("CKR_PIN_EXPIRED"); break;
   case CKR_PIN_LOCKED:                return("CKR_PIN_LOCKED"); break;
   case CKR_SESSION_CLOSED:            return("CKR_SESSION_CLOSED"); break;
   case CKR_SESSION_COUNT:             return("CKR_SESSION_COUNT"); break;
   case CKR_SESSION_HANDLE_INVALID:    return("CKR_SESSION_HANDLE_INVALID"); break;
   case CKR_SESSION_PARALLEL_NOT_SUPPORTED:return("CKR_SESSION_PARALLEL_NOT_SUPPORTED"); break;
   case CKR_SESSION_READ_ONLY:         return("CKR_SESSION_READ_ONLY"); break;
   case CKR_SESSION_EXISTS:            return("CKR_SESSION_EXISTS"); break;
   case CKR_SESSION_READ_ONLY_EXISTS:  return("CKR_SESSION_READ_ONLY_EXISTS"); break;
   case CKR_SESSION_READ_WRITE_SO_EXISTS: return("CKR_SESSION_READ_WRITE_SO_EXISTS"); break;
   case CKR_SIGNATURE_INVALID:          return("CKR_SIGNATURE_INVALID"); break;
   case CKR_SIGNATURE_LEN_RANGE:        return("CKR_SIGNATURE_LEN_RANGE"); break;
   case CKR_TEMPLATE_INCOMPLETE:        return("CKR_TEMPLATE_INCOMPLETE"); break;
   case CKR_TEMPLATE_INCONSISTENT:      return("CKR_TEMPLATE_INCONSISTENT"); break;
   case CKR_TOKEN_NOT_PRESENT:          return("CKR_TOKEN_NOT_PRESENT"); break;
   case CKR_TOKEN_NOT_RECOGNIZED:       return("CKR_TOKEN_NOT_RECOGNIZED"); break;
   case CKR_TOKEN_WRITE_PROTECTED :     return("CKR_TOKEN_WRITE_PROTECTED"); break;
//   case CKR_UNWRAPPING_KEY_HANDLE_INVALID: return("CKR_UNWRAPPING_KEY_HANDLE_INVALID"); break;
//   case CKR_UNWRAPPING_KEY_SIZE_RANGE:     return("CKR_UNWRAPPING_KEY_SIZE_RANGE"); break;
//   case CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT: return("CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT"); break;
   case CKR_USER_ALREADY_LOGGED_IN:     return("CKR_USER_ALREADY_LOGGED_IN"); break;
   case CKR_USER_NOT_LOGGED_IN:         return("CKR_USER_NOT_LOGGED_IN"); break;
   case CKR_USER_PIN_NOT_INITIALIZED:   return("CKR_USER_PIN_NOT_INITIALIZED"); break;
   case CKR_USER_TYPE_INVALID:          return(""); break;
   case CKR_USER_ANOTHER_ALREADY_LOGGED_IN: return("CKR_USER_ANOTHER_ALREADY_LOGGED_IN"); break;
//   case CKR_USER_TOO_MANY_TYPES:        return("CKR_USER_TOO_MANY_TYPES"); break;
//   case CKR_WRAPPED_KEY_INVALID:        return("CKR_WRAPPED_KEY_INVALID"); break;
//   case CKR_WRAPPED_KEY_LEN_RANGE:      return("CKR_WRAPPED_KEY_LEN_RANGE"); break;
//   case CKR_WRAPPING_KEY_HANDLE_INVALID: return("CKR_WRAPPING_KEY_HANDLE_INVALID"); break;
//   case CKR_WRAPPING_KEY_SIZE_RANGE:     return("CKR_WRAPPING_KEY_SIZE_RANGE"); break;
//   case CKR_WRAPPING_KEY_TYPE_INCONSISTENT: return("CKR_WRAPPING_KEY_TYPE_INCONSISTENT"); break;
//   case CKR_RANDOM_SEED_NOT_SUPPORTED:  return("CKR_RANDOM_SEED_NOT_SUPPORTED"); break;
//   case CKR_RANDOM_NO_RNG:              return("CKR_RANDOM_NO_RNG"); break;
//   case CKR_DOMAIN_PARAMS_INVALID:      return("CKR_DOMAIN_PARAMS_INVALID"); break;
   case CKR_BUFFER_TOO_SMALL:           return("CKR_BUFFER_TOO_SMALL"); break;
//   case CKR_SAVED_STATE_INVALID:        return(""); break;
   case CKR_INFORMATION_SENSITIVE:      return("CKR_INFORMATION_SENSITIVE"); break;
//   case CKR_STATE_UNSAVEABLE:           return("CKR_STATE_UNSAVEABLE"); break;
   case CKR_CRYPTOKI_NOT_INITIALIZED:   return("CKR_CRYPTOKI_NOT_INITIALIZED"); break;
   case CKR_CRYPTOKI_ALREADY_INITIALIZED: return("CKR_CRYPTOKI_ALREADY_INITIALIZED"); break;
   case CKR_MUTEX_BAD:                  return("CKR_MUTEX_BAD"); break;
   case CKR_MUTEX_NOT_LOCKED:           return("CKR_MUTEX_NOT_LOCKED"); break;
//   case CKR_FUNCTION_REJECTED:          return("CKR_FUNCTION_REJECTED"); break;
//   case CKR_VENDOR_DEFINED:             return("CKR_VENDOR_DEFINED"); break;
   default:
#ifdef WIN32
      sprintf_s(cerr,sizeof(cerr), "0x%0X", err);
#else
      sprintf(cerr, "0x%0X", err);
#endif
      return(cerr); break;
   }
}

