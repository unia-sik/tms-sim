/**
 * $Id: tsrun.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file tsrun.cpp
 * @brief Execute a TS taskset
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/simulation.h>
#include <core/task.h>
#include <schedulers/fpp.h>
#include <tseval/pconsumertask.h>
#include <tseval/pproducertask.h>
#include <tseval/tmppconsumertask.h>

#include <utils/logger.h>
//#define TLOGLEVEL TLL_DEBUG
#include <utils/tlogger.h>

#include <xmlio/tasksetreader.h>

#include <list>
#include <map>

#include <cmath>
#include <cstring>

using namespace std;
using namespace tmssim;


bool parseArgs(int argc, char *argv[]);
void printUsage(char* prog);
bool init();
void cleanup();

TmsTime calculateLCM(TmsTime a, TmsTime b);
TmsTime calculateGCD(TmsTime a, TmsTime b);
void insertRtaTask(PeriodicTask* t);
bool responseTimeAnalysis();


/// @name Command line parameters
/// @{

/// @brief input file (-i)
string parmInputFile = "";
bool haveInputFile = false;
/// @brief Execution steps (-n)
unsigned int parmExecSteps = 0;
bool haveExecSteps = false;
/// @}


/// @name Actual parameters
/// @{

list<PConsumerTask*> consumers;
Taskset* tasks;
unsigned int theExecSteps;
list<PeriodicTask*> rtaList;

/// @}


ostream* dataLog = &cout; // TODO: init!?


int main(int argc, char *argv[]) {
  //cout << "Start" << endl;
  //logger::setLevel(0);
  logger::setClass(0);

  int rv = 0;
  int parseArgsStatus = parseArgs(argc, argv);
  if (parseArgsStatus > 0) { // -- help was called
    return 0;
  }
  else if (parseArgsStatus < 0) { // an error occured during parsing parameters
    tError() << "Failed parsing arguments, exiting";
    return -1;
  }
  //cout << "Parsing finished" << endl;
  
  if (!init()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    rv = -1;
  }
  if (!responseTimeAnalysis()) {
    tError() << "Response time analysis failed, exiting.";
    rv = -2;
  }
  //cout << "Init finished" << endl;

  for (const Task* task: *tasks) {
    cout << *task << endl;
  }
  
  if (rv == 0) {
    // now we can run


    Simulation sim(tasks, new FPPScheduler);
    
    *dataLog << "Producer:" << endl;
    *dataLog << "TPP{N},{X} activationTime (LIB/LET/RTA) [all times physical]" << endl;
    
    *dataLog << "Consumer:" << endl;
    *dataLog << "TPC{N},{X} activationTime firstStep [Producer 1] ... [Producer n]" << endl;
    *dataLog << "\t[Producer X] = [(LIB/LIB), (LIB/LET), (LIB/RTA)]" << endl;

    Simulation::ExitCondition ec = sim.run(theExecSteps);
    if (ec != 0) {
      tError() << "Failure occurred during simulations: " << ec;
    }
    const SimulationResults& results = sim.getResults();
    *dataLog << results;

    for (PConsumerTask* ct: consumers) {
      ct->calculateStatistics();
      *dataLog << *ct << endl;
      *dataLog << ct->getPrintableData();
    }
    
  }

  cleanup();
  return rv;
}


void printUsage(char* prog) {
  //cout << "Job: " << sizeof(Job) << endl;

  cout << "Usage: " << prog << " [arguments] \n\
  Arguments:\n\
  -i INPUTFILE XML file containing the taskset (mandatory)\n\
  -n STEPS number of execution steps (optional, default=hyperperiod)\n\
" << endl;
}



/**
 * @todo return more different error codes
 */
bool parseArgs(int argc, char *argv[]) {
  
  int rv = 0;
  char *str;
  char sw;
  int p = 1;
  //int ts = -1; // not used
  
  while (p < argc) {
    if (argv[p][0] == '-') {
      sw = argv[p][1];
      if (sw == '\0') {
	// empty parameter?
	tError() << "Received stray parameter: -\n";
	goto failure;
      }
      
      if (argv[p][2] != '\0') { // -sstring
	str = &argv[p][2];
      } else {
	if (++p < argc) { // -s string
	  str = argv[p];
	} else {
	  // ???? problem!!!
	  str = NULL;
	  // TODO: this can lead to SIGSEGV below!
	}
      }
      
      switch (sw) {
	
      case '-': // this is the --PARM section
	if (strcmp(str, "help") == 0) {
	  printUsage(argv[0]);
	  rv = 1;
	  goto failure;
	}
	break;
	
	// the -PARM parameters follow

      case 'i': // input file
	if (str == NULL) {
	  tError() << "You need to specify the input file!";
	  goto failure;
	} else {
	  parmInputFile = str;
	  haveInputFile = true;
	}
	break;

      case 'n': // Number of iterations
	if (str == NULL) {
	  tError() << "You need to specify the number of iterations!";
	  goto failure;
	} else {
	  parmExecSteps = atol(str);
	  haveExecSteps = true;
	}
	break;
      default:
	tError() << "Unknown option -" << sw << "!";
	goto failure;
      }
      ++p;
    } else {
      // failure
      tError() << "Invalid paramter format";
      goto failure;
    }
  } // end while
  
    // check parameters
  if (!haveInputFile) {
    tError() <<  "You must specify an input file!";
    rv = -1;
  }
 failure:
  return rv;
}


#define INIT_FAIL success = false;		\
  goto init_end;


bool init() {
  bool success = true;

  // input file
  TasksetReader* reader = TasksetReader::getInstance();
  Taskset tmpTasks;
  TmsTime hp = 1;
  if (reader->read(parmInputFile, tmpTasks)) {
    
    //st<PProducerTask*> producers;
    map<string, PProducerTask*> producerMap;
    list<TmpPConsumerTask*> tmpConsumers;
    list<TmsTime> periods;
    tasks = new Taskset;
    for (Task* t: tmpTasks) {
      //cout << *t << endl;
      PProducerTask* pt;
      TmpPConsumerTask* ct;
      if ( (pt = dynamic_cast<PProducerTask*>(t)) != NULL) {
	pt->setDataLog(dataLog);
	cout << *pt << endl;
	tasks->push_back(pt);
	//producers.push_back(pt);
	producerMap[pt->getIdString()] = pt;
	periods.push_back(pt->getPeriod());
	//rtaList.push_back(pt);
	insertRtaTask(pt);
      }
      else if ( (ct = dynamic_cast<TmpPConsumerTask*>(t)) != NULL) {
	tmpConsumers.push_back(ct);
	periods.push_back(ct->getPeriod());
      }
      else {
	tInfo() << "Discarding unrelated task" << *t;
	delete t;
      }
    }
    // now create actual consumers
    for (TmpPConsumerTask* tct: tmpConsumers) {
      const list<string>* producerStrings = tct->getProducers();
      PProducerTask** producers = new PProducerTask*[producerStrings->size()];
      size_t i = 0;
      for (const string& pstr: *producerStrings) {
	producers[i] = producerMap[pstr];
	++i;
      }
      PConsumerTask* pc = new PConsumerTask(tct->getId(), tct->getPeriod(),
					    tct->getExecutionTime(),
					    tct->getPriority(),
					    tct->getResponseTime(),
					    producerStrings->size(),
					    producers, dataLog);
      cout << *pc << endl;
      consumers.push_back(pc);
      tasks->push_back(pc);
      //rtaList.push_back(pc);
      insertRtaTask(pc);
      tDebug() << "Created Consumer: " << *pc;
      delete tct;
    }
    tmpConsumers.clear();

    // now calculate hyperperiod
    hp = 1;
    for (TmsTime p: periods) {
      hp = calculateLCM(hp, p);
    }
  }
  else {
    tError() << "Failed reading input file " << parmInputFile;
    INIT_FAIL;
  }

  // execution steps
  if (haveExecSteps) {
    theExecSteps = parmExecSteps;
  }
  else {
    theExecSteps = hp;
  }
  
 init_end:
  return success;
}


void cleanup() {
  // no need to clean up tasks/taskset, this is done by the simulation
  /*
  for (Task* t: tasks) {
    delete t;
  }
  */
}


TmsTime calculateLCM(TmsTime a, TmsTime b) {
  return llabs(a * b) / calculateGCD(a, b);
}


TmsTime calculateGCD(TmsTime a, TmsTime b) {
  TmsTime h;
  while (b != 0) {
    h = b;
    b = a % b;
    a = h;
  }
  return a;
}


void insertRtaTask(PeriodicTask* t) {
  list<PeriodicTask*>::iterator it = rtaList.begin();
  while (it != rtaList.end() && (*it)->getPriority() <= t->getPriority()) {
    ++it;
  }
  rtaList.insert(it, t);
  tDebug() << "Inserted task " << t->getIdString()
	   << " prio " << t->getPriority();
}


bool responseTimeAnalysis() {
  for (list<PeriodicTask*>::iterator outer = rtaList.begin();
       outer != rtaList.end(); ++outer) {
    TmsTimeInterval responseTime = 0;
    for (list<PeriodicTask*>::iterator inner = rtaList.begin();
	 inner != outer; ++inner) {
      responseTime += (*inner)->getExecutionTime();
    }
    responseTime += (*outer)->getExecutionTime();
    TmsTimeInterval lastResponseTime = 0;
    while ( (responseTime != lastResponseTime)
	    && (responseTime <= (*outer)->getPeriod()) ) {
      TmsTimeInterval interference = 0;
      for (list<PeriodicTask*>::iterator inner = rtaList.begin();
	 inner != outer; ++inner) {
	interference += lround(ceil( (double)responseTime / (double)(*inner)->getPeriod())) * (*inner)->getExecutionTime();
      }
      lastResponseTime = responseTime;
      responseTime = interference + (*outer)->getExecutionTime();
    }
    if (responseTime > (*outer)->getPeriod()) {
      tError() << "Response time analysis failed for task "
	       << (*outer)->getIdString() << " RT=" << responseTime;
      return false;
    }
    else {
      PProducerTask* pt;
      PConsumerTask* ct;
      if ( (pt = dynamic_cast<PProducerTask*>((*outer))) != NULL) {
	pt->setResponseTime(responseTime);
	tInfo() << "Set response time for " << pt->getIdString()
		<< " to " << responseTime;
      }
      else if ( (ct = dynamic_cast<PConsumerTask*>((*outer))) != NULL) {
	ct->setResponseTime(responseTime);
	tInfo() << "Set response time for " << ct->getIdString()
		<< " to " << responseTime;
      }
      else {
	tError() << "Could not set response time for task " << *(*outer);
	return false;
      }
    }
  }
  return true;
}
