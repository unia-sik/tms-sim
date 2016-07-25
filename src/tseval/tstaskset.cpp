/**
 * $Id: tstaskset.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file tstaskset.h
 * @brief Taskset for evaluation of time synchronous execution
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <tseval/tstaskset.h>

#include <utils/tmsexception.h>

#include <sstream>

using namespace std;

namespace tmssim {

  const std::string TsTaskSet::CONFIG_PARAMETERS[] = {
    //std::string("producerPeriodMin"),
    //std::string("producerPeriodMax"),
    //std::string("consumerPeriodMin"),
    //std::string("consumerPeriodMax"),
    std::string("producerUtilisation"),
    std::string("consumerUtilisation"),
    std::string("numberOfProducers"),
    std::string("numberOfConsumers"),
    std::string("maxConsumerInputs"),
    std::string("maxWC")
  };

  const size_t TsTaskSet::N_CONFIG_PARAMETERS = sizeof(TsTaskSet::CONFIG_PARAMETERS) / sizeof(string);

  
  TsTaskSet::TsTaskSet(const KvFile& _kvCfg, unsigned int _seed, ostream* _dataLog)
    : seedTasks(0), random(_seed), kvCfg(_kvCfg), tmpProducers(NULL), tmpConsumers(NULL), hyperPeriod(-1), dependencies(NULL), myProducers(NULL), myConsumers(NULL), dataLog(_dataLog) {

    nProducers = kvCfg.getUInt32("numberOfProducers");
    nConsumers = kvCfg.getUInt32("numberOfConsumers");

    // @todo include bound for maximum retries
    if (!generateTempTaskSet()) {
      throw TMSException("Taskset generation exceeded retry bound");
    }

    dependencies = new DependencyMatrix(nProducers, nConsumers, kvCfg.getUInt32("maxConsumerInputs"), random);
    //cout << *dependencies;

    // now create real tasks
    createRealTasks(myProducers, myConsumers);
    
  }


  TsTaskSet::~TsTaskSet() {
    if (tmpProducers != NULL)
      delete[] tmpProducers;
    if (tmpConsumers != NULL)
      delete[] tmpConsumers;
    if (dependencies != NULL)
      delete dependencies;
    
    if (myProducers != NULL) {
      for (size_t i = 0; i < nProducers; ++i) {
	if (myProducers[i] != NULL)
	  delete myProducers[i];
      }
      delete[] myProducers;
    }
    if (myConsumers != NULL) {
      for (size_t i = 0; i < nConsumers; ++i) {
	if (myConsumers[i] != NULL)
	  delete myConsumers[i];
      }
      delete[] myConsumers;
    }
  }


  bool TsTaskSet::validate(const KvFile& kvfile, string* msg) {
    ostringstream oss;
    bool ok = true;
    for (size_t i = 0; i < N_CONFIG_PARAMETERS; ++i) {
      if (!kvfile.containsKey(CONFIG_PARAMETERS[i])) {
	ok = false;
	oss << " " << CONFIG_PARAMETERS[i];
      }
    }
    if (kvfile.getUInt32("maxConsumerInputs") * kvfile.getUInt32("numberOfConsumers") < kvfile.getUInt32("numberOfProducers")) {
      oss << " maxConsumerInputs cannot satisfy at least one consumer for each producer!";
    }

    if (msg != NULL) {
      *msg = oss.str();
    }
    return ok;
  }


  std::ostream& TsTaskSet::print(std::ostream& ost) const {
    ost << "Producers:" << endl;
    ost << "ID: C / T / R" << endl;
    for (size_t i = 0; i < nProducers; ++i) {
      ost << *myProducers[i] << endl;
      /*
      PProducerTask* s = producers[i];
      ost << s->getIdString() << ": " << s->getExecutionTime() << " / "
	  << s->getPeriod() << " / "  << s->getResponseTime() << endl;
      */
    }
    ost << "Consumers:" << endl;
    ost << "ID: C / T / R [S1...SN]" << endl;
    for (size_t i = 0; i < nConsumers; ++i) {
      ost << *myConsumers[i] << endl;
      /*
      PConsumerTask* p = consumers[i];
      ost << p->getIdString() << ": " << p->getExecutionTime() << " / "
	  << p->getPeriod() << " / "  << p->getResponseTime() << " [ ";
      PProducerTask** tss = p->getProducers();
      for (size_t j = 0; j < p->getNProducers(); ++j) {
	ost << tss[j]->getIdString() << " ";
      }
      ost << "]" << endl;
      */
    }
    ost << "P_H: " << hyperPeriod << endl;
    
    return ost;
  }

  Taskset* TsTaskSet::getSimTasks() {
    PProducerTask** producers = NULL;
    PConsumerTask** consumers = NULL;
    createRealTasks(producers, consumers);
    Taskset* tasks = new Taskset(nProducers + nConsumers);
    size_t offset = 0;
    for (size_t i = 0; i < nProducers; ++i)
      (*tasks)[offset++] = producers[i];
    for (size_t i = 0; i < nConsumers; ++i)
      (*tasks)[offset++] = consumers[i];
    // contents of arrays are taken over by simulation
    // we only have to free the arrays themselves
    delete[] producers;
    delete[] consumers;

    /*
    delete[] producers;
    producers = NULL;
    delete[] consumers;
    consumers = NULL;
    createRealTasks();
    */
    return tasks;
  }


  void TsTaskSet::setDataLog(std::ostream* ost) {
    dataLog = ost;
  }


  TmsTime TsTaskSet::calculateHyperPeriod(TempTask** tasks, size_t nTasks) {
    TmsTime hp = 1;
    for (size_t i = 0; i < nTasks; ++i) {
      hp = calculateLCM(hp, tasks[i]->period);
    }
    return hp;
  }

  
  TmsTime TsTaskSet::calculateLCM(TmsTime a, TmsTime b) {
    return llabs(a * b) / calculateGCD(a, b);
  }


  TmsTime TsTaskSet::calculateGCD(TmsTime a, TmsTime b) {
    TmsTime h;
    while (b != 0) {
      h = b;
      b = a % b;
      a = h;
    }
    return a;
  }
  
  /// @todo include do/while loop, perform calculation of hyperperiod at end
  bool TsTaskSet::generateTempTaskSet() {
    bool rtaOk = false;
    TsTaskSet::TempTask** taskArray = NULL;
    int ctr = 0;
    // @todo include bound for maximum retries
    do {
      if (tmpProducers != NULL)
	delete[] tmpProducers;
      if(tmpConsumers != NULL)
	delete[] tmpConsumers;
      if (taskArray != NULL)
	delete[] taskArray;
      
      seedTasks = random.getCurrentSeed();
      //tmpProducers = generateTempTasks(nProducers, kvCfg.getUInt32("producerPeriodMin"), kvCfg.getUInt32("producerPeriodMax"), kvCfg.getDouble("producerUtilisation"));
      tmpProducers = generateTempTasks(nProducers, PeriodGenerator(PeriodGenerator::LOW, random), kvCfg.getDouble("producerUtilisation"));
      
      //tmpConsumers = generateTempTasks(nConsumers, kvCfg.getUInt32("consumerPeriodMin"), kvCfg.getUInt32("consumerPeriodMax"), kvCfg.getDouble("consumerUtilisation"));
      tmpConsumers = generateTempTasks(nConsumers, PeriodGenerator(PeriodGenerator::HIGH, random), kvCfg.getDouble("consumerUtilisation"));
      
      list<TsTaskSet::TempTask*> tasks;
      
      for (size_t i = 0; i < nProducers; ++i) {
	TsTaskSet::TempTask* currentTask = &tmpProducers[i];
	currentTask->isProducer = true;
	list<TsTaskSet::TempTask*>::iterator it = tasks.begin();
	for ( ; it != tasks.end(); ++it) {
	  if ((*it)->period > currentTask->period)
	    break;
	}
	tasks.insert(it, currentTask);
      }
      
      for (size_t i = 0; i < nConsumers; ++i) {
	TsTaskSet::TempTask* currentTask = &tmpConsumers[i];
	list<TsTaskSet::TempTask*>::iterator it = tasks.begin();
	for ( ; it != tasks.end(); ++it) {
	  if ((*it)->period > currentTask->period)
	    break;
	}
	tasks.insert(it, currentTask);
      }
      
      //cout << "C / T" << endl;
      taskArray = new TsTaskSet::TempTask*[nProducers+nConsumers];
      size_t offset = 0;
      TmsPriority prio = TMS_MAX_PRIORITY;
      for (list<TsTaskSet::TempTask*>::iterator it = tasks.begin();
	   it != tasks.end(); ++it) {
	(*it)->priority = prio;
	++prio;
	taskArray[offset++] = *it;
	//cout << (*it)->executionTime << " / " << (*it)->period
	//   << ( (*it)->isProducer ? "*" : "" ) << endl;
      }
      //cout << "rta..." <<endl;
      rtaOk = responseTimeAnalysis(taskArray, nProducers+nConsumers);
      ++ctr;
    } while (!rtaOk);
    //cout << "Had to try " << ctr << " times..." << endl;

    // TODO: search hyperperiod!
    hyperPeriod = calculateHyperPeriod(taskArray, nProducers + nConsumers);
    delete[] taskArray;
    return true;
  }

  //TsTaskSet::TempTask* TsTaskSet::generateTempTasks(size_t nTasks, TmsTimeInterval minPeriod, TmsTimeInterval maxPeriod, double targetUtilisation) {
  TsTaskSet::TempTask* TsTaskSet::generateTempTasks(size_t nTasks, PeriodGenerator pgen, double targetUtilisation) {

    unsigned int sumWC = 0;
    float sumU = 0.0;
    unsigned int maxWC = kvCfg.getInt32("maxWC");

    TsTaskSet::TempTask* tasks = new TsTaskSet::TempTask[nTasks];

    // generate basic parameters
    for (size_t i = 0; i < nTasks; ++i) {
      //tasks[i].period = random.getIntervalRand(minPeriod, maxPeriod);
      tasks[i].period = pgen.getNumber();
      tasks[i].wc = random.getIntervalRand(1, maxWC);
      sumWC += tasks[i].wc;
      //cout << "TT: wc=" << tasks[i].wc << " p=" << tasks[i].period << endl;
    }
      
    double uw = targetUtilisation / sumWC; // $\frac{U}{W}$ to speed up calculation
    //cout << "sumWC=" << sumWC << " uw=" << uw << endl;

    for (size_t i = 0; i < nTasks; ++i) {
      double fci = uw * tasks[i].period * tasks[i].wc;
      //sumFU += fci/tasks[i].period;
      tasks[i].executionTime = lround(fci);
      if (tasks[i].executionTime == 0)
	tasks[i].executionTime = 1;
      //cout << "\t" << i << ": fci=" << fci << " c=" << tasks[i].executionTime << endl;
      sumU += (double)tasks[i].executionTime / (double)tasks[i].period;
    }
    //cout << "U_T: " << targetUtilisation << " U_S: " << sumU << endl;
    return tasks;
  }


  //bool TsTaskSet::responseTimeAnalysis(list<TsTaskSet::TempTask*>& tasks) {
  bool TsTaskSet::responseTimeAnalysis(TsTaskSet::TempTask** tasks, size_t n) {

    for (size_t i = 0; i < n; ++i) {
      // calc R_i^(0)
      TmsTimeInterval responseTime = 0;
      for (size_t j = 0; j <= i; ++j) {
	responseTime += tasks[j]->executionTime;
      }
      //cout << "R_" << i << "^(0)=" << responseTime << endl;
      TmsTimeInterval lastResponseTime = 0;
      while ( (responseTime != lastResponseTime) && (responseTime <= tasks[i]->period) ) {
	TmsTimeInterval interference = 0;
	for (size_t j = 0; j < i; ++j) {
	  interference += lround(ceil( (double)responseTime / (double)tasks[j]->period)) * tasks[j]->executionTime;
	}
	lastResponseTime = responseTime;
	responseTime = interference + tasks[i]->executionTime;
      }

      if (responseTime > tasks[i]->period)
	return false;
      else
	tasks[i]->responseTime = responseTime;
      // cout << tasks[i]->executionTime << " / " << tasks[i]->period
      // 	   << ( tasks[i]->isProducer ? "*" : "" ) << " - "
      // 	   << tasks[i]->responseTime << endl;
    }
    return true;
  }
  

  void TsTaskSet::createRealTasks(PProducerTask**& _producers, PConsumerTask**& _consumers) {
    //tasks.resize(nProducers + nConsumers);
    createProducersFromTempTasks(_producers);
    createConsumersFromTempTasks(_consumers, _producers);
  }


  void TsTaskSet::createProducersFromTempTasks(PProducerTask**& _producers) {
    if (_producers != NULL)
      throw TMSException("Producers already initialised!");

    _producers = new PProducerTask*[nProducers];
    
    for (size_t i = 0; i < nProducers; ++i) {
      TempTask& s = tmpProducers[i];
      assert(s.isProducer);
      _producers[i] = new PProducerTask(i, s.period, s.executionTime, s.priority, 2, s.responseTime, dataLog);
      //tasks.push_back(producers[i]);
    }
  }

  
  void TsTaskSet::createConsumersFromTempTasks(PConsumerTask**& _consumers, PProducerTask**& _producers) {
    if (_consumers != NULL)
      throw TMSException("Consumers already initialised!");
    if (_producers == NULL)
      throw TMSException("Initialise producers first!");

    _consumers = new PConsumerTask*[nConsumers];

    for (size_t i = 0; i < nConsumers; ++i) {
      TempTask& p = tmpConsumers[i];
      assert(!p.isProducer);
      size_t _nProducers = dependencies->getRowSum(i);
      PProducerTask** pProducers = new PProducerTask*[_nProducers];
      uint64_t theRow = dependencies->getRow(i);
      size_t offset = 0;
      for (size_t j = 0; j < nProducers; ++j) {
	if ( ((1LL << j) & theRow) != 0)
	  pProducers[offset++] = _producers[j];
      }
      _consumers[i] = new PConsumerTask(i, p.period, p.executionTime, p.priority, p.responseTime, _nProducers, pProducers, dataLog);
      //tasks.push_back(consumers[i]);
    }
  }

  

  std::ostream& operator<<(std::ostream& ost, const TsTaskSet& ts) {
    return ts.print(ost);
  }

} // NS tmssim
