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
 * $Id: displaymodel.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file displaymodel.h
 * @brief Model for schedule display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "displaymodel.h"

DisplayModelEntry::DisplayModelEntry()
  : type(DisplayModelEntry::HEAD), label(""), state(""), decoder(NULL), trace(NULL)
{
  //qDebug() << "DME::HEAD" << this;
}


DisplayModelEntry::DisplayModelEntry(QString _label)
  : type(DisplayModelEntry::SEP), label(_label), state(""), decoder(NULL), trace(NULL)
{
  //qDebug() << "DME::SEP" << this;
}


DisplayModelEntry::DisplayModelEntry(QString _label, QString _state, TaskStateWidget* _decoder, const JobInfoList* _trace)
  : type(DisplayModelEntry::TASK), label(_label), state(_state), decoder(_decoder), trace(_trace)
{
  //qDebug() << "DME::TASK" << this;
}

/*
DisplayModelEntry::DisplayModelEntry(const DisplayModelEntry& rhs)
  : type(rhs.type), label(rhs.label), state(rhs.state), trace(rhs.trace)
{
  //qDebug() << "DME::CC" << this << "<-" << &rhs;
}
*/

DisplayModelEntry::~DisplayModelEntry() {
  //qDebug() << "DME::~" << this;
  if (decoder != NULL)
    delete decoder;
}


void DisplayModel::clear() {
  // TODO: first delete
  for (DisplayModelEntry* dme: *this) {
    delete dme;
  }
  QList<DisplayModelEntry*>::clear();
  tMax = 0;
}
