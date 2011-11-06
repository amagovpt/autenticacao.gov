#include "md5.h"
#include <string>

class MD5Sum
{
public:
	MD5Sum();
	~MD5Sum();
	void			add_stream(FILE* stream);
	void			add_file(const std::string path);
	void			get_sum(unsigned char* result) const;
	std::wstring	get_sum(void) const;
private:
	struct md5_ctx		m_context;
	unsigned char		m_sum[16];
};