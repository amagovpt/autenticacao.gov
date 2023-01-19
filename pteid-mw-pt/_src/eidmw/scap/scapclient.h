/*-****************************************************************************

 * Copyright (C) 2022-2023 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#pragma once

#include <unordered_map>

#include "eidlib.h"
#include "scaperrors.h"

namespace eIDMW {

struct ScapCredentials
{
	std::string basic_user;
	std::string basic_pass;
	std::string credential_id;
};

struct ScapProvider
{
	std::string uriId;
	std::string name;
	std::string type;
	std::string nipc;
	std::string logo;

	bool operator==(const ScapProvider& p) const { return uriId == p.uriId; };
};

struct ScapSubAttribute
{
	std::string id;
	std::string description;
	std::string value;
};

struct ScapAttribute
{
	std::string id;
	std::string description;
	std::string validity;
	std::vector<ScapSubAttribute> sub_attributes;
	ScapProvider provider;
	std::string citizen_name;

	bool operator==(const ScapAttribute& a) const { return id == a.id && citizen_name == a.citizen_name; };
};

struct PDFSignatureInfo;
class OAuthAttributes;

class ScapClient
{
public:
	ScapClient(const ScapCredentials &credentials);

	/**
	 * Get all attribute providers.
	 **/
	ScapResult<std::vector<ScapProvider>> getAttributeProviders();

	/**
	 * Get citizen's SCAP Attributes for certain providers.
	 *
	 * @param card is a pointer to a PTEID_EIDCard. Passing a NULL pointer here means we intend to use CMD.
	 * @param providers is the vector of providers to query.
	 * @param allEnterprise boolean that when true means we intend to fetch all enterprise attributes.
	 * @param allEmployee boolean that when true means we intend to fetch all employee attributes.
	 **/
	ScapResult<std::vector<ScapAttribute>> getCitizenAttributes(PTEID_EIDCard *card, const std::vector<ScapProvider> &providers,
		bool allEnterprise = false, bool allEmployee = false);

	/**
	 * Perform a SCAP signature.
	 *
	 * @param device is a pointer to a PTEID_SigningDevice.
	 * @param signature_info is the structure with all parameters related to the PDF signatures, including
	 * input filenames, output path, reason, location and seal geometry
	 * @param attributes is the vector of attributes to sign with.
	 **/
	ScapResult<void> sign(PTEID_SigningDevice *device, const PDFSignatureInfo &signature_info,
		const std::vector<ScapAttribute> &attributes);

	static ScapResult<std::vector<ScapAttribute>> readAttributeCache();
	static bool clearAttributeCache();

	void cancelOAuth();

private:
	ScapCredentials m_scap_credentials;
	OAuthAttributes *m_oauth;
};

struct ScapProviderHasher
{
	std::size_t operator()(ScapProvider const& p) const noexcept {
		return std::hash<std::string>{}(p.uriId);
	}
};

struct ScapAttributeHasher
{
	std::size_t operator()(ScapAttribute const& a) const noexcept {
		return std::hash<std::string>{}(a.id + a.citizen_name);
	}
};

typedef std::unordered_map<ScapProvider, std::vector<ScapAttribute>, ScapProviderHasher> attributes_by_provider_map;

attributes_by_provider_map group_by_provider(const std::vector<ScapAttribute> &attributes);

// Format "Certified by" and "Certified Attributes" strings visible in scap signature's seals
std::pair<std::string, std::string> format_scap_seal_strings(const std::vector<ScapAttribute> &attributes);

};
