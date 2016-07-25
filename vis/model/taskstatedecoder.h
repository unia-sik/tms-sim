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
 * $Id: taskstatedecoder.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file taskstatedecoder.h
 * @brief Decode task state for display
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKSTATEDECODER_H
#define TASKSTATEDECODER_H

#include <map>
#include <string>

/**
 * @brief Defines the interface for decoding a task's state.
 *
 * Inherit from this class to define your actual decoder.
 * Use virtual inheritance, to allow combination with GUI classes.
 */
class TaskStateDecoder {
 public:
  TaskStateDecoder(const std::string& taskDescription);
  TaskStateDecoder(const TaskStateDecoder& rhs);
  virtual ~TaskStateDecoder();

  virtual const std::string& getTaskId() const;
  virtual const std::string& getTypeId() const;

  virtual void setState(const std::string& _state) = 0;

 protected:
  /**
   * Use in subclass, decode the task's parameters from this string
   */
  const std::string& taskData;
  
 private:
  std::string _taskId;
  std::string _typeId;
  std::string _taskData;
};


/**
 * @brief For convenience.
 *
 * This class overwrites the map desctructor to avoid the user having
 * to free all the map's contents.
 */
class DecoderMap: public std::map<std::string, TaskStateDecoder*> {
 public:
  ~DecoderMap();
};


#endif // !TASKSTATEDECODER_H
