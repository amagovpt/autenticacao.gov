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
#include "myQLabel.h"
#include <QApplication>


void myQLabel::mousePressEvent(QMouseEvent * ev) {
	if (ev->button() == Qt::LeftButton) {
		underlyingButton->setIcon(QIcon(imagePressed));
		QApplication::sendPostedEvents();
		
	}
	
}

void myQLabel::mouseReleaseEvent(QMouseEvent * ev) {
	if (ev->button() == Qt::LeftButton) {
		if (this->underMouse()) {
			underlyingButton->setIcon(QIcon(imageReleased));
			underlyingButton->repaint();
			QApplication::sendPostedEvents();
	    	emit doClick();
		}
	}
	
}

void myQLabel::doClick()
{
	underlyingButton->releaseMouse();
	underlyingButton->click();
};

