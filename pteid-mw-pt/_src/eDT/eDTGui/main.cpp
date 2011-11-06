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
#include <QtGui/QApplication>
#include <QtPlugin>
#include "qtsingleapplication.h"
#include "edtgui.h"
#include "user.h"

int main(int argc, char *argv[])
{
	QtSingleApplication instance("eDTSingleInstance", argc, argv);
	if (instance.sendMessage("Wake up!"))
		return 0;

	instance.initialize();

	//---------------------------------
	// we must be Admin to be able to run this tool
	//---------------------------------
	bool	bAdmin = false;
	User_ID userID = CURRENT_USER_ID;
	if ( DIAGLIB_OK != userIsAdministrator(userID,&bAdmin))
	{
		QString caption;
		caption = "Fout - Erreur";
		QString msg;
		msg += "Er is een fout opgetreden bij het nakijken van de gebruikersrechten.";
		msg += "\nUne erreur s'est produite en vérifiant les droits de l'utilisateur.";
		QMessageBox::information(NULL,caption,msg);
		return -1;
	}
	if (!bAdmin)
	{
		QString caption;
		caption = "Fout - Erreur";
		QString msg;
		msg += "Men moet administratierechten hebben om dit programma uit te voeren.";
		msg += "\nIl faut avoir les droits d'administrateur pour exécuter ce logiciel.";
		QMessageBox::information(NULL,caption,msg);
		return -1;
	}

	eDTGui w;
	instance.setActivationWindow (&w);
	QObject::connect(&instance, SIGNAL(instance(const QString&)),
		&instance, SLOT(activateMainWidget()));
	w.show();
	return instance.exec();
}