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
 * $Id: logfilereader.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file logfilereader.cpp
 * @brief Read tms-sim log files
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "logfilereader.h"

#include <fstream>
#include <iostream>
#include <boost/regex.hpp>

#include "taskstatedecoderfactory.h"

using namespace std;
using namespace boost;

#define BUFFLEN 1024

// ==SIM== Task: TP1(6/2/6) [0;1]
/// Detect lines with task definitions, extract task id
//const regex RE_TASK("^==SIM== Task: ([A-Za-z]+[0-9]+)\\(.*");
const regex RE_TASK("^==SIM== Task: ([A-Za-z]+[0-9]+.*)");
/// Detect lines with relevant events, extract event, time, and jobs
const regex RE_EVENT("^==EXEC== ([AEC])@([0-9]*) : (.*)");
/// Check whether currently executed job finished execution
const regex RE_EXEC_FINISHED("(\\{.*\\}) \\(F\\).*");
/// Check whether processor is idle
const regex RE_EXEC_IDLE("I");
/// Extract a single job from a list of jobs
const regex RE_MULT_JOB("(\\{.*?\\}\\s*)");

// {TMC4,0(0/5-13 P 2147483647 S 13) { [1111111] 7/7 D=2 | 1.16667->1}} (F)
// ==EXEC== C@0 : {TP1,0(2/2-6 P 0 S 4) }
/// Extract job data
const regex RE_JOB("\\{([A-Za-z]+[0-9]+),([0-9]*)\\(([0-9]*)/([0-9]*)-([0-9]*) P ([0-9]*) S ([0-9]*)\\) \\{(.*?)[\\}]+\\s*");
//const regex RE_JOB("\\{J([0-9]*),([0-9]*)\\(([0-9]*)/([0-9]*)-([0-9]*) P ([0-9]*) S ([0-9]*)\\) (.*?)\\}\\s*");


struct LogFileReader::JobData {
  JobData()
    : taskId(""), jobId(0), remET(0), et(0), ct(0),
      prio(0), lst(0), state("") {}
  
  JobData(string _taskId,
	  unsigned _jobId,
	  unsigned _remET,
	  unsigned _et,
	  unsigned _ct,
	  unsigned _prio,
	  unsigned _lst,
	  string _state)
    : taskId(_taskId), jobId(_jobId), remET(_remET), et(_et), ct(_ct),
      prio(_prio), lst(_lst), state(_state) {}
	  
  string taskId;
  unsigned jobId;
  unsigned remET; ///< remaining execution time
  unsigned et; ///< total execution time
  unsigned ct; ///< deadline
  unsigned prio; ///< priority
  unsigned lst; ///< last starting time (to keep deadline)
  string state; ///< task state (if applicable)
};


LogFileReader::LogFileReader(const string& _path)
  : path(_path), matchCounter(0), tMax(0), lastExecTask(""), lastExecJob(NULL) {
  decoders = new DecoderMap;
  traces = new TraceMap;
  readFile();
}


LogFileReader::~LogFileReader() {
}


const std::vector<std::string>& LogFileReader::getTasks() const {
  return tasks;
}


DecoderMap* LogFileReader::getDecoders() {
  DecoderMap* tmp = decoders;
  decoders = NULL;
  return tmp;
}


TraceMap* LogFileReader::getTraces() {
  TraceMap* tmp = traces;
  traces = NULL;
  return tmp;
}


unsigned long LogFileReader::getTMax() const {
  return tMax;
}


void LogFileReader::readFile() {
  ifstream infile(path);
  char buffer[BUFFLEN];
  string line;
  size_t lineCounter = 0;
  while (!infile.eof()) {
    infile.getline(buffer, BUFFLEN);
    //cout << buffer << endl;
    line = buffer;
    ++lineCounter;
    if (recordLine(line))
      ++matchCounter;
    
  }
  infile.close();
  cout << "Found " << matchCounter << " matching lines" << endl;
}

  
bool LogFileReader::recordLine(const string& line) {
  //cout << "Recording line: " << line << endl;
  smatch sm;
  if (regex_match(line, sm, RE_TASK)) {
    // we have found a task definition
    TaskStateDecoder* tsd = TaskStateDecoderFactory::getDecoder(sm[1]);
    if (decoders != NULL) {
      (*decoders)[tsd->getTaskId()] = tsd;
    }
    else {
      cerr << "LogFileReader::decoders == NULL - have you obtained the decoders already?" << endl;
    }
    //cout << "Found task: " << tsd->getTaskId() << endl;;
    tasks.push_back(tsd->getTaskId());
    (*traces)[tsd->getTaskId()] = new JobInfoList;
    return true;
  }
  else if (regex_match(line, sm, RE_EVENT)) {
    //cout << line << endl;
    unsigned long time = stoi(sm[2]);
    tMax = time;
    //cout << "Time: " << time << endl;
    if (sm[1] == "A") {
      //cout << "Activations" << endl;
      recordActivations(time, sm[3]);
    }
    else if (sm[1] == "E") {
      //cout << "Execution" << endl;
      recordExecution(time, sm[3]);
    }
    else if (sm[1] == "C") {
      //cout << "Cancellations" << endl;
      recordCancellations(time, sm[3]);
    }
    else {
      //cout << "Unknown action: " << sm[1] << endl;
      return false;
    }

    return true;
  }
  else {
    // nothing relevant found
    return false;
  }
}


void LogFileReader::recordActivations(unsigned long time, const std::string& str) {
  list<LogFileReader::JobData> jds = extractJobs(str);
  for (JobData& jd: jds) {
      JobInfo* jic = new JobInfo(time, jd.jobId, JobInfo::READY, jd.state);
      jic->addEvent(JobInfo::ACTIVATE);
      (*traces)[jd.taskId]->push_back(jic);
    //cout << jd.taskId << " : " << (*traces)[jd.taskId]->back() << endl;
  }
}


void LogFileReader::recordExecution(unsigned long time, const std::string& str) {
  boost::smatch sm;
  string job;
  bool finish = false;
  if (regex_search(str, sm, RE_EXEC_IDLE)) {
    lastExecTask = "";
    lastExecJob = NULL;
    //cout << "Idle" << endl;
    return;
  }
  if (regex_match(str, sm, RE_EXEC_FINISHED)) {
    //cout << "Exec finished" << endl;
    finish = true;
    job = sm[1];
  }
  else {
    job = str;
  }

  LogFileReader::JobData jd;
  if (decodeJob(job, jd)) {

    if (lastExecJob == NULL
	|| !(lastExecTask == jd.taskId && lastExecJob->jobId == jd.jobId)) {
      if (lastExecJob != NULL) {
	if (!lastExecJob->getEvent(JobInfo::FINISH)) {
	  // context switch - preempt previous job
	  const JobInfo* lastJob = (*traces)[lastExecTask]->back();
	  JobInfo* jip = new JobInfo(time, lastJob->jobId, JobInfo::READY, lastJob->taskState);
	  jip->addEvent(JobInfo::PREEMPT);
	  (*traces)[lastExecTask]->push_back(jip);
	  //cout << lastExecTask << " : " << jip << endl;
	}
      }
      // then start/resume current job
      JobInfo* prev = (*traces)[jd.taskId]->back();
      if (prev->getEvent(JobInfo::ACTIVATE)) {
	if (prev->time == time) {
	  // just activated
	  prev->addEvent(JobInfo::START);
	  prev->execState = JobInfo::EXECUTE;
	}
	else {
	  JobInfo* jin = new JobInfo(time, jd.jobId, JobInfo::EXECUTE, jd.state);
	  jin->addEvent(JobInfo::START);
	  (*traces)[jd.taskId]->push_back(jin);
	}
      }
      else {
	assert(prev->getEvent(JobInfo::PREEMPT)
	       || prev->getEvent(JobInfo::FINISH)
	       || prev->getEvent(JobInfo::CANCEL));
	// resume execution
	JobInfo* jin = new JobInfo(time, jd.jobId, JobInfo::EXECUTE, jd.state);
	jin->addEvent(JobInfo::RESUME);
	(*traces)[jd.taskId]->push_back(jin);
      }
    }
    //cout << jd.taskId << " : " << (*traces)[jd.taskId]->back() << endl;

    if (finish) {
      JobInfo* jif = new JobInfo(time+1, jd.jobId, JobInfo::NONE, jd.state);
      jif->addEvent(JobInfo::FINISH);
      (*traces)[jd.taskId]->push_back(jif);
      //cout << jd.taskId << " : " << jif << endl;
    }
    
    lastExecTask = jd.taskId;
    lastExecJob = (*traces)[jd.taskId]->back();
  } // decodeJob
  // if no job was decoded, processor was idle
}


void LogFileReader::recordCancellations(unsigned long time, const std::string& str) {
  list<LogFileReader::JobData> jds = extractJobs(str);
  for (JobData& jd: jds) {
    if ((*traces)[jd.taskId]->back()->time == time) {
      // job was just activated
      (*traces)[jd.taskId]->back()->addEvent(JobInfo::CANCEL);
      if ((*traces)[jd.taskId]->back()->jobId == jd.jobId) {
	// adjust state only if it's the same job, i.e. was just activated
	(*traces)[jd.taskId]->back()->execState = JobInfo::NONE;
      }
    }
    else {
      JobInfo* jic = new JobInfo(time, jd.jobId, JobInfo::NONE, jd.state);
      jic->addEvent(JobInfo::CANCEL);
      (*traces)[jd.taskId]->push_back(jic);
    }
    //cout << jd.taskId << " : " << (*traces)[jd.taskId]->back() << endl;
  }
}


list<LogFileReader::JobData> LogFileReader::extractJobs(const std::string& str) {
  //cout << "Jobs:\n";
  list<LogFileReader::JobData> jdl;
  boost::match_results<std::string::const_iterator> mr;
  std::string::const_iterator start = str.begin() ;
  while ( boost::regex_search(start, str.end(), mr, RE_MULT_JOB) ) {
    LogFileReader::JobData jd;
    //std::cout << "Sub-match : " << mr[1] << " found in full match: " << mr[0] <<  '\n' ;
    //cout << "\tsecond: " << *(mr[0].second) << endl;
    //cout << "Found job: \"" << mr[1] << "\"" << endl;
    if (decodeJob(mr[1], jd)) {
      jdl.push_back(jd);
    }
    start = mr[0].second ;
  }
  return jdl;
}

// "\\{([A-Za-z]+[0-9]+),([0-9]*)\\(([0-9]*)/([0-9]*)-([0-9]*) P ([0-9]*) S ([0-9]*)\\) (.*?)\\}\\s*"

// {TMD0,0(2/2-5 P 9 S 3) { [1111111111] 10/10 D=9 | 5->4.5}}
// Job: [task],[jobnum] ([remET]/[ET]-[ct] P [priority] S [st]) { [taskstate] }
bool LogFileReader::decodeJob(const string& str, LogFileReader::JobData& jd) {
  boost::smatch sm;
  //cout << "DEC: " << str << endl;
  if (regex_match(str, sm, RE_JOB)) {
    jd = LogFileReader::JobData(sm[1],
				stoul(sm[2]),
				stoul(sm[3]),
				stoul(sm[4]),
				stoul(sm[5]),
				stoul(sm[6]),
				stoul(sm[7]),
				sm[8]);
    return true;
  }
  else {
    cout << "No job match: " << str << endl;
    return false;
  }
}

