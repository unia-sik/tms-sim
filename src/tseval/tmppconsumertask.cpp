/**
 * $Id: tmppconsumertask.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file tmppconsumertask.cpp
 * @brief Temporary consumer task, only for reading from XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <tseval/tmppconsumertask.h>

#include <utility/ucnone.h>
#include <utility/uanone.h>
#include <utils/tmsexception.h>
#include <utils/tlogger.h>

#include <core/globals.h>

using namespace std;


namespace tmssim {

  static const string ELEM_NAME = "pconsumertask";


  TmpPConsumerTask::TmpPConsumerTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _executionTime, TmsPriority _priority, TmsTimeInterval _responseTime, std::list<std::string>* _producers)
    : PeriodicTask(_id, _period, _executionTime, _period, new UCNone, new UANone, 0, _priority),
      responseTime(_responseTime), producers(_producers)
  {
  }
  

  TmpPConsumerTask::TmpPConsumerTask(const TmpPConsumerTask& rhs)
    : PeriodicTask(rhs)
  {
    THROW_UNIMPLEMENTED;
  }


  TmpPConsumerTask::~TmpPConsumerTask() {
    if (producers != NULL) {
      delete producers;
    }
  }
  

  Task* TmpPConsumerTask::clone() const {
    tError() << "Cloning must be performed on taskset level!";
    THROW_UNIMPLEMENTED;
  }

  
  const std::string& TmpPConsumerTask::getClassElement() {
    THROW_UNIMPLEMENTED;
    //return ELEM_NAME;
  }

  
  int TmpPConsumerTask::writeData(UNUSED xmlTextWriterPtr writer) {
    THROW_UNIMPLEMENTED;
  }

  
  std::ostream& TmpPConsumerTask::print(std::ostream& ost) const {
    ost << getIdString() << ": " << getExecutionTime() << " / "
	<< getPeriod() << " / "  << getResponseTime() << " [ ";
    if (producers != NULL) {
      for (const string& p: *producers) {
	ost << p << " ";
      }
    }
    ost << "]";

    return ost;
  }

  
  Job* TmpPConsumerTask::spawnHook(UNUSED TmsTime now) {
    THROW_UNIMPLEMENTED;
  }


  bool TmpPConsumerTask::completionHook(UNUSED Job *job, UNUSED TmsTime now) {
    THROW_UNIMPLEMENTED;
  }


  string TmpPConsumerTask::getShortId(void) const {
    return "PCT";
  }

  
} // NS tmssim
