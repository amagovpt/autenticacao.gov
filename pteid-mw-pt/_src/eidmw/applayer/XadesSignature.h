/*-****************************************************************************

 * Copyright (C) 2012-2014, 2016-2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2013 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef XADESSIG_H
#define XADESSIG_H

#include <functional>
#include <vector>

#include "Export.h"

typedef struct zip zip_t;

namespace eIDMW
{
	class CByteArray;
	class APL_Card;
	class APL_Certifs;

	class XadesSignature
	{
	public:
		EIDMW_APL_API XadesSignature(APL_Card *card) : m_pcard(card){};
		EIDMW_APL_API XadesSignature(APL_Certifs *certs, std::function<CByteArray(const CByteArray&)> callback)
			: m_cmdCertificates(certs), m_signCallback(callback){};

		EIDMW_APL_API CByteArray &signXades(const char **paths, unsigned int pathCount);
		EIDMW_APL_API void signASiC(const char *path);

		void enableTimestamp() { m_doTimestamp = true; };
		void enableLongTermValidation() { m_doLTV = true; };

		/* Return true if some timestamp(/archival timestamp) was not applied in the last signature. */
		bool shouldThrowTimestampException() { return m_throwTimestampException; };
		bool shouldThrowLTVException() { return m_throwLTVException; };

	private:
		CByteArray &sign(const char **paths, unsigned int pathCount, zip_t *container = NULL);

		APL_Card *m_pcard = NULL;
		APL_Certifs *m_cmdCertificates = NULL;
		std::function<CByteArray(const CByteArray&)> m_signCallback;

		bool m_doTimestamp = false;
		bool m_doLTV = false;
		bool m_throwTimestampException = false;
		bool m_throwLTVException = false;

		std::vector<CByteArray> m_certs;
	};
}

#endif

