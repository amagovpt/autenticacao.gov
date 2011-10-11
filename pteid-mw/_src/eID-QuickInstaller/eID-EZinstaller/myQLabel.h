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
#ifndef __myQLabel__
#define __myQLabel__
#include <QLabel>
#include <QMouseEvent>
#include <QCommandLinkButton>

class myQLabel : public QLabel {
private:
	 void mousePressEvent(QMouseEvent * ev);
	 void mouseReleaseEvent(QMouseEvent * ev);
	 QString  imagePressed;
	 QString  imageReleased;
public:


	void setIcons (QString  _imagePressed, QString _imageReleased) {
	    imagePressed = _imagePressed;
		imageReleased = _imageReleased;
	};
	
	QCommandLinkButton * underlyingButton;
	void setunderlyingButton(QCommandLinkButton *  clb)
	{
		underlyingButton = clb;
		
		this->setFont(clb->font());
		this->setWordWrap(true);
		this->setText(clb->text());
		this->setAlignment(Qt::AlignCenter|Qt::AlignHCenter|Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

		this->setParent(clb->parentWidget());
		int labelwidth = clb->width();
		int labelheight = clb->height();
		int xpos = clb->x()+((clb->width() - labelwidth)/2);
		int ypos = clb->y()+((clb->height() - labelheight)/2);

		this->setGeometry(xpos,ypos,labelwidth,labelheight);
		this->setCursor(Qt::PointingHandCursor);
	};
	void resetLabelText(){
	    this->setText(underlyingButton->text());
	};


signals:
	void doClick();
};
#endif
