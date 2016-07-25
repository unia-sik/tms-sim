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
 * $Id: periodictask.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file periodictask.cpp
 * @brief Implementation of periodic task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/periodictask.h>
#include <xmlio/xmlutils.h>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "periodictask";

  PeriodicTask::PeriodicTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et,
			     TmsTimeInterval _ct,
			     UtilityCalculator* __uc, UtilityAggregator* __ua,
			     TmsTimeInterval o, TmsPriority _prio)
    : Task(_id, _et, _ct, __uc, __ua, _prio),
      period(_period), offset(o), lastUtility(0), historyUtility(1) {
  }
  

  PeriodicTask::PeriodicTask(const PeriodicTask& rhs)
    : Task(rhs),
      period(rhs.period), offset(0), lastUtility(0), historyUtility(1) {
  }
  
  
  TmsTime PeriodicTask::startHook(TmsTime now) {
    return now + offset;
  }
  
  
  Job* PeriodicTask::spawnHook(TmsTime now) {
    return new Job(this, activations, now, executionTime, now + relDeadline, getPriority());
  }
  
  
  TmsTimeInterval PeriodicTask::getNextActivationOffset(__attribute__((unused)) TmsTime now) {
    return period;
  }
  
  
  bool PeriodicTask::completionHook(Job *job, TmsTime now) {
    lastUtility = calcExecValue(job, now); // TODO: REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  bool PeriodicTask::cancelHook(Job *job) {
    lastUtility = 0; // REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  std::ostream& PeriodicTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << period << "/" << executionTime << "/" << relDeadline << ";" << priority << ")";
    ost << " [" << lastUtility << ";" << historyUtility << "]";
    return ost;
  }
  
  
  double PeriodicTask::getLastExecValue(void) const {
    return lastUtility;
  }
  
  
  double PeriodicTask::getHistoryValue(void) const {
    return historyUtility;
  }
  
  
  double PeriodicTask::calcExecValue(const Job *job, TmsTime complTime) const {
  if (complTime <= job->getAbsDeadline()) {
    return 1;
  }
  else {
    return 0;
  }
}


  double PeriodicTask::calcHistoryValue(const Job *job, TmsTime complTime) const {
    return (historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double PeriodicTask::calcFailHistoryValue(__attribute__((unused)) const Job *job) const {
    return historyUtility/2;
  }
  
  
  void PeriodicTask::advanceHistory(void) {
    historyUtility = (historyUtility + lastUtility) / 2;
  }
  
  
  string PeriodicTask::getShortId(void) const {
    return "P";
  }


  Task* PeriodicTask::clone() const {
    return new PeriodicTask(*this);
  }

  /*
  void PeriodicTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "periodictask");
    xmlTextWriterWriteElement(writer, (xmlChar*)"id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*)"executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterEndElement(writer);
  }
  */

  const std::string& PeriodicTask::getClassElement() {
    return ELEM_NAME;
  }


  int PeriodicTask::writeData(xmlTextWriterPtr writer) {
    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(offset)));
    return 0;
  }

  /*
  int PeriodicTask::getPeriod() const { return period; }
  int PeriodicTask::getOffset const { return offset; }
  */
} // NS tmssim
