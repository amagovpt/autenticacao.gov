#ifndef __CACHE__H__
#define __CACHE__H__

extern BOOL	CacheCertificate(char * filename, PBYTE data, int data_len);
extern BOOL getCacheFilePath(char *filename_bin, char* Path);
extern BOOL readFromCache(const char *cache_path, char * Buf);


#endif