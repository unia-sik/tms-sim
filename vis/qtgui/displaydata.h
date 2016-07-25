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
 * $Id: displaydata.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file displaydata.h
 * @brief Store all data for display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef DISPLAYDATA_H
#define DISPLAYDATA_H

#include "model/jobinfo.h"
#include "model/taskstatedecoder.h"

#include "displaymodel.h"

#include <map>
#include <list>

#include <QString>
#include <QList>

class DisplayData {
 public:
  DisplayData();

  /**
   * If successful, takes ownership of _decoders and _traces.
   */
  bool addLog(const QString& _name,
	      const std::vector<std::string>& _tasks,
	      DecoderMap* _decoders,
	      TraceMap* _traces,
	      unsigned long _tMax);
  void clear();

  unsigned long getTMax();

  const DisplayModel& getModelBySet() const;
  const DisplayModel& getModelByTask() const;

  void selectStep(unsigned long step);
  
 private:
  struct LogSet {
    /**
     * @param _name
     * @param _tasks
     * @param _decoders takes ownership
     * @param _traces takes ownership
     * @param _tMax
     */
  LogSet(const QString& _name, const std::vector<std::string>& _tasks,
	 DecoderMap* _decoders,
	 TraceMap* _traces,
	 unsigned long _tMax)
  : name(_name), tasks(_tasks), decoders(_decoders), traces(_traces), tMax(_tMax) {}

    ~LogSet() {
      delete decoders;
      delete traces;
    }
      
    
    QString name;
    std::vector<std::string> tasks;
    DecoderMap* decoders;
    TraceMap* traces;
    unsigned long tMax;

  private:
    // Don't want any copies to avoid memory problems
    LogSet(__attribute__((unused)) const LogSet& rhs) {}
  };


  void fillModelBySet(DisplayModel& dm);
  void fillModelByTask(DisplayModel& dm);

  void selectModelStep(unsigned long step, DisplayModel& dm);
  
  QList<LogSet*> data;

  QMap<std::string, unsigned> task2id;
  QMap<unsigned, QVector<std::string>> id2task;
  unsigned nTasks;

  
  unsigned long tMax;

  /**
   * Sorted by task set
   */
  DisplayModel dmSet;
  /**
   * Sorted by task
   */
  DisplayModel dmTask;
};


#endif // !DISPLAYDATA_H
