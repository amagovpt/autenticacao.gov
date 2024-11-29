/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 * SODParser.h
 *
 *  Created on: Feb 14, 2012
 *      Author: ruim
 */

#ifndef SODPARSER_H_
#define SODPARSER_H_
#include "ByteArray.h"
#include "MWException.h"
#include <openssl/types.h>
#include <vector>
#include <unordered_map>

namespace eIDMW {

class SODAttributes;

class SODParser {
public:
	SODParser() { attr = NULL; };
	virtual ~SODParser();
	void ParseSodEncapsulatedContent(const CByteArray &contents, const std::vector<int> &valid_tags);
	SODAttributes &getAttributes();

private:
	SODAttributes *attr;
};

class SODAttributes {
public:
	void add(unsigned short tag, CByteArray value);
	void setHashFunction(const EVP_MD *hash_md);
	const EVP_MD *getHashFunction();
	bool validateHash(unsigned short tag, const CByteArray &data);
	const CByteArray &get(unsigned short tag);
	const std::unordered_map<unsigned short, CByteArray> &getHashes();

private:
	std::unordered_map<unsigned short, CByteArray> m_hashes;
	const EVP_MD *hash_md;
};

} /* namespace eIDMW */
#endif /* SODPARSER_H_ */
