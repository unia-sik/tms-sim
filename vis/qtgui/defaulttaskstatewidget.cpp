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
 * $Id: defaulttaskstatewidget.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file defaulttaskstatewidget.h
 * @brief Default TaskStateWidget that displays Task ID and state string
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "defaulttaskstatewidget.h"

#include "tswconstants.h"


DefaultTaskStateWidget::DefaultTaskStateWidget(const DefaultTaskStateDecoder& defaultDecoder, QWidget* parent)
  : TaskStateDecoder(defaultDecoder), DefaultTaskStateDecoder(defaultDecoder), TaskStateWidget(defaultDecoder, parent)
{
  lState = new QLabel;
  addWidget(lState);
}


void DefaultTaskStateWidget::setState(const std::string& _state) {
  DefaultTaskStateDecoder::setState(_state);
  lState->setText(_state.c_str());
  update();
}


void DefaultTaskStateWidget::findMaxWidths(QMap<int,int>& maxWidths) const {
  TaskStateWidget::findMaxWidths(maxWidths);
  findMaxWidth(maxWidths, lState, Tsw::DefaultState);
}


void DefaultTaskStateWidget::adjustWidths(const QMap<int,int>& maxWidths) {
  TaskStateWidget::adjustWidths(maxWidths);
  adjustWidth(lState, maxWidths[Tsw::DefaultState]);
}

