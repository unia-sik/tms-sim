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
 * $Id: displaylegend.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file displaylegend.h
 * @brief Left column of schedule display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef DISPLAYLEGEND_H
#define DISPLAYLEGEND_H


#include <QtGui>


#include "displaydimension.h"
#include "displaymodel.h"


class DisplayLegend : public QWidget {
  Q_OBJECT
 public:
  DisplayLegend(const DisplayDimension& _dDim, const DisplayModel* _dModel, QWidget* parent = 0);
  ~DisplayLegend();
  
  void setYoffset(int yoff);

  //QSize sizeHint() const;

  void update();

  void setModel(const DisplayModel* _dModel);
  
 protected:
  void paintEvent(QPaintEvent *event);
  void addHeadSep();
  
 private:
  void connectStateWidgets();
  void disconnectStateWidgets();
  void adjustWidths();
  
  const DisplayDimension& dDim;
  const DisplayModel* dModel;
  int yOffset;
};


#endif // !DISPLAYLEGEND_H
