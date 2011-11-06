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
#ifndef PICTUREPOPUP_H
#define PICTUREPOPUP_H

#include <QLabel>
#include <QTimer>
#include "ui_picturepopup.h"

class PicturePopup : public QLabel
{
    Q_OBJECT
public:
    PicturePopup(QWidget *parent = 0);
    ~PicturePopup();
    void popUp( const QString&  strCaption  = QString::null
			  , long			lTimeToShow = 500
			  , long			lTimeToLive = 3000
              , long			lTimeToHide = 500
			  );

public slots:
    virtual void OnTimerAppear();
    virtual void OnTimerWait();
    virtual void OnTimerDisappear();

private:
    Ui::PicturePopupClass	ui;
    long					m_lTimeToShow;
    long					m_lTimeToLive;
    long					m_lTimeToHide;
    long					m_lDelayBetweenShowEvents;
    long					m_lDelayBetweenHideEvents;
    int						m_nStartPosX;
    int						m_nStartPosY;
    int						m_nCurrentPosX;
    int						m_nCurrentPosY;
    int						m_nTaskbarPlacement;
    int						m_nIncrement;
    QString					m_strCaption;
    int						m_nAnimStatus;
    int						m_nSkinWidth;
    int						m_nSkinHeight;
    int						m_nTaskbarHeight;
    QTimer*					m_pTimerAppear;
    QTimer*					m_pTimerDisappear;
    QTimer*					m_pTimerWait;
};



#endif // PICTUREPOPUP_H

