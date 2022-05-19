/* ****************************************************************************

* eID Middleware Project.
* Copyright (C 2021 Miguel Figueira - <miguelblcfigueira@gmail.com>
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

#ifndef __COUNTRY_CODE_LIST_H__
#define __COUNTRY_CODE_LIST_H__

#include <vector>
#include <string>
#include "Util.h"
#ifndef WIN32
#include <QString>
#endif // not WIN32


/* This list was copied from PhoneCountriesCodeListModel.qml */
const wchar_t *countryCodes[] = {
    L"+351 - Portugal",
    L"+55 - Brazil",
    L"+34 - Spain",
    L"-------------------",
    L"+93 - Afghanistan",
    L"+355 - Albania",
    L"+213 - Algeria",
    L"+684 - American Samoa",
    L"+376 - Andorra",
    L"+244 - Angola",
    L"+809 - Anguilla",
    L"+268 - Antigua",
    L"+54 - Argentina",
    L"+374 - Armenia",
    L"+297 - Aruba",
    L"+247 - Ascension Island",
    L"+61 - Australia",
    L"+672 - Australian External Territories",
    L"+43 - Austria",
    L"+994 - Azerbaijan",
    L"+242 - Bahamas",
    L"+246 - Barbados",
    L"+973 - Bahrain",
    L"+880 - Bangladesh",
    L"+375 - Belarus",
    L"+32 - Belgium",
    L"+501 - Belize",
    L"+229 - Benin",
    L"+809 - Bermuda",
    L"+975 - Bhutan",
    L"+284 - British Virgin Islands",
    L"+591 - Bolivia",
    L"+387 - Bosnia and Herzegovina",
    L"+267 - Botswana",
    L"+55 - Brazil",
    L"+284 - British V.I.",
    L"+673 - Brunei Darussalm",
    L"+359 - Bulgaria",
    L"+226 - Burkina Faso",
    L"+257 - Burundi",
	L"+238 - Cabo Verde",
    L"+855 - Cambodia",
    L"+237 - Cameroon",
    L"+1 - Canada",
    L"+1 - Caribbean Nations",
    L"+345 - Cayman Islands",
    L"+236 - Central African Republic",
    L"+235 - Chad",
    L"+56 - Chile",
    L"+86 - China (People's Republic)",
    L"+886 - China-Taiwan",
    L"+57 - Colombia",
    L"+269 - Comoros and Mayotte",
    L"+242 - Congo (Republic of Congo)",
	L"+243 - Congo (Democratic Republic of)",
    L"+506 - Costa Rica",
    L"+385 - Croatia",
    L"+53 - Cuba",
    L"+357 - Cyprus",
    L"+420 - Czech Republic",
    L"+45 - Denmark",
    L"+246 - Diego Garcia",
    L"+767 - Dominica",
    L"+809 - Dominican Republic",
    L"+253 - Djibouti",
    L"+593 - Ecuador",
    L"+20 - Egypt",
    L"+503 - El Salvador",
    L"+240 - Equatorial Guinea",
    L"+291 - Eritrea",
    L"+372 - Estonia",
	L"+268 - Eswatini",
    L"+251 - Ethiopia",
    L"+500 - Falkland Islands",
    L"+298 - Faroe Islands",
    L"+679 - Fiji",
    L"+358 - Finland",
    L"+33 - France",
    L"+596 - French Antilles",
    L"+594 - French Guiana",
    L"+241 - Gabon (Gabonese Republic",
    L"+220 - Gambia",
    L"+995 - Georgia",
    L"+49 - Germany",
    L"+233 - Ghana",
    L"+350 - Gibraltar",
    L"+30 - Greece",
    L"+299 - Greenland",
    L"+473 - Grenada/Carricou",
	L"+590 - Guadeloupe",
    L"+671 - Guam",
    L"+502 - Guatemala",
    L"+224 - Guinea",
    L"+245 - Guiné-Bissau",
    L"+592 - Guyana",
    L"+509 - Haiti",
    L"+504 - Honduras",
    L"+852 - Hong Kong",
    L"+36 - Hungary",
    L"+354 - Iceland",
    L"+91 - India",
    L"+62 - Indonesia",
    L"+98 - Iran",
    L"+964 - Iraq",
    L"+353 - Ireland (Irish Republic; Eire",
    L"+972 - Israel",
    L"+39 - Italy",
    L"+225 - Ivory Coast (La Cote d'Ivoire",
    L"+876 - Jamaica",
    L"+81 - Japan",
    L"+962 - Jordan",
    L"+7 - Kazakhstan",
    L"+254 - Kenya",
    L"+855 - Khmer Republic (Cambodia/Kampuchea",
    L"+686 - Kiribati Republic (Gilbert Islands",
    L"+82 - Korea, Republic of (South Korea",
    L"+850 - Korea, People's Republic of (North Korea",
	L"+383 - Kosovo",
    L"+965 - Kuwait",
    L"+996 - Kyrgyz Republic",
    L"+371 - Latvia",
    L"+856 - Laos",
    L"+961 - Lebanon",
    L"+266 - Lesotho",
    L"+231 - Liberia",
    L"+370 - Lithuania",
    L"+218 - Libya",
    L"+423 - Liechtenstein",
    L"+352 - Luxembourg",
    L"+853 - Macao",
    L"+261 - Madagascar",
    L"+265 - Malawi",
    L"+60 - Malaysia",
    L"+960 - Maldives",
    L"+223 - Mali",
    L"+356 - Malta",
    L"+692 - Marshall Islands",
    L"+596 - Martinique (French Antilles",
    L"+222 - Mauritania",
    L"+230 - Mauritius",
    L"+269 - Mayolte",
    L"+52 - Mexico",
    L"+691 - Micronesia (F.S. of Polynesia",
	L"+258 - Moçambique",
    L"+373 - Moldova",
    L"+33 - Monaco",
    L"+976 - Mongolia",
	L"+382 - Montenegro",
    L"+473 - Montserrat",
    L"+212 - Morocco",
    L"+95 - Myanmar (former Burma",
    L"+264 - Namibia (former South-West Africa",
    L"+674 - Nauru",
    L"+977 - Nepal",
    L"+31 - Netherlands",
    L"+599 - Netherlands Antilles",
    L"+869 - Nevis",
    L"+687 - New Caledonia",
    L"+64 - New Zealand",
    L"+505 - Nicaragua",
    L"+227 - Niger",
    L"+234 - Nigeria",
    L"+683 - Niue",
    L"+850 - North Korea",
	L"+389 - North Macedonia",
    L"+1 670 - North Mariana Islands (Saipan",
    L"+47 - Norway",
    L"+968 - Oman",
    L"+92 - Pakistan",
    L"+680 - Palau",
    L"+507 - Panama",
    L"+675 - Papua New Guinea",
    L"+595 - Paraguay",
    L"+51 - Peru",
    L"+63 - Philippines",
    L"+48 - Poland",
    L"+1 787 - Puerto Rico",
    L"+974 - Qatar",
    L"+262 - Reunion (France)",
    L"+40 - Romania",
    L"+7 - Russia",
    L"+250 - Rwanda (Rwandese Republic",
    L"+670 - Saipan",
    L"+378 - San Marino",
    L"+239 - São Tomé e Príncipe",
    L"+966 - Saudi Arabia",
    L"+221 - Senegal",
    L"+381 - Serbia",
    L"+248 - Seychelles",
    L"+232 - Sierra Leone",
    L"+65 - Singapore",
    L"+421 - Slovakia",
    L"+386 - Slovenia",
    L"+677 - Solomon Islands",
    L"+252 - Somalia",
    L"+27 - South Africa",
    L"+34 - Spain",
    L"+94 - Sri Lanka",
    L"+290 - St. Helena",
    L"+869 - St. Kitts/Nevis",
    L"+508 - St. Pierre &(et Miquelon (France",
    L"+249 - Sudan",
    L"+597 - Suriname",
    L"+46 - Sweden",
    L"+41 - Switzerland",
    L"+963 - Syrian Arab Republic (Syria",
    L"+689 - Tahiti (French Polynesia",
    L"+886 - Taiwan",
    L"+7 - Tajikistan",
    L"+255 - Tanzania (includes Zanzibar",
    L"+66 - Thailand",
	L"+670 - Timor Leste",
    L"+228 - Togo (Togolese Republic",
    L"+690 - Tokelau",
    L"+676 - Tonga",
    L"+1 868 - Trinidad and Tobago",
    L"+216 - Tunisia",
    L"+90 - Turkey",
    L"+993 - Turkmenistan",
    L"+688 - Tuvalu (Ellice Islands",
    L"+256 - Uganda",
    L"+380 - Ukraine",
    L"+971 - United Arab Emirates",
    L"+44 - United Kingdom",
    L"+598 - Uruguay",
    L"+1 - United States of America",
    L"+7 - Uzbekistan",
    L"+678 - Vanuatu (New Hebrides",
    L"+39 - Vatican City",
    L"+58 - Venezuela",
    L"+84 - Viet Nam",
    L"+1 340 - Virgin Islands",
    L"+681 - Wallis and Futuna",
    L"+685 - Western Samoa",
    L"+381 - Yemen (People's Democratic Republic of",
    L"+967 - Yemen Arab Republic (North Yemen",
    L"+260 - Zambia",
    L"+263 - Zimbabwe"
};

void stripPrefixFromMobile(std::wstring &in_mobile, int *out_codeIdx, std::wstring *out_strippedMobile) 
{
    size_t nCodes = sizeof(countryCodes) / sizeof(countryCodes[0]);
    for (size_t i = 0; i < nCodes; i++)
    {
        std::wstring countryCode(countryCodes[i]);
        std::size_t found = countryCode.find(L'-');
        std::wstring prefix;
        if (found != std::wstring::npos && found > 0)
        {
            prefix.assign(countryCode, 0, found - 1);
            std::size_t prefixIdx = in_mobile.find(prefix);
            if (prefixIdx != std::wstring::npos)
            {
                *out_codeIdx = i;
                out_strippedMobile->assign(in_mobile.substr(prefixIdx + prefix.size(), std::wstring::npos));
                return;
            }
        }
    }
}

#ifdef WIN32
std::vector<LPCWSTR> getCountryCallingCodeList()
{
    size_t nCodes = sizeof(countryCodes) / sizeof(countryCodes[0]);
    std::vector<LPCWSTR> vect(countryCodes, countryCodes + nCodes);
    return vect;
}

#else
std::vector<QString> getCountryCallingCodeList()
{
    std::vector<QString> vect;
    size_t nCodes = sizeof(countryCodes) / sizeof(countryCodes[0]);
    for (size_t i = 0; i < nCodes; i++)
    {
        vect.push_back(QString::fromWCharArray(countryCodes[i]));
    }
    return vect;

}

#endif



#endif