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
 * $Id: displaydata.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file displaydata.cpp
 * @brief Store all data for display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "displaydata.h"

#include "taskstatewidgetfactory.h"

DisplayData::DisplayData()
  : nTasks(0), tMax(0) {
}

  
bool DisplayData::addLog(const QString& _name,
			 const std::vector<std::string>& _tasks,
			 DecoderMap* _decoders,
			 TraceMap* _traces,
			 unsigned long _tMax) {
  if (data.size() > 0) {
    if (_tasks.size() != nTasks) {
      qDebug() << "Task sets don't match";
      return false;
    }
    else { // copy ids
      unsigned id = 0;
      // first check
      for (std::string task: _tasks) {
	if (task2id.contains(task) && task2id[task] != id) {
	  qWarning() << "Task" << task.c_str() << "already in task2id map, has id"
		     << task2id[task] << "new id would be" << id;
	  qWarning() << "Task set/trace will not be added!";
	  return false;
	}
	++id;
      }
      if (id > nTasks) {
	qWarning() << "Task sets have different size, new set will not be added!";
	return false;
      }
      // then copy
      id = 0;
      for (std::string task: _tasks) {
	task2id[task] = id;
	id2task[id].push_back(task);
	++id;
      }

    }
  }
  else {
    // initialise ids
    unsigned id = 0;
    for (const std::string& task: _tasks) {
      task2id[task] = id;
      id2task[id].push_back(task);
      ++id;
    }
    nTasks = id;
  }

  qDebug() << "Adding" << _name << "with" << _traces->size() << "traces";
  data.push_back(new DisplayData::LogSet(_name, _tasks, _decoders, _traces, _tMax));
  if (_tMax > tMax)
    tMax = _tMax;

  fillModelBySet(dmSet);
  fillModelByTask(dmTask);
  //qDebug() << "Models adjusted";
  return true;
}


void DisplayData::clear() {
  task2id.clear();
  id2task.clear();
  data.clear();
  dmSet.clear();
  dmTask.clear();
}


unsigned long DisplayData::getTMax() {
  return tMax;
}


const DisplayModel& DisplayData::getModelBySet() const {
  return dmSet;
}


const DisplayModel& DisplayData::getModelByTask() const {
  return dmTask;
}


void DisplayData::selectStep(unsigned long step) {
  selectModelStep(step, dmSet);
  selectModelStep(step, dmTask);
}


void DisplayData::fillModelBySet(DisplayModel& dm) {
  //qDebug() << "Filling BySet model";
  dm.clear();
  dm.push_back(new DisplayModelEntry());
  for (const LogSet* ls: data) {
    dm.push_back(new DisplayModelEntry(ls->name));
    for (const std::string& task: ls->tasks) {
      //qDebug() << "\t" << task.c_str();
      dm.push_back(new DisplayModelEntry(QString(task.c_str()), "",
					 TaskStateWidgetFactory::getWidget(ls->decoders->at(task)),
					 ls->traces->at(task)));
    }
  }
  dm.adjustTMax(tMax);
}


void DisplayData::fillModelByTask(DisplayModel& dm) {
  //qDebug() << "Filling ByTask model";
  dm.clear();
  dm.push_back(new DisplayModelEntry());
  for (unsigned id = 0; id < nTasks; ++id) {
    // TODO: add task ids?
    dm.push_back(new DisplayModelEntry(QString::number(id)));
    for (const LogSet* ls: data) {
      QString ln(ls->name);
      ln.append(":");
      ln.append(ls->tasks[id].c_str());
      //qDebug() << "\t" << ln;
      dm.push_back(new DisplayModelEntry(ln, "",
					 TaskStateWidgetFactory::getWidget(ls->decoders->at(ls->tasks[id])),
					 ls->traces->at(ls->tasks[id])));
    }
  }
  dm.adjustTMax(tMax);
}


void DisplayData::selectModelStep(unsigned long step, DisplayModel& dm) {
  for (DisplayModelEntry* dme: dm) {
    if (dme->type != DisplayModelEntry::TASK)
      continue;
    for (const JobInfo* ji: *dme->trace) {
      if (ji->time > step)
	break;
      else {
	dme->state = ji->taskState.c_str();
	if (dme->decoder != NULL) {
	  dme->decoder->setState(ji->taskState);
	}
      }
    }
  }
}
