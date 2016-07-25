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
 * $Id: defaulttaskstatewidget.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file defaulttaskstatewidget.h
 * @brief Default TaskStateWidget that displays Task ID and state string
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef QTGUI_DEFAULTTASKSTATEWIDGET_H
#define QTGUI_DEFAULTTASKSTATEWIDGET_H

#include "taskstatewidget.h"
#include "model/defaulttaskstatedecoder.h"


class DefaultTaskStateWidget: public DefaultTaskStateDecoder, public TaskStateWidget {
 public:
  DefaultTaskStateWidget(const DefaultTaskStateDecoder& defaultDecoder, QWidget* parent=NULL);

  virtual void setState(const std::string& _state);

  void findMaxWidths(QMap<int,int>& maxWidths) const;
  void adjustWidths(const QMap<int,int>& maxWidths);
  
 protected:
 private:
  QLabel* lState;
};


#endif // !QTGUI_DEFAULTTASKSTATEWIDGET_H
