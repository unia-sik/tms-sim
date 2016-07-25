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
 * $Id: stat.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file stat.cpp
 * @brief Collection of execution statistics
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/stat.h>
#include <iostream>
#include <map>
#include <vector>
#include <utility>

using namespace std;

namespace tmssim {

  Stat::Stat() :
    taskNumber(0), sumOfUtilities(0), sumOfUtilityCounts(0), cancelSteps(0), idleSteps(0) {
    for(int i=0; i<CANCEL_SLOTS; i++) {
      cancelCtr[i] = 0;
    }
  }
  
  Stat::~Stat() {
    // nothing gets allocated on the heap until now
  }
  
  void Stat::addTaskId(std::string taskId) {
    taskIds.push_back(taskId);
  }
  
  const std::map<std::string, int> Stat::getActivations() const {
    return activations;
  }
  
  void Stat::setActivations(std::string taskId, int activations) {
    this->activations[taskId] = activations;
  }
  
  const int* Stat::getCancelCtr() const {
    return cancelCtr;
  }
  
  const std::map<std::string, int>& Stat::getCancellations() const {
    return cancellations;
  }
  
  void Stat::setCancellations(std::string taskId, int cancellations) {
    this->cancellations[taskId] = cancellations;
  }
  
  const std::list<int>& Stat::getCancelStepList() const {
    return cancelStepList;
  }
  
  void Stat::addToCancelStepList(int cancelStep) {
    this->cancelStepList.push_back(cancelStep);
  }
  
  int Stat::getCancelSteps() const {
    return cancelSteps;
  }
  
  void Stat::setCancelSteps(int cancelSteps) {
    this->cancelSteps = cancelSteps;
  }
  
  const std::map<std::string, int>& Stat::getCompletions() const {
    return completions;
  }
  
  void Stat::setCompletions(std::string taskId, int completions) {
    this->completions[taskId] = completions;
  }
  
  const std::map<std::string, int>& Stat::getExecCancellations() const {
    return execCancellations;
  }
  
  void Stat::setExecCancellations(std::string taskId, int execCancellations) {
    this->execCancellations[taskId] = execCancellations;
  }
  
  const std::map<std::string, double>& Stat::getHistoryValues() const {
    return historyValues;
  }
  
  void Stat::setHistoryValues(std::string taskId, double historyValues) {
    this->historyValues[taskId] = historyValues;
  }
  
  TmsTime Stat::getIdleSteps() const {
    return idleSteps;
  }
  
  void Stat::setIdleSteps(TmsTime idleSteps) {
    this->idleSteps = idleSteps;
  }
  
  const std::map<std::string, double>& Stat::getMeanUtilities() const {
    return meanUtilities;
  }
  
  void Stat::setMeanUtilities(std::string taskId, double meanUtilities) {
    this->meanUtilities[taskId] = meanUtilities;
  }
  
  const std::map<std::string, int>& Stat::getMisses() const {
    return misses;
  }
  
  void Stat::setMisses(std::string taskId, int misses) {
    this->misses[taskId] = misses;
  }
  
  const std::map<std::string, int>& Stat::getPreemptions() const {
    return preemptions;
  }
  
  void Stat::setPreemptions(std::string taskId, int preemptions) {
    this->preemptions[taskId] = preemptions;
  }
  
  const std::string& Stat::getSchedulerId() const {
    return schedulerId;
  }
  
  void Stat::setSchedulerId(const std::string& schedulerId) {
    this->schedulerId = schedulerId;
  }
  
  int Stat::getSumOfActivations() const {
    int result = 0;
    
    std::map<std::string, int> activationsMap = this->getActivations();
    
    for (map<string, int>::iterator it = activationsMap.begin();
	 it != activationsMap.end();
	 ++it) {
        result += it->second;
    }
    return result;
  }
  
  int Stat::getSumOfCancellations() const {
    int result = 0;
    
    std::map<std::string, int> cancelationsMap = this->getCancellations();
    
    for (map<string, int>::iterator it = cancelationsMap.begin();
	 it != cancelationsMap.end();
	 ++it) {
      result += it->second;
    }
    return result;
  }
  
  int Stat::getSumOfCompletions() const {
    int result = 0;
    
    std::map<std::string, int> completionsMap = this->getCompletions();
    
    for (map<string, int>::iterator it = completionsMap.begin(); it != completionsMap.end(); ++it) {
      result += it->second;
    }
    return result;
  }
  
  int Stat::getSumOfExecCancellations() const {
    int result = 0;
    
    std::map<std::string, int> execCancelMap = this->getExecCancellations();
    
    for (map<string, int>::iterator it = execCancelMap.begin();
	 it != execCancelMap.end();
	 ++it) {
      result += it->second;
    }
    return result;
  }
  
  int Stat::getSumOfMisses() const {
    int result = 0;
    
    std::map<std::string, int> missesMap = this->getMisses();
    
    for (map<string, int>::iterator it = missesMap.begin();
	 it != missesMap.end();
	 ++it) {
      result += it->second;
    }
    return result;
  }
  
  int Stat::getSumOfPreemptions() const {
    int result = 0;
    
    std::map<std::string, int> preemptsMap = this->getPreemptions();
    
    for (map<string, int>::iterator it = preemptsMap.begin();
	 it !=  preemptsMap.end(); ++it) {
      result += it->second;
    }
    return result;
  }
  
  int Stat::getSumOfUtilities() const {
    return sumOfUtilities;
  }
  
  void Stat::setSumOfUtilities(int sumOfUtilities) {
    this->sumOfUtilities = sumOfUtilities;
  }
  
  int Stat::getSumOfUtilityCounts() const {
    return sumOfUtilityCounts;
  }
  
  void Stat::setSumOfUtilityCounts(int sumOfUtilityCounts) {
    this->sumOfUtilityCounts = sumOfUtilityCounts;
  }
  
  int Stat::getTaskNumber() const {
    return taskNumber;
  }
  
  void Stat::setTaskNumber(int taskNumber) {
    this->taskNumber = taskNumber;
  }
  
  const std::map<std::string, double>& Stat::getUtilitySums() const {
    return utilitySums;
  }
  
  void Stat::setUtilitySums(std::string taskId, double utilitySums) {
    this->utilitySums[taskId] = utilitySums;
  }
  
  const std::map<std::string, double>& Stat::getUtiltyCounts() const {
    return utiltyCounts;
  }
  
  void Stat::setUtiltyCounts(std::string taskId, double utiltyCounts) {
    this->utiltyCounts[taskId] = utiltyCounts;
  }
  
  const std::map<std::string, std::vector<int> >& Stat::getDelayCounters() const {
    return delayCounters;
  }
  
  void Stat::setDelayCounters(const std::map<std::string, std::vector<int> >& delayCounters) {
    this->delayCounters = delayCounters;
  }
  
  bool Stat::IsVerbose() const {
    return isVerbose;
  }
  
  void Stat::setVerbose(bool verbose) {
    this->isVerbose = verbose;
  }
  
  std::ostream& operator<<(std::ostream& out, Stat& statistic) {
    statistic.print(out);
    return out;
  }
  
  void Stat::print(std::ostream& out) {
    
    out << this->getSchedulerId() << std::endl;
    
    out << "# Task [hv] {act|compl|canc/ecanc|miss|preempt} [UA]" << std::endl;
    for (vector<string>::iterator taskId = this->taskIds.begin();
	 taskId != this->taskIds.end();
	 ++taskId) {
      
      out << *taskId;
      out << " {" << this->activations[*taskId] << "|";
      out << this->completions[*taskId] << "|";
      out << this->cancellations[*taskId] << "/";
      out << this->execCancellations[*taskId] << "|";
      out << this->misses[*taskId] << "|";
      out << this->preemptions[*taskId] << "} ";
      out << "[" << this->utilitySums[*taskId] << "/";
      out << this->utiltyCounts[*taskId];
      out << "=" << (this->utilitySums[*taskId] / this->utiltyCounts[*taskId]) << "]" << endl;
    }
    int tmp;
    out << "Act: " << this->getSumOfActivations() << " Compl: " << this->getSumOfCompletions() << " Canc: " << this->getSumOfCancellations() << " Ecanc: " << this->getSumOfExecCancellations()
	<< " Miss: " << this->getSumOfMisses() << " Preempt: " << this->getSumOfPreemptions() << endl;
    
    out << "U_Sys/ [" << this->getSumOfUtilities()
	<< "/" << this->getSumOfUtilityCounts() << "="
	<< ( this->getSumOfUtilities() / ((tmp=this->getSumOfUtilityCounts())!=0?tmp:-1) ) << "]" << endl;
    
    out << "Cancel statistics (how often got $i tasks cancelled in one time step?):" << endl;
    // TODO: Make CancelCtr to vector and remove magic number CANCEL_SLOTS
    for (int i = 0; i < CANCEL_SLOTS; i++) {
      out << "CC[" << (i + 1) << "]=" << cancelCtr[i] << " ";
    }
    out << endl;
    out << "Total steps with cancellations: " << this->getCancelSteps() << endl;
    
    if (this->isVerbose) {
      out << "CancelSteps: ";
      if (this->getCancelStepList().size() > 0) {
	for (list<int>::iterator it = this->cancelStepList.begin(); it != this->cancelStepList.end(); ++it) {
	  out << *it << " ";
	}
      }
      out << endl;
      
    }
    
    cout << "Idle steps: " << this->getIdleSteps() << endl;
    
  }
  
} // NS tmssim
