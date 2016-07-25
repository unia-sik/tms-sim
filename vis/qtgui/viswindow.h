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
 * $Id: viswindow.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file viswindow.h
 * @brief Main windows of tms-vis
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef VISWINDOW_H
#define VISWINDOW_H

#include <QtGui>

#include "displayarea.h"
#include "displaydata.h"
#include "displaymodel.h"
//#include "displaywidget.h"
//#include "logfilereader.h"

//#include <QScrollArea>

class VisWindow : public QMainWindow {
  Q_OBJECT;

 public:
  VisWindow();
  VisWindow(QStringList files);
  ~VisWindow();

 public slots:
  void stepSelected(unsigned long step);
  
 private slots:
  void mOpen();
  void mClear();
  void mExit();
    
  void toggleModel();
  void stepPrev();
  void stepNext();


 private:
  void init(QStringList files);

  void createMenus();
  void createToolbar();
  
  void clear();
  void loadLog(QString path);

  // Menu
  QAction* actOpen; 
  QAction* actClear;
  QAction* actExit;
  //QAction* act;

  // Toolbar
  QAction* tActToggle;
  QAction* tActPrev;
  QAction* tActNext;
  
  // Central diplay
  DisplayArea* displayArea;

  DisplayDimension dDim;

  DisplayData data;


  bool currentModelBySet;
  // TODO: React to mouse clicks on DisplayWidget, set appropriate state
  // TODO: 2nd model sorted by tasks
};

#endif // !VISWINDOW_H
