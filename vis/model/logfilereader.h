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
 * $Id: logfilereader.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file logfilereader.h
 * @brief Read tms-sim log files
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef LOGFILEREADER_H
#define LOGFILEREADER_H

#include <iostream>
#include <list>
#include <map>
#include <string>
//#include <unordered_set>
#include <vector>

#include <cstdlib>

#include "jobinfo.h"
#include "taskstatedecoder.h"


/**
 * This class is organised as a single-use object. Once the file has been
 * read and all data been obtained, the object should be destroyed. Data
 * can only be obtained once from this object.
 */
class LogFileReader {
 public:
  LogFileReader(const std::string& _path);
  virtual ~LogFileReader();

  const std::vector<std::string>& getTasks() const;
  /**
   * Call only once!
   * @return a map with the decoders, keys into the map are stored in the
   * task list that can be obtained by LogFileReader::getTasks().
   * The memory used by the map must be freed by the caller!
   */
  DecoderMap* getDecoders();
  //const std::map<std::string, JobInfoList*>& getTraces() const;
  /**
   * Call only once!
   * @return Map with all traces during the first call, caller must take
   * ownership of memory. Further calls will return NULL.
   */
  TraceMap* getTraces();
  unsigned long getTMax() const;


 private:
  struct JobData;

  void readFile();

  bool recordLine(const std::string& line);
  void recordActivations(unsigned long time, const std::string& str);
  void recordExecution(unsigned long time, const std::string& str);
  void recordCancellations(unsigned long time, const std::string& str);

  static std::list<JobData> extractJobs(const std::string& str);
  static bool decodeJob(const std::string& str, JobData& jd);

  std::string path;
  size_t matchCounter;
  unsigned long tMax;

  std::vector<std::string> tasks;
  DecoderMap* decoders;
  //std::map<std::string, JobInfoList> traces;
  TraceMap* traces;
  std::string lastExecTask;
  JobInfo* lastExecJob;
};

#endif // !LOGFILEREADER_H
