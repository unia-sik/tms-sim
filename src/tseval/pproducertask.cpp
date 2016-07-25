/**
 * $Id: pproducertask.cpp 977 2015-09-01 08:58:25Z klugeflo $
 * @file pproducertask.cpp
 * @brief Periodic sensor task for time synchronous execution eval
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <tseval/pproducertask.h>

#include <utility/ucnone.h>
#include <utility/uanone.h>
#include <utils/tmsexception.h>

//#define TLOGLEVEL TLL_WARN
#include <utils/tlogger.h>

using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "pproducertask";

  ostream& operator<<(ostream& ost, const TmsTimeElement& tte) {
    ost << "(" << tte.tReal << "/" << tte.tLogical << ")";
    return ost;
  }

  
  PProducerTask::PProducerTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _executionTime, TmsPriority _priority, size_t _bufferSize, TmsTimeInterval _responseTime, ostream* _dataLog)
    : PeriodicTask(_id, _period, _executionTime, _period, new UCNone, new UANone, 0, _priority),
      bufferSize(_bufferSize), responseTime(_responseTime), dataLog(_dataLog)
  {
    data["LIB"] = TimeBuffer(bufferSize);
    data["LIB"].push_back(0);
    data["LIB"].push_back(0);
    data["LET"] = TimeBuffer(bufferSize);
    data["LET"].push_back(0);
    data["LET"].push_back(0);
    data["RTA"] = TimeBuffer(bufferSize);
    data["RTA"].push_back(0);
    data["RTA"].push_back(0);
  }

  
  PProducerTask::PProducerTask(const PProducerTask& rhs)
    : PeriodicTask(rhs)
  {
    THROW_UNIMPLEMENTED;
  }

  
  PProducerTask::~PProducerTask() {
  }

  
  Task* PProducerTask::clone() const {
    cerr << "Cloning must be performed on taskset level!" << endl;
    THROW_UNIMPLEMENTED;
  }

  
  const std::string& PProducerTask::getClassElement() {
    return ELEM_NAME;
  }

  
  int PProducerTask::writeData(xmlTextWriterPtr writer) {
    //THROW_UNIMPLEMENTED;
    PeriodicTask::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"buffersize", STRTOXML(XmlUtils::convertToXML<TmsTimeInterval>(bufferSize)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"responsetime", STRTOXML(XmlUtils::convertToXML<TmsTimeInterval>(responseTime)));
    return 0;
  }
  

  TmsTimeElement PProducerTask::getData(const string& type, TmsTime time) const {
    if ( (type != "LIB") && (type != "LET") && (type != "RTA") ) {
      throw TMSException(string("Unknown type \"") + type + "\"");
    }
    const TimeBuffer& tb = data.at(type);
    if (tb.begin() == tb.end()) {
      return -1;
    }
    else {
      TimeBuffer::const_iterator it = tb.begin();
      if (it == tb.end())
	return 0;
      TmsTimeElement prev = (*it);//.tLogical;
      //assert(prev <= time);
      ++it;
      while (it != tb.end()) {
	if ((*it).tLogical > time) {
	  break;
	}
	/*else if (*it == time) {
	  prev = *it;
	  break;
	  }*/
	else {
	  prev = *it;
	}
	++it;
      }
      return prev;
    }
  }


  std::ostream& PProducerTask::print(std::ostream& ost) const {
    ost << getIdString() << ": " << getExecutionTime() << " / "
	<< getPeriod() << " / "  << getResponseTime();
    return ost;
  }

  
  bool PProducerTask::completionHook(Job *job, TmsTime now) {
    TmsTime timeLet = job->getAbsDeadline();
    assert(timeLet >= now);
    TmsTime timeRta = job->getActivationTime() + responseTime;
    assert(timeRta >= now);
    assert (timeRta <= timeLet);
    data["LIB"].push_back(now + 1);
    data["LET"].push_back(TmsTimeElement(now, timeLet));
    data["RTA"].push_back(TmsTimeElement(now, timeRta));
    if (dataLog != NULL) {
      *dataLog << "j" << getIdString() << "," << job->getJobId() << " "
	       << job->getActivationTime() << " (" << now << "/" << timeLet
	       << "/" << timeRta << ")"
	       << " [" << data["LIB"].back() << data["LET"].back()
	       << data["RTA"].back() << "]"
	       << endl;
    }
    return PeriodicTask::completionHook(job, now);
  }


  string PProducerTask::getShortId(void) const {
    return "PP";
  }


} // NS tmssim
