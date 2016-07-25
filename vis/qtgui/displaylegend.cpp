/*
 * This file is part of tms-sim.
 *
 * Copyright 2014 University of Augsburg
 *
 * tms-sim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tms-sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tms-sim.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: displaylegend.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file displaylegend.h
 * @brief Left column of schedule display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "displaylegend.h"

//#include <QPaintEvent>
//#include <QPainter>

DisplayLegend::DisplayLegend(const DisplayDimension& _dDim, const DisplayModel* _dModel, QWidget* parent)
  : QWidget(parent), dDim(_dDim), dModel(_dModel), yOffset(0) {
  //qDebug() << "Legend has data at" << dModel;
  connectStateWidgets();
}


DisplayLegend::~DisplayLegend() {
  disconnectStateWidgets();
}


void DisplayLegend::setYoffset(int yoff) {
  yOffset = yoff;
  update();
}


void DisplayLegend::update() {
  //qDebug() << "DL::update()";
  connectStateWidgets();
  QWidget::update();
  adjustWidths();
  QWidget::update();
  //qDebug() << "DisplayLegend:" << geometry();
}


void DisplayLegend::setModel(const DisplayModel* _dModel) {
  disconnectStateWidgets();
  dModel = _dModel;
  connectStateWidgets();
  //qDebug() << "Legend has data at" << dModel;
  //qDebug() << children();
}


void DisplayLegend::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(event->rect(), Qt::lightGray);
  //int x = dDim.getXOffset();
  int y = yOffset;

  for (DisplayModelEntry *dme: *dModel) {
    switch (dme->type) {
    case DisplayModelEntry::HEAD:
      //qDebug() << "HEAD" << y;
      y += dDim.getYOffset();
      break;
      
    case DisplayModelEntry::SEP:
      //qDebug() << "SEP" << y;
      // TODO: Write Group name
      painter.drawText(10, y+10, dme->label);
      y += dDim.getSeparatorHeight();
      break;

    case DisplayModelEntry::TASK:
      //qDebug() << "TASK" << y;
      // TODO: include state
      /*
      painter.drawText(10, y+10, dme->label);
      painter.drawText(90, y+10, dme->state);
      */
      if (dme->decoder != NULL) {
	//qDebug() << "Moving StateWidget" << dme->decoder << "for task" << dme->decoder->getTaskId().c_str() << "to" << y+10;
	dme->decoder->move(10, y);
      }
      y += dDim.getTaskHeight();
      break;
      
    default:
      qDebug() << "Invalid DisplayModelEntry:" << dme->type;
    }
  }
  //qDebug() << "DL y:" << y;
}


void DisplayLegend::connectStateWidgets() {
  if (dModel == NULL)
    return;
  //qDebug() << "Connecting widgets from" << dModel;
  for (DisplayModelEntry* dme: *dModel) {
    if (dme->decoder != NULL) {
      //qDebug() << "Connecting decoderwidget for task" << dme->decoder->getTaskId().c_str();
      dme->decoder->setParent(this);
      dme->decoder->show();
    }
  }
}


void DisplayLegend::disconnectStateWidgets() {
  if (dModel == NULL)
    return;
  //qDebug() << "DisConnecting widgets from" << dModel;
  for (DisplayModelEntry* dme: *dModel) {
    if (dme->decoder != NULL) {
      //qDebug() << "Disconnecting decoderwidget for task" << dme->decoder->getTaskId().c_str();
      dme->decoder->setParent(NULL);
    }
  }
}


void DisplayLegend::adjustWidths() {
  QMap<int, int> maxWidths;
  for (DisplayModelEntry *dme: *dModel) {
    if (dme->decoder == NULL)
      continue;
    dme->decoder->findMaxWidths(maxWidths);
  }

  for (DisplayModelEntry *dme: *dModel) {
    if (dme->decoder == NULL)
      continue;
    dme->decoder->adjustWidths(maxWidths);
  }
}
