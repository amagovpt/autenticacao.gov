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
#ifndef _GENPUR_H_
#define _GENPUR_H_

#include <QString>

class GenPur
{
private:
	static QString m_strLang[];
	static QString m_strLangName[];
public:
	enum UI_LANGUAGE
	{
		LANG_XX
		, LANG_EN
		, LANG_DEF = LANG_EN
		, LANG_NL
		, LANG_FR
		, LANG_DE
		, LANG_LAST
	};

	GenPur( void )
	{
	}
	~GenPur( void )
	{
	}
	static QString const& getLanguage(UI_LANGUAGE lang)
	{
		if(LANG_LAST > lang)
		{
			return m_strLang[lang];
		}
		return m_strLang[LANG_XX];
	}
	static UI_LANGUAGE getLanguage(QString const& lang)
	{
		for (short l= LANG_EN; l<LANG_LAST; l++)
		{
			if (0 == lang.compare(m_strLang[l], Qt::CaseInsensitive) )
			{
				return (UI_LANGUAGE)l;
			}
		}
		return LANG_XX;
	}
	static QString const& getLanguageName(QString const& lang)
	{
		for (short l= LANG_EN; l<LANG_LAST; l++)
		{
			if (0 == lang.compare(m_strLang[l], Qt::CaseInsensitive) )
			{
				return m_strLangName[l];
			}
		}
		return m_strLangName[0];
	}

};

#endif
