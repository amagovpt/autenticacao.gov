#include "MD5Sum.h"
#include <string.h>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;


MD5Sum::MD5Sum()	
{
	md5_init_ctx(&m_context);
}

MD5Sum::~MD5Sum()
{
}

void MD5Sum::add_stream(FILE* stream)
{
	md5_stream(stream,m_sum);
}

void MD5Sum::add_file(const std::string path)
{
	FILE* file=fopen(path.c_str(),"r");
	if(file!=NULL)
		add_stream(file);
	fclose(file);
}

std::wstring MD5Sum::get_sum(void) const
{
	std::wstringstream sumstream;
	for(int i=0;i<16;i++)
		sumstream << setw(2) << setfill(L'0') << setbase(16) << m_sum[i];
	return sumstream.str();
}