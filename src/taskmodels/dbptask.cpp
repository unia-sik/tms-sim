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
 * $Id: dbptask.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file dbptask.h
 * @brief Task with distance-based priority assignment
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/dbptask.h>
#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <cassert>

using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "dbptask";

  DbpTask::DbpTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et, TmsTimeInterval _ct,
		   UtilityCalculator* _uc, UtilityAggregator* _ua,
		   TmsPriority _priority, unsigned int _m, unsigned int _k, CompressedMkState _is)
    : MkTask(_id, _period, _et, _ct, _uc, _ua, _priority, _m, _k, _is)
  {
    tDebug() << "Created dbptask " << _id << " priority " << _priority << " task prio " << priority << "(" << m << "," << k << ")";
    tDebug() << "\tMonitorsum: " << monitor.getCurrentSum();
  }


  DbpTask::DbpTask(const DbpTask& rhs)
    : MkTask(rhs)
  {

  }


  DbpTask::DbpTask(const MkTask* rhs)
    : MkTask(rhs)
  {

  }


  DbpTask::~DbpTask() {

  }


  Job* DbpTask::spawnHook(TmsTime now) {
    TmsPriority dbPriority = getDistance();
    Job* job = new Job(this, activations, now, executionTime, now + relDeadline, dbPriority);
    tDebug() << "Created dbpjob " << *job << " taskprio " << dbPriority;
    return job;

    return NULL;
  }


  bool DbpTask::completionHook(Job *job, TmsTime now) {
    return MkTask::completionHook(job, now);
    //delete job;
  }


  bool DbpTask::cancelHook(Job *job) {
    return MkTask::cancelHook(job);
    //delete job;
  }


  string DbpTask::getShortId(void) const {
    return "MD";
  }

  /*
  void DbpTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer, (xmlChar*) "dbptask");
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
  
  Task* DbpTask::clone() const {
    return new DbpTask(*this);
  }


  const std::string& DbpTask::getClassElement() {
    return ELEM_NAME;
  }


  int DbpTask::writeData(xmlTextWriterPtr writer) {
    MkTask::writeData(writer);
    return 0;
  }


  MkTask* DbpTaskAllocator(MkTask* task) { return new DbpTask(task); }
  
} // NS tmssim
