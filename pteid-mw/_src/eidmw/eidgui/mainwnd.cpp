/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include <iostream>
#include <sstream>
#include <QThread>
#include <QtGui>
#include <QFileDialog>
#include <QApplication>
#include <QEvent>
#include <QPixmap>
#include <QImage>
#include <stdlib.h>	
#include <time.h>
#ifdef WIN32
#include <shlobj.h>
#endif
#include "mainwnd.h"
#include "dlgAbout.h"
#include "dlgprint.h"
#include "dlgOptions.h"
#include "Settings.h"
#include "CardInformation.h"
#include "eidlib.h"
#include "eidlibException.h"
#include "picturepopup.h"
#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include "verinfo.h"
#else
#include "beidversions.h"
#endif

static bool	g_cleaningCallback=false;
static int	g_runningCallback=0;
static unsigned int pinactivate = 1;
static unsigned int persodatastatus = 1;

//*****************************************************
// helper class to determine the GUI size multiplication factor
//*****************************************************
class multiplyerFactor
{
public:
	//------------------------------------
	// ctor
	//------------------------------------
	multiplyerFactor( void )
		: XMultiplyer(1.0)
		, YMultiplyer(1.0)
		, WMultiplyer(1.0)
		, HMultiplyer(1.0)
	{
	}
	multiplyerFactor( eZOOMSTATUS zoom )
		: XMultiplyer(1.0)
		, YMultiplyer(1.0)
		, WMultiplyer(1.0)
		, HMultiplyer(1.0)
	{
		getMultiplyerFactor(zoom);
	}
	//------------------------------------
	// calculate the multiplication factor from the zoom factor
	//------------------------------------
	void getMultiplyerFactor( eZOOMSTATUS zoom )
	{
		double X_INCREMENT[]={1.0,WINDOW_WIDTH_MEDIUM/WINDOW_WIDTH_SMALL,   WINDOW_WIDTH_LARGE/WINDOW_WIDTH_SMALL,   WINDOW_WIDTH_HUGE/WINDOW_WIDTH_SMALL}; 
		double Y_INCREMENT[]={1.0,WINDOW_HEIGHT_MEDIUM/WINDOW_HEIGHT_SMALL, WINDOW_HEIGHT_LARGE/WINDOW_HEIGHT_SMALL, WINDOW_HEIGHT_HUGE/WINDOW_HEIGHT_SMALL};
		double W_INCREMENT[]={1.0,WINDOW_WIDTH_MEDIUM/WINDOW_WIDTH_SMALL,   WINDOW_WIDTH_LARGE/WINDOW_WIDTH_SMALL,   WINDOW_WIDTH_HUGE/WINDOW_WIDTH_SMALL};
		double H_INCREMENT[]={1.0,WINDOW_HEIGHT_MEDIUM/WINDOW_HEIGHT_SMALL, WINDOW_HEIGHT_LARGE/WINDOW_HEIGHT_SMALL, WINDOW_HEIGHT_HUGE/WINDOW_HEIGHT_SMALL};

		XMultiplyer = 1.0;
		YMultiplyer = 1.0;
		WMultiplyer = 1.0;
		HMultiplyer = 1.0;

		switch(zoom)
		{
		case ZOOM_HUGE:
			XMultiplyer *= X_INCREMENT[ZOOM_HUGE];
			YMultiplyer *= Y_INCREMENT[ZOOM_HUGE];
			WMultiplyer *= W_INCREMENT[ZOOM_HUGE];
			HMultiplyer *= H_INCREMENT[ZOOM_HUGE];
			break;
		case ZOOM_LARGE:
			XMultiplyer *= X_INCREMENT[ZOOM_LARGE];
			YMultiplyer *= Y_INCREMENT[ZOOM_LARGE];
			WMultiplyer *= W_INCREMENT[ZOOM_LARGE];
			HMultiplyer *= H_INCREMENT[ZOOM_LARGE];
			break;
		case ZOOM_MEDIUM:
			XMultiplyer *= X_INCREMENT[ZOOM_MEDIUM];
			YMultiplyer *= Y_INCREMENT[ZOOM_MEDIUM];
			WMultiplyer *= W_INCREMENT[ZOOM_MEDIUM];
			HMultiplyer *= H_INCREMENT[ZOOM_MEDIUM];
			break;
		case ZOOM_SMALL:
			XMultiplyer *= X_INCREMENT[ZOOM_SMALL];
			YMultiplyer *= Y_INCREMENT[ZOOM_SMALL];
			WMultiplyer *= W_INCREMENT[ZOOM_SMALL];
			HMultiplyer *= H_INCREMENT[ZOOM_SMALL];
			break;
		}
	};
	double XMultiplyer;
	double YMultiplyer;
	double WMultiplyer;
	double HMultiplyer;
};

//*****************************************************
// size of the main window in the different zoom states
//*****************************************************
static int    windowSizes[4][2]= 
{
	 {(int) WINDOW_WIDTH_SMALL  , (int) WINDOW_HEIGHT_SMALL}
	,{(int) WINDOW_WIDTH_MEDIUM , (int) WINDOW_HEIGHT_MEDIUM}
	,{(int) WINDOW_WIDTH_LARGE  , (int) WINDOW_HEIGHT_LARGE}
	,{(int) WINDOW_WIDTH_HUGE   , (int) WINDOW_HEIGHT_HUGE}
};

//*****************************************************
// style sheets used in the GUI
//*****************************************************
enum eStyleSheet
{
	STYLESHEET_NONE = -1
	,STYLESHEET_BUTTON
	,STYLESHEET_TITLE_1
	,STYLESHEET_TITLE_2
	,STYLESHEET_FOOTER_1
	,STYLESHEET_NORMAL_LABEL
	,STYLESHEET_NORMAL_VALUE
	,STYLESHEET_BIG_VALUE
	,STYLESHEET_SIS_VALUE
	,STYLESHEET_SMALL_RED
	,STYLESHEET_SMALL_BLUE
	,STYLESHEET_SMALL_REDRIGHT
};

//*****************************************************
// structure definition for point sizes in relation to the style sheet
// used
//*****************************************************
struct zoomInfoStyleSheets
{
	int		stylesheet_id;			// style sheet ID
	int		pointSizes[4];			// point size to be used
};
//*****************************************************
// Array containing the font point sizes to be used for the
// different style sheets for each zoom status
//*****************************************************
static zoomInfoStyleSheets stylesheetsInfo[]=
{
	 {STYLESHEET_BUTTON,		{BUTTON_POINTSIZE_SMALL,     BUTTON_POINTSIZE_MEDIUM,	  BUTTON_POINTSIZE_LARGE,	  BUTTON_POINTSIZE_HUGE}}
	,{STYLESHEET_TITLE_1,		{TITLE1_POINTSIZE_SMALL,     TITLE1_POINTSIZE_MEDIUM,	  TITLE1_POINTSIZE_LARGE,	  TITLE1_POINTSIZE_HUGE}}
	,{STYLESHEET_TITLE_2,		{TITLE2_POINTSIZE_SMALL,     TITLE2_POINTSIZE_MEDIUM,	  TITLE2_POINTSIZE_LARGE,	  TITLE2_POINTSIZE_HUGE}}
	,{STYLESHEET_FOOTER_1,		{FOOTER1_POINTSIZE_SMALL,    FOOTER1_POINTSIZE_MEDIUM,	  FOOTER1_POINTSIZE_LARGE,	  FOOTER1_POINTSIZE_HUGE}}
	,{STYLESHEET_NORMAL_LABEL,	{LABEL_POINTSIZE_SMALL,      LABEL_POINTSIZE_MEDIUM,	  LABEL_POINTSIZE_LARGE,	  LABEL_POINTSIZE_HUGE}}
	,{STYLESHEET_NORMAL_VALUE,	{NORMALVALUE_POINTSIZE_SMALL,NORMALVALUE_POINTSIZE_MEDIUM,NORMALVALUE_POINTSIZE_LARGE,NORMALVALUE_POINTSIZE_HUGE}}
	,{STYLESHEET_BIG_VALUE,		{BIGVALUE_POINTSIZE_SMALL,   BIGVALUE_POINTSIZE_MEDIUM,   BIGVALUE_POINTSIZE_LARGE,	  BIGVALUE_POINTSIZE_HUGE}}
	,{STYLESHEET_SIS_VALUE,		{SISVALUE_POINTSIZE_SMALL,   SISVALUE_POINTSIZE_MEDIUM,   SISVALUE_POINTSIZE_LARGE,	  SISVALUE_POINTSIZE_HUGE}}
	,{STYLESHEET_SMALL_RED,		{SMALL_POINTSIZE_SMALL,      SMALL_POINTSIZE_MEDIUM,      SMALL_POINTSIZE_LARGE,	  SMALL_POINTSIZE_HUGE}}
	,{STYLESHEET_SMALL_BLUE,	{SMALL_POINTSIZE_SMALL,      SMALL_POINTSIZE_MEDIUM,      SMALL_POINTSIZE_LARGE,	  SMALL_POINTSIZE_HUGE}}
	,{STYLESHEET_SMALL_REDRIGHT,{SMALL_POINTSIZE_SMALL,      SMALL_POINTSIZE_MEDIUM,      SMALL_POINTSIZE_LARGE,	  SMALL_POINTSIZE_HUGE}}
};
//*****************************************************
// structure definition for the position of a widget
// defined as X,Y,width,height
//*****************************************************
struct widgetPos
{
	int pos[4];
};

//*****************************************************
// structure definition for widget base position on the window
//*****************************************************
struct widgetInfo
{
	const char*  widgetName;		//!< name of the widget
	widgetPos    position;			//!< position on the window (x,y,width,height)
	eStyleSheet  style;				//!< style of the widget
};

//*****************************************************
// Array containing widget position on the window
// The position is defined as the position in the smallest zoom factor
// The position is multiplied by a 'zoom factor'.
// When zooming, the window is in fact enlarged, so we can multiply
// the x,y,w,h with the same factor as the window is enlarged.
//*****************************************************
static widgetInfo widgetTabInfo[]=
{
	//-------------------------------------------------------------------------------
	//								SMALL            
	// {objectName					{  x, y,  w, h},                style used}
	//-------------------------------------------------------------------------------

	//------- tab Identity --------
	 {"lblIdentity_Head1_1",		{{ 10,  1,181, 30}},			STYLESHEET_TITLE_1}
	,{"lblIdentity_Head1_2",		{{200,  1,181, 30}},			STYLESHEET_TITLE_1}
	,{"lblIdentity_Head1_3",		{{380,  1,181, 30}},			STYLESHEET_TITLE_1}
	,{"lblIdentity_Head1_4",		{{570,  1,181, 30}},			STYLESHEET_TITLE_1}
	,{"lblIdentity_Head2_1",		{{ 10, 35,181, 18}},			STYLESHEET_TITLE_2}
	,{"lblIdentity_Head2_2",		{{200, 35,181, 18}},			STYLESHEET_TITLE_2}
	,{"lblIdentity_Head2_3",		{{380, 35,181, 18}},			STYLESHEET_TITLE_2}
	,{"lblIdentity_Head2_4",		{{570, 35,181, 18}},			STYLESHEET_TITLE_2}
	,{"lblIdentity_Name",			{{ 10, 80,207, 18}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_Name",			{{222, 80,529, 20}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_GivenNames",		{{ 10,105,207, 18}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_GivenNames",		{{222,105,529, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_BirthDate",		{{222,127,417, 20}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_BirthDate",		{{222,151,417, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_Sex",			{{630,123,106, 20}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_Sex",			{{630,151,106, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_Nationality",	{{222,175,253, 36}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_Nationality",	{{330,175,270, 36}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_Country",		{{570,170,176, 28}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_Country",		{{570,204,176, 16}},			STYLESHEET_NORMAL_VALUE}
	//,{"lblIdentity_Card_Number",	{{222,220,341, 20}},			STYLESHEET_NORMAL_LABEL}
	//,{"txtIdentity_Card_Number",	{{222,240,341, 20}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_Height",			{{222,220,341, 20}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_Height",			{{222,240,341, 20}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_DocumentNumber",	{{222,260,341, 20}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_DocumentNumber",	{{222,290,341, 20}},			STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_ValidFrom_Until",{{ 10,347,240, 18}},			STYLESHEET_NORMAL_LABEL}
	,{"txtIdentity_ValidFrom_Until",{{ 10,367,240, 18}},			STYLESHEET_NORMAL_VALUE}
	//,{"lblIdentity_Parents",		{{ 350,347,240, 18}},			STYLESHEET_NORMAL_LABEL}
	//,{"txtIdentity_Parents",		{{ 350,367,240, 18}},			STYLESHEET_NORMAL_VALUE}
	//,{"lblIdentity_ImgPerson",		{{30,130,50,50}},				STYLESHEET_NORMAL_LABEL}
	//------- tab foreigners --------
	,{"txtForeigners_Card_Number",	{{463, 11,288, 24}},			STYLESHEET_BIG_VALUE}
	,{"label",						{{ 37, 63,230, 20}},			STYLESHEET_NORMAL_LABEL}
	,{"txtForeigners_Name",			{{280, 63,532, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"txtForeigners_GivenNames",	{{280, 89,532, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"label_2",					{{ 37,115,230, 18}},			STYLESHEET_NORMAL_LABEL}
	,{"txtForeigners_ValidTot",		{{280,115,532, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"label_3",					{{ 37,141,230, 18}},			STYLESHEET_NORMAL_LABEL}
	,{"txtForeigners_PlaceOfIssue",	{{280,141,532, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"label_4",					{{280,165,532, 18}},			STYLESHEET_NORMAL_LABEL}
	,{"txtForeigners_CardType",		{{280,193,532, 18}},			STYLESHEET_NORMAL_VALUE}
//	,{"label_5",					{{280,217,532, 18}},			STYLESHEET_NORMAL_LABEL}
//	,{"txtForeigners_Remarks",		{{280,245,532, 18}},			STYLESHEET_NORMAL_VALUE}
	,{"lblForeigners_Footer_1",		{{280,410,532, 18}},			STYLESHEET_FOOTER_1}
	,{"lblForeigners_Footer_2",		{{280,425,532, 18}},			STYLESHEET_FOOTER_1}
	,{"lblForeigners_ImgPerson",	{{ 63,230,150,270}},			STYLESHEET_NONE}
	//------- tab SIS --------
	,{"txtSis_SocialSecurityNumber",{{520, 63,231, 28}},			STYLESHEET_SIS_VALUE}
	,{"txtSis_Name",				{{286,160,465, 28}},			STYLESHEET_SIS_VALUE}
	,{"txtSis_GivenNames",			{{286,192,465, 28}},			STYLESHEET_SIS_VALUE}
	,{"txtSis_BirthDate",			{{286,226,152, 28}},			STYLESHEET_SIS_VALUE}
	,{"txtSis_LogicalNumber",		{{  9,390,245, 28}},			STYLESHEET_SIS_VALUE}
	,{"txtSis_ValidFrom",			{{566,390,185, 28}},			STYLESHEET_SIS_VALUE}
	,{"lblSis_Sex",					{{444,220, 20, 28}},			STYLESHEET_NONE}
	//------- tab identity extra --------
	,{"lblIdentity_TaxNo",					{{  9, 50,201, 28}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_TaxNo",				{{  9, 84,201, 16}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_SocialSecurityNo",		{{  230, 50,201, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_SocialSecurityNo",	{{  230, 84,201, 16}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_HealthNo",				{{  470, 50,201, 28}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_HealthNo",			{{  470, 84,201, 16}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_CardVersion",			{{  9, 130,201, 28}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_CardVersion",		{{  9, 164,201, 16}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_DocumentType",			{{  240, 130,201, 28}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_DocumentType",		{{  240, 164,201, 16}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_IssuingEntity",			{{  470, 130,201, 28}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_IssuingEntity",		{{  470, 164,201, 16}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_PlaceOfIssue",			{{9, 206,176, 28}},		STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_PlaceOfIssue",		{{15, 240,260, 28}},	STYLESHEET_NORMAL_VALUE}
	,{"lblChip_Number",						{{410,300,353, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_ChipNumber",		{{410,320,353, 18}},	STYLESHEET_NORMAL_VALUE}

	/*,{"lblAdress_Street",					{{  9,102,211, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Adress_Street",		{{  9,136,350, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblAdress_PostalCode",				{{  9,158,201, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Adress_PostalCode",	{{  9,192,201, 18}},	STYLESHEET_NORMAL_VALUE}*/
	,{"lblCard_Number",						{{  9,300,353, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Card_Number",		{{  9,320,353, 18}},	STYLESHEET_NORMAL_VALUE}

	,{"lblAdress_Muncipality",				{{230,158,176, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Adress_Muncipality",{{230,192,201, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblAdress_Country",					{{  9,214,201, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Adress_Country",	{{  9,246,176, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblCard_ValidFrom_Until",			{{  9,340,353, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_ValidFrom_Until",	{{  9,360,353, 18}},	STYLESHEET_NORMAL_VALUE}

	//,{"lblSpecialStatus",					{{230,214,176, 34}},	STYLESHEET_NORMAL_LABEL}
	//,{"txtSpecialStatus",					{{230,246,201, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblIdentity_Title",					{{410, 46,353, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Title",				{{410, 80,353, 18}},	STYLESHEET_NORMAL_VALUE}

	,{"lblIdentity_Remarks",				{{410,158,201, 34}},	STYLESHEET_NORMAL_LABEL}
	,{"txtIdentityExtra_Remarks1",			{{410,200,201, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"txtIdentityExtra_Remarks2",			{{410,220,201, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"txtIdentityExtra_Remarks3",			{{410,240,201, 18}},	STYLESHEET_NORMAL_VALUE}

	//------- tab SIS extra --------
	,{"lblSisExtra_Card_ValidFrom_Until",	{{  9, 63,243, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtSisExtra_ValidFrom_Until",		{{258, 63,493, 18}},	STYLESHEET_SIS_VALUE}

	//------- tab Foreigner extra
	,{"lblForeignersExtra_BirthDate",		{{ 90, 52,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_BirthDate",		{{280, 52,576, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblForeignersExtra_Nationality",		{{ 90, 74,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Nationality",		{{280, 74,576, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblForeignersExtra_Sex",				{{ 90, 96,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Sex",				{{280, 96,576, 18}},	STYLESHEET_NORMAL_VALUE}
	
	,{"lblForeignersExtra_street",			{{ 90,118,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Adress_Street",	{{280,118,180, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblForeignersExtra_PostalCode",		{{ 90,142,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Adress_PostalCode",{{280,142,100, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblForeignersExtra_municipality",	{{350,142,100, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Adress_Muncipality",{{455,142,250, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblForeignersExtra_country",			{{ 90,165,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Adress_Country",	{{280,165,100, 18}},	STYLESHEET_NORMAL_VALUE}

	,{"lblForeignersExtra_Remarks1",		{{ 90,188,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_Remarks1",		{{280,211,400, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"txtForeignersExtra_Remarks2",		{{280,234,400, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"txtForeignersExtra_Remarks3",		{{280,257,400, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"txtForeignerSpecialStatus",			{{280,185,576, 18}},	STYLESHEET_NORMAL_VALUE}

	,{"lblForeignersExtra_NationalNumber",	{{ 40,296,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_NationalNumber",	{{230,296,609, 18}},	STYLESHEET_NORMAL_VALUE}
	,{"lblForeignersExtra_ChipNumber",		{{ 40,318,180, 18}},	STYLESHEET_NORMAL_LABEL}
	,{"txtForeignersExtra_ChipNumber",		{{230,318,609, 18}},	STYLESHEET_NORMAL_VALUE}

	//------- tab Certificate
	,{"lblCert_Owner",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtCert_Owner",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblCert_Issuer",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtCert_Issuer",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblCert_KeyLenght",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtCert_KeyLenght",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblCert_ValidFrom",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtCert_ValidFrom",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblCert_ValidUntil",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtCert_ValidUntil",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblCert_Status",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtCert_Status",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblCert_InfoAdd",					{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"btnCert_Register",					{{0,0,0,0}},			STYLESHEET_BUTTON}
	,{"btnCert_Details",					{{0,0,0,0}},			STYLESHEET_BUTTON}
	,{"btnOCSPCheck",						{{0,0,0,0}},			STYLESHEET_BUTTON}

	//------- tab PIN
	,{"lblPIN_Name",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtPIN_Name",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblPIN_ID",							{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtPIN_ID",							{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblPIN_Status",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"txtPIN_Status",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"lblPIN_InfoAdd",						{{0,0,0,0}},			STYLESHEET_NORMAL_LABEL}
	,{"btnPIN_Test",						{{0,0,0,0}},			STYLESHEET_BUTTON}
	,{"btnPIN_Change",						{{0,0,0,0}},			STYLESHEET_BUTTON}

	//------- tab Info
	,{"lblInfo_Top",		/*{ 160, 40,587, 16}*/{{0,0,0,0}},	STYLESHEET_NORMAL_LABEL}
	,{"lblInfo_bottom",		{{   1,345,740, 14}},					STYLESHEET_NORMAL_LABEL}
	,{"lblInfo_Img1",		/*{  10, 10,147, 79}*/{{0,0,0,0}},	STYLESHEET_NONE}
	
};

//*****************************************************
// Map with all the widget information needed
//*****************************************************
typedef QMap<QString,eStyleSheet>	tWidgetMapStyle;

static tWidgetMapStyle widgetMapStyle;

//*****************************************************
// Initialize the map with all widget information
//*****************************************************
void InitWidgetMapStyle()
{
	for (size_t widget=0;widget<sizeof(widgetTabInfo)/sizeof(struct widgetInfo);widget++)
	{
		widgetMapStyle[widgetTabInfo[widget].widgetName]=widgetTabInfo[widget].style;
	}
}

//*****************************************************
// file list of DLL's to be displayed in the info tab
//*****************************************************
static const char* fileList[]=
{
#ifdef BEID_35
#ifdef WIN32
#ifdef _DEBUG
	  "beid35guiD.exe"
	, "beid35libCppD.dll"
	, "beid35commonD.dll"
	, "beid35applayerD.dll"
	, "beid35DlgsWin32D.dll"
	, "beid35cardlayerD.dll"
	, "beidpkcs11D.dll"
#else
	  "beid35gui.exe"
	, "beid35libCpp.dll"
	, "beid35common.dll"
	, "beid35applayer.dll"
	, "beid35DlgsWin32.dll"
	, "beid35cardlayer.dll"
	, "beidpkcs11.dll"
#endif
#elif __APPLE__
	  "beidgui"
	, "libbeidlib"
	, "libbeidapplayer"
	, "libbeidcommon"
	, "libbeidcardlayer"
	, "libdialogsQT"
	, "libbeidpkcs11"
#else
	  "beidgui"
	, "libbeidlib.so"
	, "libbeidapplayer.so"
	, "libbeidcommon.so"
	, "libbeidcardlayer.so"
	, "libdialogsQT.so"
	, "libbeidpkcs11.so"
#endif

#else

#ifdef WIN32
#ifdef _DEBUG
	"beidguiD.exe"
	, "beidlibCppD.dll"
	, "beidcommonD.dll"
	, "beidapplayerD.dll"
	, "beidDlgsWin32D.dll"
	, "beidcardlayerD.dll"
	, "beidpkcs11D.dll"
#else
	  "beidgui.exe"
	, "beidlibCpp.dll"
	, "beidcommon.dll"
	, "beidapplayer.dll"
	, "beidDlgsWin32.dll"
	, "beidcardlayer.dll"
	, "beidpkcs11.dll"
#endif
#elif __APPLE__
	"beidgui"
	, "libbeidlib"
	, "libbeidapplayer"
	, "libbeidcommon"
	, "libbeidcardlayer"
	, "libdialogsQT"
	, "libbeidpkcs11"
#else
	  "beidgui"
	, "libbeidlib.so"
	, "libbeidapplayer.so"
	, "libbeidcommon.so"
	, "libbeidcardlayer.so"
	, "libdialogsQT.so"
	, "libbeidpkcs11.so"
#endif
#endif
};

void MainWnd::createTrayMenu()
{
	m_pMinimizeAction = new QAction(tr("Mi&nimize"), this);
	connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(showMinimized()));

	m_pRestoreAction = new QAction(tr("&Restore"), this);
	connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(restoreWindow()));

	m_pTrayIconMenu = new QMenu(this);
	m_pTrayIconMenu->addAction(m_pMinimizeAction);
	m_pTrayIconMenu->addAction(m_pRestoreAction);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_ui.actionOptions);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_ui.actionE_xit);

	if (!m_pTrayIcon)
	{
		m_pTrayIcon = new QSystemTrayIcon( this );
		m_pTrayIcon->setToolTip(QString("EID Viewer"));
	}
	m_pTrayIcon->setContextMenu( m_pTrayIconMenu );
	if (isHidden())
	{
		m_pMinimizeAction->setEnabled(false);
	}
	else
	{
		m_pRestoreAction->setEnabled(false);
	}
}

//*****************************************************
// certificate contexts per card reader
//*****************************************************
tCertPerReader	MainWnd::m_certContexts;		//!< certificate contexts per reader

//*****************************************************
// ctor
//*****************************************************
MainWnd::MainWnd( GUISettings& settings, QWidget *parent ) 
	: QMainWindow(parent)
	, m_Zoom(ZOOM_SMALL)
	, m_pPrinter(NULL)
	, m_Pop(NULL)
	, m_TypeCard(BEID_CARDTYPE_UNKNOWN)
	, m_Language(GenPur::LANG_EN)
	, m_CurrReaderName("")
	, m_virtReaderContext(NULL)
	, m_UseKeyPad(false)
	, m_Settings(settings)
	, m_timerReaderList(NULL)
	, m_STATUS_MSG_TIME(5000)
	, m_ShowBalloon(false)
	, m_msgBox(NULL)
	, m_connectionStatus((BEID_CertifStatus)-1)
{
	///InitWidgetMapStyle();

	//------------------------------------
	// install the translator object and load the .qm file for
	// the given language.
	//------------------------------------
	qApp->installTranslator(&m_translator);

	GenPur::UI_LANGUAGE CurrLng   = m_Settings.getGuiLanguageCode();
	GenPur::UI_LANGUAGE LoadedLng = LoadTranslationFile(CurrLng);

	m_Language = LoadedLng;

    m_ui.setupUi(this);

	QAction* toggleview = m_ui.toolBar->toggleViewAction();
	toggleview->setVisible(false);

	//------------------------------------
	// make sure we have a fixed, unsizeable window
	//------------------------------------

	setFixedSize( windowSizes[m_Zoom][0], windowSizes[m_Zoom][1] );

	setFixedSize( 850, 700 );

	//------------------------------------
	// disable the reload button until the first time a card is plugged in. 
	// In case of autoread of the inserted card, there is no interference possible 
	// between the button and the autoread.
	//------------------------------------
	setEnableReload(false);
	InitLanguageMenu();

	Qt::WindowFlags flags = windowFlags();
	flags ^= Qt::WindowMaximizeButtonHint;
	setWindowFlags( flags );

	//------------------------------------
	// set the window Icon (as it appears in the left corner of the window)
	//------------------------------------
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );

	m_pPrinter	= new QPrinter();

	setLanguageMenu(m_Language);		// check the language in the language menu

	Show_Splash();

	connect( m_ui.actionShow_Toolbar, SIGNAL(toggled(bool)), m_ui.toolBar, SLOT(setVisible(bool)) );

	//------------------------------------
	//SysTray
	//------------------------------------
	m_pTrayIcon = new QSystemTrayIcon( this );
	m_pTrayIcon->setToolTip(QString("EID Viewer"));
	createTrayMenu();

	connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	m_Pop = new PicturePopup(0);		// create the popup object for card insert/remove

	//----------------------------------------------------
	// in case of autoread the card at startup, we have to load the data
	// This will be handled in the callback functions.
	// At startup, a callback will be called and the card will be loaded
	// if necessary
	//----------------------------------------------------
	setEventCallbacks();

	enableFileMenu();
	enablePrintMenu();

	m_ui.actionShow_Toolbar->setChecked( m_Settings.getShowToolbar() );

	//----------------------------------
	// button register/details the certificates is by default
	// disabled
	//----------------------------------
	setEnabledCertifButtons( false );

	//----------------------------------
	// OCSP check button disabled by default
	//----------------------------------
	m_ui.btnCert_Register->setEnabled(false);

	//----------------------------------
	// button for Pins by default disabled
	//----------------------------------
	setEnabledPinButtons( false );

	//----------------------------------
	// some items are not available on other platforms than windows
	// so hide them, such that users can't click them.
	//----------------------------------
#ifndef WIN32
	m_ui.btnCert_Details->hide();
	m_ui.btnCert_Register->hide();
	m_ui.lblCert_InfoAdd->hide();
#endif

	//----------------------------------
	// set a timer to check if the number of card readers is changed
	//----------------------------------
	if (!m_timerReaderList)
	{
		m_timerReaderList = new QTimer(this);
		connect(m_timerReaderList, SIGNAL(timeout()), this, SLOT(updateReaderList()));
		m_timerReaderList->start(TIMERREADERLIST); 
	}
	m_ui.actionZoom_Out->setEnabled(false);


	//------------------------------------
	// set the tray Icon (as it appears in the traybar)
	//------------------------------------
	setCorrespondingTrayIcon(NULL);

	if ( m_Settings.getStartMinimized() )
	{
		Qt::WindowStates state = windowState();
		state ^= Qt::WindowMinimized;
		setWindowState( state );
	}
	else 
	{
		showNoReaderMsg();
	}
}

//*****************************************************
// show the message if no reader are available
//*****************************************************
void MainWnd::showNoReaderMsg( void )
{
	if ( 0 == ReaderSet.readerCount() )
	{
		QString strCaption(tr("Checking card readers"));
		QString strMessage(tr("No card readers are detected.\n"));
		strMessage += tr("Please check the card readers are connected and/or verify the ");
#ifdef WIN32
		strMessage += tr("smart card service ");
#else
		strMessage += tr("pcsc daemon ");
#endif
		strMessage += tr("is running.");
		if(!m_msgBox)
		{
			m_msgBox =  new QMessageBox(QMessageBox::Warning,strCaption,strMessage,QMessageBox::Ok,this);
			m_msgBox->setModal(true);
			m_msgBox->show();
		}
	}
}

//*****************************************************
// show the windows
//*****************************************************
void MainWnd::showNormal( void )
{
	m_ShowBalloon=true;
	QMainWindow::showNormal();
}

//*****************************************************
// Respond to other instance
//*****************************************************
void MainWnd::messageRespond( const QString& message)
{
	BEID_LOG(BEID_LOG_LEVEL_DEBUG, "eidgui", "messageRespond - Receive message = %s",message.toLatin1().data());

	if(message.compare("Restore Windows")==0)
	{
		restoreWindow();
	}
	else if (message.startsWith("Open File"))
	{
		QString filePath = (QString) message;
		filePath.remove(0,9);

		OpenSelectedEid((const QString)filePath);
	}
}

//*****************************************************
// restore the window in its previous state
//*****************************************************
void MainWnd::restoreWindow( void )
{
	if( this->isHidden() )
	{
		showNoReaderMsg();

		if (m_Settings.getAutoCardReading())
		{
			loadCardData();
		}
		this->activateWindow();
	}
	this->showNormal();
}
//*****************************************************
// update the readerlist. In case a reader is added to the machine
// at runtime.
//*****************************************************
void MainWnd::updateReaderList( void )
{
	//----------------------------------------------------
	// check if the number of readers is changed
	//----------------------------------------------------
	try
	{
		if (ReaderSet.isReadersChanged())
		{
			stopAllEventCallbacks();
			ReaderSet.releaseReaders();
			m_CI_Data.Reset();
		}
		if ( 0 == m_callBackHandles.size() )
		{
			setEventCallbacks();
		}
		setCorrespondingTrayIcon(NULL);		
	}
	catch(...)
	{
		stopAllEventCallbacks();
		ReaderSet.releaseReaders();
		m_CI_Data.Reset();
		setCorrespondingTrayIcon(NULL);		
	}
}
//*****************************************************
// Enable/disable the reload button/menu item
//*****************************************************
void MainWnd::setEnableReload( bool bEnabled )
{
	m_ui.actionReload_eID->setEnabled(bEnabled);
}

//*****************************************************
// Enable/disable the PIN buttons
//*****************************************************
void MainWnd::setEnabledPinButtons( bool bEnabled )
{
	if (bEnabled)
	{
		//----------------------------------------------------
		// only if an item is selected, the buttons are enabled
		//----------------------------------------------------
		QList<QTreeWidgetItem *> selectedItems =  m_ui.treePIN->selectedItems ();
		if (selectedItems.size()==0)
		{
			bEnabled = false;
		}
	}
	m_ui.btnPIN_Test->setEnabled(bEnabled);
	m_ui.btnPIN_Change->setEnabled(bEnabled);
}

//*****************************************************
// enable/disable the certificate buttons
//*****************************************************
void MainWnd::setEnabledCertifButtons( bool bEnabled )
{
	m_ui.btnCert_Register->setEnabled(bEnabled);
	m_ui.btnCert_Details->setEnabled(bEnabled);
}
//*****************************************************
// dtor
//*****************************************************
MainWnd::~MainWnd( void )
{
	cleanupCallbackData();
 	if(m_pPrinter)
 	{
 		delete m_pPrinter;
		m_pPrinter = NULL;
 	}
 	if(m_Pop)
 	{
 		delete m_Pop;
		m_Pop = NULL;
 	}

	releaseVirtualReader();
}
//*****************************************************
// cleanup the callback data
//*****************************************************
void MainWnd::cleanupCallbackData()
{
	
	while(g_runningCallback)
	{
#ifdef WIN32
		::Sleep(100);
#else
		::usleep(100000);
#endif
	}
	
	g_cleaningCallback = true;
	
	for (tCallBackData::iterator it = m_callBackData.begin()
		; it != m_callBackData.end()
		; it++
		)
	{
		CallBackData* pCallbackData = it.value();
		delete pCallbackData;
	}
	m_callBackData.clear();
	
	g_cleaningCallback = false;
}

//*****************************************************
// stop the event callbacks and delete the corresponding callback data
// objects.
//*****************************************************
void MainWnd::stopAllEventCallbacks( void )
{
	
	for (tCallBackHandles::iterator it = m_callBackHandles.begin()
		; it != m_callBackHandles.end()
		; it++
		)
	{
		BEID_ReaderContext& readerContext = ReaderSet.getReaderByName(it.key().toLatin1());
		unsigned long handle = it.value();
		readerContext.StopEventCallback(handle);
	}
	m_callBackHandles.clear();

	cleanupCallbackData();
}

//*****************************************************
// Enable the File save menu items depending on the fact we have a card loaded or not
//*****************************************************
void MainWnd::enableFileSave( bool bEnabled )
{
	m_ui.actionSave_eID->setEnabled(bEnabled);
	m_ui.actionSave_eID_as->setEnabled(bEnabled);;
}
void MainWnd::enableFileMenu( void )
{
	m_ui.actionSave_eID->setEnabled(false);
	m_ui.actionSave_eID_as->setEnabled(false);

	if (m_CI_Data.isDataLoaded())
	{
		BEID_XMLDoc &fulldoc = m_CI_Data.m_pCard->getDocument(BEID_DOCTYPE_FULL);
		if(fulldoc.isAllowed())
		{
			m_ui.actionSave_eID->setEnabled(true);
			m_ui.actionSave_eID_as->setEnabled(true);
		}
	}
}
//*****************************************************
// Enable the File print menu items depending on the fact we have a card loaded or not
// If we have a card loaded, then we can print
//*****************************************************
void MainWnd::enablePrintMenu( void )
{
	bool bEnable = false;
	if (m_CI_Data.isDataLoaded())
	{
		switch(m_CI_Data.m_CardInfo.getType())
		{
		case BEID_CARDTYPE_EID:
		case BEID_CARDTYPE_KIDS:
		case BEID_CARDTYPE_FOREIGNER:
			bEnable = true;
			break;
		case BEID_CARDTYPE_SIS:
			bEnable = true;
			break;
		default:
			break;
		}
	}
	m_ui.actionPrint_eID->setEnabled(bEnable);
	m_ui.actionPrinter_Settings->setEnabled(bEnable);;
}


//*****************************************************
// Load a translation file
// In case the new language can not be loaded, depending on the
// parameters, either the old language will be reloaded or the
// default language will be loaded.
//*****************************************************
GenPur::UI_LANGUAGE MainWnd::LoadTranslationFile(GenPur::UI_LANGUAGE NewLanguage)
{

	QString strTranslationFile;
	strTranslationFile = QString("eidmw_") + GenPur::getLanguage(NewLanguage);

	if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
	{
		// this should not happen, since we've built the menu with the translation filenames
	}
	return NewLanguage;
}
void MainWnd::on_tabWidget_Identity_currentChanged( int index )
{
	/*index = index;

	if ((index >= 0) && (index < m_ui.tabWidget_Identity->count()))
	{
		QWidget *currentWidget=	m_ui.tabWidget_Identity->currentWidget();
		m_ui.tabWidget_Identity->setAccessibleName(currentWidget->objectName());
	}
	*/
}

void MainWnd::showEvent( QShowEvent * event )
{
	m_pMinimizeAction->setEnabled(true);
	m_pRestoreAction->setEnabled(false);
	event->accept();
}
void MainWnd::resizeEvent( QResizeEvent * event )
{
	event->accept();
}

void MainWnd::closeEvent( QCloseEvent *event)
{
	if ( m_pTrayIcon->isVisible() ) 
	{
		if (m_msgBox)
		{
			delete(m_msgBox);
			m_msgBox = NULL;
		}
		//clearGuiContent();
		hide();
		if(m_ShowBalloon)
		{
			QString title(tr("eID is still running"));
			QString message(tr("The application will continue to run. To stop this application, select 'Exit' from the menu."));
			m_pTrayIcon->showMessage ( title, message, QSystemTrayIcon::Information) ;
			m_ShowBalloon = false;
		}
		m_pMinimizeAction->setEnabled(false);
		m_pRestoreAction->setEnabled(true);
		event->ignore();
	}
}
 
//*****************************************************
// Tray icon activation detection
//*****************************************************
void MainWnd::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
	switch (reason) 
	{
		case QSystemTrayIcon::Trigger:
			break;

		case QSystemTrayIcon::DoubleClick:
			if( this->isHidden() )
			{
				this->restoreWindow();
				m_pMinimizeAction->setEnabled(true);
				m_pRestoreAction->setEnabled(false);
			}
			else
			{
				m_pMinimizeAction->setEnabled(false);
				m_pRestoreAction->setEnabled(true);
				this->hide();
			}
			break;
		
		case QSystemTrayIcon::MiddleClick:
			m_pTrayIcon->showMessage( "eID", "eID reader: Tray icon", QSystemTrayIcon::Information, 3 * 1000 ); // 3 sec
			break;
		
		default:
			break;
	}
}

//*****************************************************
// remove the certificates of a card in a specific reader
//*****************************************************
bool MainWnd::RemoveCertificates( QString const& readerName )
{
	return RemoveCertificates( readerName.toLatin1().data() );
}

bool MainWnd::RemoveCertificates( const char* readerName )
{
#ifdef WIN32

	if ( !readerName || 0 == strlen(readerName) )
	{
		return false;
	}

 	PCCERT_CONTEXT	pCertContext = NULL;
	int nrCerts = m_certContexts[readerName].size();
	for (int CertIdx=0;CertIdx<nrCerts;CertIdx++)
	{

		// ----------------------------------------------------
		// create the certificate context with the certificate raw data
		// ----------------------------------------------------
		PCCERT_CONTEXT  pDesiredCert	= NULL;
		HCERTSTORE		hMyStore		= CertOpenSystemStore(NULL, "MY");
		
		pCertContext = m_certContexts[readerName][CertIdx];

		if ( NULL != hMyStore )
		{
			// ----------------------------------------------------
			// look if we already have the certificate in the store
			// If the certificate is not found --> NULL
			// ----------------------------------------------------
			if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
				)
			{
				CertDeleteCertificateFromStore( pDesiredCert );
			}
		}
	}
	if (nrCerts>0)
	{
		forgetCertificates( readerName );
	}
#endif
	return true;
}

//*****************************************************
// import the certificates from the card in a specific reader
//*****************************************************
bool MainWnd::ImportCertificates( QString const& readerName )
{
	return ImportCertificates( readerName.toLatin1().data() );
}

bool MainWnd::ImportCertificates( const char* readerName )
{
#ifdef WIN32
	if ( !readerName || 0 == strlen(readerName) )
	{
		return false;
	}

	PCCERT_CONTEXT	pCertContext = NULL;
	QString			strTip;
	bool			bImported	= false;
			
	BEID_ReaderContext&  ReaderContext  = ReaderSet.getReaderByName(readerName);
	if (!ReaderContext.isCardPresent())
	{
		return false;
	}
	try
	{
		BEID_EIDCard&		 Card			= ReaderContext.getEIDCard();
		BEID_Certificates&	 certificates	= Card.getCertificates();

		for (size_t CertIdx=0;CertIdx<Card.certificateCount();CertIdx++)
		{
			BEID_Certificate&	 cert			= certificates.getCertFromCard(CertIdx);
			const BEID_ByteArray certData		= cert.getCertData();

			// ----------------------------------------------------
			// create the certificate context with the certificate raw data
			// ----------------------------------------------------
			pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());

			if( pCertContext )
			{
				unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
				CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

				// ----------------------------------------------------
				// Only store the context of the certificates with usages for an end-user 
				// i.e. no CA or root certificates
				// ----------------------------------------------------
				if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
				{
					if(StoreAuthorityCerts (pCertContext, KeyUsageBits, readerName))                     
					{
						bImported = true;
					}
				}
				else
				{
					if(StoreUserCerts (Card, pCertContext, KeyUsageBits, cert, readerName))
					{
						bImported = true;
					}
				}
				pCertContext = NULL;
			}
		}
	}
	catch (BEID_Exception& e)
	{
		long err = e.GetError();
		err = err;
		QString strCaption(tr("Retrieving certificates"));
		QString strMessage(tr("Error retrieving certificates"));
		QMessageBox::information(NULL,strCaption,strMessage);
	}

//	showCertImportMessage(bImported);

	return bImported;
#else
	return true;
#endif
}

//*****************************************************
// Show a messagebox that the certificate import was successful
//*****************************************************
void MainWnd::showCertImportMessage(bool bImported)
{
#ifdef WIN32
	QString caption(tr("Register certificate"));
	QString message(tr("Registration of certificate "));
	if (bImported)
	{
		message += tr("successful");
	}
	else
	{
		message += tr("failed");
	}
	QMessageBox::information(0,caption,message);
#endif
}

//*****************************************************
// Import the selected certificate
//*****************************************************
bool MainWnd::ImportSelectedCertificate( void )
{
#ifdef WIN32
	PCCERT_CONTEXT	pCertContext = NULL;
	QString			strTip;
	bool			bImported	= false;

	QTreeWidgetItemIterator it(m_ui.treeCert);

	while (*it) 
	{
		// ----------------------------------------------------
		// only if certificate is selected, store it
		// ----------------------------------------------------
		if ( (*it)->isSelected() )
		{
			unsigned long CertIdx = getCertificateIndex( (*it)->text(0) );

			if (CertIdx != -1)
			{
				BEID_ReaderContext&  ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
				BEID_EIDCard&		 Card			= ReaderContext.getEIDCard();
				BEID_Certificates&	 certificates	= Card.getCertificates();
				BEID_Certificate&	 cert			= certificates.getCertFromCard(CertIdx);
				const BEID_ByteArray certData		= cert.getCertData();

				// ----------------------------------------------------
				// create the certificate context with the certificate raw data
				// ----------------------------------------------------
				pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());

				if( pCertContext )
				{
					unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
					CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

					// ----------------------------------------------------
					// Only store the context of the certificates with usages for an end-user 
					// ----------------------------------------------------
					if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
					{
						if(StoreAuthorityCerts (pCertContext, KeyUsageBits, m_CurrReaderName.toLatin1().data()))                     
						{
							bImported = true;
						}
					}
					else
					{
						if(StoreUserCerts (Card, pCertContext, KeyUsageBits, cert, m_CurrReaderName.toLatin1().data()))
						{

							//now store each time the issuer until we're done
							// an exception is thrown when there is no issuer
							bool bDone = false;
							BEID_Certificate* currCert = &cert;
							while(!bDone)
							{
								try
								{
									BEID_Certificate*	 issuer		= &currCert->getIssuer();
									const BEID_ByteArray certData	= issuer->getCertData();

									pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());
									StoreAuthorityCerts (pCertContext, KeyUsageBits, m_CurrReaderName.toLatin1().data());
									currCert	 = issuer;
								}
								catch (BEID_ExCertNoIssuer& e)
								{
									long err = e.GetError();
									err = err;
									// ok, we're at the end of the tree
									bDone = true;
								}
							}

							bImported = true;
						}
					}
					pCertContext = NULL;
				}
				showCertImportMessage(bImported);
			}
		}
		++it;
	}
	return bImported;
#else
	return true;
#endif
}

//*****************************************************
// store the authority certificates of the card in a specific reader
//*****************************************************
bool MainWnd::StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const char* readerName)
{
#ifdef WIN32
	bool			bRet		 = false;
	HCERTSTORE		hMemoryStore = NULL;   // memory store handle
	PCCERT_CONTEXT  pDesiredCert = NULL;

	if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData
					 , pCertContext->pCertInfo->Subject.pbData
					 , pCertContext->pCertInfo->Subject.cbData
				     )
		)
	{
		hMemoryStore = CertOpenSystemStoreA (NULL, "ROOT");
	}
	else
	{
		hMemoryStore = CertOpenSystemStoreA (NULL, "CA");
	}

	if(NULL != hMemoryStore)
	{
		pDesiredCert = CertFindCertificateInStore( hMemoryStore
												 , X509_ASN_ENCODING
												 , 0
												 , CERT_FIND_EXISTING
												 , pCertContext 
												 , NULL
												 );
		if( pDesiredCert )
		{
			CertFreeCertificateContext(pDesiredCert);
		}
		else
		{
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_SERVER_AUTH);
			if(CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
			{
				m_certContexts[readerName].push_back(pCertContext);
				bRet = true;
			}
		}
		CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}
	return bRet;
#else
	return true;
#endif
}

//*****************************************************
// store the user certificates of the card in a specific reader
//*****************************************************
bool MainWnd::StoreUserCerts (BEID_EIDCard& Card, PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, BEID_Certificate& cert, const char* readerName)
{
#ifdef WIN32
	unsigned long	dwFlags			= CERT_STORE_NO_CRYPT_RELEASE_FLAG;
	PCCERT_CONTEXT  pDesiredCert	= NULL;
	PCCERT_CONTEXT  pPrevCert		= NULL;
	HCERTSTORE		hMyStore		= CertOpenSystemStore(NULL, "MY");

	if ( NULL != hMyStore )
	{
		// ----------------------------------------------------
		// look if we already have a certificate with the same 
		// subject (contains name and NNR) in the store
		// If the certificate is not found --> NULL
		// ----------------------------------------------------
		do
		{
			if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_SUBJECT_NAME, &(pCertContext->pCertInfo->Subject) , pPrevCert)))
			{
				// ----------------------------------------------------
				// If the certificates are identical and function 
				// succeeds, the return value is nonzero, or TRUE.
				// ----------------------------------------------------
				if(NULL == CertCompareCertificate(X509_ASN_ENCODING,pCertContext->pCertInfo,pDesiredCert->pCertInfo) ||
					!ProviderNameCorrect(pDesiredCert) )
				{
					// ----------------------------------------------------
					// certificates are not identical, but have the same 
					// subject (contains name and NNR),
					// so we remove the one that was already in the store
					// ----------------------------------------------------
					if(NULL == CertDeleteCertificateFromStore(pDesiredCert))
					{
						if (E_ACCESSDENIED == GetLastError())
						{
							QString strCaption(tr("Deleting former certificate"));
							QString strMessage(tr("Error deleting former certificate"));
							QMessageBox::information(NULL,strCaption,strMessage);
						}
					}
					pPrevCert = NULL;
					continue;
				}
			}
			pPrevCert = pDesiredCert;
		}while (NULL != pDesiredCert);


		if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
			)
		{
			m_certContexts[readerName].push_back(pCertContext);
			// ----------------------------------------------------
			// certificate is already in the store, then just return
			// ----------------------------------------------------
			CertFreeCertificateContext(pDesiredCert);
			CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
			return true;
		}

		// ----------------------------------------------------
		// Initialize the CRYPT_KEY_PROV_INFO data structure.
		// Note: pwszContainerName and pwszProvName can be set to NULL 
		// to use the default container and provider.
		// ----------------------------------------------------
		CRYPT_KEY_PROV_INFO* pCryptKeyProvInfo	= new CRYPT_KEY_PROV_INFO;
		unsigned long		 dwPropId			= CERT_KEY_PROV_INFO_PROP_ID; 

		// ----------------------------------------------------
		// Get the serial number
		// ----------------------------------------------------
		BEID_CardVersionInfo& versionInfo = Card.getVersionInfo();
		const char*			  pSerialKey  = versionInfo.getSerialNumber();

		QString				  strContainerName;

		if (UseMinidriver())
		{
			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				strContainerName = "NR_";
			}
			else
			{
				strContainerName = "DS_";
			}
			strContainerName += pSerialKey;
			pCryptKeyProvInfo->pwszProvName			= L"Microsoft Base Smart Card Crypto Provider";
			pCryptKeyProvInfo->dwKeySpec			= AT_SIGNATURE;
		}
		else
		{
			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				strContainerName = "Signature";
			}
			else
			{
				strContainerName = "Authentication";
			}

			strContainerName += "(";
			strContainerName += pSerialKey;
			strContainerName += ")";
			pCryptKeyProvInfo->pwszProvName		= L"Belgium Identity Card CSP";
			pCryptKeyProvInfo->dwKeySpec		= AT_KEYEXCHANGE;
		}
		pCryptKeyProvInfo->pwszContainerName	= (LPWSTR)strContainerName.utf16();
		pCryptKeyProvInfo->dwProvType			= PROV_RSA_FULL;
		pCryptKeyProvInfo->dwFlags				= 0;
		pCryptKeyProvInfo->cProvParam			= 0;
		pCryptKeyProvInfo->rgProvParam			= NULL;

		// Set the property.
		if (CertSetCertificateContextProperty(
			pCertContext,       // A pointer to the certificate
			// where the propertiy will be set.
			dwPropId,           // An identifier of the property to be set. 
			// In this case, CERT_KEY_PROV_INFO_PROP_ID
			// is to be set to provide a pointer with the
			// certificate to its associated private key 
			// container.
			dwFlags,            // The flag used in this case is   
			// CERT_STORE_NO_CRYPT_RELEASE_FLAG
			// indicating that the cryptographic 
			// context aquired should not
			// be released when the function finishes.
			pCryptKeyProvInfo   // A pointer to a data structure that holds
			// infomation on the private key container to
			// be associated with this certificate.
			))
		{
			if (NULL != pCryptKeyProvInfo)
			{
				delete pCryptKeyProvInfo;
				pCryptKeyProvInfo = NULL;
			}
			// Set friendly names for the certificates
			CRYPT_DATA_BLOB tpFriendlyName	= {0, 0};
			unsigned long	ulID			= 0;

			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				ulID = 0x03;
			}
			else
			{
				ulID = 0x02;
			}

			QString			strFriendlyName;
			strFriendlyName = QString::fromUtf8(cert.getOwnerName());
			int iFriendLen = (strFriendlyName.length() + 1) * sizeof(QChar);

			tpFriendlyName.pbData = new unsigned char[iFriendLen];

			memset(tpFriendlyName.pbData, 0, iFriendLen);
			memcpy(tpFriendlyName.pbData, strFriendlyName.utf16(), iFriendLen - sizeof(QChar));

			tpFriendlyName.cbData = iFriendLen;

			if (CertSetCertificateContextProperty(
				pCertContext,       // A pointer to the certificate
				// where the propertiy will be set.
				CERT_FRIENDLY_NAME_PROP_ID,           // An identifier of the property to be set. 
				// In this case, CERT_KEY_PROV_INFO_PROP_ID
				// is to be set to provide a pointer with the
				// certificate to its associated private key 
				// container.
				dwFlags,            // The flag used in this case is   
				// CERT_STORE_NO_CRYPT_RELEASE_FLAG
				// indicating that the cryptographic 
				// context aquired should not
				// be released when the function finishes.
				&tpFriendlyName   // A pointer to a data structure that holds
				// infomation on the private key container to
				// be associated with this certificate.
				))
			{
				if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
				{
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
				}
				else
				{
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_CLIENT_AUTH);
				}
				CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL);
				m_certContexts[readerName].push_back(pCertContext);
			}

			if (NULL != tpFriendlyName.pbData)
			{
				delete [] (tpFriendlyName.pbData);
				tpFriendlyName.pbData = NULL;
			}
		}
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMyStore = NULL;
	}
#endif
	return true;
}

//*****************************************************
// button to check certificates clicked
//*****************************************************
void MainWnd::on_btnOCSPCheck_clicked( void )
{
	//----------------------------------------------------
	// get the selected certificate and get the OCSP response
	//----------------------------------------------------
	try
	{
		m_ui.btnOCSPCheck->setEnabled(false);
		m_ui.btnOCSPCheck->repaint();		//Need to be call explicitly else the repaint comes too late
		QTreeWidgetItemIterator it(m_ui.treeCert);
		while (*it) 
		{
			if ( (*it)->isSelected() )
			{
				QString CertLabel = (*it)->text(0);
				BEID_Certificates* pCerts = m_CI_Data.m_CertifInfo.getCertificates();

				for (unsigned long idx=0; idx<pCerts->countFromCard();idx++)
				{
					BEID_Certificate& cert = pCerts->getCertFromCard(idx);
					QString lbl = cert.getLabel();
					if (lbl == CertLabel)
					{
						QString strMessage(tr("Checking certificate OCSP status"));
						m_ui.statusBar->showMessage(strMessage,m_STATUS_MSG_TIME);

						BEID_CertifStatus status = cert.getStatus(BEID_VALIDATION_LEVEL_NONE, BEID_VALIDATION_LEVEL_MANDATORY);
						((QTreeCertItem*)(*it))->setOcspStatus(status);

						strMessage = tr("Done");
						m_ui.statusBar->showMessage(strMessage,m_STATUS_MSG_TIME);

						QString strCertStatus = "";
						getCertStatusText(status, strCertStatus);
						m_ui.txtCert_Status->setText(strCertStatus);
						m_ui.txtCert_Status->setAccessibleName(strCertStatus);
					}
				}
			}
			it++;
		}
		m_ui.btnOCSPCheck->setEnabled(true);
	}
	catch (BEID_ExNoCardPresent& e)
	{
		e = e;
		QString strMessage(tr("No card found"));
		m_ui.statusBar->showMessage(strMessage,m_STATUS_MSG_TIME);
	}
	catch (BEID_Exception& e)
	{
		e = e;
	}
}

//*****************************************************
// button to register certificate clicked
//*****************************************************
void MainWnd::on_btnCert_Register_clicked( void )
{
#ifdef WIN32
	ImportSelectedCertificate();
#endif
}

//*****************************************************
// get the index of a certificate starting from its label
//*****************************************************
unsigned long MainWnd::getCertificateIndex(QString const& CertLabel )
{
	try
	{
		BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		//------------------------------------
		// make always sure a card is present
		//------------------------------------
		if (ReaderContext.isCardPresent())
		{
			BEID_EIDCard&		Card		= ReaderContext.getEIDCard();
			BEID_Certificates&	certificates= Card.getCertificates();

			//------------------------------------
			// find the certificate with this label
			//------------------------------------
			for (unsigned long CertIdx=0; CertIdx<certificates.countFromCard(); CertIdx++)
			{
				BEID_Certificate&	cert			= certificates.getCertFromCard(CertIdx);
				QString				CurrCertLabel	= QString::fromUtf8(cert.getLabel());

				if (CurrCertLabel==CertLabel)
				{
					return CertIdx;
				}
			}
		}
	}
	catch (BEID_Exception& e)
	{
		unsigned long err = 0;
		err = e.GetError();
		QString msg = tr("Card not present");
	}
	return -1;
}

//****************************************************
// Show certificate details (only for Windows)
// The certificate data is retrieved from the card and
// passed to the Windows dialog that displays the details
//****************************************************
void MainWnd::on_btnCert_Details_clicked( void )
{
#if WIN32
	QTreeWidgetItemIterator it(m_ui.treeCert);
	while (*it) 
	{
		if ( (*it)->isSelected() )
		{
			QString CertLabel = (*it)->text(0);
			try
			{
				BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

				//------------------------------------
				// make always sure a card is present
				//------------------------------------
				if (ReaderContext.isCardPresent())
				{
					BEID_EIDCard&		Card		= ReaderContext.getEIDCard();
					BEID_Certificates&	certificates= Card.getCertificates();

					//------------------------------------
					// find the certificate with this label
					//------------------------------------
					for (size_t CertIdx=0; CertIdx<certificates.countFromCard(); CertIdx++)
					{
						BEID_Certificate&	cert			= certificates.getCertFromCard(CertIdx);
						QString				CurrCertLabel	= QString::fromUtf8(cert.getLabel());

						if (CurrCertLabel==CertLabel)
						{
							const BEID_ByteArray certData = cert.getCertData();

							CRYPTUI_VIEWCERTIFICATE_STRUCT tCert = {0};
							tCert.dwSize		= sizeof(CRYPTUI_VIEWCERTIFICATE_STRUCT);
							tCert.hwndParent	= this->winId();
							tCert.dwFlags		= CRYPTUI_DISABLE_EDITPROPERTIES;
							tCert.pCertContext	= CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());
							BOOL bChange		= FALSE;
							CryptUIDlgViewCertificate(&tCert, &bChange);
							CertFreeCertificateContext (tCert.pCertContext);
						}
					}
				}
			}
			catch (BEID_Exception e)
			{

			}
		}
		++it;
	} 
#endif
}

//*****************************************************
// PIN item selection changed
//*****************************************************
void MainWnd::on_treePIN_itemSelectionChanged ( void )
{
	QList<QTreeWidgetItem *> selectedItems = m_ui.treePIN->selectedItems();
	if (selectedItems.size()==1)
	{
		setEnabledPinButtons(true);
	}
	else
	{
		setEnabledPinButtons(false);
	}
}

//*****************************************************
// a certificate has been selected
//*****************************************************
void MainWnd::on_treeCert_itemSelectionChanged ( void )
{
 	QList<QTreeWidgetItem *> selectedItems = m_ui.treeCert->selectedItems();
 	if (selectedItems.size()==1)
 	{
 		on_treeCert_itemClicked((QTreeCertItem*)selectedItems[0], 0);
 	}
	
}
//*****************************************************
// a certificate has been clicked
//*****************************************************
void MainWnd::on_treeCert_itemClicked(QTreeWidgetItem* baseItem, int column)
{
	QTreeCertItem* item=(QTreeCertItem*)baseItem;
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

	QString CertLabel = item->text(column);

	//------------------------------------
	// fill in the GUI
	//------------------------------------
	m_ui.txtCert_Issuer->setText( item->getIssuer());
	m_ui.txtCert_Issuer->setAccessibleName( item->getIssuer());
	m_ui.txtCert_Owner->setText( item->getOwner());
	m_ui.txtCert_Owner->setAccessibleName( item->getOwner());
	m_ui.txtCert_ValidFrom->setText(item->getValidityBegin());
	m_ui.txtCert_ValidFrom->setAccessibleName(item->getValidityBegin());
	m_ui.txtCert_ValidUntil->setText(item->getValidityEnd());
	m_ui.txtCert_ValidUntil->setAccessibleName(item->getValidityEnd());
	m_ui.txtCert_KeyLenght->setText(item->getKeyLen());
	m_ui.txtCert_KeyLenght->setAccessibleName(item->getKeyLen());

	QString strCertStatus;
	getCertStatusText(item->getOcspStatus(), strCertStatus);
	m_ui.txtCert_Status->setText(strCertStatus);
	m_ui.txtCert_Status->setAccessibleName(strCertStatus);

	if(!ReaderContext.isCardPresent())
	{
		m_ui.btnCert_Register->setEnabled(false);
		m_ui.btnCert_Details->setEnabled(false);
		m_ui.btnOCSPCheck->setEnabled(false);
		m_ui.txtCert_Status->setEnabled(false);
	}
	else if ( 0 < item->childCount())
	{
		m_ui.btnCert_Register->setEnabled(false);
		m_ui.btnCert_Details->setEnabled(true);
		m_ui.btnOCSPCheck->setEnabled(false);
		m_ui.txtCert_Status->setEnabled(false);
	}
	else
	{
		m_ui.btnCert_Register->setEnabled(true);
		m_ui.btnCert_Details->setEnabled(true);
		m_ui.btnOCSPCheck->setEnabled(true);
		m_ui.txtCert_Status->setEnabled(true);
	}

}


//*****************************************************
// A Pin is clicked
//*****************************************************
void MainWnd::on_treePIN_itemClicked(QTreeWidgetItem* item, int column)
{
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	QString PinName = item->text(column);

	BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

	//------------------------------------
	// make always sure a card is present
	//------------------------------------
	if (!ReaderContext.isCardPresent())
	{
		return;
	}
	BEID_EIDCard&	Card	= ReaderContext.getEIDCard();
	BEID_Pins&		Pins	= Card.getPins();

	//------------------------------------
	// find the pin with this label
	//------------------------------------
	for (unsigned long PinIdx=0; PinIdx<Pins.count(); PinIdx++)
	{
		BEID_Pin&	Pin			= Pins.getPinByNumber(PinIdx);
		QString		CurrPinName	= Pin.getLabel();

		if ( CurrPinName == PinName.toUtf8().constData())
		{
			QString PINId;
			PINId = PINId.setNum(Pin.getId());
			QString PINStatus(tr("Not available"));

			//------------------------------------
			// fill in the fields
			//------------------------------------
			m_ui.txtPIN_Name->setText(trUtf8(Pin.getLabel()));
			m_ui.txtPIN_Name->setAccessibleName(CurrPinName);
			m_ui.txtPIN_ID->setText(PINId);
			m_ui.txtPIN_ID->setAccessibleName(PINId);
			m_ui.txtPIN_Status->setText(PINStatus);
			m_ui.txtPIN_Status->setAccessibleName(PINStatus);
			setEnabledPinButtons(true);

			break;
		}
	}
}

//*****************************************************
// Reload EID is clicked
//*****************************************************
void MainWnd::on_actionReload_eID_triggered( void )
{
    	pinactivate = 1;
	m_connectionStatus = (BEID_CertifStatus)-1;
	m_CI_Data.Reset(); 
	loadCardData();
}

//****************************************************
// Callback function used by the Readercontext to notify insertion/removal
// of a card
// The callback comes at:
// - startup
// - insertion of a card
// - removal of a card
// - add/remove of a cardreader
// When a card is inserted we post a custom event to the GUI telling that
// a new card is inserted. A postEvent is called because this function is
// called from another thread than the main GUI thread.
//****************************************************
void cardEventCallback(long lRet, unsigned long ulState, CallBackData* pCallBackData)
{
	g_runningCallback++;
	
	try
	{
		BEID_ReaderContext& readerContext = ReaderSet.getReaderByName(pCallBackData->getReaderName().toLatin1());

		//------------------------------------
		// is card retracted from reader?
		//------------------------------------
		if (!readerContext.isCardPresent())
		{
			if ( pCallBackData->getMainWnd()->getSettings().getRemoveCert())
			{
				//------------------------------------
				// remove the certificates
				//------------------------------------
				MainWnd::RemoveCertificates(pCallBackData->getReaderName() );
			}
			PopupEvent* event = new PopupEvent(pCallBackData->getReaderName(),PopupEvent::ET_CARD_REMOVED);
			QCoreApplication::postEvent(pCallBackData->getMainWnd(),event);
			
			g_runningCallback--;
			return;
		}
		//------------------------------------
		// is card inserted ?
		//------------------------------------
		if (readerContext.isCardChanged(pCallBackData->m_cardID))
		{
			//------------------------------------
			// send an event to the main app to show the picture
			// we must use the postEvent since this callback function resides in a different thread than the
			// main thread.
			//------------------------------------

			PopupEvent* event = new PopupEvent(pCallBackData->getReaderName(),PopupEvent::ET_CARD_CHANGED);
			QCoreApplication::postEvent(pCallBackData->getMainWnd(),event);
		}
	}
	catch (BEID_ExBadTransaction& e)
	{
		long err = e.GetError();
		err = err;
	}
	catch (...)
	{
		int x=0;
		x++;
		// we catch ALL exceptions. This is because otherwise the thread throwing the exception stops		
	}
	
	g_runningCallback--;
}

//*****************************************************
// forget all the certificates we kept for all readers
//*****************************************************
void MainWnd::forgetAllCertificates( void )
{
#ifdef WIN32
	bool bRefresh = true;;
	for (unsigned long readerIdx=0; readerIdx<ReaderSet.readerCount(bRefresh); readerIdx++)
	{
		const char* readerName = ReaderSet.getReaderByNum(readerIdx).getName();
		forgetCertificates(readerName);
	}
#endif
}

//*****************************************************
// forget all the certificates we kept for a specific reader
//*****************************************************
void MainWnd::forgetCertificates( QString const& reader )
{
	char readerName[256];
	readerName[0]=0;
	if (reader.length()>0)
	{
		strcpy(readerName,reader.toUtf8().data());
	}
#ifdef WIN32
	while ( 0 < m_certContexts[readerName].size() )
	{
		PCCERT_CONTEXT pContext = m_certContexts[readerName][m_certContexts[readerName].size()-1];
		CertFreeCertificateContext(pContext);
		m_certContexts[readerName].erase(m_certContexts[readerName].end()-1);
	}
#endif
}

//*****************************************************
// Ask if it is a test card
//*****************************************************
bool MainWnd::askAllowTestCard( void )
{
	//----------------------------------------------------------------
	// try to lock a mutex.
	// This is done for the case there are 2 or more cards inserted.
	// When 2 cards are inserted simultaneously, the card readers will post
	// each a custom event. The first event coming in will check if it is
	// a testcard and end up here. The messagebox will pop up and wait for either OK or CANCEL.
	// In the meantime, the second custum event will be processed by the main messageloop.
	// As a consequence, the card will also be tested to see if it is a testcard.
	// That means that we end up here again and a second messagebox is popped up.
	// To avoid 2 messageboxes to be popped up, we use a mutex.
	// The first request to allow a testcard will test-and-lock the mutex and popup the messagebox.
	// The second request will test the mutex, but will be unable to lock the mutex and return.
	// No second messagebox is popped up
	//----------------------------------------------------------------

	if ( !m_mutex.tryLock () )
	{	
		return false;
	}
	bool    bAllowTestCard = false;
	QString strCaption     = tr("Belgian EID Middleware");
	QString strMessage     = tr("The Root Certificate is not correct.\nThis may be a test card.\n\nDo you want to accept it?");

	if (QMessageBox::Yes == QMessageBox::warning(this,strCaption,strMessage,QMessageBox::Yes|QMessageBox::No))
	{
		bAllowTestCard = true;
	}
	m_mutex.unlock();
	return bAllowTestCard;
}

//*****************************************************
// remove the virtual reader
//*****************************************************
void MainWnd::releaseVirtualReader( void )
{
	if (m_virtReaderContext)
	{
		delete m_virtReaderContext;
		m_virtReaderContext = NULL;
	}
}
//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardData( void )
{
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;
		
		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		BEID_CardType lastFoundCardType = BEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			BEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				BEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case BEID_CARDTYPE_FOREIGNER:
				case BEID_CARDTYPE_KIDS:
				case BEID_CARDTYPE_EID:
				{
						try
						{
							BEID_EIDCard& Card = ReaderContext.getEIDCard();
							if (Card.isTestCard()&&!Card.getAllowTestCard())
							{
								if (askAllowTestCard())
								{
									Card.setAllowTestCard(true);
								}
								else
								{
									on_actionClear_triggered();
									break;
								}
							}
							const char* readerName = ReaderSet.getReaderName(ReaderIdx);
							m_CurrReaderName = readerName;
							Show_Identity_Card(Card);
							//Show_Address_Card(Card);
	
							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_SIS:
					{
						try
						{
							BEID_MemoryCard& Card = ReaderContext.getSISCard();
							m_CurrReaderName = ReaderSet.getReaderName(ReaderIdx);
							Show_Memory_Card(Card);
							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != BEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == BEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowBEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (BEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardBadType e) 
	{
		QString msg(tr("Bad card type"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowBEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (BEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowBEIDError( e.GetError(), msg );
	}
}


//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardDataAddress( void )
{
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		BEID_CardType lastFoundCardType = BEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			BEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				BEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case BEID_CARDTYPE_FOREIGNER:
				case BEID_CARDTYPE_KIDS:
				case BEID_CARDTYPE_EID:
				{
						try
						{
							BEID_EIDCard& Card = ReaderContext.getEIDCard();
							if (Card.isTestCard()&&!Card.getAllowTestCard())
							{
								if (askAllowTestCard())
								{
									Card.setAllowTestCard(true);
								}
								else
								{
									on_actionClear_triggered();
									break;
								}
							}
							const char* readerName = ReaderSet.getReaderName(ReaderIdx);
							m_CurrReaderName = readerName;
							Show_Address_Card(Card);

							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_SIS:
					{
						try
						{
							BEID_MemoryCard& Card = ReaderContext.getSISCard();
							m_CurrReaderName = ReaderSet.getReaderName(ReaderIdx);
							Show_Memory_Card(Card);
							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != BEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == BEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowBEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (BEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardBadType e)
	{
		QString msg(tr("Bad card type"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowBEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (BEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowBEIDError( e.GetError(), msg );
	}
}

//*****************************************************
// load the card data
//*****************************************************
bool MainWnd::loadCardDataPersoData( void )
{
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		BEID_CardType lastFoundCardType = BEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			BEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				BEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case BEID_CARDTYPE_FOREIGNER:
				case BEID_CARDTYPE_KIDS:
				case BEID_CARDTYPE_EID:
				{
						try
						{
							BEID_EIDCard& Card = ReaderContext.getEIDCard();
							if (Card.isTestCard()&&!Card.getAllowTestCard())
							{
								if (askAllowTestCard())
								{
									Card.setAllowTestCard(true);
								}
								else
								{
									on_actionClear_triggered();
									break;
								}
							}
							const char* readerName = ReaderSet.getReaderName(ReaderIdx);
							m_CurrReaderName = readerName;
							Show_PersoData_Card(Card);

							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_SIS:
					{
						try
						{
							BEID_MemoryCard& Card = ReaderContext.getSISCard();
							m_CurrReaderName = ReaderSet.getReaderName(ReaderIdx);
							Show_Memory_Card(Card);
							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != BEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == BEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowBEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (BEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardBadType e)
	{
		QString msg(tr("Bad card type"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowBEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (BEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowBEIDError( e.GetError(), msg );
	}
	return true;
}

//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardDataCertificates( void )
{
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		BEID_CardType lastFoundCardType = BEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			BEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				BEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case BEID_CARDTYPE_FOREIGNER:
				case BEID_CARDTYPE_KIDS:
				case BEID_CARDTYPE_EID:
				{
						try
						{
							BEID_EIDCard& Card = ReaderContext.getEIDCard();
							if (Card.isTestCard()&&!Card.getAllowTestCard())
							{
								if (askAllowTestCard())
								{
									Card.setAllowTestCard(true);
								}
								else
								{
									on_actionClear_triggered();
									break;
								}
							}
							const char* readerName = ReaderSet.getReaderName(ReaderIdx);
							m_CurrReaderName = readerName;
							Show_Certificates_Card(Card);

							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_SIS:
					{
						try
						{
							BEID_MemoryCard& Card = ReaderContext.getSISCard();
							m_CurrReaderName = ReaderSet.getReaderName(ReaderIdx);
							Show_Memory_Card(Card);
							ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
						}
						catch (BEID_ExCardBadType const& e)
						{
							QString errcode;
							errcode = errcode.setNum(e.GetError());
						}
					}
					break;
				case BEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != BEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == BEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowBEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (BEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardBadType e)
	{
		QString msg(tr("Bad card type"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowBEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (BEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (BEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowBEIDError( e.GetError(), msg );
	}
}
//*****************************************************
// Test PIN clicked
//*****************************************************
void MainWnd::on_btnPIN_Test_clicked()
{
	on_actionPINRequest_triggered();
}

//*****************************************************
// Change PIN clicked
//*****************************************************
void MainWnd::on_btnPIN_Change_clicked()
{
	on_actionPINChange_triggered();
}

//*****************************************************
// Open a EID file clicked
//*****************************************************
void MainWnd::on_actionOpen_eID_triggered( void )
{
	QString caption(tr("Open eID"));
	caption = caption.remove(QChar('&'));
	m_ui.statusBar->showMessage(caption);

	QString			baseDir=m_Settings.getDefSavePath();
#ifdef WIN32
	if (0==baseDir.length())
	{
		TCHAR strPath[ MAX_PATH ];
		SHGetSpecialFolderPath( 0				// Hwnd
			,strPath			// String buffer.
			,CSIDL_PERSONAL  // CSLID of folder (CSIDL_PERSONAL)
			,FALSE			// Create if doesn't exists?
			);
		baseDir = strPath;
	}
#else
	if (0==baseDir.length())
	{
		baseDir		 = QDir::homePath();
	}
#endif

	QString		fileName = QFileDialog::getOpenFileName( this, caption, baseDir, tr("eID bin Files (*.eid);;eID XML files (*.xml);;eID CSV files (*.csv);;all files (*.*)") );

	if ( 0 == fileName.length())
	{
		return;
	}
	OpenSelectedEid( fileName );
}
//*****************************************************
// Open the selected EID file
//*****************************************************
void MainWnd::OpenSelectedEid( const QString& fileName )
{
	QString caption(tr("Open eID"));
	caption = caption.remove(QChar('&'));

	QFile		eidFile(fileName);
	QFileInfo	fileInfo(eidFile);

	if (fileInfo.isReadable())
	{
		QString		  fileSuffix    = fileInfo.completeSuffix();
		BEID_FileType fileType		= BEID_FILETYPE_UNKNOWN;

		if ( "xml" == fileSuffix.toLower() )
		{
			fileType = BEID_FILETYPE_XML;
		}
		else if ( "csv" == fileSuffix.toLower() )
		{
			fileType = BEID_FILETYPE_CSV;
		}
		else if ( "eid" == fileSuffix.toLower() )
		{
			fileType = BEID_FILETYPE_TLV;
		}
		else
		{
			QString strCaption(caption);
			QString strMessage(tr("Incorrect file extension.\nPlease specify .xml, .csv or .eid"));
			QMessageBox::information(this,strCaption,strMessage);
			return;
		}

		m_ui.statusBar->showMessage(caption+": "+fileName);
		try
		{
			releaseVirtualReader();
			m_virtReaderContext = new BEID_ReaderContext(fileType,fileName.toLatin1());
			if(m_virtReaderContext->isCardPresent())
			{
				BEID_CardType cardType = m_virtReaderContext->getCardType();

				switch(cardType)
				{
				case BEID_CARDTYPE_EID:
				case BEID_CARDTYPE_KIDS:
				case BEID_CARDTYPE_FOREIGNER:
					{
						BEID_EIDCard& card = m_virtReaderContext->getEIDCard();
						if (card.isTestCard())
						{
							if (!askAllowTestCard())
							{
								break;
							}
							card.setAllowTestCard(true);
						}
						m_CI_Data.Reset();
						Show_Identity_Card(card);
					}
					break;
				case BEID_CARDTYPE_SIS:
					{
						BEID_SISCard& card = m_virtReaderContext->getSISCard();
						m_CI_Data.Reset();
						Show_Memory_Card(card);
					}
					break;
				case BEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
				enablePrintMenu();
				enableFileSave(false);
//				enableFileMenu();
			}
			else
			{
				QString caption(tr("Warning"));
				QString msg(tr("Failed to read eID file"));
				QMessageBox::warning( this, caption,  msg, QMessageBox::Ok );
			}
		}
		catch (BEID_Exception& e)
		{
			long err = e.GetError();
			err = err;
		}
	}
	else
	{
		QString caption(tr("Warning"));
		QString msg(tr("Failed to open eID file"));
		QMessageBox::warning( this, caption,  msg, QMessageBox::Ok );
	}
	m_ui.statusBar->showMessage(tr("Done"));
}

//*****************************************************
// Save EID file clicked
// We must check if we have been working with a file or a real
// card.
//*****************************************************
void MainWnd::on_actionSave_eID_triggered( void )
{
	QString caption(tr("Save eID"));
	caption = caption.remove(QChar('&'));
	//------------------------------------
	// default filename is national number (both SIS and EID contain this number)
	//------------------------------------

	BEID_ReaderContext* pReaderContext = NULL;

	if ( 0 < m_CurrReaderName.length() )
	{
		BEID_ReaderContext &readerContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		pReaderContext = &readerContext;
	}
	else if ( NULL != m_virtReaderContext )
	{
		pReaderContext = m_virtReaderContext;
	}
	else
	{
		return;
	}
	//------------------------------------
	// make always sure a card is present
	//------------------------------------
	if (pReaderContext->isCardPresent())
	{
		m_ui.statusBar->showMessage(caption,m_STATUS_MSG_TIME);
		BEID_Card&		card		 = pReaderContext->getCard();
		BEID_CardType	cardType	 = pReaderContext->getCardType();
		QString			baseFilename = QDir::toNativeSeparators( createBaseFilename( cardType ));;
		QString			baseDir		 = m_Settings.getDefSavePath();
#ifdef WIN32
		if (0==baseDir.length())
		{
			TCHAR strPath[ MAX_PATH ];
			SHGetSpecialFolderPath( 0				// Hwnd
				,strPath			// String buffer.
				,CSIDL_PERSONAL  // CSLID of folder (CSIDL_PERSONAL)
				,FALSE			// Create if doesn't exists?
				);
			baseDir = strPath;
		}
#else
		if (0==baseDir.length())
		{
			baseDir		 = QDir::homePath();
		}
#endif

		if ( baseFilename.size()>0 )
		{
			QString saveStatus = tr("Failed");
			baseFilename.append(".eid");
			QString	targetFile	 = baseDir+"/"+baseFilename;
			if (saveCardDataToFile( targetFile , card ))
			{
				saveStatus = tr("Done");
				QMessageBox::information(NULL,caption, caption + ": " + targetFile + " " + saveStatus);
			}
			m_ui.statusBar->showMessage(caption + ": " + targetFile + " " + saveStatus,m_STATUS_MSG_TIME);
		}
	}
	else
	{
		QString msg(tr("No card present"));
		QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
	}
}

//*****************************************************
// Save EID as... clicked
// We must check if we have been working with a file or a real
// card.
//*****************************************************
void MainWnd::on_actionSave_eID_as_triggered()
{
	QString caption(tr("Save eID as"));
	BEID_ReaderContext* pReaderContext = NULL;

	if ( 0 < m_CurrReaderName.length() )
	{
		BEID_ReaderContext &readerContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		pReaderContext = &readerContext;
	}
	else if ( NULL != m_virtReaderContext )
	{
		pReaderContext = m_virtReaderContext;
	}
	else
	{
		return;
	}

	//------------------------------------
	// make always sure a card is present
	//------------------------------------
	if (pReaderContext->isCardPresent())
	{
		m_ui.statusBar->showMessage(caption,m_STATUS_MSG_TIME);
		BEID_CardType	cardType	 = pReaderContext->getCardType();
		BEID_Card&		card		 = pReaderContext->getCard();
		QString			baseFilename = createBaseFilename(cardType);
		QString			baseDir		 = m_Settings.getDefSavePath();
#ifdef WIN32
		if(0==baseDir.length())
		{
			TCHAR strPath[ MAX_PATH ];
			SHGetSpecialFolderPath( 0				// Hwnd
				,strPath			// String buffer.
				,CSIDL_PERSONAL  // CSLID of folder (CSIDL_PERSONAL)
				,FALSE			// Create if doesn't exists?
				);
			baseDir = strPath;
		}
#else
		if(0==baseDir.length())
		{
			baseDir		 = QDir::homePath();
		}
#endif
		if ( baseFilename.size()>0 )
		{
			//------------------------------------
			// filename must contain an extension or the file-exist detection of the dialog will not work properly
			//------------------------------------
			QString		targetFile   = QDir::toNativeSeparators(baseDir+"/"+baseFilename+".eid");
			QString		selectedFilter;
			QStringList	fileNames;
			QFileDialog dialog(this, caption, targetFile, tr("eID bin Files (*.eid);;eID XML files (*.xml);;eID CSV files (*.csv)"));
			dialog.setAcceptMode(QFileDialog::AcceptSave);
			dialog.setDefaultSuffix("eid");
			dialog.setOption(QFileDialog::DontUseNativeDialog);
			dialog.setDirectory(baseDir);
			QList<QUrl> urls;
			urls << QUrl::fromLocalFile(baseDir);
			dialog.setSidebarUrls(urls);

			if (!dialog.exec())
			{
				return;
			}

			QDir dir = dialog.directory();
			fileNames = dialog.selectedFiles();
			QString fileName = fileNames.at(0);

			if (fileName.length()>0)
			{
				QFileInfo fileInfo(dir.absolutePath(),fileName);
				fileName = fileInfo.baseName();
				selectedFilter = dialog.selectedNameFilter();
				if (selectedFilter.contains("*.eid"))
				{
					fileName += ".eid";
				}
				else if (selectedFilter.contains("*.xml"))
				{
					fileName += ".xml";
				}
				else if (selectedFilter.contains("*.csv"))
				{
					fileName += ".csv";
				}
				else
				{
					// this should never happen
				}
				fileInfo.setFile(dir.absolutePath(),fileName);
				QString saveStatus = tr("Failed");
				if (saveCardDataToFile( fileInfo.filePath(), card ))
				{
					QString savePath = fileInfo.absolutePath();
					m_Settings.setDefSavePath(savePath);
					saveStatus = tr("Done");
				}
				m_ui.statusBar->showMessage(caption + ": " + fileName + " " + saveStatus,m_STATUS_MSG_TIME);
				QMessageBox::information(NULL,caption, caption + ": " + fileInfo.filePath() + " " + saveStatus);
			}
		}
	}
	else
	{
		QString msg(tr("No card present"));
		QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
	}
}

//*****************************************************
// create a file name for the EID card data to store.
// The filename is basically the national number with an extension.
// For the SIS card, '_sis' is added to make the difference with
// the EID card data.
//*****************************************************
QString MainWnd::createBaseFilename( BEID_CardType const& cardType )
{
	QString baseFilename;

	switch(cardType)
	{
	case BEID_CARDTYPE_SIS:
		{
			tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.m_PersonExtraInfo.getFields();
			baseFilename = PersonFields[SOCIALSECURITYNUMBER];
			baseFilename.append("_sis");
		}
		break;
	case BEID_CARDTYPE_EID:
	case BEID_CARDTYPE_KIDS:
	case BEID_CARDTYPE_FOREIGNER:
		{
			tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();
			baseFilename = PersonFields[NATIONALNUMBER];
		}
	default:
		break;
	}
	return baseFilename;
}

//*****************************************************
// Save the card data to the file
//*****************************************************
bool MainWnd::saveCardDataToFile(QString const& fileName, BEID_Card& card )
{
	QString strCaption(tr("Save eID"));
	bool			bRet	 = false;
	QFileInfo		fileInfo(fileName);
	QString			fileSuffix = fileInfo.completeSuffix();

	BEID_ByteArray* fileData	= NULL;

	try
	{
		BEID_XMLDoc&	doc		 = card.getDocument(BEID_DOCTYPE_FULL);

		if ("xml" == fileSuffix.toLower())
		{
			fileData = new BEID_ByteArray(doc.getXML());
		}
		else if ("csv" == fileSuffix.toLower())
		{
			fileData = new BEID_ByteArray(doc.getCSV());
		}
		else if ("eid" == fileSuffix.toLower())
		{
			fileData = new BEID_ByteArray(doc.getTLV());
		}
		else
		{
			QString strMessage(tr("Incorrect file extension.\nPlease specify .xml, .csv or .eid"));
			QMessageBox::information(this,strCaption,strMessage);
			bRet = false;
			return bRet;
		}
	}
	catch(BEID_Exception &e)
	{
		e=e;
		if (fileData)
		{
			delete fileData;
			fileData = NULL;
		}
		QString strMessage(tr("Error writing file"));
		QMessageBox::information(this,strCaption,strMessage);
		bRet = false;
		return bRet;
	}

	if (fileData)
	{
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
		{
			QString msg(tr("Error opening file"));
			QMessageBox::information(this,strCaption,msg+": "+fileName);
			bRet = false;
		}
		else if ( -1 == file.write((const char*)fileData->GetBytes(),fileData->Size()))
		{
			QString msg(tr("Error writing file"));
			QMessageBox::information(this,strCaption,msg+": "+fileName);
			bRet = false;
		}
		else
		{
			bRet = true;
		}
		file.close();
		delete fileData;
		fileData = NULL;
	}
	return bRet;
}

//*****************************************************
// About clicked
//*****************************************************
void MainWnd::on_actionAbout_triggered( void )
{
#ifdef WIN32 //version info for Windows
	QFileInfo	fileInfo(m_Settings.getExePath()) ;

	QString filename = QCoreApplication::arguments().at(0);
	CFileVersionInfo VerInfo;
	if(VerInfo.Open(filename.toLatin1()))
	{
		char version[256];
		VerInfo.QueryStringValue(VI_STR_FILEVERSION, version);
		m_Settings.setGuiVersion(version);
	}

#else //linux, apple
	QString strVersion (WIN_GUI_VERSION_STRING);
	m_Settings.setGuiVersion(strVersion);
#endif
	dlgAbout * dlg = new dlgAbout( m_Settings.getGuiVersion() , this);
	dlg->exec();
	delete dlg;
}

//*****************************************************
// Options clicked
//*****************************************************
void MainWnd::on_actionOptions_triggered(void)
{
	if( this->isHidden() )
	{
		this->showNormal(); // Otherwise the application will end if the options dialog gets closed
	}

	dlgOptions* dlg = new dlgOptions( m_Settings, this );

	dlg->setUseKeyPad( m_Settings.getUseVirtualKeyPad() );
	dlg->setShowToolbar( m_Settings.getShowToolbar() );
	dlg->setShowPicture( m_Settings.getShowPicture() );
	dlg->setShowNotification( m_Settings.getShowNotification() );

	m_ui.actionOptions->setEnabled(false);

	if( dlg->exec() )
	{
		m_UseKeyPad = dlg->getUseKeyPad();
		m_ui.actionShow_Toolbar->setChecked( m_Settings.getShowToolbar() );

		if( !m_ui.txtIdentity_Name->text().isEmpty() )
			m_ui.lblIdentity_ImgPerson->setPixmap( m_imgPicture );
//			m_ui.lblForeigners_ImgPerson->setPixmap( m_imgPicture );

	}
	delete dlg;

	m_ui.actionOptions->setEnabled(true);
}

//*****************************************************
// Print clicked
//*****************************************************
void MainWnd::on_actionPrint_eID_triggered()
{
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();
	QString cardTypeText = GetCardTypeText(CardFields[CARD_TYPE]);

	dlgPrint* dlg = new dlgPrint( this, m_CI_Data, m_Language, cardTypeText);
	dlg->exec();
	delete dlg;
}

void MainWnd::on_actionPrinter_Settings_triggered()
{
	QPrintDialog* pPrintDialog = new QPrintDialog( m_pPrinter, this );
	pPrintDialog->exec();
	delete pPrintDialog;
}

//*****************************************************
// Authentication Pin request
//*****************************************************
void MainWnd::authPINRequest_triggered()
{
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	try
	{
		BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		//------------------------------------
		// make always sure a card is present
		//------------------------------------
		QString		  caption(tr("Identity Card: PIN verification"));

		if (ReaderContext.isCardPresent())
		{
			//QString PinName = m_ui.txtPIN_Name->text();
			QString PinName = "PIN da Autenticação";
			if (PinName.length()==0)
			{
				//--------------------------
				// no pin selected in tree
				//--------------------------
				return;
			}

			BEID_EIDCard&	Card	= ReaderContext.getEIDCard();
			BEID_Pins&		Pins	= Card.getPins();

			for (unsigned long PinIdx=0; PinIdx<Pins.count(); PinIdx++)
			{
				BEID_Pin&	Pin			= Pins.getPinByNumber(PinIdx);
				QString		CurrPinName	= Pin.getLabel();

				if (CurrPinName==PinName)
				{
					unsigned long triesLeft = -1;
					bool		  bResult   = Pin.verifyPin("",triesLeft);
					//QString		  msg(tr("PIN verification "));

					QString msg = bResult?tr("PIN verification passed"):tr("PIN verification failed");
					if (!bResult)
					{
						//--------------------------
						// if it remains to -1, then CANCEL was pressed
						// so, don't give a message
						//--------------------------
						if ((unsigned long)-1!=triesLeft)
						{
							QString nrTriesLeft;
							nrTriesLeft.setNum(triesLeft);
							msg += "\n";
							msg += "( ";
							msg += tr("Number of tries left: ") + nrTriesLeft + " )";
							m_ui.txtPIN_Status->setText(msg);
							m_ui.txtPIN_Status->setAccessibleName(msg);
						}
						else
						{
							break;
						}
					}
 					else
 					{
 						QString nrTriesLeft;
 						nrTriesLeft.setNum(triesLeft);
 						m_ui.txtPIN_Status->setText(tr("Not available"));
						m_ui.txtPIN_Status->setAccessibleName(tr("Not available"));
 					}
					QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
					break;
				}
			}
		}
		else
		{
			QString msg(tr("No card present"));
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
		}
	}
	catch (BEID_Exception &e)
	{
		QString msg(tr("General exception"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowBEIDError( 0, msg );
	}
	return;
}

//*****************************************************
// Address Pin request
//*****************************************************
bool MainWnd::addressPINRequest_triggered()
{
	//Workaround: Make PIN window called only one time
	pinactivate = 0;

	/*if (!m_CI_Data.isDataLoaded())
	{
		return true;
	}*/
	try
	{
		BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		//------------------------------------
		// make always sure a card is present
		//------------------------------------
		QString		  caption(tr("Identity Card: PIN verification"));

		if (ReaderContext.isCardPresent())
		{
			//QString PinName = m_ui.txtPIN_Name->text();
			QString PinName = "PIN da Morada";
			if (PinName.length()==0)
			{
				//--------------------------
				// no pin selected in tree
				//--------------------------
				return false;
			}

			BEID_EIDCard&	Card	= ReaderContext.getEIDCard();
			BEID_Pins&		Pins	= Card.getPins();

			for (unsigned long PinIdx=0; PinIdx<Pins.count(); PinIdx++)
			{
				BEID_Pin&	Pin			= Pins.getPinByNumber(PinIdx);
				QString		CurrPinName	= Pin.getLabel();

				if (CurrPinName==PinName)
				{
					unsigned long triesLeft = -1;
					bool		  bResult   = Pin.verifyPin("",triesLeft);
					//QString		  msg(tr("PIN verification "));

					QString msg = bResult?tr("PIN verification passed"):tr("PIN verification failed");
					if (!bResult)
					{
						//--------------------------
						// if it remains to -1, then CANCEL was pressed
						// so, don't give a message
						//--------------------------
						if ((unsigned long)-1!=triesLeft)
						{
							QString nrTriesLeft;
							nrTriesLeft.setNum(triesLeft);
							msg += "\n";
							msg += "( ";
							msg += tr("Number of tries left: ") + nrTriesLeft + " )";
							m_ui.txtPIN_Status->setText(msg);
							m_ui.txtPIN_Status->setAccessibleName(msg);
						}
						else
						{
							pinactivate = 1;
							return false;
						}
					}
 					else
 					{
 						QString nrTriesLeft;
 						nrTriesLeft.setNum(triesLeft);
 						m_ui.txtPIN_Status->setText(tr("Not available"));
						m_ui.txtPIN_Status->setAccessibleName(tr("Not available"));
 					}
					QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
					break;
				}
			}
		}
		else
		{
			QString msg(tr("No card present"));
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
		}
	}
	catch (BEID_Exception &e)
	{
		QString msg(tr("General exception"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowBEIDError( 0, msg );
	}
	return true;
}

//*****************************************************
// Pin test button clicked
//*****************************************************
void MainWnd::on_actionPINRequest_triggered()
{
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	try
	{
		BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		//------------------------------------
		// make always sure a card is present
		//------------------------------------
		QString		  caption(tr("Identity Card: PIN verification"));

		if (ReaderContext.isCardPresent())
		{
			QString PinNameRaw = m_ui.txtPIN_Name->text();
			QString PinName = PinNameRaw.toUtf8().constData();

			if (PinName.length()==0)
			{
				//--------------------------
				// no pin selected in tree
				//--------------------------
				return;
			}

			BEID_EIDCard&	Card	= ReaderContext.getEIDCard();
			BEID_Pins&		Pins	= Card.getPins();

			for (unsigned long PinIdx=0; PinIdx<Pins.count(); PinIdx++)
			{
				BEID_Pin&	Pin			= Pins.getPinByNumber(PinIdx);
				QString		CurrPinName	= Pin.getLabel();

				if (CurrPinName==PinName)
				{
					unsigned long triesLeft = -1;
					bool		  bResult   = Pin.verifyPin("",triesLeft);
					//QString		  msg(tr("PIN verification "));

					QString msg = bResult?tr("PIN verification passed"):tr("PIN verification failed");
					if (!bResult)
					{
						//--------------------------
						// if it remains to -1, then CANCEL was pressed
						// so, don't give a message
						//--------------------------
						if ((unsigned long)-1!=triesLeft)
						{
							QString nrTriesLeft;
							nrTriesLeft.setNum(triesLeft);
							msg += "\n";
							msg += "( ";
							msg += tr("Number of tries left: ") + nrTriesLeft + " )";
							m_ui.txtPIN_Status->setText(msg);
							m_ui.txtPIN_Status->setAccessibleName(msg);
						}
						else
						{
							break;
						}
					}
 					else
 					{
 						QString nrTriesLeft;
 						nrTriesLeft.setNum(triesLeft);
 						m_ui.txtPIN_Status->setText(tr("Not available"));
						m_ui.txtPIN_Status->setAccessibleName(tr("Not available"));
 					}
					QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
					break;
				}
			}
		}
		else
		{
			QString msg(tr("No card present"));
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
		}
	}
	catch (BEID_Exception &e)
	{
		QString msg(tr("General exception"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowBEIDError( 0, msg );
	}
	return;
}

//*****************************************************
// PIN change button clicked
//*****************************************************
void MainWnd::on_actionPINChange_triggered()
{
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	try
	{
		BEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		QString			caption(tr("Identity Card: PIN change"));

		//------------------------------------
		// make always sure a card is present
		//------------------------------------
		if (ReaderContext.isCardPresent())
		{
			QString PinNameRaw = m_ui.txtPIN_Name->text();
			QString PinName = PinNameRaw.toUtf8().constData();
			if (PinName.length()==0)
			{
				//--------------------------
				// no pin selected in tree
				//--------------------------
				return;
			}

			BEID_EIDCard&	Card	= ReaderContext.getEIDCard();
			BEID_Pins&		Pins	= Card.getPins();

			for (unsigned long PinIdx=0; PinIdx<Pins.count(); PinIdx++)
			{
				BEID_Pin&	Pin			= Pins.getPinByNumber(PinIdx);
				QString		CurrPinName	= Pin.getLabel();

				if (CurrPinName==PinName)
				{
					unsigned long	triesLeft = -1;
					bool			bResult   = Pin.changePin("","",triesLeft, PinName.toStdString().c_str());
					QString			msg(tr("PIN change "));

					msg += bResult?tr("passed"):tr("failed");
					if (!bResult)
					{ 
						//--------------------------
						// if it remains to -1, then CANCEL was pressed
						// so don't give a message
						//--------------------------
						if ((unsigned long)-1 != triesLeft)
						{
							QString nrTriesLeft;nrTriesLeft.setNum(triesLeft);
							msg += tr("\n( Number of tries left: ") + nrTriesLeft + " )";
							m_ui.txtPIN_Status->setText(msg);
							m_ui.txtPIN_Status->setAccessibleName(msg);
						}
						else
						{
							break;
						}
					}

					QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
					break;
				}
			}
		}
		else
		{
			QString msg(tr("No card present"));
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
		}
	}
	catch (BEID_Exception &e)
	{
		QString msg(tr("General exception"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowBEIDError( 0, msg );
	}
}

//


//******************************************************
// Buttons to control tabs
//******************************************************

void MainWnd::on_btnSelectTab_Identity_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(1);
}

void MainWnd::on_btnSelectTab_Identity_Extra_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(2);
}

void MainWnd::on_btnSelectTab_Address_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(3);
	refreshTabAddress();
}

void MainWnd::on_btnSelectTab_Certificates_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(4);
	refreshTabCertificates();
}

void MainWnd::on_btnSelectTab_PinCodes_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(5);
}

void MainWnd::on_btnSelectTab_Notes_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(6);
	refreshTabPersoData();
}












//******************************************************
// hide all the tabs
//******************************************************
void MainWnd::hideTabs()
{
/*	QList<QWidget *> allTabs = m_ui.tabWidget_Identity->findChildren<QWidget *>();
	for (int i = allTabs.size()-1; i >=0; --i) 
	{	
		m_ui.tabWidget_Identity->removeTab(i);		// hide the tab (this is not a delete!!)
	}
	*/
}

void MainWnd::tabaddress_select(int index)
{
	if(index == 2)
		refreshTabAddress();

}

void MainWnd::tabpersodata_select(int index)
{

}

void MainWnd::tabcertificates_select(int index)
{
	if(index == 4)
		refreshTabCertificates();
}

//******************************************************
// Show the tabs 
//******************************************************
void MainWnd::showTabs()
{
	m_ui.stackedWidget->setCurrentIndex(0);

	hideTabs();

	if ( !m_CI_Data.isDataLoaded() )
	{
		return;
	}

	switch (m_TypeCard) 
	{
	case BEID_CARDTYPE_EID:

		if(BEID_EIDCard::isApplicationAllowed())
		{
			//m_ui.tabWidget_Identity->addTab(m_ui.tabIdentity,tr("&Identity"));
			refreshTabIdentity();
//			m_ui.tabWidget_Identity->addTab(m_ui.tabIdentity_extention,tr("Identity E&xtra"));
//			m_ui.tabWidget_Identity->addTab(m_ui.tabAddress,tr("Address"));
//			connect (m_ui.tabWidget_Identity,SIGNAL (currentChanged(int)), this, SLOT(tabaddress_select(int)));
//			m_ui.tabWidget_Identity->addTab(m_ui.tabPersoData,tr("Personal Data"));
//			connect (m_ui.tabWidget_Identity,SIGNAL (currentChanged(int)), this, SLOT(tabpersodata_select(int)));


			refreshTabIdentityExtra();

			//m_ui.lblIdentity_Parents->setVisible(true);
			//m_ui.lblIdentity_Parents_2->setVisible(false);

			//refreshTabAddress();

			//TODO Verify if this is really needed
			//refreshTabPersoData();

			//TODO Remove the background for now until we have a new ones
 			//m_imgBackground_Front = QPixmap( ":/images/Images/Background_IDFront.jpg" ); // background
 			//m_imgBackground_Back = QPixmap( ":/images/Images/Background_IDBack.jpg" ); // background
		}

//		m_ui.tabWidget_Identity->addTab(m_ui.tabCertificates,tr("C&ertificates"));
//		connect (m_ui.tabWidget_Identity,SIGNAL (currentChanged(int)), this, SLOT(tabcertificates_select(int)));
//		m_ui.tabWidget_Identity->addTab(m_ui.tabCardPin,tr("C&ard && PIN"));
		refreshTabCardPin();
		refreshTabInfo();

		break;

	case BEID_CARDTYPE_KIDS:
		if(BEID_EIDCard::isApplicationAllowed())
		{
//			m_ui.tabWidget_Identity->addTab(m_ui.tabIdentity,tr("&Identity"));
			refreshTabIdentity();
//			m_ui.tabWidget_Identity->addTab(m_ui.tabIdentity_extention,tr("Identity E&xtra"));
			refreshTabIdentityExtra();

			//m_ui.lblIdentity_Parents->setVisible(true);
			//m_ui.lblIdentity_Parents_2->setVisible(true);

			m_imgBackground_Front = QPixmap( ":/images/Images/Background_KidsFront.jpg" ); // background
			m_imgBackground_Back = QPixmap( ":/images/Images/Background_KidsBack.jpg" ); // background
		}

//		m_ui.tabWidget_Identity->addTab(m_ui.tabCertificates,tr("C&ertificates"));
		refreshTabCertificates();
//		m_ui.tabWidget_Identity->addTab(m_ui.tabCardPin,tr("C&ard && PIN"));
		refreshTabCardPin();
		refreshTabInfo();

		break;
		
	case BEID_CARDTYPE_FOREIGNER:
		{
			if(BEID_EIDCard::isApplicationAllowed())
			{
				//tFieldMap& fields = m_CI_Data.m_CardInfo.getFields();
				//int cardSubtype = fields[CARD_TYPE].toInt();
				int cardSubtype = m_CI_Data.m_CardInfo.getSubType();
				if ( cardSubtype >= CardInfo::EUROPEAN_E && cardSubtype <= CardInfo::EUROPEAN_F_PLUS )
				{
//					m_ui.tabWidget_Identity->addTab(m_ui.tabIdentity,tr("&Identity"));
					refreshTabIdentity();

//					m_ui.tabWidget_Identity->addTab(m_ui.tabIdentity_extention,tr("Identity E&xtra"));
					refreshTabIdentityExtra();

					m_imgBackground_Front = QPixmap( ":/images/Images/Background_IDFront.jpg" ); // background
					m_imgBackground_Back = QPixmap( ":/images/Images/Background_IDBack.jpg" ); // background

					//m_ui.lblIdentity_Parents->setVisible(false);
					//m_ui.lblIdentity_Parents_2->setVisible(false);

				}
				else
				{
//					m_ui.tabWidget_Identity->addTab(m_ui.tabForeigners,tr("&Identity"));
					refreshTabForeigners();
//					m_ui.tabWidget_Identity->addTab(m_ui.tabForeigners_Back,tr("Identity E&xtra"));
					refreshTabForeignersExtra();

					m_imgBackground_Front = QPixmap( ":/images/Images/Background_ForeignersFront.jpg" ); // background
					m_imgBackground_Back = QPixmap( ":/images/Images/Background_ForeignersBack.jpg" ); // background
				}
			}

//			m_ui.tabWidget_Identity->addTab(m_ui.tabCertificates,tr("C&ertificates"));
			refreshTabCertificates();
//			m_ui.tabWidget_Identity->addTab(m_ui.tabCardPin,tr("C&ard && PIN"));
			refreshTabCardPin();
			refreshTabInfo();
		}
		break;

	case BEID_CARDTYPE_SIS:
//		m_ui.tabWidget_Identity->addTab(m_ui.tabSis,tr("&Identity"));
		refreshTabSis();
//		m_ui.tabWidget_Identity->addTab(m_ui.tabSis_Back,tr("Identity E&xtra"));
		refreshTabSisExtra();
		refreshTabInfo();

		m_imgBackground_Front = QPixmap( ":/images/Images/Background_SisFront.jpg" ); // background
		m_imgBackground_Back = QPixmap( ":/images/Images/Background_SisBack.jpg" ); // background

		break;
		
	default:
			break;
	}

	setLanguage();

	//------------------------------------------------------
	// set the tabs to the first visible tab
	// set the widget stack to the tabs (not to the splash screen)
	//------------------------------------------------------
//	m_ui.tabWidget_Identity->setCurrentIndex(0);
	m_ui.stackedWidget->setCurrentIndex(1);
	setZoom();
}

//*****************************************************
// show the logo
//*****************************************************
void MainWnd::Show_Splash()
{
	m_TypeCard = BEID_CARDTYPE_UNKNOWN;
	m_ui.stackedWidget->setCurrentIndex(0);
}

//*****************************************************
// show the tabs for an ID card
//*****************************************************
void MainWnd::Show_Identity_Card(BEID_EIDCard& Card)
{
	LoadDataID(Card);
	showTabs();
	enableFileMenu();
	bool bOCSPCheckEnabled = false;
	if (!m_virtReaderContext)
	{
		bOCSPCheckEnabled = true;
	}
	m_ui.btnOCSPCheck->setEnabled(bOCSPCheckEnabled);
	m_ui.txtCert_Status->setEnabled(bOCSPCheckEnabled);
}

void MainWnd::Show_Address_Card(BEID_EIDCard& Card)
{
	LoadDataAddress(Card);
	/*showTabs();
	enableFileMenu();
	bool bOCSPCheckEnabled = false;
	if (!m_virtReaderContext)
	{
		bOCSPCheckEnabled = true;
	}
	m_ui.btnOCSPCheck->setEnabled(bOCSPCheckEnabled);
	m_ui.txtCert_Status->setEnabled(bOCSPCheckEnabled);*/
}

void MainWnd::Show_PersoData_Card(BEID_EIDCard& Card)
{
	LoadDataPersoData(Card);
	/*showTabs();
	enableFileMenu();
	bool bOCSPCheckEnabled = false;
	if (!m_virtReaderContext)
	{
		bOCSPCheckEnabled = true;
	}
	m_ui.btnOCSPCheck->setEnabled(bOCSPCheckEnabled);
	m_ui.txtCert_Status->setEnabled(bOCSPCheckEnabled);*/
}

void MainWnd::Show_Certificates_Card(BEID_EIDCard& Card)
{
	LoadDataCertificates(Card);
	/*showTabs();
	enableFileMenu();
	bool bOCSPCheckEnabled = false;
	if (!m_virtReaderContext)
	{
		bOCSPCheckEnabled = true;
	}
	m_ui.btnOCSPCheck->setEnabled(bOCSPCheckEnabled);
	m_ui.txtCert_Status->setEnabled(bOCSPCheckEnabled);*/
}
//*****************************************************
// show the tabs for a memory card (SIS)
//*****************************************************
void MainWnd::Show_Memory_Card( BEID_MemoryCard& Card )
{
	LoadDataMC(Card);
	showTabs();
}

//*****************************************************
// load the data of a memory card (SIS card)
//*****************************************************
void MainWnd::LoadDataMC(BEID_MemoryCard& Card)
{
	m_TypeCard=Card.getType();
	if(!m_CI_Data.isDataLoaded())
	{
		m_CI_Data.LoadData(Card,m_CurrReaderName);

		//------------------------------------------------------
		// fill in the table with the software info
		//------------------------------------------------------

		fillSoftwareInfo();
	}
}

//*****************************************************
// fill up the certificate tree
//*****************************************************
void MainWnd::fillCertTree(BEID_Certificate *cert, short level, QTreeCertItem* item)
{
#define COLUMN_CERT_NAME 0

	//------------------------------------------------------
	// in case of root level, then create an item in the tree widget
	// else create an item connected to the previous item
	//------------------------------------------------------
	if (0==level)
	{
		item = new QTreeCertItem( m_ui.treeCert, 0 );
	}
	else
	{
		item = new QTreeCertItem( item, 0 );
	}

	//------------------------------------------------------
	// set the text for the the created item
	//------------------------------------------------------
	const char* label = cert->getLabel();
	QString strLabel = QString::fromUtf8(label);
	item->setText(COLUMN_CERT_NAME, strLabel);

	item->setIssuer(QString::fromUtf8(cert->getIssuerName(),-1));
	item->setOwner(QString::fromUtf8(cert->getOwnerName(),-1));
	item->setValidityBegin(cert->getValidityBegin());
	item->setValidityEnd(cert->getValidityEnd());
	QString	strKeyLen;
	strKeyLen=strKeyLen.setNum(cert->getKeyLength());
	item->setKeyLen(strKeyLen);
	item->setOcspStatus(BEID_CERTIF_STATUS_OCSP_NOT_CHECKED);

	BEID_Certificate*	child	= NULL;

	for (unsigned long ulIndex = 0; ulIndex<cert->countChildren();ulIndex++)
	{
		//------------------------------------------------------
		// get the child certificate if it exists
		//------------------------------------------------------
		child=&cert->getChildren(ulIndex);
		fillCertTree(child, level+1, item);
	}
}

void MainWnd::fillCertificateList( void )
{
	if ( BEID_CARDTYPE_SIS == m_CI_Data.m_CardInfo.getType())
	{
		clearTabCertificates();
		return;
	}
	BEID_Certificates* certificates = m_CI_Data.m_CertifInfo.getCertificates();

	if (!certificates)
	{
		return;
	}

	try
	{
		BEID_Certificate& certificate = certificates->getRoot();

		short Level=0;
		fillCertTree(&certificate,Level,NULL);

		m_ui.treeCert->expandAll();
		m_ui.treeCert->sortItems(0,Qt::AscendingOrder);

		QList<QTreeWidgetItem *> itemList = m_ui.treeCert->findItems ( QString("Signature"), Qt::MatchContains|Qt::MatchRecursive );
		if (itemList.size()>0)
		{
			itemList[0]->setSelected(true);
			on_treeCert_itemClicked((QTreeCertItem *)itemList[0], 0);
		}
	}
// 	catch (BEID_ExNoCardPresent &e)
// 	{
// 		return;
// 	}
	catch(BEID_ExCertNoRoot &e)
	{
		long err = e.GetError();
		err = err;
		return;
	}
}

//**************************************************
// Load data of the ID card
//**************************************************
void MainWnd::LoadDataID(BEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();

	if(!m_CI_Data.isDataLoaded())
	{
		m_CI_Data.LoadData(Card,m_CurrReaderName);
		imgPicture = QImage();

		// Debug Qt imageformats plugins
		/*QString fileFormats = "(";
		// Get all inputformats
		for (int i = 0; i < QImageReader::supportedImageFormats().count(); i++) {
			fileFormats += "*."; // Insert wildcard
		    fileFormats += QString(QImageReader::supportedImageFormats().at(i)).toLower(); // Insert the format
		    fileFormats += " "; // Insert a space
		}
			fileFormats += ")\n";

		std::string utf8_text = fileFormats.toUtf8().constData();

		std::cout << " " << utf8_text << endl;*/

		imgPicture.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData);
		imgPicturescaled = imgPicture.scaled(150, 190);
		m_imgPicture = QPixmap::fromImage(imgPicturescaled);

		//------------------------------------------------------
		// Certificates Tab
		//------------------------------------------------------
		clearTabCertificates();

		//------------------------------------------------------
		// Address Tab
		//------------------------------------------------------
		clearTabAddress();

		//------------------------------------------------------
		// fill in the tree of Certificates
		//------------------------------------------------------
		//fillCertificateList();

		//------------------------------------------------------
		// PIN's Tab
		//------------------------------------------------------
#define TYPE_PINTREE_ITEM 0
#define COLUMN_PIN_NAME   0

		clearTabPins();

		//------------------------------------------------------
		// fill in the tree of Pins
		//------------------------------------------------------
		fillPinList( Card );

		//------------------------------------------------------
		// fill in the table with the card info
		//------------------------------------------------------

		fillCardVersionInfo( Card );

		//------------------------------------------------------
		// fill in the table with the software info
		//------------------------------------------------------

		fillSoftwareInfo();
	}
}


void MainWnd::LoadDataAddress(BEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();
	m_CI_Data.LoadDataAddress(Card,m_CurrReaderName);
	if(!m_CI_Data.isDataLoaded())
	{
		//m_CI_Data.LoadDataAddress(Card,m_CurrReaderName);

		// Debug Qt imageformats plugins
		/*QString fileFormats = "(";
		// Get all inputformats
		for (int i = 0; i < QImageReader::supportedImageFormats().count(); i++) {
			fileFormats += "*."; // Insert wildcard
		    fileFormats += QString(QImageReader::supportedImageFormats().at(i)).toLower(); // Insert the format
		    fileFormats += " "; // Insert a space
		}
			fileFormats += ")\n";

		std::string utf8_text = fileFormats.toUtf8().constData();

		std::cout << " " << utf8_text << endl;*/

		//m_imgPicture.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData);
		//m_imgPicture.scaled(0.2, 0.2, Qt::KeepAspectRatio, Qt::FastTransformation );
		//m_imgPicture.scaled(50,50,Qt::IgnoreAspectRatio,Qt::FastTransformation);

		//------------------------------------------------------
		// Certificates Tab
		//------------------------------------------------------
		clearTabCertificates();

		//------------------------------------------------------
		// Address Tab
		//------------------------------------------------------
		clearTabAddress();

		//------------------------------------------------------
		// fill in the tree of Certificates
		//------------------------------------------------------
		//fillCertificateList();

		//------------------------------------------------------
		// PIN's Tab
		//------------------------------------------------------
#define TYPE_PINTREE_ITEM 0
#define COLUMN_PIN_NAME   0

		clearTabPins();

		//------------------------------------------------------
		// fill in the tree of Pins
		//------------------------------------------------------
		fillPinList( Card );

		//------------------------------------------------------
		// fill in the table with the card info
		//------------------------------------------------------

		fillCardVersionInfo( Card );

		//------------------------------------------------------
		// fill in the table with the software info
		//------------------------------------------------------

		fillSoftwareInfo();
	}
}

void MainWnd::LoadDataPersoData(BEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();
	m_CI_Data.LoadDataPersoData(Card,m_CurrReaderName);

	//clearTabCertificates();
	//fillCertificateList();

	/*if(!m_CI_Data.isDataLoaded())
	{
		//------------------------------------------------------
		// Certificates Tab
		//------------------------------------------------------
		//clearTabCertificates();

		//------------------------------------------------------
		// fill in the tree of Certificates
		//------------------------------------------------------
		//fillCertificateList();

		//------------------------------------------------------
		// PIN's Tab
		//------------------------------------------------------
#define TYPE_PINTREE_ITEM 0
#define COLUMN_PIN_NAME   0

		clearTabPins();

		//------------------------------------------------------
		// fill in the tree of Pins
		//------------------------------------------------------
		fillPinList( Card );

		//------------------------------------------------------
		// fill in the table with the card info
		//------------------------------------------------------

		fillCardVersionInfo( Card );

		//------------------------------------------------------
		// fill in the table with the software info
		//------------------------------------------------------

		fillSoftwareInfo();
	}*/
}

void MainWnd::LoadDataCertificates(BEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();
	m_CI_Data.LoadDataCertificates(Card,m_CurrReaderName);

	clearTabCertificates();
	fillCertificateList();

	/*if(!m_CI_Data.isDataLoaded())
	{
		//------------------------------------------------------
		// Certificates Tab
		//------------------------------------------------------
		clearTabCertificates();

		//------------------------------------------------------
		// fill in the tree of Certificates
		//------------------------------------------------------
		fillCertificateList();

		//------------------------------------------------------
		// PIN's Tab
		//------------------------------------------------------
#define TYPE_PINTREE_ITEM 0
#define COLUMN_PIN_NAME   0

		clearTabPins();

		//------------------------------------------------------
		// fill in the tree of Pins
		//------------------------------------------------------
		fillPinList( Card );

		//------------------------------------------------------
		// fill in the table with the card info
		//------------------------------------------------------

		fillCardVersionInfo( Card );

		//------------------------------------------------------
		// fill in the table with the software info
		//------------------------------------------------------

		fillSoftwareInfo();
	}*/
}

//*****************************************************
// fill the PIN list on the window
//*****************************************************
void MainWnd::fillPinList(BEID_EIDCard& Card)
{
	BEID_Pins& Pins = Card.getPins();

	for ( unsigned long PinNr = 0; PinNr < Pins.count(); PinNr++ )
	{
		BEID_Pin& Pin = Pins.getPinByNumber(PinNr);
		QString		DispPinName	= QString::fromLocal8Bit(Pin.getLabel());

		QTreeWidgetItem* PinTreeItem = new QTreeWidgetItem( TYPE_PINTREE_ITEM );

		PinTreeItem->setText(COLUMN_PIN_NAME, trUtf8(Pin.getLabel()));
		m_ui.treePIN->addTopLevelItem ( PinTreeItem );
		if (0==PinNr)
		{
			PinTreeItem->setSelected(true);
		}
	}
	m_ui.treePIN->expandAll();

	QList<QTreeWidgetItem*> treeList = m_ui.treePIN->findItems("Basic PIN",Qt::MatchFixedString|Qt::MatchRecursive);
	if (treeList.size() > 0)
	{
		treeList[0]->setSelected(true);
		on_treePIN_itemClicked(treeList[0], 0);
	}
}

QString MainWnd::getFinalLinkTarget(QString baseName)
{
	QFileInfo info(baseName);
	if (info.isSymLink() && info.exists())
	{
		baseName = info.symLinkTarget();
		baseName = getFinalLinkTarget(baseName);
//QMessageBox::information(this,"debug_getFinalLinkTarget",baseName);		
	}
	
	return baseName;
}



//**************************************************
// fill the software info table
//**************************************************
void MainWnd::fillSoftwareInfo( void )
{

	/*


	QStringList libPaths = QProcess::systemEnvironment();
	QStringList searchPaths;
	QMap<QString,QString> softwareInfo;

#ifdef WIN32

	//--------------------------------
	// search paths are:
	// 1. path of executable
	// 2. current directory
	// 3. windows system directory
	// 4. windows directory
	// 5. PATH
	//--------------------------------

	//--------------------------------
	// 1. exe path
	//--------------------------------
	searchPaths.push_back(QCoreApplication::applicationDirPath());

	//--------------------------------
	// 2. current path
	//--------------------------------
	searchPaths.push_back(QDir::currentPath());

	//--------------------------------
	// 3. system path
	//--------------------------------
	char sysDir[MAX_PATH];
	QString systemPath = GetSystemDirectory(sysDir,MAX_PATH);
	searchPaths.push_back(sysDir);

	//--------------------------------
	// 4. system path
	//--------------------------------
	char winDir[MAX_PATH];
	GetWindowsDirectory(winDir,MAX_PATH);
	searchPaths.push_back(winDir);

	//--------------------------------
	// 5. PATH variable
	//--------------------------------
	int idx = -1;
	QRegExp envPATH("^PATH=.+");

	if ( (idx=libPaths.indexOf(envPATH))>0)
	{
		QString strPATH = libPaths.at(idx);
		strPATH = strPATH.mid(strPATH.indexOf("=")+1);
		QStringList subPaths;
		subPaths = strPATH.split(";");
		foreach(QString p, subPaths)
		{
			searchPaths.push_back(p);
		}
	}

	foreach (QString path, searchPaths)
	{
		for ( size_t idx=0; idx<sizeof(fileList)/sizeof(char*); idx++)
		{
			if (softwareInfo.end()==softwareInfo.find(fileList[idx]))
			{
				QFileInfo fileInfo(QDir::toNativeSeparators(path+"/"+fileList[idx]));
				if (fileInfo.isFile())
				{
					CFileVersionInfo VerInfo;
					if(VerInfo.Open(fileInfo.filePath().toLatin1()))
					{
						char version[256];
						//VerInfo.QueryStringValue(VI_STR_PRODUCTVERSION, version);
						VerInfo.QueryStringValue(VI_STR_FILEVERSION, version);
						softwareInfo[QString(fileList[idx])] = QString(version);
					}
				}
			}
		}
	}
#elif defined __APPLE__
	//--------------------------------
	// search paths are:
	// 1. DYLD_LIBRARY_PATH
	// 2. /usr/local/lib    !!! assumes we're installing in /usr/local/lib !!!
	//--------------------------------
	//--------------------------------
	// 1. DYLD_LIBRARY_PATH variable
	//--------------------------------
	int idx = -1;
	QRegExp envPATH("^DYLD_LIBRARY_PATH=.+");
	if ( (idx=libPaths.indexOf(envPATH))>0)
	{
		QString strPATH = libPaths.at(idx);
		strPATH = strPATH.mid(strPATH.indexOf("=")+1);

		QStringList subPaths;
		subPaths = strPATH.split(":");
		foreach(QString p, subPaths)
		{
			searchPaths.push_back(p);
		}
	}
	//--------------------------------
	// 2. /usr/local/lib
	//--------------------------------
	QString exePath = QCoreApplication::applicationDirPath();
	searchPaths.push_back("/usr/local/lib");

	foreach (QString path, searchPaths)
	{
		for ( size_t idx=0; idx<sizeof(fileList)/sizeof(char*); idx++)
		{
			if (softwareInfo.end()==softwareInfo.find(fileList[idx]))
			{
				QString thisFile(fileList[idx]);
				//thisFile += ".*.*.*.dylib";
				//--------------------------------
				// we take the base filename and will follow the symbolic links until the last
				//--------------------------------
				thisFile += ".dylib";
				QDir fileInfo(path,thisFile);
				//QStringList theFiles = fileInfo.entryList();
				QFileInfoList theFiles = fileInfo.entryInfoList();
				
				if (theFiles.size()>0)
				{
					QString version;
					QString baseName(theFiles[0].absoluteFilePath());
					QString caption;
					caption ="debug_before_getFinalLinkTarget";
					baseName = getFinalLinkTarget(baseName);
					caption = "debug_after_getFinalLinkTarget";
					
					//--------------------------------
					// abcdefg.x.y.z.dylib
					// +-----+             == basename
					// therefore the x.y.z length can be calculated as:
					//    startpoint: baseName().size()+1  ( +1 for the dot after the basename)
					//    length: fileName().size()
					//            - (baseName().size()+1)   ( +1 for the dot after the basename)
					//            - ".dylib".size()
					//--------------------------------
					QFileInfo info(baseName);
			
					version = info.fileName();
					version = version.mid(info.baseName().size()+1,version.size()-(info.baseName().size()+1)-QString(".dylib").size());
										
					softwareInfo[thisFile]=QString(version);
				}
			}
		}
	}
#else
// Linux
	//--------------------------------
	// search paths are:
	// 1. LD_LIBRARY_PATH
	// 2. paths in file /etc/ld.so.conf
	//    The problem with this is that this file can include other .conf files
	//    This becomes too complicated for its purpose here, so we omit this.
	// 3. ../lib
	//--------------------------------
	//--------------------------------
	// 1. LD_LIBRARY_PATH variable
	//--------------------------------
	int idx = -1;
	QRegExp envPATH("^LD_LIBRARY_PATH=.+");
	if ( (idx=libPaths.indexOf(envPATH))>0)
	{
		QString strPATH = libPaths.at(idx);
		strPATH = strPATH.mid(strPATH.indexOf("=")+1);

		//printf("Splitting: %s\n",strPATH.toLatin1().data());

		QStringList subPaths;
		subPaths = strPATH.split(":");
		foreach(QString p, subPaths)
		{
			searchPaths.push_back(p);
		}
	}
	//--------------------------------
	// 3. ../lib (relative to the exe path
	//--------------------------------
	QString exePath = QCoreApplication::applicationDirPath();
	searchPaths.push_back(exePath+"/../lib");

	foreach (QString path, searchPaths)
	{
		for ( size_t idx=0; idx<sizeof(fileList)/sizeof(char*); idx++)
		{
			if (softwareInfo.end()==softwareInfo.find(fileList[idx]))
			{
				QFileInfo fileInfo(path+"/"+fileList[idx]);
				if (fileInfo.isFile())
				{
					QString thisFile;
					thisFile = fileList[idx];
					thisFile += ".?.?.?" ;
					QDir thisDir(path,thisFile);
					QStringList allFiles=thisDir.entryList();
					foreach(QString p,allFiles)
					{
						QString version = p.mid(p.indexOf(".so.")+4);
						softwareInfo[QString(fileList[idx])] = QString(version);
					}
				}
			}
		}
	}

#endif

	m_ui.tblInfo->setRowCount( softwareInfo.size() );
	m_ui.tblInfo->setColumnCount( 2 );

	QTableWidgetItem*	newItem = NULL;
	int					RowNr = 0;
	int					ColNr = 0;

	Qt::ItemFlags flags;
	flags &= !Qt::ItemIsEditable;

	for ( QMap<QString,QString>::iterator itData=softwareInfo.begin()
		; itData != softwareInfo.end()
		; itData++, ColNr=0, RowNr++
		)
	{
		newItem = new QTableWidgetItem( itData.key() );
		newItem->setFlags(flags);
		m_ui.tblInfo->setItem( RowNr, ColNr++, newItem );
		newItem = new QTableWidgetItem( itData.value() );
		newItem->setFlags(flags);
		m_ui.tblInfo->setItem( RowNr, ColNr, newItem );
	}

	*/
}

//**************************************************
// fill the table with the card info
// This function has to be called each time the language is changed
//**************************************************
void MainWnd::fillCardVersionInfo( BEID_EIDCard& Card )
{
	BEID_CardVersionInfo& CardVersionInfo = Card.getVersionInfo();

	QTableWidgetItem* newItem = NULL;

	int			RowNr = 0;
	int			ColNr = 0;
	QMap<QString,const char*> Data;

	Data[tr("Serial nr")]							= CardVersionInfo.getSerialNumber();
	Data[tr("Component Code")]						= CardVersionInfo.getComponentCode();
	Data[tr("OS Number")]							= CardVersionInfo.getOsNumber();
	Data[tr("OS Number")]							= CardVersionInfo.getOsVersion();
	Data[tr("Softmask Number")]						= CardVersionInfo.getSoftmaskNumber();
	Data[tr("Softmask Version")]					= CardVersionInfo.getSoftmaskVersion();
	Data[tr("Applet Version")]						= CardVersionInfo.getAppletVersion();
	Data[tr("Global OS Version")]					= CardVersionInfo.getGlobalOsVersion();
	Data[tr("Applet Interface Version")]			= CardVersionInfo.getAppletInterfaceVersion();
	Data[tr("PKCS1 Support")]						= CardVersionInfo.getPKCS1Support();
	Data[tr("Key Exchange Version")]				= CardVersionInfo.getKeyExchangeVersion();
	Data[tr("Applet Life Cycle")]					= CardVersionInfo.getAppletLifeCycle();
	Data[tr("Graphical Personalisation")]			= CardVersionInfo.getGraphicalPersonalisation();
	Data[tr("Electrical Personalisation Interface")]= CardVersionInfo.getElectricalPersonalisationInterface();

	//--------------------------------------------
	// make sure we start with a cleaned list
	//--------------------------------------------
	/*
	m_ui.tblCardAndPIN->clear();

	m_ui.tblCardAndPIN->setRowCount( Data.size() );
	Qt::ItemFlags flags;
	flags &= !Qt::ItemIsEditable;

	for ( QMap<QString,const char*>::iterator itData = Data.begin()
		; itData != Data.end()
		; itData++, ColNr=0, RowNr++
		)
	{
		newItem = new QTableWidgetItem( itData.key() );
		newItem->setFlags(flags);
		m_ui.tblCardAndPIN->setItem( RowNr, ColNr++, newItem );
		newItem = new QTableWidgetItem( itData.value() );
		newItem->setFlags(flags);
		m_ui.tblCardAndPIN->setItem( RowNr, ColNr, newItem );
	}

	QStringList labels;
	labels.push_back(tr("Field"));
	labels.push_back(tr("Value"));

	m_ui.tblCardAndPIN->setHorizontalHeaderLabels(labels);
	*/
}

//**************************************************
// clear button clicked
// - back to the main screen
// - clear the data of the loaded card
// - make sure virtual reader is cleared
// - current reader name reset
//**************************************************
void MainWnd::on_actionClear_triggered()
{
	QString msg = tr("Clear");
	msg.remove(QChar('&'));
	m_ui.statusBar->showMessage(msg,m_STATUS_MSG_TIME);
	clearGuiContent();

}
//*****************************************************
// Clear the content of the GUI
//*****************************************************
void MainWnd::clearGuiContent( void )
{
	Show_Splash();

	m_CI_Data.Reset();
	releaseVirtualReader();
	m_CurrReaderName = "";

	setStatus( SPECIALSTATUS_NA );

	clearTabCertificates();
	clearTabPins();

	enableFileMenu();
	enablePrintMenu();
}

//*****************************************************
// put the card status info on the window
//*****************************************************
void MainWnd::setStatus( unsigned int Status )
{
	QString tmp;
	switch( Status )
	{
	case SPECIALSTATUS_NA:
		tmp = "";
		break;
	case SPECIALSTATUS_YELLOWCANE:
		tmp = "Yellow cane";
		break;

	case SPECIALSTATUS_WHITECANE:
		tmp = "White cane";
		break;

	case SPECIALSTATUS_EXTMINORITY:
		tmp = "Extended minority";
		break;

	default:
		tmp = "Unkown";
		break;
	}


}

//**************************************************
// Zoomin triggered
//**************************************************
void MainWnd::on_actionZoom_In_triggered(void)
{
	m_ui.actionZoom_In->setEnabled(true);
	m_ui.actionZoom_Out->setEnabled(true);
	if( m_Zoom < ZOOM_HUGE )
	{
		//------------------------------------------
		// size the main window 
		//------------------------------------------
		m_Zoom = eZOOMSTATUS(m_Zoom+1);
		setFixedSize( windowSizes[m_Zoom][0], windowSizes[m_Zoom][1] );
		// size the stacked widget here in function of the toolbar enable or disabled
		//m_ui.stackedWidget->setFixedSize( windowSizes[m_Zoom][0]/10, windowSizes[m_Zoom][1]/10 );

		//------------------------------------------
		// size all widgets
		//------------------------------------------
		setZoom();
	}
	if ( m_Zoom==ZOOM_HUGE )
	{
		m_ui.actionZoom_In->setEnabled(false);
	}
}

//**************************************************
// Zoomout triggered
//**************************************************
void MainWnd::on_actionZoom_Out_triggered(void)
{
	m_ui.actionZoom_In->setEnabled(true);
	m_ui.actionZoom_Out->setEnabled(true);
	if( m_Zoom > ZOOM_SMALL )
	{
		m_Zoom = eZOOMSTATUS(m_Zoom-1);
		setFixedSize( windowSizes[m_Zoom][0], windowSizes[m_Zoom][1] );
		// size the stacked widget here in function of the toolbar enable or disabled
		//m_ui.stackedWidget->setFixedSize( windowSizes[m_Zoom][0]/10, windowSizes[m_Zoom][1]/10 );
		setZoom();
	}
	if ( m_Zoom==ZOOM_SMALL )
	{
		m_ui.actionZoom_Out->setEnabled(false);
	}
}

//**************************************************
// set everything acoording to the zoom factor
//**************************************************
void MainWnd::setZoom()
{
	//-----------------------------------------------------------
	// Zoom the tabs
	//-----------------------------------------------------------
	// Zoomactions depend on the card type
	//-----------------------------------------------------------
	switch (m_TypeCard) 
	{
	case BEID_CARDTYPE_EID:
	case BEID_CARDTYPE_KIDS:
		zoomTabIdentity();
		zoomTabIdentityExtra();
		zoomTabAddress();
		zoomTabCertificates();
		zoomTabCardPin();
		zoomTabInfo();
		break;

	case BEID_CARDTYPE_FOREIGNER:
		{
			int cardSubType = m_CI_Data.m_CardInfo.getFields()[CARD_TYPE].toInt();
			if( cardSubType>=CardInfo::EUROPEAN_E && cardSubType<=CardInfo::EUROPEAN_F_PLUS )
			{
				zoomTabIdentity();
				zoomTabIdentityExtra();
				zoomTabCertificates();
				zoomTabCardPin();
				zoomTabInfo();
			}
			else
			{
				zoomTabForeigners();
				zoomTabForeignersExtra();
				zoomTabCertificates();
				zoomTabCardPin();
				zoomTabInfo();
			}
		}
		break;

		case BEID_CARDTYPE_SIS:
			zoomTabSis();
			zoomTabSisExtra();
			zoomTabCardPin();
			zoomTabInfo();
		break;

		case BEID_CARDTYPE_UNKNOWN:
		default:
			return;
			break;
	}
	
	//-----------------------------------------------------------
	// Zoom the toolbar icons
	//-----------------------------------------------------------
	multiplyerFactor multiplyer(m_Zoom);
	int IconSize = (int) (TOOLBAR_ICON_BASE * multiplyer.HMultiplyer);
	m_ui.toolBar->setIconSize( QSize( IconSize, IconSize ) );

	//-----------------------------------------------------------
	// tab label
	// looks like we can't change the text size??
	//-----------------------------------------------------------

//	QFont tmpFont = m_ui.tabWidget_Identity->font();
//	tmpFont.setPointSize( stylesheetsInfo[STYLESHEET_NORMAL_LABEL].pointSizes[m_Zoom] );
//	m_ui.tabWidget_Identity->setFont( tmpFont );
//	m_ui.tabWidget_Identity->setIconSize( QSize( IconSize, IconSize ) );

	zoomBackground();
}

//**************************************************
// Zoom the background images. 
//**************************************************
void MainWnd::zoomBackground()
{
/*	//-----------------------------------------------------------
	// Set Background Image for Tab Identity and Tab Identity_extention
	//-----------------------------------------------------------
	multiplyerFactor mFactor(m_Zoom);
	QPalette Pal; 
	QBrush brush;
	
	if( !m_imgBackground_Front.isNull() )
	{
		switch (m_TypeCard) 
		{
			case BEID_CARDTYPE_EID:
			case BEID_CARDTYPE_KIDS:
//				m_ui.tabIdentity->setAutoFillBackground( true );
//				Pal = m_ui.tabIdentity->palette();
				brush = QBrush( m_imgBackground_Front.scaled( m_ui.tabWidget_Identity->width()
															, m_ui.tabWidget_Identity->height()
															//,Qt::KeepAspectRatio 
															) 
							  );
				Pal.setBrush(m_ui.tabIdentity->backgroundRole(), brush);
				m_ui.tabIdentity->setPalette( Pal );
				break;

			case BEID_CARDTYPE_SIS:
				m_ui.tabSis->setAutoFillBackground( true );
				Pal = m_ui.tabSis->palette();
				brush = QBrush( m_imgBackground_Front.scaled( m_ui.tabWidget_Identity->width()
															, m_ui.tabWidget_Identity->height()
															//, Qt::KeepAspectRatio 
															) 
							  );
				Pal.setBrush(m_ui.tabSis->backgroundRole(), brush);
				m_ui.tabSis->setPalette( Pal );
				break;

			case BEID_CARDTYPE_FOREIGNER:
				{
					int cardSubtype = m_CI_Data.m_CardInfo.getFields()[CARD_TYPE].toInt();
					if(cardSubtype >= CardInfo::EUROPEAN_E && cardSubtype <= CardInfo::EUROPEAN_F_PLUS)
					{
						m_ui.tabIdentity->setAutoFillBackground( true );
						Pal = m_ui.tabIdentity->palette();
						brush = QBrush( m_imgBackground_Front.scaled( m_ui.tabWidget_Identity->width()
																	, m_ui.tabWidget_Identity->height()
																	//, Qt::KeepAspectRatio 
																	) 
									  );
						Pal.setBrush(m_ui.tabIdentity->backgroundRole(), brush);
						m_ui.tabIdentity->setPalette( Pal );
					}
					else
					{
						m_ui.tabForeigners->setAutoFillBackground( true );
						Pal = m_ui.tabForeigners->palette();
						brush = QBrush( m_imgBackground_Front.scaled( m_ui.tabWidget_Identity->width()
																	, m_ui.tabWidget_Identity->height()
																	//, Qt::KeepAspectRatio 
																	) 
									  );
						Pal.setBrush(m_ui.tabForeigners->backgroundRole(), brush);
						m_ui.tabForeigners->setPalette( Pal );
					}
				}
				break;
			case BEID_CARDTYPE_UNKNOWN:
			default:
				break;
		}
	}

	if( !m_imgBackground_Back.isNull() )
	{
		switch (m_TypeCard) 
		{
		case BEID_CARDTYPE_EID:
		case BEID_CARDTYPE_KIDS:
				m_ui.tabIdentity_extention->setAutoFillBackground( true );
				Pal = m_ui.tabIdentity_extention->palette();
				brush = QBrush( m_imgBackground_Back.scaled( m_ui.tabWidget_Identity->width()
														   , m_ui.tabWidget_Identity->height()
														   //,Qt::KeepAspectRatio 
														   ) 
							  );
				Pal.setBrush(m_ui.tabIdentity_extention->backgroundRole(), brush);
				m_ui.tabIdentity_extention->setPalette( Pal );
				break;

		case BEID_CARDTYPE_SIS:
				m_ui.tabSis_Back->setAutoFillBackground( true );
				Pal = m_ui.tabSis_Back->palette();
				brush = QBrush( m_imgBackground_Back.scaled( m_ui.tabWidget_Identity->width()
															,m_ui.tabWidget_Identity->height()
															//,Qt::KeepAspectRatio 
															) 
							  );
				Pal.setBrush(m_ui.tabSis_Back->backgroundRole(), brush);
				m_ui.tabSis_Back->setPalette( Pal );
				break;

		case BEID_CARDTYPE_FOREIGNER:
			{
				int cardSubtype = m_CI_Data.m_CardInfo.getFields()[CARD_TYPE].toInt();
				if(cardSubtype>=CardInfo::EUROPEAN_E && cardSubtype<= CardInfo::EUROPEAN_F_PLUS)
				{
					m_ui.tabIdentity_extention->setAutoFillBackground( true );
					Pal = m_ui.tabIdentity_extention->palette();
					brush = QBrush( m_imgBackground_Back.scaled( m_ui.tabWidget_Identity->width()
																,m_ui.tabWidget_Identity->height()
																//,Qt::KeepAspectRatio 
																) 
								  );
					Pal.setBrush(m_ui.tabIdentity_extention->backgroundRole(), brush);
					m_ui.tabIdentity_extention->setPalette( Pal );
				}
				else
				{
					m_ui.tabForeigners_Back->setAutoFillBackground( true );
					Pal = m_ui.tabForeigners_Back->palette();
					brush = QBrush( m_imgBackground_Back.scaled( m_ui.tabWidget_Identity->width()
																,m_ui.tabWidget_Identity->height()
																//,Qt::KeepAspectRatio 
																) 
								  );
					Pal.setBrush(m_ui.tabForeigners_Back->backgroundRole(), brush);
					m_ui.tabForeigners_Back->setPalette( Pal );
				}
				break;
			}
		case BEID_CARDTYPE_UNKNOWN:
		default:
			break;
		}
	}
*/
}
//**************************************************
// Zoom all items on the tab 'Identity'
//**************************************************
void MainWnd::zoomTabIdentity( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabIdentity->findChildren<QWidget *>();
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);

	//----------------------------------------------------
	// E/E+/F/F+ cards have the same layout as belgian eID cards
	// The text (like 'identity card') must be changed depending on the
	// card type.
	//----------------------------------------------------
	int cardSubtype = m_CI_Data.m_CardInfo.getFields()[CARD_TYPE].toInt();
	switch( cardSubtype )
	{
	case CardInfo::EUROPEAN_E:
//		m_ui.lblIdentity_Head2_1->setText(tr("txt_ECard_1"));
//		m_ui.lblIdentity_Head2_2->setText(tr("txt_ECard_2"));
		m_ui.lblIdentity_Head2_3->setText(tr("txt_ECard_3"));
		m_ui.lblIdentity_Head2_4->setText(tr("txt_ECard_4"));
		break;
	case CardInfo::EUROPEAN_E_PLUS:
//		m_ui.lblIdentity_Head2_1->setText(tr("txt_ECard_plus_1"));
//		m_ui.lblIdentity_Head2_2->setText(tr("txt_ECard_plus_2"));
		m_ui.lblIdentity_Head2_3->setText(tr("txt_ECard_plus_3"));
		m_ui.lblIdentity_Head2_4->setText(tr("txt_ECard_plus_4"));
		break;
	case CardInfo::EUROPEAN_F:
//		m_ui.lblIdentity_Head2_1->setText(tr("txt_FCard_1"));
//		m_ui.lblIdentity_Head2_2->setText(tr("txt_FCard_2"));
		m_ui.lblIdentity_Head2_3->setText(tr("txt_FCard_3"));
		m_ui.lblIdentity_Head2_4->setText(tr("txt_FCard_4"));
		break;
	case CardInfo::EUROPEAN_F_PLUS:
//		m_ui.lblIdentity_Head2_1->setText(tr("txt_FCard_plus_1"));
//		m_ui.lblIdentity_Head2_2->setText(tr("txt_FCard_plus_2"));
		m_ui.lblIdentity_Head2_3->setText(tr("txt_FCard_plus_3"));
		m_ui.lblIdentity_Head2_4->setText(tr("txt_FCard_plus_4"));
		break;
	default:
//		m_ui.lblIdentity_Head2_1->setText(tr("txtIdentityCard_1"));
		//m_ui.lblIdentity_Head2_2->setText(tr("txtIdentityCard_2"));
		//m_ui.lblIdentity_Head2_3->setText(tr("txtIdentityCard_3"));
		//m_ui.lblIdentity_Head2_4->setText(tr("txtIdentityCard_4"));
		break;
	}

	//-----------------------------------------------------------
	// Zoom the picture as well
	//-----------------------------------------------------------
	int height = m_imgPicture.height();
	int width  = m_imgPicture.width();
	multiplyerFactor mfactor(m_Zoom);
	m_ui.lblIdentity_ImgPerson->setFixedHeight( (int) (height*mfactor.HMultiplyer) );
	m_ui.lblIdentity_ImgPerson->setFixedWidth( (int) (width*mfactor.WMultiplyer) );

	*/

}

//*****************************************************
// zoom the tab with extra info ( tab with backside of the card)
//*****************************************************
void MainWnd::zoomTabIdentityExtra(void)
{
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabIdentity_extention->findChildren<QWidget *>();
//	zoomAllWidgets(allWidgets);
//	setWidgetsPointSize(allWidgets);
//	setWidgetPosition(allWidgets);

}


//*****************************************************
// zoom the tab with PTeid Address
//*****************************************************
void MainWnd::zoomTabAddress(void)
{
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabAddress->findChildren<QWidget *>();
//	zoomAllWidgets(allWidgets);
//	setWidgetsPointSize(allWidgets);
//	setWidgetPosition(allWidgets);

}



//*****************************************************
// zoom the tab with foreigners info
//*****************************************************
void MainWnd::zoomTabForeigners( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabForeigners->findChildren<QWidget *>();
	//zoomAllWidgets(allWidgets);
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);

	int height = m_imgPicture.height();
	int width  = m_imgPicture.width();
	multiplyerFactor mfactor(m_Zoom);
	m_ui.lblForeigners_ImgPerson->setFixedHeight( (int) (height*mfactor.HMultiplyer) );
	m_ui.lblForeigners_ImgPerson->setFixedWidth( (int) (width*mfactor.WMultiplyer) );
*/
}

//*****************************************************
// zoom tab with foreigner extra info (backside of card)
//*****************************************************
void MainWnd::zoomTabForeignersExtra(void)
{	
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabForeigners_Back->findChildren<QWidget *>();
//	zoomAllWidgets(allWidgets);
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);
	*/
}

//*****************************************************
// zoom tab info of SIS card
//*****************************************************
void MainWnd::zoomTabSis(void)
{
/*
	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabSis->findChildren<QWidget *>();
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);

	multiplyerFactor mfactor(m_Zoom);
	m_ui.lblSis_Sex->setFixedHeight( (int) (IMG_SEX_SIZE_BASE*mfactor.HMultiplyer) );
	m_ui.lblSis_Sex->setFixedWidth(  (int) (IMG_SEX_SIZE_BASE*mfactor.WMultiplyer) );

// 	m_ui.lblSis_Sex->setFixedHeight( IMG_SEX_SIZE_BASE + IMG_SEX_SIZE_INCREMENT * m_Zoom  );
// 	m_ui.lblSis_Sex->setFixedWidth( (int) ( IMG_SEX_SIZE_BASE + IMG_SEX_SIZE_INCREMENT * m_Zoom  ) );
 *
 */
}

//*****************************************************
// zoom tab with SIS card extra info (back side of card)
//*****************************************************
void MainWnd::zoomTabSisExtra(void)
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabSis_Back->findChildren<QWidget *>();
//	zoomAllWidgets(allWidgets);
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);
	*/
}

//*****************************************************
// zoom tab with certificate info
//*****************************************************
void MainWnd::zoomTabCertificates( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabCertificates->findChildren<QWidget *>();
	zoomAllWidgets(allWidgets);

	QFont font;
	font.setPointSize (stylesheetsInfo[STYLESHEET_NORMAL_LABEL].pointSizes[m_Zoom] );
	{
		QTreeWidgetItem* header = m_ui.treeCert->headerItem();
		if (header)
		{
			header->setFont(0,font);
		}
	}
*/
}

//*****************************************************
// zoom tab with PIN info
//*****************************************************
void MainWnd::zoomTabCardPin(void)
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabCardPin->findChildren<QWidget *>();
//	zoomAllWidgets(allWidgets);
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);

	QFont font;
	font.setPointSize (stylesheetsInfo[STYLESHEET_NORMAL_LABEL].pointSizes[m_Zoom] );
	{
		QTableWidgetItem* header = NULL;
		header = m_ui.tblCardAndPIN->horizontalHeaderItem ( 0 );
		if (header)
		{
			header->setFont(font);
			header = m_ui.tblCardAndPIN->horizontalHeaderItem ( 1 );
			header->setFont(font);
		}
	}
	{
		QTreeWidgetItem* header = m_ui.treePIN->headerItem();
		if (header)
		{
			header->setFont(0,font);
		}
	}

	m_ui.tblCardAndPIN->setColumnWidth( 0, ( width()-100 )/2 );
	m_ui.tblCardAndPIN->setColumnWidth( 1, ( width()-100 )/2 );
*/
}

//*****************************************************
// zoom tab with general info
//*****************************************************
void MainWnd::zoomTabInfo(void)
{
/*
	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabInfo->findChildren<QWidget *>();
//	zoomAllWidgets(allWidgets);
	setWidgetsPointSize(allWidgets);
	setWidgetPosition(allWidgets);

	QFont font;
	font.setPointSize (stylesheetsInfo[STYLESHEET_NORMAL_LABEL].pointSizes[m_Zoom] );
	QTableWidgetItem* header = NULL;
	header = m_ui.tblInfo->horizontalHeaderItem ( 0 );
	if (header)
	{
		header->setFont(font);
	}
	header = m_ui.tblInfo->horizontalHeaderItem ( 1 );
	if (header)
	{
		header->setFont(font);
	}

	m_ui.tblInfo->setColumnWidth( 0, ( width()-100 )/2 );
	m_ui.tblInfo->setColumnWidth( 1, ( width()-100 )/2 );
*/
}
void MainWnd::setWidgetsPointSize(QList<QWidget *> &allWidgets)
{
	zoomAllWidgets(allWidgets);
}

//*****************************************************
// change the position and size of a widget
//*****************************************************
void MainWnd::setWidgetPosition(QList<QWidget *>& allWidgets)
{
	for (int x=0; x<allWidgets.size(); x++)
	{
		QRect point;
		for (size_t widget=0;widget<sizeof(widgetTabInfo)/sizeof(struct widgetInfo);widget++)
		{
			if (allWidgets.at(x)->objectName()==widgetTabInfo[widget].widgetName)
			{
				int newXPos   = widgetTabInfo[widget].position.pos[0];
				int newYPos   = widgetTabInfo[widget].position.pos[1];
				int newWidth  = widgetTabInfo[widget].position.pos[2];
				int newHeight = widgetTabInfo[widget].position.pos[3];

				if(newXPos==0 && newYPos==0 && newWidth==0 && newHeight==0)
					break;

				multiplyerFactor mfactor(m_Zoom);

				point.setX((int) (newXPos*mfactor.XMultiplyer));
				point.setY((int) (newYPos*mfactor.YMultiplyer));
				point.setWidth((int) (newWidth*mfactor.WMultiplyer));
				point.setHeight((int) (newHeight*mfactor.HMultiplyer));

				allWidgets.at(x)->setGeometry(point);
				break;
			}
		}
	}
}

//*****************************************************
// zoom all the widgets
//*****************************************************
void MainWnd::zoomAllWidgets(QList<QWidget *> &allWidgets)
{

	multiplyerFactor mFactor(m_Zoom);

	for (int i = 0; i < allWidgets.size(); ++i) 
	{		
		QString name = allWidgets.at(i)->objectName();
		eStyleSheet style=widgetMapStyle[name];

		QFont tmpFont = allWidgets.at(i)->font();
		if( widgetMapStyle[allWidgets.at(i)->objectName()]!=STYLESHEET_NONE )
		{
			tmpFont.setPointSize( stylesheetsInfo[style].pointSizes[m_Zoom] );
		}
		allWidgets.at(i)->setFont( tmpFont );
	}
}

//*****************************************************
// init all the tabs
//*****************************************************
void MainWnd::initAllTabs( void )
{
	initTabIdentity();
	initTabIdentityExtra();
	initTabAddress();
	initTabForeigners();
	initTabForeignersExtra();
	initTabSis();
	initTabSisExtra();
	initTabPersoData();
	initTabCertificates();
	initTabCardPin();
	initTabInfo();
}

//*****************************************************
// initialize the tab with ID info (front side of card)
//*****************************************************
void MainWnd::initTabIdentity( void )
{
	// Apply StyleSheet
//        QList<QWidget *> allWidgets = m_ui.tabIdentity->findChildren<QWidget *>();
  //      initAllWidgets(allWidgets);

        m_ui.txtIdentity_Sex->setAlignment(Qt::AlignRight);

	//m_ui.lblIdentity_Parents_2->setFrameShadow(QFrame::Plain);
	//m_ui.lblIdentity_Parents_2->setFrameShape(QFrame::Box);
	//QPalette p = m_ui.lblIdentity_Parents_2->palette();
	//p.setBrush(QPalette::WindowText, QColor::fromRgb( 255, 0, 0) );
	//m_ui.lblIdentity_Parents_2->setPalette(p);

}

//*****************************************************
// initialize the tab with ID extra info (card back side)
//*****************************************************
void MainWnd::initTabIdentityExtra( void )
{
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabIdentity_extention->findChildren<QWidget *>();
//	initAllWidgets(allWidgets);
}

//*****************************************************
// initialize the tab with PTeid Address
//*****************************************************
void MainWnd::initTabAddress( void )
{
	//addressPINRequest_triggered();
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabAddress->findChildren<QWidget *>();
//	initAllWidgets(allWidgets);
}

//*****************************************************
// init tab with foreigner info (front side of card)
//*****************************************************
void MainWnd::initTabForeigners( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabForeigners->findChildren<QWidget *>();
	initAllWidgets(allWidgets);

	m_ui.txtForeigners_Card_Number->setAlignment(Qt::AlignRight);
*/
}

//*****************************************************
// init tab with foreigner extra info (back side of card)
//*****************************************************
void MainWnd::initTabForeignersExtra( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabForeigners_Back->findChildren<QWidget *>();
	initAllWidgets(allWidgets);
	*/
}

//*****************************************************
// init tab with SIS card info (front side of card)
//*****************************************************
void MainWnd::initTabSis( void )
{
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabSis->findChildren<QWidget *>();
//	initAllWidgets(allWidgets);

	//m_ui.txtSis_LogicalNumber->setAlignment(Qt::AlignLeft);
	//m_ui.txtSis_SocialSecurityNumber->setAlignment(Qt::AlignRight);
	//m_ui.txtSis_ValidFrom->setAlignment(Qt::AlignRight);
}

//*****************************************************
// init tab with SIS card extra info (back side of card)
//*****************************************************
void MainWnd::initTabSisExtra( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabSis_Back->findChildren<QWidget *>();
	initAllWidgets(allWidgets);
*/
}

//*****************************************************
// init the tab with certificate info
//*****************************************************
void MainWnd::initTabCertificates( void )
{
/*	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabCertificates->findChildren<QWidget *>();
	initAllWidgets(allWidgets);
*/
}

//*****************************************************
// init tab with PTEID Personal Data
//*****************************************************
void MainWnd::initTabPersoData( void )
{
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabPersoData->findChildren<QWidget *>();
//	initAllWidgets(allWidgets);

	//m_ui.txtSis_LogicalNumber->setAlignment(Qt::AlignLeft);
	//m_ui.txtSis_SocialSecurityNumber->setAlignment(Qt::AlignRight);
	//m_ui.txtSis_ValidFrom->setAlignment(Qt::AlignRight);
}

//*****************************************************
// init the tab with the PIN info
//*****************************************************
void MainWnd::initTabCardPin( void )
{
	// Apply StyleSheet
//	QList<QWidget *> allWidgets = m_ui.tabCardPin->findChildren<QWidget *>();
//	initAllWidgets(allWidgets);

/*
  	m_ui.tblCardAndPIN->setColumnCount( 2 );

	QTableWidgetItem *newItem = new QTableWidgetItem( tr("Field") );
 	m_ui.tblCardAndPIN->setHorizontalHeaderItem( 0, newItem );

	newItem = new QTableWidgetItem( tr("Value") );
	m_ui.tblCardAndPIN->setHorizontalHeaderItem( 1, newItem );

	m_ui.tblCardAndPIN->setColumnWidth( 0, 348 );
	m_ui.tblCardAndPIN->setColumnWidth( 1, 348 );
	*/
}

//*****************************************************
// init the tab with the software info (DLLs and version info)
//*****************************************************
void MainWnd::initTabInfo( void )
{
/*
	// Apply StyleSheet
	QList<QWidget *> allWidgets = m_ui.tabInfo->findChildren<QWidget *>();
	initAllWidgets(allWidgets);

	multiplyerFactor mfactor(m_Zoom);

	QFont font;
	font.setPointSize (stylesheetsInfo[STYLESHEET_NORMAL_LABEL].pointSizes[m_Zoom] );

	m_ui.tblInfo->setColumnCount( 2 );
	QTableWidgetItem *newItem = new QTableWidgetItem( tr("Name") );
	newItem->setFont(font);
	m_ui.tblInfo->setHorizontalHeaderItem( 0, newItem );
	newItem = new QTableWidgetItem( tr("Version") );
	newItem->setFont(font);
	m_ui.tblInfo->setHorizontalHeaderItem( 1, newItem );
	m_ui.tblInfo->setColumnWidth( 0, 348 );
	m_ui.tblInfo->setColumnWidth( 1, 348 );
	*/
}

//*****************************************************
// initialize all the given widgets
//*****************************************************
void MainWnd::initAllWidgets(QList<QWidget *> &allWidgets)
{
	for (int i = 0; i < allWidgets.size(); ++i) 
	{		
		QFont tmpFont = allWidgets.at(i)->font();
		QPalette p = allWidgets.at(i)->palette();
		if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_BUTTON )
		{
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_TITLE_1 )
		{
			tmpFont.setFamily( "Helvetica" );
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_TITLE_2 )
		{
			tmpFont.setFamily( "Helvetica" );
			tmpFont.setBold( true );
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_FOOTER_1 )
		{
			tmpFont.setBold( true );
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_NORMAL_LABEL )
		{
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_NORMAL_VALUE )
		{
			tmpFont.setBold( true );
			tmpFont.setFamily( "Arial" );

			p.setBrush(QPalette::Base, Qt::transparent);
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_BIG_VALUE )
		{
			tmpFont.setFamily( "Courrier New" );

			p.setBrush(QPalette::Base, Qt::transparent);
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_SIS_VALUE )
		{
			tmpFont.setFamily( "Courrier New" );

			p.setBrush(QPalette::Base, Qt::transparent);
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_SMALL_RED )
		{
			tmpFont.setBold( true );
			((QLabel *)allWidgets.at(i))->setText(((QLabel *)allWidgets.at(i))->text().toUpper());
			p.setBrush(QPalette::WindowText, QColor::fromRgb( 183,  48, 88) );
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_SMALL_BLUE )
		{
			tmpFont.setBold( true );
			((QLabel *)allWidgets.at(i))->setText(((QLabel *)allWidgets.at(i))->text().toUpper());
			p.setBrush(QPalette::WindowText, QColor::fromRgb( 92,  105, 150) );
		}
		else if( widgetMapStyle[allWidgets.at(i)->objectName()]==STYLESHEET_SMALL_REDRIGHT )
		{
			((QLabel *)allWidgets.at(i))->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

			tmpFont.setBold( true );
			((QLabel *)allWidgets.at(i))->setText(((QLabel *)allWidgets.at(i))->text().toUpper());
			p.setBrush(QPalette::WindowText, QColor::fromRgb( 183,  48, 88) );
		}
		allWidgets.at(i)->setFont( tmpFont );
		allWidgets.at(i)->setPalette(p);
	}
}

//*****************************************************
// refresh the tab with the ID data (front of card)
//*****************************************************
void MainWnd::refreshTabIdentity( void )
{

	m_ui.lblIdentity_ImgPerson->setPixmap(m_imgPicture);
	m_ui.lblIdentity_ImgPerson->show();

	tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();

	m_ui.txtIdentity_Name->setText		 ( PersonFields[NAME] );
	m_ui.txtIdentity_Name->setAccessibleName ( PersonFields[NAME] );
	m_ui.txtIdentity_GivenNames->setText ( PersonFields[FIRSTNAME] );
	m_ui.txtIdentity_GivenNames->setAccessibleName ( PersonFields[FIRSTNAME] );
	m_ui.txtIdentity_Nationality->setText( PersonFields[NATIONALITY] );
	m_ui.txtIdentity_Nationality->setAccessibleName( PersonFields[NATIONALITY] );
	m_ui.txtIdentity_BirthDate->setText ( PersonFields[BIRTHDATE] );
	m_ui.txtIdentity_BirthDate->setAccessibleName ( PersonFields[BIRTHDATE] );
	m_ui.txtIdentity_Sex->setText        ( PersonFields[SEX] );
	m_ui.txtIdentity_Sex->setAccessibleName ( PersonFields[SEX] );

	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	QString cardNumber = m_CI_Data.m_CardInfo.formatCardNumber(CardFields[CARD_NUMBER],m_CI_Data.m_pCard->getType());
	m_ui.txtIdentity_Height->setText	 ( PersonFields[HEIGHT] );
	m_ui.txtIdentity_Height->setAccessibleName	 ( PersonFields[HEIGHT] );
	m_ui.txtIdentity_Country->setText	 ( PersonFields[COUNTRY] );
	m_ui.txtIdentity_Country->setAccessibleName	 ( PersonFields[COUNTRY] );
	m_ui.txtIdentity_DocumentNumber->setText	 ( PersonFields[DOCUMENTNUMBER] );
	m_ui.txtIdentity_DocumentNumber->setAccessibleName	 ( PersonFields[DOCUMENTNUMBER] );
	m_ui.txtIdentity_ValidFrom_Until->setText( CardFields[CARD_VALIDFROM] + " - " + CardFields[CARD_VALIDUNTIL] );
	m_ui.txtIdentity_ValidFrom_Until->setAccessibleName( CardFields[CARD_VALIDFROM] + " - " + CardFields[CARD_VALIDUNTIL] );
	m_ui.txtIdentity_Parents->setText( PersonFields[PARENTS] );
	m_ui.txtIdentity_Parents->setAccessibleName( PersonFields[PARENTS] );
}

//*****************************************************
// refresh the tab with the ID extra info (card back side)
//*****************************************************
void MainWnd::refreshTabIdentityExtra()
{
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();

	m_ui.txtIdentityExtra_TaxNo->setText	( PersonFields[TAXNO] );
	m_ui.txtIdentityExtra_TaxNo->setAccessibleName	( PersonFields[TAXNO] );
	m_ui.txtIdentityExtra_SocialSecurityNo->setText	( PersonFields[SOCIALSECURITYNO] );
	m_ui.txtIdentityExtra_SocialSecurityNo->setAccessibleName	( PersonFields[SOCIALSECURITYNO] );
	m_ui.txtIdentityExtra_HealthNo->setText	( PersonFields[HEALTHNO] );
	m_ui.txtIdentityExtra_HealthNo->setAccessibleName	( PersonFields[HEALTHNO] );
	m_ui.txtIdentityExtra_CardVersion->setText	( PersonFields[CARDVERSION] );
	m_ui.txtIdentityExtra_CardVersion->setAccessibleName	( PersonFields[CARDVERSION] );
	m_ui.txtIdentityExtra_DocumentType->setText	( PersonFields[DOCUMENTTYPE] );
	m_ui.txtIdentityExtra_DocumentType->setAccessibleName	( PersonFields[DOCUMENTTYPE] );
	m_ui.txtIdentityExtra_IssuingEntity->setText	( PersonFields[ISSUINGENTITY] );
	m_ui.txtIdentityExtra_IssuingEntity->setAccessibleName	( PersonFields[ISSUINGENTITY] );

	m_ui.txtIdentityExtra_LocalofRequest->setText	( PersonFields[LOCALOFREQUEST] );
	m_ui.txtIdentityExtra_LocalofRequest->setAccessibleName	( PersonFields[LOCALOFREQUEST] );

	m_ui.txtIdentityExtra_Validate->setText ( PersonFields[VALIDATION] );
	m_ui.txtIdentityExtra_Validate->setAccessibleName ( PersonFields[VALIDATION] );

	QString cardNumber = m_CI_Data.m_CardInfo.formatCardNumber(CardFields[CARD_NUMBER],m_CI_Data.m_CardInfo.getType());

	QString nationalNumber = m_CI_Data.m_PersonInfo.formatNationalNumber( PersonFields[NATIONALNUMBER],m_CI_Data.m_CardInfo.getType() );

	m_ui.txtIdentityExtra_ValidFrom_Until->setText( CardFields[CARD_VALIDFROM] + " - " + CardFields[CARD_VALIDUNTIL] );
	m_ui.txtIdentityExtra_ValidFrom_Until->setAccessibleName( CardFields[CARD_VALIDFROM] + " - " + CardFields[CARD_VALIDUNTIL] );

	tFieldMap& PersonExtraFields = m_CI_Data.m_PersonInfo.m_PersonExtraInfo.getFields();
	
	QMap<QString,QString> SpecialStatus;
	SpecialStatus["0"] = tr("none");
	SpecialStatus["1"] = tr("white cane");
	SpecialStatus["2"] = tr("extended minority");
	SpecialStatus["3"] = tr("white cane/extended minority");
	SpecialStatus["4"] = tr("yellow cane");
	SpecialStatus["5"] = tr("yellow cane/extended minority");

	/*tFieldMap& AddressFields = m_CI_Data.m_PersonInfo.m_AddressInfo.getFields();

	m_ui.txtIdentityExtra_Adress_Street->setText	( AddressFields[ADDRESS_STREET] );
	m_ui.txtIdentityExtra_Adress_Street->setAccessibleName	( AddressFields[ADDRESS_STREET] );
	m_ui.txtIdentityExtra_Adress_PostalCode->setText( AddressFields[ADDRESS_ZIPCODE] );
	m_ui.txtIdentityExtra_Adress_PostalCode->setAccessibleName( AddressFields[ADDRESS_ZIPCODE] );
	m_ui.txtIdentityExtra_Adress_Muncipality->setText( AddressFields[ADDRESS_CITY] );
	m_ui.txtIdentityExtra_Adress_Muncipality->setAccessibleName( AddressFields[ADDRESS_CITY] );
	m_ui.txtIdentityExtra_Adress_Country->setText	( AddressFields[ADDRESS_COUNTRY] );
	m_ui.txtIdentityExtra_Adress_Country->setAccessibleName	( AddressFields[ADDRESS_COUNTRY] );*/

	tFieldMap& MiscFields = m_CI_Data.m_MiscInfo.getFields();

	QStringList Remarks = fillRemarksField(MiscFields);

	int idx=0;
	int FieldCnt = Remarks.size();
	if (FieldCnt>0)
	{
//		m_ui.txtIdentityExtra_Remarks1->setText(Remarks[idx++]);
//		m_ui.txtIdentityExtra_Remarks1->setAccessibleName(Remarks[idx++]);
		FieldCnt--;
	}
	if (FieldCnt>0)
	{
//		m_ui.txtIdentityExtra_Remarks2->setText(Remarks[idx++]);
//		m_ui.txtIdentityExtra_Remarks2->setAccessibleName(Remarks[idx++]);
		FieldCnt--;
	}
	if (FieldCnt>0)
	{
//		m_ui.txtIdentityExtra_Remarks3->setText(Remarks[idx++]);
//		m_ui.txtIdentityExtra_Remarks3->setAccessibleName(Remarks[idx++]);
		FieldCnt--;
	}

}

//*****************************************************
// refresh the tab with the PTeid Address
//*****************************************************
void MainWnd::refreshTabAddress( void )
{

	if (pinactivate == 1)
	{
		if (addressPINRequest_triggered())
			loadCardDataAddress();
	}else{
		loadCardDataAddress();
	}
	tFieldMap& AddressFields = m_CI_Data.m_AddressInfo.getFields();

	m_ui.txtAddress_Municipality->setText		 ( AddressFields[ADDRESS_MUNICIPALITY] );
	m_ui.txtAddress_Municipality->setAccessibleName ( AddressFields[ADDRESS_MUNICIPALITY] );
	m_ui.txtAddress_District->setText			( AddressFields[ADDRESS_DISTRICT] );
	m_ui.txtAddress_District->setAccessibleName ( AddressFields[ADDRESS_DISTRICT] );
	m_ui.txtAddress_CivilParish->setText			( AddressFields[ADDRESS_CIVILPARISH] );
	m_ui.txtAddress_CivilParish->setAccessibleName ( AddressFields[ADDRESS_CIVILPARISH] );
	m_ui.txtAddress_StreetType1->setText			( AddressFields[ADDRESS_STREETTYPE1] );
	m_ui.txtAddress_StreetType1->setAccessibleName ( AddressFields[ADDRESS_STREETTYPE1] );
	m_ui.txtAddress_StreetType2->setText			( AddressFields[ADDRESS_STREETTYPE2] );
	m_ui.txtAddress_StreetType2->setAccessibleName ( AddressFields[ADDRESS_STREETTYPE2] );
	m_ui.txtAddress_StreetName->setText			( AddressFields[ADDRESS_STREETNAME] );
	m_ui.txtAddress_StreetName->setAccessibleName ( AddressFields[ADDRESS_STREETNAME] );
	m_ui.txtAddress_BuildingType1->setText			( AddressFields[ADDRESS_BUILDINGTYPE1] );
	m_ui.txtAddress_BuildingType1->setAccessibleName ( AddressFields[ADDRESS_BUILDINGTYPE1] );
	m_ui.txtAddress_BuildingType2->setText			( AddressFields[ADDRESS_BUILDINGTYPE2] );
	m_ui.txtAddress_BuildingType2->setAccessibleName ( AddressFields[ADDRESS_BUILDINGTYPE2] );
	m_ui.txtAddress_DoorNo->setText			( AddressFields[ADDRESS_DOORNO] );
	m_ui.txtAddress_DoorNo->setAccessibleName ( AddressFields[ADDRESS_DOORNO] );
	m_ui.txtAddress_Floor->setText			( AddressFields[ADDRESS_FLOOR] );
	m_ui.txtAddress_Floor->setAccessibleName ( AddressFields[ADDRESS_FLOOR] );
	m_ui.txtAddress_Side->setText			( AddressFields[ADDRESS_SIDE] );
	m_ui.txtAddress_Side->setAccessibleName ( AddressFields[ADDRESS_SIDE] );
	m_ui.txtAddress_Locality->setText			( AddressFields[ADDRESS_LOCALITY] );
	m_ui.txtAddress_Locality->setAccessibleName ( AddressFields[ADDRESS_LOCALITY] );
	m_ui.txtAddress_Zip4->setText			( AddressFields[ADDRESS_ZIP4] );
	m_ui.txtAddress_Zip4->setAccessibleName ( AddressFields[ADDRESS_ZIP4] );
	m_ui.txtAddress_Zip3->setText			( AddressFields[ADDRESS_ZIP3] );
	m_ui.txtAddress_Zip3->setAccessibleName ( AddressFields[ADDRESS_ZIP3] );
	m_ui.txtAddress_Place->setText			( AddressFields[ADDRESS_PLACE] );
	m_ui.txtAddress_Place->setAccessibleName ( AddressFields[ADDRESS_PLACE] );
	m_ui.txtAddress_PostalLocality->setText			( AddressFields[ADDRESS_POSTALLOCALITY] );
	m_ui.txtAddress_PostalLocality->setAccessibleName ( AddressFields[ADDRESS_POSTALLOCALITY] );
}

void MainWnd::PersoDataSaveButtonClicked( void )
{
	std::string PersoDataFile = "3f005f00ef07";
	std::string Misc = "misc";
	QString TxtPersoDataString = m_ui.txtPersoData->toPlainText().toUtf8();

	BEID_ReaderContext &ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
	BEID_SmartCard	 &Card	= ReaderContext.getEIDCard();
	BEID_Pins &Pins	= Card.getPins();
	BEID_Pin &Pin	= Pins.getPinByNumber(1);

	authPINRequest_triggered();

	const BEID_ByteArray oData(reinterpret_cast<const unsigned char*> (TxtPersoDataString.toStdString().c_str()), TxtPersoDataString.toStdString().size());

	Card.writeFile(PersoDataFile.c_str(), oData, &Pin, Misc.c_str());
}
//*****************************************************
// refresh the tab with the PTeid Personal Data
//*****************************************************
void MainWnd::refreshTabPersoData( void )
{
	persodatastatus = 0;

	loadCardDataPersoData();

	tFieldMap& PersoDataFields = m_CI_Data.m_PersoDataInfo.getFields();

	m_ui.txtPersoData->clear();
	m_ui.txtPersoData->insertPlainText	 ( PersoDataFields[PERSODATA_INFO] );

	connect(m_ui.btnPersoDataSave, SIGNAL(clicked()), this, SLOT( PersoDataSaveButtonClicked()));
}
//*****************************************************
// get the text for the type of card 
//*****************************************************
QString MainWnd::GetCardTypeText(QString const& cardType)
{
	QString strCardType;
	int iDocType = cardType.toInt();
	switch (iDocType)
	{
	case 11:
		strCardType	= tr("A. Bewijs van inschrijving in het vreemdelingenregister - Tijdelijk verblijf");
		break;
	case 12:
		strCardType	= tr("B. Bewijs van inschrijving in het vreemdelingenregister");
		break;
	case 13:
		strCardType	= tr("C. Identiteitskaart voor vreemdeling");
		break;
	case 14:
		strCardType	= tr("D. EG - langdurig ingezetene");
		break;
	case 15:
		strCardType	= tr("E. Verklaring van inschrijving");
		break;
	case 16:
		strCardType	= tr("E+. Verklaring van inschrijving");
		break;
	case 17:
		strCardType	= tr("F. Verblijfskaart van een familielid van een burger van de Unie");
		break;
	case 18:
		strCardType	= tr("F+. Verblijfskaart van een familielid van een burger van de Unie");
		break;
	default:
		strCardType = tr("Unknown");
		break;
	}
	return strCardType;
}
QString MainWnd::getSpecialOrganizationText( QString const& code)
{
	QString trSpecialOrganization;
	if ( "1" == code)
	{
		trSpecialOrganization = tr("SHAPE");
	}
	else
	{
		trSpecialOrganization = tr("NATO");
	}
	return trSpecialOrganization;
}
QString MainWnd::getDuplicataText( void )
{
	return tr("Duplicate: ");
}
QString MainWnd::getFamilyMemberText( void )
{
	return tr("Family member");
}
//*****************************************************
// Create the string to put in the 'remarks' field
//*****************************************************
QStringList MainWnd::fillRemarksField( tFieldMap& MiscFields )
{
	QStringList Remarks;
	//QString MemberOfFamily		= MiscFields[MEMBEROFFAMILY];
	QString Duplicata			= MiscFields[DUPLICATA];
	QString SpecialOrganization = MiscFields[SPECIALORGANIZATION];

	/*if (MemberOfFamily.size()>0)
	{
		Remarks.append(getFamilyMemberText());
	}*/
	if (Duplicata.size()>0 && "00"!=Duplicata)
	{
		Remarks.append( getDuplicataText() + Duplicata );
	}
	if (SpecialOrganization.size()>0)
	{
		Remarks.append( getSpecialOrganizationText(SpecialOrganization) );
	}
	return Remarks;
}

//*****************************************************
// refresh the tab with foreigner data (front of card)
//*****************************************************
void MainWnd::refreshTabForeigners( void )
{
	/*
	m_ui.lblForeigners_ImgPerson->setPixmap(m_imgPicture);

	tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();

	m_ui.txtForeigners_Name->setText		( PersonFields[NAME] );
	m_ui.txtForeigners_Name->setAccessibleName( PersonFields[NAME] );
	m_ui.txtForeigners_GivenNames->setText	( PersonFields[FIRSTNAME] );
	m_ui.txtForeigners_GivenNames->setAccessibleName( PersonFields[FIRSTNAME] );

	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	QString cardNumber = m_CI_Data.m_CardInfo.formatCardNumber(CardFields[CARD_NUMBER],m_CI_Data.m_CardInfo.getType());

	m_ui.txtForeigners_Card_Number->setText ( cardNumber );
	m_ui.txtForeigners_Card_Number->setAccessibleName ( cardNumber );
	QString cardTypeText = GetCardTypeText(CardFields[CARD_TYPE]);
	m_ui.txtForeigners_CardType->setText    ( cardTypeText );
	m_ui.txtForeigners_CardType->setAccessibleName( cardTypeText );
	m_ui.txtForeigners_ValidTot->setText    ( CardFields[CARD_VALIDUNTIL] );
	m_ui.txtForeigners_ValidTot->setAccessibleName( CardFields[CARD_VALIDUNTIL] );
	m_ui.txtForeigners_PlaceOfIssue->setText( CardFields[CARD_PLACEOFISSUE] + " " + CardFields[CARD_VALIDFROM]);
	m_ui.txtForeigners_PlaceOfIssue->setAccessibleName( CardFields[CARD_PLACEOFISSUE] + " " + CardFields[CARD_VALIDFROM]);
	*/
}

//*****************************************************
// refresh tab with foreigner extra info (back of card)
//*****************************************************
void MainWnd::refreshTabForeignersExtra( void )
{
	/*tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();

	m_ui.txtForeignersExtra_BirthDate->setText		( PersonFields[BIRTHDATE] + " " + PersonFields[BIRTHPLACE]);
	m_ui.txtForeignersExtra_BirthDate->setAccessibleName ( PersonFields[BIRTHDATE] + " " + PersonFields[BIRTHPLACE]);
	m_ui.txtForeignersExtra_Nationality->setText	( PersonFields[NATIONALITY] );
	m_ui.txtForeignersExtra_Nationality->setAccessibleName( PersonFields[NATIONALITY] );
	m_ui.txtForeignersExtra_Sex->setText			( PersonFields[SEX] );
	m_ui.txtForeignersExtra_Sex->setAccessibleName	( PersonFields[SEX] );

	QString nationalNumber = m_CI_Data.m_PersonInfo.formatNationalNumber(PersonFields[NATIONALNUMBER],m_CI_Data.m_CardInfo.getType());
	m_ui.txtForeignersExtra_NationalNumber->setText ( nationalNumber );
	m_ui.txtForeignersExtra_NationalNumber->setAccessibleName ( nationalNumber );*/

	//tFieldMap& AddressFields = m_CI_Data.m_PersonInfo.m_AddressInfo.getFields();

	/*m_ui.txtForeignersExtra_Adress_Street->setText	( AddressFields[ADDRESS_STREET] );
	m_ui.txtForeignersExtra_Adress_Street->setAccessibleName( AddressFields[ADDRESS_STREET] );
	m_ui.txtForeignersExtra_Adress_PostalCode->setText( AddressFields[ADDRESS_ZIPCODE] );
	m_ui.txtForeignersExtra_Adress_PostalCode->setAccessibleName( AddressFields[ADDRESS_ZIPCODE] );
	m_ui.txtForeignersExtra_Adress_Muncipality->setText( AddressFields[ADDRESS_CITY] );
	m_ui.txtForeignersExtra_Adress_Muncipality->setAccessibleName( AddressFields[ADDRESS_CITY] );
	m_ui.txtForeignersExtra_Adress_Country->setText	( AddressFields[ADDRESS_COUNTRY] );
	m_ui.txtForeignersExtra_Adress_Country->setAccessibleName( AddressFields[ADDRESS_COUNTRY] );

	tFieldMap& MiscFields = m_CI_Data.m_MiscInfo.getFields();

	QStringList Remarks = fillRemarksField(MiscFields);

	m_ui.txtForeignersExtra_Remarks1->setText("");
	m_ui.txtForeignersExtra_Remarks1->setAccessibleName("");
	m_ui.txtForeignersExtra_Remarks2->setText("");
	m_ui.txtForeignersExtra_Remarks2->setAccessibleName("");
	m_ui.txtForeignersExtra_Remarks3->setText("");
	m_ui.txtForeignersExtra_Remarks3->setAccessibleName("");

	int idx=0;
	int FieldCnt = Remarks.size();
	if (FieldCnt>0)
	{
		m_ui.txtForeignersExtra_Remarks1->setText(Remarks[idx++]);
		m_ui.txtForeignersExtra_Remarks1->setAccessibleName(Remarks[idx++]);
		FieldCnt--;
	}
	if (FieldCnt>0)
	{
		m_ui.txtForeignersExtra_Remarks2->setText(Remarks[idx++]);
		m_ui.txtForeignersExtra_Remarks2->setAccessibleName(Remarks[idx++]);
		FieldCnt--;
	}
	if (FieldCnt>0)
	{
		m_ui.txtForeignersExtra_Remarks3->setText(Remarks[idx++]);
		m_ui.txtForeignersExtra_Remarks3->setAccessibleName(Remarks[idx++]);
		FieldCnt--;
	}

	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();
	m_ui.txtForeignersExtra_ChipNumber->setText( CardFields[CHIP_NUMBER] );
	m_ui.txtForeignersExtra_ChipNumber->setAccessibleName( CardFields[CHIP_NUMBER] );

	tFieldMap& PersonExtraFields = m_CI_Data.m_PersonInfo.m_PersonExtraInfo.getFields();

	QMap<QString,QString> SpecialStatus;
	SpecialStatus["0"] = tr("none");
	SpecialStatus["1"] = tr("white cane");
	SpecialStatus["2"] = tr("extended minority");
	SpecialStatus["3"] = tr("white cane/extended minority");
	SpecialStatus["4"] = tr("yellow cane");
	SpecialStatus["5"] = tr("yellow cane/extended minority");

	m_ui.txtForeignerSpecialStatus->setText(SpecialStatus[PersonExtraFields[SPECIALSTATUS]]);
	m_ui.txtForeignerSpecialStatus->setAccessibleName(SpecialStatus[PersonExtraFields[SPECIALSTATUS]]);*/

}

//*****************************************************
// refresh the tab with SIS card info (front of card)
//*****************************************************
void MainWnd::refreshTabSis( void )
{
/*	tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();
	QString FirstName;

	m_ui.txtSis_Name->setText			( PersonFields[NAME] );
	m_ui.txtSis_Name->setAccessibleName	( PersonFields[NAME] );

	FirstName=PersonFields[FIRSTNAME];
	if(PersonFields[INITIALS]!="")
	{
		FirstName+=" ";
		FirstName+=PersonFields[INITIALS];
	}
	m_ui.txtSis_GivenNames->setText		( FirstName );
	m_ui.txtSis_GivenNames->setAccessibleName		( FirstName );
	m_ui.txtSis_BirthDate->setText		( PersonFields[BIRTHDATE] );
	m_ui.txtSis_BirthDate->setAccessibleName		( PersonFields[BIRTHDATE] );
	tFieldMap& PersonExtraFields = m_CI_Data.m_PersonInfo.m_PersonExtraInfo.getFields();

	m_ui.txtSis_SocialSecurityNumber->setText( PersonExtraFields[SOCIALSECURITYNUMBER] );
	m_ui.txtSis_SocialSecurityNumber->setAccessibleName( PersonExtraFields[SOCIALSECURITYNUMBER] );

	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	QString cardNumber = m_CI_Data.m_CardInfo.formatCardNumber(CardFields[CARD_NUMBER],m_CI_Data.m_CardInfo.getType());

	m_ui.txtSis_LogicalNumber->setText	( cardNumber );
	m_ui.txtSis_LogicalNumber->setAccessibleName	( cardNumber );
	m_ui.txtSis_ValidFrom->setText		( CardFields[CARD_VALIDFROM] );
	m_ui.txtSis_ValidFrom->setAccessibleName		( CardFields[CARD_VALIDFROM] );

	if (m_CI_Data.m_PersonInfo.isFemale())
	{
		m_imgSex=QPixmap(":/images/Images/Female.png");
	}
	else if (m_CI_Data.m_PersonInfo.isMale())
	{
		m_imgSex=QPixmap(":/images/Images/Male.png");
	}
	else
	{
		//!! oops
	}
	m_ui.lblSis_Sex->setPixmap( m_imgSex );
	*/

}

//*****************************************************
// refresh tab with SIS card extra info (back of card)
//*****************************************************
void MainWnd::refreshTabSisExtra( void )
{
/*	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();
	m_ui.txtSisExtra_ValidFrom_Until->setText( CardFields[CARD_VALIDFROM] + " - " + CardFields[CARD_VALIDUNTIL] );
	m_ui.txtSisExtra_ValidFrom_Until->setAccessibleName( CardFields[CARD_VALIDFROM] + " - " + CardFields[CARD_VALIDUNTIL] );
	*/
}

//*****************************************************
// clear the tab with the certificates
//*****************************************************
void MainWnd::clearTabCertificates( void )
{
	m_ui.treeCert->clear();

	m_ui.txtCert_Owner->setText( "" );
	m_ui.txtCert_Owner->setAccessibleName( "" );
	m_ui.txtCert_Issuer->setText( "" );
	m_ui.txtCert_Issuer->setAccessibleName( "" );
	m_ui.txtCert_ValidFrom->setText( "" );
	m_ui.txtCert_ValidFrom->setAccessibleName( "" );
	m_ui.txtCert_ValidUntil->setText( "" );
	m_ui.txtCert_ValidUntil->setAccessibleName( "" );
	m_ui.txtCert_KeyLenght->setText( "" );
	m_ui.txtCert_KeyLenght->setAccessibleName( "" );
	m_ui.txtCert_Status->setText( "" );
	m_ui.txtCert_Status->setAccessibleName( "" );
}

//*****************************************************
// clear the tab with the PIN info
//*****************************************************
void MainWnd::clearTabAddress( void )
{ 
       m_ui.txtAddress_Municipality->setText            ( "" );
       m_ui.txtAddress_Municipality->setAccessibleName ( "" );
       m_ui.txtAddress_District->setText                       ( "" );
       m_ui.txtAddress_District->setAccessibleName ( "" );
       m_ui.txtAddress_CivilParish->setText                    ( "" );
       m_ui.txtAddress_CivilParish->setAccessibleName ( "" );
       m_ui.txtAddress_StreetType1->setText                    ( "" );
       m_ui.txtAddress_StreetType1->setAccessibleName ( "" );
       m_ui.txtAddress_StreetType2->setText                    ( "" );
       m_ui.txtAddress_StreetType2->setAccessibleName ( "" );
       m_ui.txtAddress_StreetName->setText                     ( "" );
       m_ui.txtAddress_StreetName->setAccessibleName ( "" );
       m_ui.txtAddress_BuildingType1->setText                  ( "" );
       m_ui.txtAddress_BuildingType1->setAccessibleName ( "" );
       m_ui.txtAddress_BuildingType2->setText                  ( "" );
       m_ui.txtAddress_BuildingType2->setAccessibleName ( "" );
       m_ui.txtAddress_DoorNo->setText                 ( "" );
       m_ui.txtAddress_DoorNo->setAccessibleName ( "" );
       m_ui.txtAddress_Floor->setText                  ( "" );
       m_ui.txtAddress_Floor->setAccessibleName ( "" );
       m_ui.txtAddress_Side->setText                   ( "" );
       m_ui.txtAddress_Side->setAccessibleName ( "" );
       m_ui.txtAddress_Locality->setText                       ( "" );
       m_ui.txtAddress_Locality->setAccessibleName ( "" );
       m_ui.txtAddress_Zip4->setText                   ( "" );
       m_ui.txtAddress_Zip4->setAccessibleName ( "" );
       m_ui.txtAddress_Zip3->setText                   ( "" );
       m_ui.txtAddress_Zip3->setAccessibleName ( "" );
       m_ui.txtAddress_Place->setText                  ( "" );
       m_ui.txtAddress_Place->setAccessibleName ( "" );
       m_ui.txtAddress_PostalLocality->setText                 ( "" );
       m_ui.txtAddress_PostalLocality->setAccessibleName ( "" );
}


//*****************************************************
// clear the tab with the PIN info
//*****************************************************
void MainWnd::clearTabPins( void )
{
	m_ui.treePIN->clear();

	m_ui.txtPIN_Name->setText( "" );
	m_ui.txtPIN_Name->setAccessibleName( "" );
	m_ui.txtPIN_ID->setText( "" );
	m_ui.txtPIN_ID->setAccessibleName( "" );
	m_ui.txtPIN_Status->setText( "" );
	m_ui.txtPIN_Status->setAccessibleName( "" );
}


//*****************************************************
// refresh the tab with the certificates
//*****************************************************
void MainWnd::refreshTabCertificates( void )
{
	loadCardDataCertificates();
	//BEID_Certificates* CertificateFields = m_CI_Data.m_CertifInfo.getCertificates();
	//We look for the selected item
	QList<QTreeWidgetItem *> selectedItems = m_ui.treeCert->selectedItems();
	if(selectedItems.size()==0)
	{
		//If no item is selected, we select the signature certificate
		selectedItems = m_ui.treeCert->findItems ( QString("Signature"), Qt::MatchContains|Qt::MatchRecursive );
		if (selectedItems.size()>0)
		{
			selectedItems[0]->setSelected(true);
		}
	}

	if (selectedItems.size()>0)
	{
		on_treeCert_itemClicked((QTreeCertItem *)selectedItems[0], 0);
	}
}

//*****************************************************
// refresh the tab with the PIN info
//*****************************************************
void MainWnd::refreshTabCardPin( void )
{
	if ( 0 < m_CurrReaderName.length() )
	{
		BEID_ReaderContext& ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		if (ReaderContext.isCardPresent())
		{
			switch(ReaderContext.getCardType())
			{
			case BEID_CARDTYPE_EID:
			case BEID_CARDTYPE_KIDS:
			case BEID_CARDTYPE_FOREIGNER:
				{
					BEID_EIDCard&		Card		   = ReaderContext.getEIDCard();
					fillCardVersionInfo( Card );
					m_ui.txtPIN_Status->setText(tr("Not available"));
					m_ui.txtPIN_Status->setAccessibleName(tr("Not available"));
				}
			case BEID_CARDTYPE_SIS:
			case BEID_CARDTYPE_UNKNOWN:
			default:
				break;
			}
		}
	}
}

//*****************************************************
// refresh the tab with the software info
//*****************************************************
void MainWnd::refreshTabInfo( void )
{
}

//**************************************************
// menu items to change the language
// Each menu item has its associated function. To make Qt to
// change the language, just load another translation file.
// This load will generate an event of type QEvent::LanguageChange.
// To capture the event, the function 'changeEvent(QEvent *event)' has to be 
// implemented and the event type has to be checked. If it is a change 
// of language, just call the function 'translateUi()' (that is generated 
// automatically by Qt) and that's it.
//**************************************************

//**************************************************
// Switch UI language to English
//**************************************************
void MainWnd::setLanguageEn( void )
{
	setLanguage(GenPur::LANG_EN);
}

//**************************************************
// Switch UI language to Dutch
//**************************************************
void MainWnd::setLanguageNl( void )
{
	setLanguage(GenPur::LANG_NL);
}

//**************************************************
// Switch UI language to French
//**************************************************
void MainWnd::setLanguageFr( void )
{
	setLanguage(GenPur::LANG_FR);
}

//**************************************************
// Switch UI language to German
//**************************************************
void MainWnd::setLanguageDe( void )
{
	setLanguage(GenPur::LANG_DE);
}

//**************************************************
// set the language menu to the current language
//**************************************************
void MainWnd::setLanguage( void )
{
	setLanguage(m_Language);
}

//**************************************************
// change the language of the interface to the given language
// if it could not be loaded, don't change the language
//**************************************************
void MainWnd::setLanguage(GenPur::UI_LANGUAGE language)
{
	GenPur::UI_LANGUAGE lngLoaded = LoadTranslationFile(language);
	if ( lngLoaded == language)
	{
		m_Language = lngLoaded;					// keep what language we are in
	}
	setLanguageMenu(lngLoaded);					// set the language menu according to the loaded language
	//showTabs();
}

//**************************************************
// change the language of the interface to the given language
//**************************************************
void MainWnd::setLanguageMenu(QString const& strLang)
{
	setLanguageMenu(GenPur::getLanguage(strLang));		// set the correct menu item checked
}
//**************************************************
// Set the language menu item 
//**************************************************
void MainWnd::setLanguageMenu( GenPur::UI_LANGUAGE language)
{
	//----------------------------------------------
	// remove the check marks on the menu
	//----------------------------------------------

	for (QMap<GenPur::UI_LANGUAGE,QAction*>::iterator it = m_LanguageActions.begin()
		; it != m_LanguageActions.end()
		; it++
		)
	{
		(*it)->setChecked(false);
	}

	//----------------------------------------------
	// set the language check mark and write the setting
	//----------------------------------------------
	if (m_LanguageActions.end() != m_LanguageActions.find(language))
	{
		m_LanguageActions[language]->setChecked(true);
		m_Settings.setGuiLanguage(language);
	}
}

//**************************************************
// Initialize the language menu.
// Depending on the .qm files found we add menu items.
// It is assumed the .qm files are located with the exe.
// The translation files have the format:
// <prefix><language>.qm
// with the language a 2-character language string
//**************************************************
void MainWnd::InitLanguageMenu( void )
{
	QDir directory(m_Settings.getExePath());

	QString filePrefix(TRANSLATION_FILE_PREFIX);

	QStringList FileFilters;
	FileFilters << filePrefix + "*.qm";
	QStringList fileList = directory.entryList(FileFilters,QDir::Files);

	for (int x=0;x<fileList.size();x++)
	{

		QString language = fileList[x].mid(filePrefix.length(),2);
		QString filename = filePrefix + language + ".qm";

		if ( "eidmw_nl.qm" == fileList[x])
		{
			QString LanguageName = tr("&Portuguese");
			QAction *action1 = new QAction(LanguageName,this);
			m_LanguageActions[GenPur::LANG_NL]=action1;
			action1->setCheckable(true);
			m_ui.menuLanguage->addAction(action1);
			connect(action1, SIGNAL( triggered() ), this, SLOT(setLanguageNl()) );
		}
		else if( "eidmw_en.qm" == fileList[x])
		{
			QString LanguageName = tr("&English");
			QAction *action1 = new QAction(LanguageName,this);
			m_LanguageActions[GenPur::LANG_EN]=action1;
			action1->setCheckable(true);
			m_ui.menuLanguage->addAction(action1);
			connect(action1, SIGNAL( triggered() ), this, SLOT(setLanguageEn()) );
		}
	}
}

//**************************************************
// set tray icon corresponding to card(s) in the reader(s)
// no card reader selected:
//   display card inserted when a card is in any reader
// a card reader selected:
//   display card inserted if the card is in the selected reader
//**************************************************
void MainWnd::setCorrespondingTrayIcon( PopupEvent* pPopupEvent )
{

	QIcon TrayIco;
	TrayIco = QIcon( ":/images/Images/Icons/reader_nocard.png" );
	if(!m_pTrayIcon->isVisible())
	{
		m_pTrayIcon->setIcon(TrayIco);
		m_pTrayIcon->show();
	}

	if ( NULL==pPopupEvent  )
	{
		if ( 0==ReaderSet.readerCount() )
		{
			TrayIco = QIcon( ":/images/Images/Icons/reader_error.png" );
			m_pTrayIcon->setIcon(TrayIco);
			m_pTrayIcon->show();
		}
		return;
	}
	else if ( PopupEvent::ET_CARD_CHANGED == pPopupEvent->getType() )
	{
		QString readerName;

		//----------------------------------------------------------
		// if no card reader selected, find the first card reader with a card
		// present. If this is the card reader that gave the event, check
		// if it is an unknown card and adjust the icon accordingly.
		//----------------------------------------------------------
		if ( (unsigned long)-1 == m_Settings.getSelectedReader() )
		{
			TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
			for (unsigned long nrReaders=0; nrReaders<ReaderSet.readerCount();nrReaders++)
			{
				BEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(nrReaders);
				readerName = readerContext.getName();
				if (readerContext.isCardPresent() && readerName==pPopupEvent->getReaderName())
				{
					BEID_CardType	    cardType	  = readerContext.getCardType();
					if ( BEID_CARDTYPE_UNKNOWN==cardType )
					{
						TrayIco = QIcon( ":/images/Images/Icons/reader_question.png" );
					}
					nrReaders = ReaderSet.readerCount();	// stop looping
				}
			}
		}
		else
		{
			unsigned long ReaderIdx		 = m_Settings.getSelectedReader();
			readerName = ReaderSet.getReaderName(ReaderIdx);

			if (pPopupEvent->getReaderName() == readerName)
			{
				BEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(ReaderIdx);
				BEID_CardType	    cardType	  = readerContext.getCardType();
				if ( BEID_CARDTYPE_UNKNOWN==cardType )
				{
					TrayIco = QIcon( ":/images/Images/Icons/reader_question.png" );
				}
				else
				{
					TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
				}
			}
			else
			{
				return;
			}
		}
	}
	else if ( PopupEvent::ET_CARD_REMOVED == pPopupEvent->getType() )
	{
		if ( (unsigned long)-1 == m_Settings.getSelectedReader() )
		{
			for (unsigned long nrReaders=0; nrReaders<ReaderSet.readerCount();nrReaders++)
			{
				BEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(nrReaders);
				QString readerName = readerContext.getName();
				if (readerContext.isCardPresent())
				{
					TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
					BEID_CardType	    cardType	  = readerContext.getCardType();
					if ( BEID_CARDTYPE_UNKNOWN==cardType )
					{
						TrayIco = QIcon( ":/images/Images/Icons/reader_question.png" );
					}
					nrReaders = ReaderSet.readerCount();	// stop looping
				}
			}
		}
		else
		{
			unsigned long		ReaderIdx	  = m_Settings.getSelectedReader();
			BEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (readerContext.isCardPresent())
			{
				TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
			}
		}
	}
	m_pTrayIcon->setIcon(TrayIco);
	m_pTrayIcon->show();
}

//**************************************************
// custom event from the callback function
// When a card is inserted, we must determine if we have to reload
// the data.
// - don't reload in case the app is set as trayApplet.
// - at startup, we receive an event if a card was inserted
// - Add/remove of a card reader will make the card reader reset. Therefore
//   we also clean the data we might have in memory.
//   The fact that our internal data has a card pointer == NULL, tells
//   us that we either start up, or have cardreaders added/removed.
//**************************************************
void MainWnd::customEvent( QEvent* pEvent )
{
#ifdef WIN32
	if( pEvent->type() == QEvent::User+1 )
	{
		QuitEvent* qEvent = (QuitEvent*)pEvent;
		//----------------------------------------------------------
		// so we have to close down the application
		//----------------------------------------------------------
		if ( qEvent->m_msg == WM_QUERYENDSESSION)
		{
			on_actionE_xit_triggered();
		}
	}
	else
#endif 
	if( pEvent->type() == QEvent::User )
	{
		try
		{
 			if (!m_Pop)
 			{
				pEvent->accept();
				return;
 			}

			//----------------------------------------------------------
			// look what card reader sent the event and get the picture from
			// the card, and show the picture
			//----------------------------------------------------------
			PopupEvent*	pPopupEvent	= (PopupEvent*)pEvent;

			setCorrespondingTrayIcon(pPopupEvent);

			if (pPopupEvent->getType() ==  PopupEvent::ET_CARD_REMOVED && m_CI_Data.m_cardReader==pPopupEvent->getReaderName())
			{
				setEnabledPinButtons(false);
				setEnabledCertifButtons(false);
				m_ui.btnOCSPCheck->setEnabled(false);
				m_ui.txtCert_Status->setEnabled(false);
				enableFileSave(false);
			}
			//----------------------------------------------------------
			// card has been changed in a reader
			//----------------------------------------------------------
			if (pPopupEvent->getType() ==  PopupEvent::ET_CARD_CHANGED)
			{
				QString	cardReader = pPopupEvent->getReaderName();

				//----------------------------------------------------------
				// show a message in the status bar that a card has been inserted
				//----------------------------------------------------------
				QString statusMsg;
				statusMsg += tr("Leitor de cartoes: ");
				statusMsg += pPopupEvent->getReaderName();
				m_ui.statusBar->showMessage(statusMsg,m_STATUS_MSG_TIME);

				BEID_ReaderContext& readerContext	= ReaderSet.getReaderByName(cardReader.toLatin1());

				if (m_Settings.getShowNotification())
				{
					QString title(tr("Cartao Inserido:"));
					m_pTrayIcon->showMessage ( title, statusMsg, QSystemTrayIcon::Information) ;
				}

				//----------------------------------------------------------
				// if no card is present, then just enable the reload button
				//----------------------------------------------------------
				if (!readerContext.isCardPresent())
				{
					setEnableReload(true);
					pEvent->accept();
					return;
				}

				//----------------------------------------------------------
				// This custom event is arrived since a card is changed in a reader
				// check in which reader it is and disable the PIN/CERT buttons
				// to avoid inconsistencies.
				// We check on the card data pointer to verify if this is the first time
				// we have an event for 'card-changed'. The first time the pCard is NULL
				// meaning that no card was read yet, and thus no inconsistencies can exist.
				//----------------------------------------------------------
				BEID_CardType cardType = readerContext.getCardType();

				switch( cardType )
				{
				case BEID_CARDTYPE_EID:
				case BEID_CARDTYPE_KIDS:
				case BEID_CARDTYPE_FOREIGNER:
					{
						try
						{
							BEID_EIDCard& card = readerContext.getEIDCard();
							doPicturePopup( card );

						}
						catch (BEID_ExNotAllowByUser& e)
						{
							long err = e.GetError();
							err = err;
						}
						//------------------------------------
						// register certificates when needed
						//------------------------------------
						if (m_Settings.getRegCert())
						{
							bool bImported = ImportCertificates(cardReader);
							if (!isHidden())
							{
								showCertImportMessage(bImported);
							}
						}
						if (isHidden())
						{
							break;
						}
						//------------------------------------------------
						// first load the data if necessary, because this will check the test cards as well
						// and will ask if test cards are allowed
						//------------------------------------------------
						if ( m_Settings.getAutoCardReading() )
						{
							m_CI_Data.Reset(); 
							loadCardData();
						}
					}
					break;
				case BEID_CARDTYPE_SIS:
					//------------------------------------------------
					// first load the data if necessary, because this will check the test cards as well
					// and will ask if test cards are allowed
					//------------------------------------------------
					if ( m_Settings.getAutoCardReading() )
					{
						m_CI_Data.Reset(); 
						loadCardData();
					}
					break;
				case BEID_CARDTYPE_UNKNOWN:
					{
						clearGuiContent();
						QString msg(tr("Unknown card type"));
						ShowBEIDError( 0, msg );
					}
				default:
					break;
				}
			}
			pEvent->accept();		
		}
		catch (BEID_Exception& e)
		{
			long err = e.GetError();
			err = err;
		}
	}
	setEnableReload(true);
}
//**************************************************
// show the picture on the Card
//**************************************************
void MainWnd::doPicturePopup( BEID_Card& card )
{
	//------------------------------------------------
	// just return, we don't show the picture when the card is inserted
	// The setting to show the picture is used for the textballoon
	//------------------------------------------------
	return;
	if (!m_Settings.getShowPicture())
	{
		return;
	}
	if (card.getType() != BEID_CARDTYPE_SIS)
	{
		//------------------------------------------------
		// To show the picture we must:
		// - keep the status if test cards were allowed or not
		// - allways allow a testcard
		// - load the picture for the popup
		// - reset the allowTestCard like the user has set it
		//------------------------------------------------
		BEID_EIDCard& eidCard		 = static_cast<BEID_EIDCard&>(card);
		bool		  bAllowTestCard = eidCard.getAllowTestCard();

		if (!bAllowTestCard)
		{
			eidCard.setAllowTestCard(true);
		}
		/*const BEID_ByteArray& picture = eidCard.getPicture().getData();
		QPixmap				  pixMap;

		if (pixMap.loadFromData(picture.GetBytes(), picture.Size()))
		{
			m_Pop->setPixmap(pixMap);
			m_Pop->popUp();
		}*/
		eidCard.setAllowTestCard(bAllowTestCard);
	}
}

//**************************************************
// This function overloads the default QMainWindow::changeEvent(..) function
// Now we can check for the event of type QEvent::LanguageChange
// that is generated by the QTranslator::load() function, called when
// the menu item to change the language is selected.
// If we have to change the UI language, then call the retranslateUi()
// function on the UI and the interface will be set accordingly.
// Important to know is that the function 'retranslateUi()' seems to reset
// properties of the LineEdit-fields. e.g.: the background transparency is reset
// and the font size. Therefore, we have to initialize all the tabs and set the 
// zoom factor again.
//**************************************************
void MainWnd::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) 
	{
		m_ui.retranslateUi(this);
		initAllTabs();
		try
		{
			refreshTabCardPin();
			refreshTabIdentityExtra();
			//refreshTabAddress();
			//refreshTabCertificates();
			refreshTabForeignersExtra();
			//refreshTabPersoData();
		}
		catch (BEID_ExNoCardPresent& e)
		{
			e=e;
		}
		catch (BEID_Exception& e)
		{
			e=e;
		}
		createTrayMenu();
		setZoom();
		m_ui.lblIdentity_ImgPerson->setPixmap( m_imgPicture );

	} 
	else if (event->type() == QEvent::WindowStateChange )
	{
		QWindowStateChangeEvent* ev = (QWindowStateChangeEvent*)event;	
		if (ev->oldState()== Qt::WindowNoState)
		{
			QApplication::postEvent(this, new QCloseEvent());
		}
		else if (ev->oldState()==Qt::WindowMinimized)
		{
		}
	}
	else
	{
		QWidget::changeEvent(event);
	}
}

//**************************************************
// Quit the application.
// Release the SDK before stopping.
//**************************************************
void MainWnd::on_actionE_xit_triggered(void)
{
	try
	{
		hide();
		if (m_Settings.getRemoveCert())
		{
			for (unsigned long readerCount=0;readerCount<ReaderSet.readerCount();readerCount++)
			{
				QString readerName = ReaderSet.getReaderName(readerCount);
				RemoveCertificates( readerName );
			}
		}

		//-------------------------------------------------------------------
		// we must release all the certificate contexts before releasing the SDK.
		// After Release, no more calls should be done to the SDK and as such
		// noting should be done in the dtor
		//-------------------------------------------------------------------
		forgetAllCertificates();
		stopAllEventCallbacks();

		qApp->quit();
	}
	catch (BEID_Exception &e)
	{
		QString msg(tr("General exception"));
		ShowBEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowBEIDError( 0, msg );
	}
}
//**************************************************
// set the event callback functions
//**************************************************
void MainWnd::setEventCallbacks( void )
{
	//----------------------------------------
	// for all the reader, create a callback such we can know
	// afterwards, which reader called us
	//----------------------------------------
	try
	{
		size_t maxcount=ReaderSet.readerCount(true);
		for (size_t Ix=0; Ix<maxcount; Ix++)
		{
			void (*fCallback)(long lRet, unsigned long ulState, void* pCBData);
	
			const char*			 readerName		= ReaderSet.getReaderName(Ix);
			BEID_ReaderContext&  readerContext  = ReaderSet.getReaderByNum(Ix);
			CallBackData*		 pCBData		= new CallBackData(readerName,this);
	
			fCallback = (void (*)(long,unsigned long,void *))&cardEventCallback;
	
			m_callBackHandles[readerName] = readerContext.SetEventCallback(fCallback,pCBData);
			m_callBackData[readerName]	  = pCBData;
		}
	}
	catch(BEID_Exception& e)
	{
		QString msg(tr("setEventCallbacks"));
		ShowBEIDError( e.GetError(), msg );
	}

}

//**************************************************
// display a messagebox with the error code
//**************************************************
void MainWnd::ShowBEIDError( unsigned long ErrCode, QString const& msg )
{
	if (isHidden())
	{
		return;
	}
	QString strCaption(tr("Error"));
	QString strMessage;
	strMessage = strMessage.setNum(ErrCode,16);
	strMessage += ": ";
	strMessage += msg;
	QMessageBox::warning( this, strCaption,  strMessage, QMessageBox::Ok );
}

#ifdef WIN32
//**************************************************
// Use Minidriver if OS is Vista or later
//**************************************************
BOOL MainWnd::UseMinidriver( void )
{
    OSVERSIONINFO osvi;
    BOOL bIsWindowsVistaorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsVistaorLater = (osvi.dwMajorVersion >= 6);

	return bIsWindowsVistaorLater;
}

//**************************************************
// Checks of older registered certificates are not
// still bound to the CSP when the minidriver is used
//**************************************************
bool MainWnd::ProviderNameCorrect (PCCERT_CONTEXT pCertContext )
{
	unsigned long dwPropId= CERT_KEY_PROV_INFO_PROP_ID; 
	DWORD cbData = 0;
	CRYPT_KEY_PROV_INFO * pCryptKeyProvInfo;

	if (!UseMinidriver())
		return true;

	if(!(CertGetCertificateContextProperty(
		pCertContext,		// A pointer to the certificate where the property will be set.
		dwPropId,           // An identifier of the property to get.
		NULL,               // NULL on the first call to get the length.
		&cbData)))          // The number of bytes that must be allocated for the structure.
	{
		if (GetLastError() != CRYPT_E_NOT_FOUND) // The certificate does not have the specified property.
			return false;
	}
	if(!(pCryptKeyProvInfo = (CRYPT_KEY_PROV_INFO *)malloc(cbData)))
	{
		return true;
	}
	if(CertGetCertificateContextProperty(pCertContext, dwPropId, pCryptKeyProvInfo, &cbData))
	{
		if (!wcscmp(pCryptKeyProvInfo->pwszProvName, L"Belgium Identity Card CSP"))
			return false;
	}
	return true;
}
#endif
