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
#include "ezWorker.h"


ezWorker::ezWorker(void)
{

}

ezWorker::~ezWorker(void)
{
}
string ezWorker::inttostr(const int getal) {
  char s[32];
  sprintf_s(s, sizeof(s), "%d", getal);  //_itoa(getal,s,10);  _itoa is niet portable ...
  return string(s);
}
  

bool ezWorker::WildCardFit(string searchprofile, string stringToCheck) {  
	if ( Wildcard::wildcardfit(searchprofile.c_str(), stringToCheck.c_str()) ) 
	{ return true;}
    return false;
}

string ezWorker::TextFromNode(QDomNode theNode)
{
   QDomElement element;
   string returnValue = "";
   element = theNode.toElement();
   if (theNode.isElement()) {
	   if (element.text() != NULL) {
			 returnValue = element.text().toStdString();
	   };
   };
   return returnValue;
}

string ezWorker::NodeAsString(QDomNode theNode) {
	QDomDocument theDoc;
	theDoc.setContent(QString(""));
	theDoc.appendChild(theNode.cloneNode());
	return theDoc.toString().toStdString();
}
string ezWorker::AttribTextFromNode(QDomNode theNode, string attrib)
{
   QDomElement element;
   QDomAttr a;
   string returnValue = "";
   element = theNode.toElement();
   if (!element.isNull()) {
       a =  element.attributeNode(QString(attrib.c_str()));
	   if (!a.isNull()) {returnValue = a.value().toStdString();};
   }

   return returnValue;

}
string ezWorker::GetExtraInfoItem(string theXml, string attrib)
{
   xml_in.setContent(QString(theXml.c_str()));	
   QDomNode Params = xml_in.documentElement().toElement();

   QDomElement e_elem;

   QDomNode elem = Params.namedItem(QString("ExtraInfo")).firstChild();

   while (!elem.isNull()) {

	   e_elem = elem.toElement();
	   if (AttribTextFromNode(elem,"InfoItemName") == attrib) {
		   return e_elem.text().toStdString();
	   }

	   elem = elem.nextSibling();
   }
   return "";

}


QDomNode ezWorker::CreateAttribTextNode(const string tag, const string val, const string AttribName, const string Attribval){
	QDomNode dn ;
	QDomText dt ; 
    QDomAttr Attrib;
	dn = xml_in.createElement(QString(tag.c_str()));
	dt = xml_in.createTextNode(QString(val.c_str()));
	dn.appendChild(dt);

    Attrib = xml_in.createAttribute(QString(AttribName.c_str()));
    Attrib.setNodeValue(QString(Attribval.c_str()));
    dn.toElement().setAttributeNode(Attrib);

	return dn;
}
QDomNode ezWorker::CreateTextNode(const string tag, const string val)
{
	QString vtag;

//	string vtag;
	vtag = QString(tag.c_str());
		vtag = vtag.replace(" ","_");
		vtag = vtag.replace(":","_");
		vtag = vtag.replace("<","_");
		vtag = vtag.replace(">","_");

	QDomNode dn ;
	QDomText dt ; 
	dn = xml_in.createElement(vtag);
	dt = xml_in.createTextNode(QString(val.c_str()));
	dn.appendChild(dt);
	return dn;
}

QDomNode ezWorker::CreateInfoTextNode(const string tag, const string val){
	QDomNode dn ;
	QDomText dt ; 
    QDomAttr Attrib;
	dn = xml_in.createElement(QString("InfoItem"));
	dt = xml_in.createTextNode(QString(val.c_str()));
	dn.appendChild(dt);

    Attrib = xml_in.createAttribute("InfoItemName");
    Attrib.setNodeValue(QString(tag.c_str()));
    dn.toElement().setAttributeNode(Attrib);

	return dn;

}

string ezWorker::GetNamedItem(QDomNode theNode, string tag) 
{
	string result ;
	try {
 	      result = theNode.namedItem(QString(tag.c_str())).toElement().text().toStdString();
	}
	catch (...) {
	    result = "";
	}
	return result;
}
string ezWorker::GetNamedItem(const string theXml, string tag) {
   xml_in.setContent(QString(theXml.c_str()));	
   QDomNode theNode = xml_in.documentElement().toElement();
   return GetNamedItem(theNode,tag);

}
QDomNode ezWorker::xmlToNode(const string theXml) {
   xml_in.setContent(QString(theXml.c_str()));	
   QDomNode theNode = xml_in.documentElement().toElement();
   return theNode;

}


std::string ezWorker::toHEX(std::vector<BYTE>& b) {
    string data = "";
    data.reserve(64);
    for (vector<BYTE>::iterator it = b.begin(); it != b.end(); ++it) {
        char buf[16];
        sprintf_s(buf, sizeof(buf), "%2.2x", *it);
        //data = data + &buf[0] ;
        data.append(buf);
    }
	return data;
}


string ezWorker::GetLastErrorText(DWORD errorCode) {
    string resultaat = "";
	switch (errorCode)	{
		case ERROR_FILE_NOT_FOUND: {
            resultaat =  "Error code : "+ inttostr(errorCode) + " : File not found.";
            break;
        }
		case ERROR_ACCESS_DENIED : {
            resultaat =  "Error code : "+ inttostr(errorCode) + " : Access denied. File is locked ?";
            break;
        }
		default : {
            resultaat =  "Error code : "+ inttostr(errorCode) + " : No translation available";
            break;
        }
	}
	return resultaat;
}

