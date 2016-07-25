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
 * $Id: taskstatewidget.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file taskstatewidget.cpp
 * @brief Display of a task's state
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "taskstatewidget.h"

#include "tswconstants.h"

TaskStateWidget::TaskStateWidget(std::string __taskDescription, QWidget* parent)
  : TaskStateDecoder(__taskDescription), QWidget(parent) {
  init();
}


TaskStateWidget::TaskStateWidget(const TaskStateDecoder& decoder, QWidget* parent)
  : TaskStateDecoder(decoder), QWidget(parent) {
  init();
}


void TaskStateWidget::addWidget(QWidget* w) {
  layout->addWidget(w);
  update();
}


void TaskStateWidget::findMaxWidths(QMap<int,int>& maxWidths) const {
  findMaxWidth(maxWidths, lTaskId, Tsw::TaskId);
}


void TaskStateWidget::adjustWidths(const QMap<int,int>& maxWidths) {
  adjustWidth(lTaskId, maxWidths[Tsw::TaskId]);
  /*QRect geo = lTaskId->geometry();
  geo.setWidth(maxWidths[0]);
  lTaskId->setGeometry(geo);*/
}


void TaskStateWidget::findMaxWidth(QMap<int,int>& maxWidths, QWidget* w, int id) {
  int width = w->sizeHint().width();
  if (maxWidths[id] < width)
    maxWidths[id] = width;
}

void TaskStateWidget::adjustWidth(QWidget* w, int width) {
  QRect geo = w->geometry();
  geo.setWidth(width);
  w->setGeometry(geo);
  w->updateGeometry();
}


void TaskStateWidget::init() {
  //qDebug() << "Creating TaskStateWidget for task" << getTaskId().c_str();
  layout = new QHBoxLayout(this);
  //layout->addWidget(new QPushButton("T"));
  lTaskId = new QLabel(getTaskId().c_str(), this);
  layout->addWidget(lTaskId);
  setLayout(layout);
}
