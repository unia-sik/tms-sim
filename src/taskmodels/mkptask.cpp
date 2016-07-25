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
 * $Id: mkptask.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkptask.cpp
 * @brief (m,k)-firm real-time task with fixed (m,k)-patterns
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mkptask.h>
#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <cmath>

#include <limits.h>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "mkptask";

  MkpTask::MkpTask(unsigned int _id, TmsTimeInterval _period,
		   TmsTimeInterval _et, TmsTimeInterval _ct,
		   UtilityCalculator* _uc, UtilityAggregator* _ua,
		   TmsPriority _priority, unsigned int _m, unsigned int _k,
		   unsigned int _spin, bool _useSpin, CompressedMkState _is,
		   bool _relaxed)
    : MkTask(_id, _period, _et, _ct, _uc, _ua, _priority, _m, _k, _spin, _is),
      relaxed(_relaxed)
      //, a(0)
  {
    //cout << "MkpTask " << getShortId() << endl;
    if (_useSpin) {
      actSpin = spin;
      //cmpAdd = 1;
      //cout << "Spin task " << getShortId() << endl;
    }
    else {
      actSpin = 0;
      //cmpAdd = 0;
    }
    //monitor = new MKMonitor(m, k);
    tDebug() << "Created mkptask " << _id << " priority " << _priority << " task prio " << priority << "(" << m << "," << k << ") @ " << this;
    tDebug() << "\tMonitorsum: " << monitor.getCurrentSum();
  }


  MkpTask::MkpTask(const MkpTask& rhs)
    : MkTask(rhs), actSpin(rhs.actSpin), relaxed(rhs.relaxed)
      //, a(0)
  {
    tDebug() << "Copied mkptask to " << this;
  }


  MkpTask::MkpTask(const MkTask* rhs)
    : MkTask(rhs), actSpin(0), relaxed(false)
      //, a(0)
  {
    //tDebug() << "Copied mkptask to " << this;
  }


  MkpTask::~MkpTask() {
    //delete monitor;
  }


  /**
   * we can use the activations field of the Task class instead of keeping
   * another counter a
   */
  Job* MkpTask::spawnHook(TmsTime now) {
    int _prio;
    //double tmp = floor(ceil( (double)(activations + actSpin) * (double)m / (double)k) * (double)k / (double)m);
    //if (int(tmp) == activations) { // mandatory
    if (isJobMandatory(activations)) {
      _prio = priority;
    }
    else { // optional
      _prio = TMS_MIN_PRIORITY;
    }
    Job* job = new Job(this, activations, now, executionTime, now + relDeadline, _prio);
    tDebug() << this << " Created mkjob " << *job /*<< " for tmp=" << tmp*/
	     << " a=" << activations << " taskprio " << priority
	     << " @ " << job << " t@ " << job->getTask();
    //++a;
    return job;
  }


  bool MkpTask::isJobMandatory(unsigned int jobId) const {
    double tmp = floor(ceil( (double)(jobId + actSpin) * (double)m / (double)k) * (double)k / (double)m) - actSpin;
    //cout << getIdString() << " job " << jobId << " spin: " << actSpin
    //<< " m: " << m << " k: " << k << " tmp: " << tmp << " mand: " << (tmp == jobId) << endl;
    return tmp == jobId;
  }


  void MkpTask::enableSpin() {
    actSpin = spin;
    //cmpAdd = 1;
  }


  void MkpTask::disableSpin() {
    actSpin = 0;
    //cmpAdd = 0;
  }


  void MkpTask::setRelaxed(bool _relaxed) {
    relaxed = _relaxed;
  }

  
  bool MkpTask::getRelaxed() const {
    return relaxed;
  }


  bool MkpTask::completionHook(Job *job, TmsTime now) {
    return MkTask::completionHook(job, now);
    //delete job;
  }


  bool MkpTask::cancelHook(Job *job) {
    bool mandFail = (job->getPriority() != TMS_MIN_PRIORITY);
    bool stateValid = MkTask::cancelHook(job);
    //return MkTask::cancelHook(job);
    if (relaxed) {
      return stateValid;
    }
    else {
      //cout << "mandFail: " << mandFail << endl;
      // need not regard state: if mandatory instances successful, then
      // constraint is guaranteed
      return !mandFail;
    }
  }


  string MkpTask::getShortId(void) const {
    return "MC";
  }

  /*
  void MkpTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer, (xmlChar*) "mkptask");
    xmlTextWriterWriteElement(writer, (xmlChar*) "id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*) "executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    
    xmlTextWriterWriteElement(writer, (xmlChar*) "priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "m", STRTOXML(XmlUtils::convertToXML<int>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "k", STRTOXML(XmlUtils::convertToXML<int>(this->k)));
    xmlTextWriterEndElement(writer);
  }
  */
  
  Task* MkpTask::clone() const {
    return new MkpTask(*this);
  }


  const std::string& MkpTask::getClassElement() {
    return ELEM_NAME;
  }


  int MkpTask::writeData(xmlTextWriterPtr writer) {
    MkTask::writeData(writer);
    return 0;
  }

  
  MkTask* MkpTaskAllocator(MkTask* task) { return new MkpTask(task); }


  MkTask* MkpTaskWithSpinAllocator(MkTask* task) {
    MkpTask* t = dynamic_cast<MkpTask*>(MkpTaskAllocator(task));
    t->enableSpin();
    return t;
  }

  MkTask* RelaxedMkpTaskAllocator(MkTask* task) {
    MkpTask* t = dynamic_cast<MkpTask*>(MkpTaskAllocator(task));
    t->setRelaxed(true);
    return t;
  }


  MkTask* RelaxedMkpTaskWithSpinAllocator(MkTask* task) {
    MkpTask* t = dynamic_cast<MkpTask*>(MkpTaskWithSpinAllocator(task));
    t->setRelaxed(true);
    return t;
  }

} // NS tmssim
