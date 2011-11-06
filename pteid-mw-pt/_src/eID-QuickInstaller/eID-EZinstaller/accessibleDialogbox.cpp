/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2009-2010 FedICT.
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

#include "accessibleDialogbox.h"


QT_BEGIN_NAMESPACE

accessibleDialogbox::accessibleDialogbox( QString title,QString body, QString acceptButton, QString cancelButton, QWidget *parent )
{
	dialog = new QDialog(parent,Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
	dialog->setWindowTitle(title);
	dialog->setAccessibleName(title);
	dialog->setModal ( TRUE );

    label = new QLabel(body);
	label->setFocusPolicy(Qt::TabFocus);
	label->setAccessibleName(body);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

#if defined(Q_WS_MAC)
    label->setContentsMargins(16, 0, 0, 0);
	QFont f = dialog->font();
    f.setBold(true);
    label->setFont(f);
#elif !defined(Q_WS_QWS)
    label->setContentsMargins(2, 0, 0, 0);
    label->setIndent(9);
#endif

	if(acceptButton != NULL)
	{
		ok = new QPushButton( acceptButton );
		ok->setAccessibleName( acceptButton );
		dialog->connect( ok, SIGNAL(clicked()), SLOT(accept()) );
	}
	if(cancelButton != NULL)
	{
		cancel = new QPushButton( cancelButton );
		cancel->setAccessibleName( cancelButton );
		dialog->connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
	}

	grid = new QGridLayout(dialog);

#ifndef Q_WS_MAC
    grid->addWidget(label, 0, 1, 1, 1);

	if(acceptButton != NULL)
	{
		grid->addWidget(ok, 2, 0, 1, 2,Qt::AlignLeft);
	}
	if(cancelButton != NULL)
	{
		grid->addWidget(cancel, 2, 3, 1, 2,Qt::AlignRight);
	}
#else
    grid->setMargin(0);
    grid->setVerticalSpacing(8);
    grid->setHorizontalSpacing(0);
    grid->setContentsMargins(24, 15, 24, 20);
    //grid->addWidget(iconLabel, 0, 0, 2, 1, Qt::AlignTop | Qt::AlignLeft);
    grid->addWidget(label, 0, 1, 1, 1);
    // -- leave space for information label --
    grid->setRowStretch(1, 100);
    grid->setRowMinimumHeight(2, 6);

    if(acceptButton != NULL)
	{
		grid->addWidget(ok, 2, 0, 1, 2,Qt::AlignLeft);
	}
	if(cancelButton != NULL)
	{
		grid->addWidget(cancel, 2, 3, 1, 2,Qt::AlignRight);
	}
#endif
}


accessibleDialogbox::~accessibleDialogbox()
{
	if(dialog)
	{
		delete dialog;
		dialog = NULL;
	}
}

int accessibleDialogbox::exec( )
{
	return dialog->exec();
}

int accessibleDialogbox::showDialogBox( QString title,QString body, QString acceptButton, QString cancelButton, QWidget *parent )
{
	accessibleDialogbox msgBox( title,body,acceptButton,cancelButton,parent );
	return msgBox.exec();
}

QT_END_NAMESPACE
