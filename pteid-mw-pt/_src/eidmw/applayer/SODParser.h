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

namespace eIDMW {

class SODAttributes;

class SODParser {
public:
	virtual ~SODParser();
	void ParseSodEncapsulatedContent(const CByteArray & contents);
	SODAttributes &getHashes();

private:
	SODAttributes *attr;
};

class SODAttributes {
public:
	CByteArray hashes[4];
};

} /* namespace eIDMW */
#endif /* SODPARSER_H_ */
