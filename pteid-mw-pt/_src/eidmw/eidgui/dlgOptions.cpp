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
#include "dlgOptions.h"
#include "eidlib.h"
#include "Settings.h"

using namespace eIDMW;

dlgOptions::dlgOptions( GUISettings& settings, QWidget *parent ) 
	: QDialog(parent)
	, m_Settings(settings)
{	
	ui.setupUi(this); 
	ui.chbShowPicture->hide();
	ui.chbShowToolbar->hide();
	ui.chbAutoCardReading->setChecked( settings.getAutoCardReading() );
	ui.chbWinAutoStart->setChecked( settings.getAutoStartup() );
#ifndef WIN32 
//#ifndef __APPLE__
	ui.chbWinAutoStart->hide();
//#endif
#endif

	ui.chbStartMinimized->setChecked( settings.getStartMinimized() );
	ui.chbShowNotification->setChecked( settings.getShowNotification() );

	ui.chbRegCert->setChecked( settings.getRegCertSetting() );
	ui.chbRemoveCert->setChecked( settings.getRemoveCertSetting() );

	ui.chbRegCert->setEnabled(settings.getAutoCardReading());
	ui.chbRemoveCert->setEnabled(settings.getAutoCardReading());

//	ui.chbUseKeyPad->setChecked( settings.getUseVirtualKeyPad() );
	ui.cmbCardReader->addItem(tr("Not specified"));
	const char* const* ReaderList = ReaderSet.readerList();
	for (unsigned long Idx=0; Idx<ReaderSet.readerCount(); Idx++)
	{
		const char* readerName = ReaderList[Idx];
		ui.cmbCardReader->addItem(readerName);
	}
	int SelectedReader = m_Settings.getSelectedReader()+1;
	ui.cmbCardReader->setCurrentIndex( SelectedReader );

#ifndef WIN32
	ui.chbRegCert->hide();
	ui.chbRemoveCert->hide();
	ui.groupBox->hide();
#endif
}

dlgOptions::~dlgOptions()
{	
}
void dlgOptions::on_chbShowToolbar_toggled( bool bChecked )
{
	m_Settings.setShowToolbar( bChecked );
}
void dlgOptions::on_chbShowPicture_toggled( bool bChecked )
{
	m_Settings.setShowPicture( bChecked );
}
void dlgOptions::on_chbShowNotification_toggled( bool bChecked )
{
	m_Settings.setShowNotification( bChecked );
}
/*
void dlgOptions::on_chbUseKeyPad_toggled( bool bChecked )
{
	m_Settings.setUseVirtualKeyPad( bChecked );
}
*/
void dlgOptions::on_chbAutoCardReading_toggled( bool bChecked )
{
	m_Settings.setAutoCardReading(bChecked);
	ui.chbRegCert->setEnabled(bChecked);
	ui.chbRemoveCert->setEnabled(bChecked);
}
void dlgOptions::on_chbWinAutoStart_toggled( bool bChecked )
{
	m_Settings.setAutoStartup( bChecked );
}
void dlgOptions::on_chbStartMinimized_toggled( bool bChecked )
{
	m_Settings.setStartMinimized( bChecked );
}
void dlgOptions::on_cmbCardReader_activated ( int index )
{
	m_Settings.setSelectedReader(index-1);	// the first item is 'no cardreader' Put then the index to illegal (==-1)
}
void dlgOptions::on_chbRegCert_toggled( bool bChecked )
{
	m_Settings.setRegCert(bChecked);
}
void dlgOptions::on_chbRemoveCert_toggled( bool bChecked )
{
	m_Settings.setRemoveCert(bChecked);
}
