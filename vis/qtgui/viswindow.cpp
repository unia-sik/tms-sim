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
 * $Id: viswindow.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file viswindow.h
 * @brief Main windows of tms-vis
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "viswindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMenuBar>

#include "displayarea.h"

VisWindow::VisWindow() {
  qDebug() << "Not yet implemented, exiting";
  abort();
}


VisWindow::VisWindow(QStringList files)
  : currentModelBySet(true) {
  //qDebug() << "Using logfile @ " << files.first();
  qDebug() << "Init...";
  init(files);
  qDebug() << "Init finished!";
}


VisWindow::~VisWindow() {
}


void VisWindow::stepSelected(unsigned long step) {
  data.selectStep(step);
  displayArea->update();
}


void VisWindow::init(QStringList files) {
  createMenus();
  createToolbar();
  //qsa = new QScrollArea();
  /*
  qsa = new DisplayArea();
  qsa->setWidget(dw);
  qsa->resize(800, 600);
  setCentralWidget(qsa);
  */
  displayArea = new DisplayArea(dDim, &data.getModelBySet());
  for (QString file : files) {
    qDebug() << "Loading log file" << file;
    loadLog(file);
  }
  qDebug() << "Loading finished";
  setCentralWidget(displayArea);
  resize(800, 600);
  connect(displayArea->getDisplayWidget(), SIGNAL(stepSelected(unsigned long)),
	  this, SLOT(stepSelected(unsigned long)));
  displayArea->getDisplayWidget()->setStep(0);
}


void VisWindow::createMenus() {
  QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
  actOpen = new QAction(tr("&Open"), NULL);
  actOpen->setShortcut(tr("CTRL+O"));
  connect(actOpen, SIGNAL(triggered()), this, SLOT(mOpen()));
  fileMenu->addAction(actOpen);
  actClear = new QAction(tr("&Clear"), NULL);
  connect(actClear, SIGNAL(triggered()), this, SLOT(mClear()));
  fileMenu->addAction(actClear);
  actExit = new QAction(tr("E&xit"), NULL);
  actExit->setShortcut(tr("CTRL+Q"));
  connect(actExit, SIGNAL(triggered()), this, SLOT(mExit()));
  fileMenu->addAction(actExit);
  //fileMenu->addAction(act);
  qDebug() << "Created menu";
}


void VisWindow::createToolbar() {
  QToolBar* toolBar = new QToolBar("Tools");
  tActToggle = toolBar->addAction("T");
  connect(tActToggle, SIGNAL(triggered()), this, SLOT(toggleModel()));
  tActPrev = toolBar->addAction("<");
  connect(tActPrev, SIGNAL(triggered()), this, SLOT(stepPrev()));
  tActNext = toolBar->addAction(">");
  connect(tActNext, SIGNAL(triggered()), this, SLOT(stepNext()));

  addToolBar(toolBar);
}


void VisWindow::clear() {
}


void VisWindow::loadLog(QString path) {
  LogFileReader lfr(path.toStdString());

  const std::vector<std::string>& tasks = lfr.getTasks();
  DecoderMap* decoders = lfr.getDecoders();
  //std::map<std::string, JobInfoList> traces = lfr.getTraces();
  TraceMap* traces = lfr.getTraces();
  int tMax = lfr.getTMax();


  if (!data.addLog(path, tasks, decoders, traces, tMax)) {
    qDebug() << "Adding log to data pool failed";
    delete decoders;
    delete traces;
    return;
  }

  displayArea->update();
}


void VisWindow::mOpen() {
  QFileDialog qfl(this, tr("Open log file"), ".");
  qfl.setAcceptMode(QFileDialog::AcceptOpen);
  qfl.setDefaultSuffix("txt");
  qfl.setFileMode(QFileDialog::ExistingFiles);
  qfl.setModal(true);
  qfl.setNameFilter("Log files (*.txt *.log)");
  if (qfl.exec()) {
    QStringList files = qfl.selectedFiles();
    // TODO: clear display and  load files
    for (QString s : files) {
      qDebug() << "\t" << s;
      loadLog(s);
    }
  }
  
}


void VisWindow::mClear() {
  qDebug() << "mClear";
  clear();
}


void VisWindow::mExit() {
  close();
}


void VisWindow::toggleModel() {
  //qDebug() << "Toggle DisplayModel...";
  // TODO: adjust model handling implementation!
  if (currentModelBySet) {
    displayArea->setModel(&data.getModelByTask());
    currentModelBySet = false;
  }
  else {
    displayArea->setModel(&data.getModelBySet());
    currentModelBySet = true;
  }
}


void VisWindow::stepPrev() {
  DisplayWidget *dw = displayArea->getDisplayWidget();
  dw->setStep(dw->getXBar() - 1);
}


void VisWindow::stepNext() {
  DisplayWidget *dw = displayArea->getDisplayWidget();
  dw->setStep(dw->getXBar() + 1);
}

