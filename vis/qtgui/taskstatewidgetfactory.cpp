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
 * $Id: taskstatewidgetfactory.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file taskstatewidgetfactory.cpp
 * @brief Creation of taskstatewidgets from taskstatedecoders
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "taskstatewidgetfactory.h"

#include "defaulttaskstatewidget.h"
#include "mktaskstatewidget.h"


TaskStateWidget* TaskStateWidgetFactory::getWidget(const TaskStateDecoder* decoder) {
  if (decoder == NULL) {
    return NULL;
  }
  std::string typeId = decoder->getTypeId();
  // first char should be 'T', ignore this
  switch (typeId[1]) {
  case 'M':
    return new MkTaskStateWidget(*(dynamic_cast<const MkTaskStateDecoder*>(decoder)));
    
  default:
    return new DefaultTaskStateWidget(*(dynamic_cast<const DefaultTaskStateDecoder*>(decoder)));
  }
}
