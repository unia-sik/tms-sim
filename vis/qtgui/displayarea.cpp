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
 * $Id: displayarea.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file displayarea.cpp
 * @brief The scrollable display area
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "displayarea.h"


DisplayArea::DisplayArea(const DisplayDimension& _dDim, const DisplayModel* _dModel, QWidget* parent)
  : QSplitter(parent), /*QScrollArea(parent),*/ dDim(_dDim), dModel(_dModel) {
  /*saLegend = new QScrollArea(this);
  saLegend->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  saLegend->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);*/
  legend = new DisplayLegend(dDim, dModel);
  lsa = new LegendScrollArea(this);
  lsa->setWidget(legend);
  addWidget(lsa);
  //saLegend->setWidget(legend);
  dw = new DisplayWidget(dDim, dModel);
  dsa = new DisplayScrollArea(this);
  dsa->setWidget(dw);
  addWidget(dsa);
  /*setWidget(dw);
    setViewportMargins(dDim.getLegendWidth(), 0, 0, 0);*/
  //addWidget(legend);
  //QScrollArea* saD = new QScrollArea;
  //saD->setWidget(dw);
  //addWidget(saD);
}


void DisplayArea::update() {
  //qDebug() << "DA::update()";
  legend->update();
  dw->update();
  //QScrollArea::update();
  QSplitter::update();
}


void DisplayArea::setModel(const DisplayModel* _dModel) {
  dModel = _dModel;
  legend->setModel(dModel);
  dw->setModel(dModel);
  update();
}


void DisplayArea::resizeEvent(QResizeEvent *event) {
  //qDebug() << "DisplayArea::resizeEvent";
  //QScrollArea::resizeEvent(event);
  QSplitter::resizeEvent(event);
  
  QRect cr = contentsRect();
  legend->setGeometry(QRect(cr.left(), cr.top(), dDim.getLegendWidth(), cr.height() /*- saLegend->horizontalScrollBar()->height()*/));
  //saLegend->setGeometry(QRect(cr.left(), cr.top(), dDim.getLegendWidth(), cr.height()));
}


void DisplayArea::scrollContentsBy(__attribute__((unused)) int dx, __attribute__((unused)) int dy) {
  //qDebug() << "DisplayArea::scrollContentsBy" << dx << dy;
  //QScrollArea::scrollContentsBy(dx, dy);
  //QSplitter::scrollContentsBy(dx, dy);
  //qDebug() << "\t" << dw->frameGeometry();
  //legend->scroll(0, dy);
  // simple scrolling doesn't work, so we need to repaint with proper offset

  int yoff = dw->frameGeometry().topLeft().y();
  legend->setYoffset(yoff);

  //qDebug() << "(" << dx << "," << dy << ")";
  //lsa->verticalScrollBar()->setValue(lsa->verticalScrollBar()->value() + dy);
  
}


DisplayArea::DisplayScrollArea::DisplayScrollArea(DisplayArea* parent)
  : QScrollArea(parent), da(parent), dw(NULL) {
}


void DisplayArea::DisplayScrollArea::setWidget(DisplayWidget* _dw) {
  dw = _dw;
  QScrollArea::setWidget(dw);
}


void DisplayArea::DisplayScrollArea::scrollContentsBy(int dx, int dy) {
  //qDebug() << "DisplayArea::scrollContentsBy" << dx << dy;
  QScrollArea::scrollContentsBy(dx, dy);
  //QSplitter::scrollContentsBy(dx, dy);
  //qDebug() << "\t" << dw->frameGeometry();
  //legend->scroll(0, dy);
  // simple scrolling doesn't work, so we need to repaint with proper offset
  /*
  int yoff = dw->frameGeometry().topLeft().y();
  legend->setYoffset(yoff);  
  */
  // notify DisplayArea, which has to scroll LegendArea
  da->scrollContentsBy(dx, dy);
}


DisplayArea::LegendScrollArea::LegendScrollArea(QWidget* parent)
  : QScrollArea(parent), dl(NULL) {
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}


void DisplayArea::LegendScrollArea::setWidget(DisplayLegend* _dl) {
  dl = _dl;
  QScrollArea::setWidget(dl);
}
