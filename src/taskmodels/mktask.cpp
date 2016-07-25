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
 * $Id: mktask.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mktask.h
 * @brief General task with (m,k)-firm real-time constraints
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mktask.h>
#include <utils/logger.h>
#include <utils/tlogger.h>

#include <cassert>

using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "mktask";

  MkTask::MkTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et, TmsTimeInterval _ct,
		 UtilityCalculator* _uc, UtilityAggregator* _ua,
		 TmsPriority _priority, unsigned int _m, unsigned int _k,
		 unsigned int _spin, CompressedMkState _is)
    : PeriodicTask(_id, _period, _et, _ct, _uc, _ua, 0, _priority),
      m(_m), k(_k), spin(_spin), monitor(_m, _k, _is)
  {
    tDebug() << "Created mktask " << _id << " priority " << _priority << " task prio " << priority << "(" << m << "," << k << ")";
    tDebug() << "\tMonitorsum: " << monitor.getCurrentSum();
    //monitor = new MkMonitor(m, k);
  }


  MkTask::MkTask(const MkTask& rhs)
    : PeriodicTask(rhs),
      m(rhs.m), k(rhs.k), spin(rhs.spin), monitor(rhs.m, rhs.k, rhs.getMonitor().getState())
  {
    
  }


  MkTask::MkTask(const MkTask* rhs)
    : PeriodicTask(*rhs),
      m(rhs->m), k(rhs->k), spin(rhs->spin), monitor(rhs->m, rhs->k, rhs->getMonitor().getState())
  {
    
  }


  MkTask::~MkTask() {
    
  }


  bool MkTask::operator==(const MkTask& rhs) const {
    return ( (getPeriod() == rhs.getPeriod())
	     && (getOffset() == rhs.getOffset())
	     && (getExecutionTime() == rhs.getExecutionTime())
	     && (getRelativeDeadline() == rhs.getRelativeDeadline())
	     && (getM() == rhs.getM())
	     && (getK() == rhs.getK())
	     && (getSpin() == rhs.getSpin()) );
  }
  

  bool MkTask::operator!=(const MkTask& rhs) const {
    return !(*this == rhs);
  }


  int MkTask::getDistance() const {
    /*const unsigned int* vals = monitor.getVals();
      unsigned int pos = monitor.getPos();*/
    int l = calcL(m);
    assert(l >= 0);
    int distance = k - l + 1;
    return distance;
  }


  int MkTask::calcL(unsigned int n, const unsigned int* vals, unsigned int pos) const {
    if (n > k) {
      tError() << "Invalid parameter n=" << n << " > " << k << "!";
      return -1;
    }
    unsigned int p;
    if (pos == 0)
      p = k - 1;
    else
      p = pos - 1;

    unsigned int ctr = 0;
    unsigned int i;
    for (i = 0; i < k; ++i) {
      if (vals[p] == 1)
	ctr++;
      if (ctr == n)
	break;

      if (p == 0)
	p = k - 1;
      else
	--p;
    }
    return i + 1;
  }


  int MkTask::calcL(int n) const {
    return calcL(n, monitor.getVals(), monitor.getPos());
  }


  bool MkTask::completionHook(Job *job, TmsTime now) {
    monitor.push(1);
    return PeriodicTask::completionHook(job, now);
  }


  bool MkTask::cancelHook(Job *job) {
    monitor.push(0);
    if (monitor.getCurrentSum() < m) {
      LOG(LOG_CLASS_TASK) << "(" << m << "," << k << ") condition violated by job " << job->getIdString() << "(k=" << monitor.getCurrentSum() << ")";
    }
    PeriodicTask::cancelHook(job);
    return monitor.isStateValid();
  }


  const MkMonitor& MkTask::getMonitor() const {
    return monitor;
  }


  const std::string& MkTask::getClassElement() {
    return ELEM_NAME;
  }


  int MkTask::writeData(xmlTextWriterPtr writer) {
    PeriodicTask::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*) "m", STRTOXML(XmlUtils::convertToXML<int>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "k", STRTOXML(XmlUtils::convertToXML<int>(this->k)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "spin", STRTOXML(XmlUtils::convertToXML<int>(this->spin)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "mkstate", STRTOXML(monitor.printState()));
    return 0;
  }


  std::ostream& MkTask::print(std::ostream& ost) const {
    PeriodicTask::print(ost);
    ost << " (" << m << "," << k << ") spin=" << spin << " mkstate = " << monitor.printState();
    return ost;
  }


  string MkTask::getShortId(void) const {
    return "M";
  }


  std::string MkTask::strState() const {
    ostringstream oss;
    oss << "{ [" << monitor.printState() << "] " << monitor.getCurrentSum() << "/" << k << " D=" << getDistance() << " | " << getUA()->getCurrentUtility() << "->" << getUA()->predictUtility(0) << "}";
    return oss.str();
  }


  MkTask* MkTaskAllocator(MkTask* task) { return new MkTask(task); }

} // NS tmssim
