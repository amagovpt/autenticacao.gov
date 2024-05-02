/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "AttributeFactory.h"

namespace eIDMW {
CitizenAttribute uriToAttribute(std::string uri) {
	if (uri == ATTR_URI_NIC)
		return NIC;
	else if (uri == ATTR_URI_NAME)
		return NAME;
	else if (uri == ATTR_URI_SURNAME)
		return SURNAME;
	else if (uri == ATTR_URI_COMPLETENAME)
		return COMPLETENAME;
	else if (uri == ATTR_URI_DOCTYPE)
		return DOCTYPE;
	else if (uri == ATTR_URI_DOCNATIONALITY)
		return DOCNATIONALITY;
	else if (uri == ATTR_URI_DOCNUMBER)
		return DOCNUMBER;
	return NONE;
}

std::string attributeToUri(CitizenAttribute attribute) {
	switch (attribute) {
	case NIC:
		return ATTR_URI_NIC;
	case NAME:
		return ATTR_URI_NAME;
	case SURNAME:
		return ATTR_URI_SURNAME;
	case COMPLETENAME:
		return ATTR_URI_COMPLETENAME;
	case DOCTYPE:
		return ATTR_URI_DOCTYPE;
	case DOCNATIONALITY:
		return ATTR_URI_DOCNATIONALITY;
	case DOCNUMBER:
		return ATTR_URI_DOCNUMBER;
	default:
		return "";
	}
}
} // namespace eIDMW