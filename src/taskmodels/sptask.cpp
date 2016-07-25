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
 * $Id: sptask.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file sptask.cpp
 * @brief Implementation of sporadic periodic task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/sptask.h>

#include <xmlio/xmlutils.h>
#include <cstdlib>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "sptask";


  SPTask::SPTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et,
		 unsigned int _seed, TmsTimeInterval _ct,
		 UtilityCalculator* __uc, UtilityAggregator* __ua,
		 double _probability, TmsTimeInterval _offset,
		 TmsPriority _prio)
    : Task(_id, _et, _ct, __uc, __ua, _prio),
      period(_period), offset(_offset),
      lastUtility(0), historyUtility(1), activationPending(false), seed(_seed),
      probability(_probability), baseSeed(_seed)
  {    
  }
  
  
  SPTask::SPTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et,
		 unsigned int _seed,
		 UtilityCalculator* __uc, UtilityAggregator* __ua,
		 TmsPriority _prio)
    : Task(_id, _et, _period, __uc, __ua, _prio),
      period(_period), offset(0),
      lastUtility(0), historyUtility(1), activationPending(false), seed(_seed),
      probability(0.5), baseSeed(_seed)
  {
  }


  SPTask::SPTask(const SPTask &rhs)
    : Task(rhs), period(rhs.period), offset(rhs.offset),
      lastUtility(0), historyUtility(1), activationPending(false),
      seed(rhs.seed), probability(rhs.probability), baseSeed(rhs.seed)
  {
  }
  
  
  TmsTime SPTask::startHook(TmsTime now) {
    seed = baseSeed;
    return now + offset;
  }
  
  
  Job* SPTask::spawnHook(TmsTime now) {
    return new Job(this, activations, now, executionTime, now + relDeadline, getPriority());
  }
  
  
  TmsTimeInterval SPTask::getNextActivationOffset(__attribute__((unused)) TmsTime now) {
    unsigned int rval;
    int i = 1;
    while ( (rval=rand_r(&seed)) < (RAND_MAX * probability)) {
      ++i;
    }
    return i*period;
  }
  
  
  bool SPTask::completionHook(Job *job, TmsTime now) {
    lastUtility = calcExecValue(job, now); // TODO: use new utility classes!
    advanceHistory();
    delete job;
    return true;
  }

  
  bool SPTask::cancelHook(Job *job) {
    lastUtility = 0;
    advanceHistory();
    delete job;
    return true;
  }
  

  std::ostream& SPTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << period << "/" << executionTime << "/" << relDeadline << ")";
    ost << " [" << lastUtility << ";" << historyUtility << "]";
    return ost;
  }
  
  
  double SPTask::getLastExecValue(void) const {
    return lastUtility;
  }
  
  
  double SPTask::getHistoryValue(void) const {
    return historyUtility;
  }
  
  
  double SPTask::calcExecValue(const Job *job, TmsTime complTime) const {
    if (complTime <= job->getAbsDeadline()) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  
  double SPTask::calcHistoryValue(const Job *job, TmsTime complTime) const {
    return (historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double SPTask::calcFailHistoryValue(__attribute__((unused)) const Job *job) const {
    return historyUtility/2;
  }
  
  
  void SPTask::advanceHistory(void) {
    historyUtility = (historyUtility + lastUtility) / 2;
    //std::cout << "Advanced history to " << historyUtility << std::endl;
  }
  
  
  string SPTask::getShortId(void) const {
    return "p";
  }

  //Task* t = new PeriodicTask(this->getId(),this->period,this->getEt(),ct,this->getUC().clone(),this->getUA().clone(),this->offset,this->getPrio());

  Task* SPTask::clone() const {
    /*
    Task* t = new SPTask(this->getId(), this->period, this->getEt(),
			 this->seed, this->ct, this->getUC()->clone(),
			 this->getUA()->clone(), this->probability,
			 this->offset, this->getPriority());
    return t;
    */
    return new SPTask(*this);
  }

  /*
  void SPTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "sporadicperiodictask");
    xmlTextWriterWriteElement(writer, (xmlChar*)"id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*)"executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    
    xmlTextWriterWriteElement(writer, (xmlChar*)"priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"seed", STRTOXML(XmlUtils::convertToXML<unsigned int>(this->seed, std::hex)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"probability", STRTOXML(XmlUtils::convertToXML<double>(this->probability)));
    xmlTextWriterEndElement(writer);
  }
  */
  
  const std::string& SPTask::getClassElement() {
    return ELEM_NAME;
  }


  int SPTask::writeData(xmlTextWriterPtr writer) {
    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"seed", STRTOXML(XmlUtils::convertToXML<unsigned int>(this->seed, std::hex)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"probability", STRTOXML(XmlUtils::convertToXML<double>(this->probability)));
    return 0;
  }
  
} // NS tmssim
