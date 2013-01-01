/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "HUD2.h"
#include "UASManager.h"
#include "UAS.h"

HUD2::~HUD2()
{
    disconnect(&timer);
    timer.stop();
}

HUD2::HUD2(QWidget *parent)
    : QWidget(parent)
{
    uas   = NULL;
    usegl = false;

    data = new HUD2data();
    hudpainter = new HUD2Painter(data, parent);

    surface_gl = new HUD2PaintSurfaceGL(hudpainter, data, this);
    surface    = new HUD2PaintSurface(hudpainter, data, this);

    layout.addWidget(surface,    0, 0);
    layout.addWidget(surface_gl, 0, 0);

    if (usegl == true){
        surface->hide();
        btn.setText(tr("GL"));
        //connect(&timer, SIGNAL(timeout()), surface_gl, SLOT(animate()));
    }
    else{
        surface_gl->hide();
        btn.setText(tr("Soft"));
        //connect(&timer, SIGNAL(timeout()), surface, SLOT(animate()));
    }

    connect(&btn, SIGNAL(clicked()), this, SLOT(togglerenderer()));
    layout.addWidget(&btn, 1, 0);
    setLayout(&layout);

    timer.start(100);

    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(setActiveUAS(UASInterface*)));
    createActions();
    if (UASManager::instance()->getActiveUAS() != NULL)
        setActiveUAS(UASManager::instance()->getActiveUAS());
}

void HUD2::repaint(void){
    if (usegl == true)
        surface_gl->repaint();
    else
        surface->repaint();
}


void HUD2::togglerenderer(void)
{
    if (usegl == true){
        surface_gl->hide();
        surface->show();
        btn.setText(tr("Soft"));
        usegl = false;
    }
    else{
        surface_gl->show();
        surface->hide();
        btn.setText(tr("GL"));
        usegl = true;
    }
}


void HUD2::updateAttitude(UASInterface* uas, double roll, double pitch, double yaw, quint64 timestamp)
{
    Q_UNUSED(uas);
    Q_UNUSED(timestamp);
    if (!isnan(roll) && !isinf(roll) && !isnan(pitch) && !isinf(pitch) && !isnan(yaw) && !isinf(yaw))
    {
        data->roll  = roll;
        data->pitch = pitch;
        data->yaw   = yaw;
        repaint();
        //qDebug() << "received values: " << roll << pitch << yaw;
    }
}

/**
 * @param uas the UAS/MAV to monitor/display with the HUD
 */
void HUD2::setActiveUAS(UASInterface* uas)
{
    if (this->uas != NULL) {
        // Disconnect any previously connected active MAV
        disconnect(this->uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*, double, double, double, quint64)));
        disconnect(this->uas, SIGNAL(attitudeChanged(UASInterface*,int,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*,int,double, double, double, quint64)));
        disconnect(this->uas, SIGNAL(batteryChanged(UASInterface*, double, double, int)), this, SLOT(updateBattery(UASInterface*, double, double, int)));
        disconnect(this->uas, SIGNAL(statusChanged(UASInterface*,QString,QString)), this, SLOT(updateState(UASInterface*,QString)));
        disconnect(this->uas, SIGNAL(modeChanged(int,QString,QString)), this, SLOT(updateMode(int,QString,QString)));
        disconnect(this->uas, SIGNAL(heartbeat(UASInterface*)), this, SLOT(receiveHeartbeat(UASInterface*)));

        disconnect(this->uas, SIGNAL(localPositionChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateLocalPosition(UASInterface*,double,double,double,quint64)));
        disconnect(this->uas, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateGlobalPosition(UASInterface*,double,double,double,quint64)));
        disconnect(this->uas, SIGNAL(speedChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateSpeed(UASInterface*,double,double,double,quint64)));
        disconnect(this->uas, SIGNAL(waypointSelected(int,int)), this, SLOT(selectWaypoint(int, int)));
    }

    if (uas) {
        // Now connect the new UAS
        // Setup communication
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*, double, double, double, quint64)));
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,int,double,double,double,quint64)), this, SLOT(updateAttitude(UASInterface*,int,double, double, double, quint64)));
        connect(uas, SIGNAL(batteryChanged(UASInterface*, double, double, int)), this, SLOT(updateBattery(UASInterface*, double, double, int)));
        connect(uas, SIGNAL(statusChanged(UASInterface*,QString,QString)), this, SLOT(updateState(UASInterface*,QString)));
        connect(uas, SIGNAL(modeChanged(int,QString,QString)), this, SLOT(updateMode(int,QString,QString)));
        connect(uas, SIGNAL(heartbeat(UASInterface*)), this, SLOT(receiveHeartbeat(UASInterface*)));

        connect(uas, SIGNAL(localPositionChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateLocalPosition(UASInterface*,double,double,double,quint64)));
        connect(uas, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateGlobalPosition(UASInterface*,double,double,double,quint64)));
        connect(uas, SIGNAL(speedChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateSpeed(UASInterface*,double,double,double,quint64)));
        connect(uas, SIGNAL(waypointSelected(int,int)), this, SLOT(selectWaypoint(int, int)));

        // Set new UAS
        this->uas = uas;
    }
}

void HUD2::createActions()
{
//    enableHUDAction = new QAction(tr("Enable HUD"), this);
//    enableHUDAction->setStatusTip(tr("Show the HUD instruments in this window"));
//    enableHUDAction->setCheckable(true);
//    enableHUDAction->setChecked(hudInstrumentsEnabled);
//    connect(enableHUDAction, SIGNAL(triggered(bool)), this, SLOT(enableHUDInstruments(bool)));

//    enableVideoAction = new QAction(tr("Enable Video Live feed"), this);
//    enableVideoAction->setStatusTip(tr("Show the video live feed"));
//    enableVideoAction->setCheckable(true);
//    enableVideoAction->setChecked(videoEnabled);
//    connect(enableVideoAction, SIGNAL(triggered(bool)), this, SLOT(enableVideo(bool)));

//    selectOfflineDirectoryAction = new QAction(tr("Select image log"), this);
//    selectOfflineDirectoryAction->setStatusTip(tr("Load previously logged images into simulation / replay"));
//    connect(selectOfflineDirectoryAction, SIGNAL(triggered()), this, SLOT(selectOfflineDirectory()));
}

