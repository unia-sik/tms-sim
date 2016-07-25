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
 * $Id: displayarea.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file displayarea.h
 * @brief The scrollable display area
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef DISPLAYAREA_H
#define DISPLAYAREA_H


#include <QtGui>


#include "displaydimension.h"
#include "displaylegend.h"
#include "displaymodel.h"
#include "displaywidget.h"


class DisplayArea : public QSplitter { //QScrollArea {

  Q_OBJECT

 public:
  DisplayArea(const DisplayDimension& _dDim, const DisplayModel* _dModel, QWidget* parent=0);

  void update();

  DisplayWidget* getDisplayWidget() { return dw; }

  void setModel(const DisplayModel* _dModel);
  
 protected:
  void resizeEvent(QResizeEvent *event);
  void scrollContentsBy(int dx, int dy);

 private slots:
   //void updateLegend(const QRect &, int);
  
 private:

  class DisplayScrollArea: public QScrollArea {
 public:
   DisplayScrollArea(DisplayArea* parent);
   void setWidget(DisplayWidget* _dw);
 protected:
   void scrollContentsBy(int dx, int dy);
 private:
   DisplayArea* da;
   DisplayWidget* dw;
 };

 class LegendScrollArea: public QScrollArea {
 public:
   LegendScrollArea(QWidget* parent = NULL);
   void setWidget(DisplayLegend* _dl);
   
 protected:
 private:
   DisplayLegend* dl;

 };

  DisplayLegend* legend;
  LegendScrollArea* lsa;
  DisplayWidget* dw;
  DisplayScrollArea* dsa;
  //QScrollArea* saLegend;
  QStringList tasks;
  const DisplayDimension& dDim;
  const DisplayModel* dModel;
};


#endif // !DISPLAYAREA_H
