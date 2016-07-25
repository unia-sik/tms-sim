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
 * $Id: taskstatewidget.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file taskstatewidget.h
 * @brief Display of a task's state
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKSTATEWIDGET_H
#define TASKSTATEWIDGET_H

#include <QtGui>

#include "model/taskstatedecoder.h"


class TaskStateWidget: virtual public TaskStateDecoder, public QWidget {
 public:
  TaskStateWidget(std::string __taskDescription, QWidget* parent=NULL);
  TaskStateWidget(const TaskStateDecoder& decoder, QWidget* parent=NULL);

  /**
   * Overwrite in subclass, first call superclass method
   */
  void findMaxWidths(QMap<int,int>& maxWidths) const;
  /**
   * Overwrite in subclass, first call superclass method
   */
  void adjustWidths(const QMap<int,int>& maxWidths);

 protected:
  virtual void addWidget(QWidget* w);

  static void findMaxWidth(QMap<int,int>& maxWidths, QWidget* w, int id);
  static void adjustWidth(QWidget* w, int width);

 private:
  void init();
  
  QHBoxLayout* layout;
  QLabel* lTaskId;
};


#endif // !TASKSTATEWIDGET_H
