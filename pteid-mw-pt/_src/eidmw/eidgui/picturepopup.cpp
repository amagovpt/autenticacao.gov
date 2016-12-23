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
#include <QDesktopWidget>
#include <QMessageBox>
#include "picturepopup.h"

#define IDT_HIDDEN		     0
#define IDT_APPEARING		 1
#define IDT_WAITING		     2
#define IDT_DISAPPEARING	 3
#define TASKBAR_ON_TOP		 1
#define TASKBAR_ON_LEFT		 2
#define TASKBAR_ON_RIGHT	 3
#define TASKBAR_ON_BOTTOM	 4

PicturePopup::PicturePopup(QWidget *parent)
	: QLabel(parent, Qt::Popup)
	, m_lTimeToShow(0)
	, m_lTimeToLive(0)
	, m_lTimeToHide(0)
	, m_lDelayBetweenShowEvents(0)
	, m_lDelayBetweenHideEvents(0)
	, m_nStartPosX(0)
	, m_nStartPosY(0)
	, m_nCurrentPosX(0)
	, m_nCurrentPosY(0)
	, m_nTaskbarPlacement(0)
	, m_nIncrement(7)
	, m_strCaption("")
	, m_nAnimStatus(IDT_HIDDEN)
	, m_nTaskbarHeight(0)
{
	ui.setupUi(this);

    setFrameStyle( WinPanel | Raised );
    setAlignment( Qt::AlignCenter );
    setScaledContents(false);
    m_pTimerAppear = new QTimer(this);
    connect(m_pTimerAppear, SIGNAL(timeout()), this, SLOT(OnTimerAppear()));
    m_pTimerDisappear = new QTimer(this);
    connect(m_pTimerDisappear, SIGNAL(timeout()), this, SLOT(OnTimerDisappear()));
    m_pTimerWait = new QTimer(this);
    connect(m_pTimerWait, SIGNAL(timeout()), this, SLOT(OnTimerWait()));
}

PicturePopup::~PicturePopup()
{
	//-----------------------------
	// make sure timers are stopped
	//-----------------------------
	if(m_pTimerAppear)
	{
		m_pTimerAppear->stop();
		delete(m_pTimerAppear);
		m_pTimerAppear = NULL;
	}
	if(m_pTimerDisappear)
	{
		m_pTimerDisappear->stop();
		delete(m_pTimerDisappear);
		m_pTimerDisappear = NULL;
	}
	if(m_pTimerWait)
	{
		m_pTimerWait->stop();
		delete(m_pTimerWait);
		m_pTimerWait = NULL;
	}
}

void PicturePopup::popUp(  const QString & strCaption
						 , long			lTimeToShow
						 , long			lTimeToLive
						 , long			lTimeToHide
						 )
{
	const QPixmap*	pict  = pixmap();
	int				width = pict->width();
	int				height= pict->height();
	resize(width,height);

	m_nSkinHeight	= height;
	m_nSkinWidth	= width;

	unsigned int nDesktopHeight = 0;
	unsigned int nDesktopWidth  = 0;
	unsigned int nScreenWidth   = 0;
	unsigned int nScreenHeight  = 0;

	QRect rcDesktop;
	QRect rcScreen;
	m_strCaption	= strCaption;
	m_lTimeToShow	= lTimeToShow;
	m_lTimeToLive	= lTimeToLive;
	m_lTimeToHide	= lTimeToHide;

    QDesktopWidget* desktopWidget = QApplication::desktop();			// get the desktop
	int primaryScreen = desktopWidget->primaryScreen();					// get the primary screen

    rcDesktop		= desktopWidget->availableGeometry(primaryScreen);	// get the desktop rectangle
	rcScreen		= desktopWidget->screenGeometry(primaryScreen);		// get the screen geometry

	nDesktopWidth	= rcDesktop.width();
	nDesktopHeight	= rcDesktop.height();
	nScreenWidth	= rcScreen.width();
	nScreenHeight	= rcScreen.height();

    if(nScreenWidth < 1024)
    {
	    m_nSkinHeight = 100;
	    m_nSkinWidth = 70;
    }
 	bool bTaskbarOnRight	= nDesktopWidth  < nScreenWidth && rcDesktop.left() == 0;
 	bool bTaskbarOnLeft		= nDesktopWidth  < nScreenWidth && rcDesktop.left() != 0;
 	bool bTaskBarOnTop		= nDesktopHeight < nScreenHeight && rcDesktop.top() !=0;

	switch (m_nAnimStatus)
	{
		case IDT_HIDDEN:
			if (bTaskbarOnRight)
			{
				m_lDelayBetweenShowEvents = m_lTimeToShow/(m_nSkinWidth/m_nIncrement);
				m_lDelayBetweenHideEvents = m_lTimeToHide/(m_nSkinWidth/m_nIncrement);
				m_nStartPosX			  = rcDesktop.right();
				m_nStartPosY			  = rcDesktop.bottom() - m_nSkinHeight;
				m_nTaskbarPlacement		  = TASKBAR_ON_RIGHT;
			}
			else if (bTaskbarOnLeft)
			{
				m_lDelayBetweenShowEvents = m_lTimeToShow/(m_nSkinWidth/m_nIncrement);
				m_lDelayBetweenHideEvents = m_lTimeToHide/(m_nSkinWidth/m_nIncrement);
				m_nStartPosX			  = rcDesktop.left() - m_nSkinWidth;
				m_nStartPosY			  = rcDesktop.bottom() - m_nSkinHeight;
				m_nTaskbarPlacement		  = TASKBAR_ON_LEFT;
			}
			else if (bTaskBarOnTop)
			{
				m_lDelayBetweenShowEvents = m_lTimeToShow/(m_nSkinHeight/m_nIncrement);
				m_lDelayBetweenHideEvents = m_lTimeToHide/(m_nSkinHeight/m_nIncrement);
				m_nStartPosX			  = rcDesktop.right() - m_nSkinWidth;
				m_nStartPosY			  = rcDesktop.top() - m_nSkinHeight;
				m_nTaskbarPlacement		  = TASKBAR_ON_TOP;
			}
			else //if (bTaskbarOnBottom)
			{
				// Taskbar is on the bottom or Invisible
                m_nTaskbarHeight		  = nScreenHeight - nDesktopHeight;
				m_lDelayBetweenShowEvents = m_lTimeToShow/(m_nSkinHeight/m_nIncrement);
				m_lDelayBetweenHideEvents = m_lTimeToHide/(m_nSkinHeight/m_nIncrement);
				m_nStartPosX		      = rcDesktop.right() - m_nSkinWidth;
				m_nStartPosY			  = rcDesktop.bottom();
				m_nTaskbarPlacement		  = TASKBAR_ON_BOTTOM;
			}
			m_nCurrentPosX = m_nStartPosX;
			m_nCurrentPosY = m_nStartPosY;
            m_pTimerAppear->start(m_lDelayBetweenShowEvents);
			break;
		case IDT_WAITING:
			repaint();
            m_pTimerWait->stop();
			m_pTimerWait->setSingleShot(true);
            m_pTimerWait->start(m_lTimeToLive);
			break;
		case IDT_APPEARING:
			repaint();
			break;
		case IDT_DISAPPEARING:
            m_pTimerDisappear->stop();
			m_pTimerWait->setSingleShot(true);
            m_pTimerWait->start(m_lTimeToLive);
			if (bTaskbarOnRight)
			{
				m_nCurrentPosX=rcDesktop.right() - m_nSkinWidth;
			}
			else if (bTaskbarOnLeft)
			{
				m_nCurrentPosX=rcDesktop.left();
			}
			else if (bTaskBarOnTop)
			{
				m_nCurrentPosY=rcDesktop.top();
			}
			else //if (bTaskbarOnBottom)
			{
				m_nCurrentPosY=rcDesktop.bottom() - m_nSkinHeight;
			}
			setGeometry(m_nCurrentPosX, m_nCurrentPosY, m_nSkinWidth, m_nSkinHeight);
			repaint();
			break;
	}
}

void PicturePopup::OnTimerAppear()
{
    m_nAnimStatus = IDT_APPEARING;
    bool bContinue = false;

    switch(m_nTaskbarPlacement)
    {
	    case TASKBAR_ON_BOTTOM:
		    if (m_nCurrentPosY > (m_nStartPosY - m_nSkinHeight))
            {
			    m_nCurrentPosY -= m_nIncrement;
                bContinue = true;
            }
		    break;
	    case TASKBAR_ON_TOP:
		    if ((m_nCurrentPosY - m_nStartPosY) < m_nSkinHeight)
            {
			    m_nCurrentPosY += m_nIncrement;
                bContinue = true;
            }
		    break;
	    case TASKBAR_ON_LEFT:
		    if ((m_nCurrentPosX - m_nStartPosX) < m_nSkinWidth)
            {
			    m_nCurrentPosX += m_nIncrement;
                bContinue = true;
            }
		    break;
	    case TASKBAR_ON_RIGHT:
		    if (m_nCurrentPosX > (m_nStartPosX-m_nSkinWidth))
            {
			    m_nCurrentPosX-=m_nIncrement;
                bContinue = true;
            }
		    break;
    }
    if(!bContinue)
    {
        m_pTimerAppear->stop();
		m_pTimerWait->setSingleShot(true);
        m_pTimerWait->start(m_lTimeToLive);
		m_nAnimStatus = IDT_WAITING;
    }
    setGeometry(m_nCurrentPosX, m_nCurrentPosY, m_nSkinWidth, m_nSkinHeight);
    show();
}

void PicturePopup::OnTimerWait()
{
    m_pTimerDisappear->start(m_lDelayBetweenHideEvents);
}

void PicturePopup::OnTimerDisappear()
{
    bool bContinue = false;
    m_nAnimStatus=IDT_DISAPPEARING;
    switch(m_nTaskbarPlacement)
    {
	    case TASKBAR_ON_BOTTOM:
		    if (m_nCurrentPosY<m_nStartPosY + m_nTaskbarHeight)
            {
			    m_nCurrentPosY += m_nIncrement;
                bContinue = true;
            }
	        break;
	    case TASKBAR_ON_TOP:
		    if (m_nCurrentPosY>m_nStartPosY)
            {
			    m_nCurrentPosY -= m_nIncrement;
                bContinue = true;
            }
		    break;
	    case TASKBAR_ON_LEFT:
		    if (m_nCurrentPosX>m_nStartPosX)
            {
			    m_nCurrentPosX -= m_nIncrement;
                bContinue = true;
            }
		    break;
	    case TASKBAR_ON_RIGHT:
		    if (m_nCurrentPosX<m_nStartPosX)
            {
			    m_nCurrentPosX += m_nIncrement;
                bContinue = true;
            }
		    break;
    }
    if(!bContinue)
    {
        m_pTimerDisappear->stop();
        m_nAnimStatus = IDT_HIDDEN;
        hide();
    }
    setGeometry(m_nCurrentPosX, m_nCurrentPosY, m_nSkinWidth, m_nSkinHeight);
}
