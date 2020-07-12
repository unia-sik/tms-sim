/**
 * $Id: pconsumertask.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file pconsumertask.h
 * @brief Periodic processor task for time synchronous execution eval 
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <pconsumertask.h>

#include <utility/ucnone.h>
#include <utility/uanone.h>
#include <utils/tmsexception.h>

#include <utils/tlogger.h>

#include <sstream>

#include <core/globals.h>

#include <cmath>

using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "pconsumertask";

  
  const string PConsumerTask::EVALS[] = {
    "LIB", "LET", "RTA"
  };

  
  const size_t PConsumerTask::N_EVALS = sizeof(EVALS) / sizeof(string);
  

  PConsumerTask::PConsumerJob::PConsumerJob(PConsumerTask* task, unsigned int jid, TmsTime _activationTime, TmsTimeInterval _executionTime, TmsTime _absDeadline, TmsPriority _priority, int _nProducers, PProducerTask* _producers[])
    : Job(task, jid, _activationTime, _executionTime, _absDeadline, _priority),
      pTask(task), nProducers(_nProducers), producers(_producers), firstStep(0)
  {
    dtLIB = new TmsTimeElement[nProducers];
    dtLET = new TmsTimeElement[nProducers];
    dtRTA = new TmsTimeElement[nProducers];

    for (int i = 0; i < nProducers; ++i) {
      // read data for LET and LIB
      dtLET[i] = producers[i]->getData("LET", _activationTime);
      dtRTA[i] = producers[i]->getData("RTA", _activationTime);
    }
  }

  
  PConsumerTask::PConsumerJob::~PConsumerJob() {
    delete[] dtLIB;
    delete[] dtLET;
    delete[] dtRTA;
  }

  
  bool PConsumerTask::PConsumerJob::execStep(TmsTime now) {
    //pTask->executionStarted(this, now);
    if (__builtin_expect( (getExecutionTime() == getRemainingExecutionTime()), 0)) {
      firstStep = now;
      // first exec step, read data for LIB
      for (int i = 0; i < nProducers; ++i) {
	dtLIB[i] = producers[i]->getData("LIB", now);
      }

      /*
      cout << "ej" << getTask()->getIdString() << "," << getJobId() << " @ "
	   << getActivationTime() << "/" << getFirstStep();
      for (int i = 0; i < nProducers; ++i) {
	cout << " [" << dtLIB[i] << " " << dtLET[i] << " " << dtRTA[i] << "]";
      }
      cout << endl;
      */      
    }
    return Job::execStep(now);
  }
  

  PConsumerTask::PConsumerTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _executionTime, TmsPriority _priority, TmsTimeInterval _responseTime, size_t _nProducers, PProducerTask** _producers, ostream* _dataLog)
    : PeriodicTask(_id, _period, _executionTime, _period, new UCNone, new UANone, 0, _priority),
      responseTime(_responseTime), nProducers(_nProducers), producers(_producers),
      dataLog(_dataLog), statistics(NULL)
  {
    ages = new map<std::string, std::list<TmsTimeInterval>*>[nProducers];
    for (size_t i = 0; i < nProducers; ++i) {
      ages[i]["LIB"] = new list<TmsTimeInterval>;
      ages[i]["LET"] = new list<TmsTimeInterval>;
      ages[i]["RTA"] = new list<TmsTimeInterval>;
    }
    rtaGaps = new list<TmsTimeInterval>[nProducers];
  }
  

  PConsumerTask::PConsumerTask(const PConsumerTask& rhs)
    : PeriodicTask(rhs)
  {
    THROW_UNIMPLEMENTED;
  }


  PConsumerTask::~PConsumerTask() {
    delete[] producers;
    for (size_t i = 0; i < nProducers; ++i) {
      delete ages[i]["LIB"];
      delete ages[i]["LET"];
      delete ages[i]["RTA"];
    }
    delete[] ages;
    delete[] rtaGaps;
    
    if (statistics != NULL) {
      delete[] (*statistics)["LIB"];
      delete[] (*statistics)["LET"];
      delete[] (*statistics)["RTA"];
      delete statistics;
    }
  }
  

  Task* PConsumerTask::clone() const {
    tError() << "Cloning must be performed on taskset level!";
    THROW_UNIMPLEMENTED;
  }

  
  const std::string& PConsumerTask::getClassElement() {
    return ELEM_NAME;
  }

  
  int PConsumerTask::writeData(xmlTextWriterPtr writer) {
    PeriodicTask::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"responsetime", STRTOXML(XmlUtils::convertToXML<TmsTimeInterval>(responseTime)));
    if (nProducers > 0) {
      xmlTextWriterStartElement(writer, (xmlChar*) "producers");
      for (size_t i = 0; i < nProducers; ++i) {
	xmlTextWriterWriteElement(writer, (xmlChar*)"producer",
				  STRTOXML(producers[i]->getIdString()));
      }
      xmlTextWriterEndElement(writer); // close producers list
    }
    return 0;
  }

  
  void PConsumerTask::executionStarted(UNUSED const PConsumerTask::PConsumerJob *pjob, UNUSED TmsTime now) {
    
  }


  std::ostream& PConsumerTask::print(std::ostream& ost) const {
    ost << getIdString() << ": " << getExecutionTime() << " / "
	<< getPeriod() << " / "  << getResponseTime() << " [ ";
    for (size_t j = 0; j < nProducers; ++j) {
      ost << producers[j]->getIdString() << " ";
    }
    ost << "]";

    if (statistics != NULL) {
      ost << " Stat:";
      // foreach sensor
      for (size_t j = 0; j < nProducers; ++j) {
	ost << " [";
	// foreach scenario
	for (size_t e = 0; e < N_EVALS; ++e) {
	  const string& eval = EVALS[e];
	  ost << (*statistics)[eval][j];
	}
	ost << "]";
      }
      ost << " RTA gaps:";
      for (size_t i = 0; i < nProducers; ++i) {
	ost << " [" << (*statistics)["RTA"][i].minGap << " "
	    << (*statistics)["RTA"][i].maxGap << "]";
      }
    }
    return ost;
  }


  std::string PConsumerTask::getPrintableData() const {
    if (statistics == NULL)
      return "No data available!";

    ostringstream oss;
    
    // foreach sensor
    for (size_t j = 0; j < nProducers; ++j) {
      //ost << " [";
      // foreach scenario
      for (size_t e = 0; e < N_EVALS; ++e) {
	const string& eval = EVALS[e];
	//ost << (*statistics)[eval][j];
	Statistics& stat = (*statistics)[eval][j];
	oss << producers[j]->getIdString() << "@" << EVALS[e]
	    << " " << stat << "(" << stat.rawDataSorted.size() << ")";
	/*
	for (vector<TmsTimeInterval>::iterator it = stat.rawDataSorted.begin();
	     it != stat.rawDataSorted.end(); ++it) {
	  oss << " " << *it << ";";
	}
	*/
	oss << endl;

      }
      //ost << "]";
    }
    return oss.str();
  }


  void PConsumerTask::calculateStatistics() {
    if (statistics != NULL)
      throw TMSException("Statistics already calculated!");

    statistics = new map<std::string, Statistics*>;
    //map<std::string, Statistics*>& stat = *statistics;
    (*statistics)["LIB"] = new PConsumerTask::Statistics[nProducers];
    (*statistics)["LET"] = new PConsumerTask::Statistics[nProducers];
    (*statistics)["RTA"] = new PConsumerTask::Statistics[nProducers];

    for (size_t i = 0; i < nProducers; ++i) {
      for (size_t e = 0; e < N_EVALS; ++e) {
	const string& eval = EVALS[e];
	Statistics& st = (*statistics)[eval][i];
	list<TmsTimeInterval>* data = ages[i][eval];
	st.bsize = data->size();
	st.mu = calculateMu(data);
	st.var = calculateVar(data, st.mu);
	st.sigma = sqrt(st.var);
	//sortData(&st.rawDataSorted, data);
      }

      TmsTimeInterval tiMin = TMS_TIME_INTERVAL_MAX;
      TmsTimeInterval tiMax = TMS_TIME_INTERVAL_MIN;
      for (TmsTimeInterval ti: rtaGaps[i]) {
	if (ti < tiMin)
	  tiMin = ti;
	if (ti > tiMax)
	  tiMax = ti;
      }
      Statistics& st = (*statistics)["RTA"][i];
      st.minGap = tiMin;
      st.maxGap = tiMax;
    }
  }


  double PConsumerTask::calculateMu(const list<TmsTimeInterval>* data) const {
    TmsTime sum = 0;
    for (list<TmsTimeInterval>::const_iterator it = data->begin();
	 it != data->end(); ++it) {
      sum += *it;
    }
    return (double)sum / (double)data->size();
  }

  
  double PConsumerTask::calculateVar(const std::list<TmsTimeInterval>* data, double mu) const {
    TmsTime sum = 0;
    for (list<TmsTimeInterval>::const_iterator it = data->begin();
	 it != data->end(); ++it) {
      sum += (*it - mu) * (*it - mu);
    }
    return (double)sum / (double)data->size();
  }


  void PConsumerTask::sortData(std::vector<TmsTimeInterval>* dataSorted, const list<TmsTimeInterval>* data) {
    list<TmsTimeInterval> tempList;
    for (list<TmsTimeInterval>::const_iterator itRead = data->begin();
	 itRead != data->end(); ++itRead) {
      list<TmsTimeInterval>::iterator itWrite = tempList.begin();
      while (itWrite != tempList.end()) {
	if (*itWrite >= *itRead)
	  break;
	else
	  ++itWrite;
      }
      tempList.insert(itWrite, *itRead);
    }
    dataSorted->resize(tempList.size());
    size_t offset = 0;
    for (list<TmsTimeInterval>::const_iterator itRead = tempList.begin();
	 itRead != tempList.end(); ++itRead, ++offset) {
      (*dataSorted)[offset] = *itRead;
    }
  }

  
  Job* PConsumerTask::spawnHook(TmsTime now) {
    return new PConsumerTask::PConsumerJob(this, activations, now, executionTime, now + relDeadline, getPriority(), nProducers, producers);
  }


  bool PConsumerTask::completionHook(Job *job, TmsTime now) {
    
    PConsumerTask::PConsumerJob* pjob = dynamic_cast<PConsumerTask::PConsumerJob*>(job);
    assert(pjob != NULL);

    TmsTime fstep = pjob->getFirstStep();
    
    const TmsTimeElement* dtLIB = pjob->getDtLIB();
    const TmsTimeElement* dtLET = pjob->getDtLET();
    const TmsTimeElement* dtRTA = pjob->getDtRTA();

    for (size_t i = 0; i < nProducers; ++i) {
      ages[i]["LIB"]->push_back(fstep - dtLIB[i].tReal);
      ages[i]["LET"]->push_back(fstep - dtLET[i].tReal);
      ages[i]["RTA"]->push_back(fstep - dtRTA[i].tReal);
      // TODO: additionally store for RTA job->getActivationTime() - dtRTA[i].tLogical
      rtaGaps[i].push_back(job->getActivationTime() - dtRTA[i].tLogical);
    }
    
    if (dataLog != NULL) {
      *dataLog << "j" << getIdString() << "," << pjob->getJobId() << " "
	      << pjob->getActivationTime() << " " << pjob->getFirstStep();
      for (size_t i = 0; i < nProducers; ++i) {
	*dataLog << " [" << dtLIB[i] << " " << dtLET[i] << " " << dtRTA[i] << "]";
      }
      *dataLog << endl;
    }
    
    
    return PeriodicTask::completionHook(job, now);
  }


  string PConsumerTask::getShortId(void) const {
    return "PC";
  }


  ostream& operator<<(ostream& ost, const PConsumerTask::Statistics& stat) {
    ost.precision(2);
    ost << std::fixed << "{" << stat.mu << "/" << stat.sigma << "/" << stat.var << "}";
    return ost;
  }
  

} // NS tmssim
