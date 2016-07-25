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
 */

/**
 * $Id: displaywidget.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file displaywidget.h
 * @brief Display Schedule
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QList>
#include <QWidget>

#include "model/logfilereader.h"
#include "displaydimension.h"
#include "displaymodel.h"


class DisplayWidget : public QWidget {
  Q_OBJECT

 public:
  DisplayWidget(const DisplayDimension& _dDim, const DisplayModel* _dModel, QWidget* parent=0);

  void update();

  void setStep(unsigned long step);
  unsigned long getXBar() const;
  
  void setModel(const DisplayModel* _dModel);

 signals:
  void stepSelected(unsigned long step);
  
 protected:
  void mouseMoveEvent(QMouseEvent* event);
  void adjustGeometry();
  void adjustMaxX();
  void adjustMaxY();
  void adjustTMax();
  
  void paintEvent(QPaintEvent *event);
  int paintHead(QPainter* qp);
  void paintSchedule(QPainter *qp, const DisplayModelEntry* dme, int y0);

 private:

  void drawActivation(QPainter *qp, int x, int y);
  void drawStart(QPainter *qp, int x, int y);
  void drawPreemption(QPainter *qp, int x, int y);
  void drawResume(QPainter *qp, int x, int y);
  void drawCompletion(QPainter *qp, int x, int y);
  void drawCancel(QPainter *qp, int x, int y);

  const DisplayDimension& dDim;
  const DisplayModel* dModel;

  unsigned long tMax;
  
  int groupSize;
  int maxX;
  int maxY;

  long xBar;
};



#endif // !DISPLAYWIDGET_H
