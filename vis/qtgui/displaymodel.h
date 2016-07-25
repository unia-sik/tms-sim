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
 * $Id: displaymodel.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file displaymodel.h
 * @brief Model for schedule display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include <QtGui>

#include "taskstatewidget.h"

#include "model/jobinfo.h"

struct DisplayModelEntry {
  enum Type {
    NONE = 0,
    HEAD,
    SEP,
    TASK
  };

  /**
   * Create Type::HEAD entry
   */
  DisplayModelEntry();
  
  /**
   * Create Type::SEP entry
   */
  DisplayModelEntry(QString _label);

  /**
   * Create Type::TASK entry
   * @param _trace NO ownership is taken!
   * @param _decoder ownerwship IS taken!
   */
  // TODO: add decoderwidget
  DisplayModelEntry(QString _label, QString _state, TaskStateWidget* _decoder, const JobInfoList* _trace);

  ~DisplayModelEntry();
  
  Type type;
  QString label;
  QString state;
  TaskStateWidget* decoder;
  const JobInfoList* trace;

private:
  DisplayModelEntry(__attribute__((unused)) const DisplayModelEntry& rhs) {}

};


/**
 * Takes ownership of entries
 */
class DisplayModel : public QList<DisplayModelEntry*> {
 public:
 DisplayModel() : tMax(0) {}
  
  void adjustTMax(int _tMax) {
    if (tMax < _tMax)
      tMax = _tMax;
  }

  unsigned int getTMax() const { return tMax; }

  void clear();
  
 private:
  int tMax;
};


#endif // !DISPLAYMODEL_H
