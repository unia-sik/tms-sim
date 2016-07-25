/**
 * $Id: tseval.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file tseval.cpp
 * @brief Main file for evaluation of time synchronous execution
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */


#include <core/simulation.h>
#include <schedulers/fpp.h>
#include <tseval/tstaskset.h>
#include <tseval/dependencymatrix.h>

#include <utils/logger.h>
#ifndef TLOGLEVEL
#define TLOGLEVEL TLL_DEBUG
#endif
#include <utils/tlogger.h>

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include <cassert>

using namespace std;
using namespace tmssim;


void testDependencyMatrix();
void testTsTaskSet();
bool parseArgs(int argc, char *argv[]);
void printUsage(char* prog);
bool init();

void runSingle();
void runMulti();
void executeTaskSet(TsTaskSet *ts, ostream* ost);

/**
 * This function handles the generation of tasksets
 * Actually, we might also do this in the main thread, if we like?
 */
void generatorThread();

/**
 * This function shall implement picking a taskset (i.e. a MKEval object)
 * from the globally shared queue that is prepared by the main thread
 * and then executing the attached simulations.
 */
void simulatorThread(int id);


/**
 * Thread-safely put a taskset into the #genTaskSets list.
 * @param ts
 */
void putTaskSet(TsTaskSet* ts);

/**
 * Thread-safely take a taskset from the #genTaskSets list
 * @return a taskset, or NULL if the list is empty and no more task sets
 * will be generated.
 */
TsTaskSet* fetchTaskSet();


/// @name Command line parameters
/// @{

/// @brief generator configuration file parameter (-c)
string parmCfgFile = "";
bool haveCfgFile = false;
/**
 * @brief number of simulation threads parameter (-m)
 *
 * The default value of this parameter is determined by the number of
 * hardware thread contexts available on the platform.
 */
unsigned int parmNThreads = 0;
bool haveNThreads = false;
/// @brief seed parameter (-s)
unsigned int parmSeed = 0;
bool haveSeed = false;
// @brief single run to cout
bool parmSingle = false;
/// @brief number of task sets parameter (-T)
int parmNTaskSets = -1;
bool haveNTaskSets = false;
/// @brief taskset log prefix (-p)
string parmLogPrefix = "";
bool haveLogPrefix = false;
/// @}

/// @name Actual parameters
/// @{

const int DEFAULT_N_TASKSETS = 100;

/// @brief Seed used for taskset generation
unsigned int theSeed;
/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;
/// @brief Number of simulator threads
int theNThreads = 0;
/// @brief Number of tasksets
int theNTaskSets = -1;
/// @brief Log Prefix
string theLogPrefix = "";
/// @}

/// @name Multithreading
/// @{

/// @brief lock for stdout
mutex outLock;
/// @brief Thread for task set generation
thread tGenerator;
/// @brief Threads for simulation
thread* tSimulator = NULL;
/// @}

/// @name Taskset generation
/// @{
/// @brief Tasksets generated by tGenerator
list<TsTaskSet*> genTaskSets;
/// @brief Indicator that taskset generation is finished
bool tsGenerationFinished = false;
/// @brief lock for genTaskSets and tsGenerationFinished
mutex gtsLock;
/// @brief simulatorThreads wait at this conditional for new task sets in genTaskSets
condition_variable gtsCond;
/// @}


ofstream* logptr = NULL;
ostream* logSingle = &cout;



int main(int argc, char* argv[]) {
  //testDependencyMatrix();
  //logger::setLevel(-1);
  logger::setClass(0);

  int parseArgsStatus = parseArgs(argc, argv);
  if (parseArgsStatus > 0) { // -- help was called
    return 0;
  }
  else if (parseArgsStatus < 0) { // an error occured during parsing parameters
    tError() << "Failed parsing arguments, exiting";
    return -1;
  }

  if (!init()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }

  //testTsTaskSet();

  if (parmSingle) {
    runSingle();
  }
  else {
    runMulti();
  }
  if (logptr != NULL) {
    logptr->close();
    delete logptr;
  }
  return 0;
}


void testDependencyMatrix() {
  Random random(42);
  for (unsigned i = 0; i < 10; ++i) {
    cout << DependencyMatrix(5, 10, 4, random) << endl;
  }
}


void testTsTaskSet() {
  Random random(theSeed);
  cout << "Seed: " << theSeed << endl;
  TsTaskSet ts(*gcfg, random.getNumber(), &cout);
  cout << ts;
  Taskset* sts = ts.getSimTasks();
  Simulation sim(sts, new FPPScheduler);

  cout << "Producer:" << endl;
  cout << "jTS{N},{X} activationTime (LIB/LET/RTA)" << endl;
  
  cout << "Consumer:" << endl;
  cout << "jTC{N},{X} activationTime firstStep [Producer 1] ... [Producer n]" << endl;
  cout << "\t[Producer X] = [(LIB/LIB), (LIB/LET), (LIB/RTA)]" << endl;

  Simulation::ExitCondition ec = sim.run(ts.getHyperPeriod());
  if (ec != 0) {
    tError() << "Failure occurred during simulations: " << ec;
  }
  const SimulationResults& results = sim.getResults();
  cout << results;

  for (Taskset::iterator it = sts->begin(); it != sts->end(); ++it) {
    PConsumerTask* pt = dynamic_cast<PConsumerTask*>(*it);
    if (pt != NULL) {
      pt->calculateStatistics();
      cout << *pt << endl;
    }
  }

}


void printUsage(char* prog) {
  //cout << "Job: " << sizeof(Job) << endl;

  cout << "Usage: " << prog << " [arguments] \n\
  Arguments:\n\
  -c CFGFILE KVFile with settings for the task set generator (mandatory)\n\
  -s SEED [default=time(NULL)]\n\
  #-t TASKSETSIZE [default=5]\n\
  #-n SIMULATIONSTEPS [default=100]\n\
  -m SIMULATIONTHREADS [default=" << thread::hardware_concurrency() << "]\n\
  -S run single simulation and output to stdout\n\
  -T NTASKSETS [default=100]\n\
  #-u UTILISATION [default=1.0]\n\
  #-d U. DEVIATION [default=0.1]\n\
  -p LOG_PREFIX\n\
" << endl;
}


/*
  -c CFGFILE
  -s SEED
  -t TASKSETSIZE
  -n SIMULATIONSTEPS
  -m THREADS
  -T N_TASKSETS
 */

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

      case 'c': // config file
	if (str == NULL) {
	  tError() << "You need to specify the config file!";
	  goto failure;
	} else {
	  parmCfgFile = str;
	  haveCfgFile = true;
	}
	break;
	/*
      case 'd': // Utilisation deviation
	if (str == NULL) {
	  tError() << "You need to specify a valid utilisation deviation!";
	  goto failure;
	}
	else {
	  parmUtilisationDeviation = strtod(str, NULL);
	  haveUtilisationDeviation = true;
	}
	break;
	*/

      case 'f':
	logptr = new ofstream(str, ios::out | ios::trunc);
	logSingle = logptr;
	break;

	
      case 'm': // max. number of threads
	if (str == NULL) {
	  tError() << "You need to specify the number of threads!";
	  goto failure;
	} else {
	  parmNThreads = atoi(str);
	  haveNThreads = true;
	}
	break;
	/*
      case 'n': // Number of iterations
	if (str == NULL) {
	  tError() << "You need to specify the number of iterations!";
	  goto failure;
	} else {
	  parmSteps = atol(str);
	  haveSteps = true;
	}
	break;
	*/
      case 'p': // Log prefix
	if (str == NULL) {
	  tError() << "You must specify a logfiles prefix!";
	  goto failure;
	}
	else {
	  parmLogPrefix = str;
	  haveLogPrefix = true;
	}
	break;
	
      case 's': // Seed
	if (str == NULL) {
	  tError() << "You need to specify a seed!";
	  goto failure;
	} else {
	  parmSeed = atoi(str);
	  haveSeed = true;
	}
	break;
      case 'S': // Single run to stdout
	parmSingle = true;
	--p;
	break;
	/*
      case 't': // Task set size
	if (str == NULL) {
	  tError() << "You need to specify a valid task set size!";
	  goto failure;
	} else {
	  parmTaskSetSize = atoi(str);
	  haveTaskSetSize = true;
	}
	break;
	*/
      case 'T': // Number of task sets
	if (str == NULL) {
	  tError() << "You need to specify a valid number of task sets!";
	  goto failure;
	} else {
	  parmNTaskSets = atoi(str);
	  haveNTaskSets = true;
	}
	break;
	/*
      case 'u': // Utilisation
	if (str == NULL) {
	  tError() << "You need to specify a valid utilisation!";
	  goto failure;
	}
	else {
	  parmUtilisation = strtod(str, NULL);
	  haveUtilisation = true;
	}
	break;
	*/	
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
  if (!haveCfgFile) {
    tError() <<  "You must specify a configuration file for the task set generator!";
    rv = -1;
  }
 failure:
  return rv;
}

#define INIT_FAIL success = false;		\
  goto init_end;

bool init() {
  bool success = true;
  unsigned int hwThreads = thread::hardware_concurrency();
  // config file:
  gcfg = new KvFile(parmCfgFile);
  string msg;
  if (!TsTaskSet::validate(*gcfg, &msg)) {
    tError() << "Invalid configuration file, missing parameters:" << msg;
    INIT_FAIL;
  }
  else {
    tInfo() << "Configuration file read successfully!";
  }
  
  if (haveNThreads) {
    if (parmNThreads > hwThreads) {
      tWarn() << "You specified more threads than are available in hardware -"
	      << " falling back to nHWThreads = " << hwThreads;
      theNThreads = hwThreads;
    }
    else {
      theNThreads = parmNThreads;
    }
  }
  else {
    theNThreads = hwThreads;
  }

  if (haveNTaskSets) {
    if (parmNTaskSets <= 0) {
      tError() << "Invalid number of execution steps: " << parmNTaskSets;
      INIT_FAIL;
    }
    else {
      theNTaskSets = parmNTaskSets;
    }
  }
  else {
    theNTaskSets = DEFAULT_N_TASKSETS;
  }

  if (haveLogPrefix) {
    theLogPrefix = parmLogPrefix;
  }
  
  if (haveSeed) {
    theSeed = parmSeed;
  }
  else {
    theSeed = time(NULL);
  }
  
 init_end:
  return success;
}


void putTaskSet(TsTaskSet* ts) {
  unique_lock<mutex> lck(gtsLock);
  genTaskSets.push_back(ts);
  gtsCond.notify_one();
}


TsTaskSet* fetchTaskSet() {
  unique_lock<mutex> lck(gtsLock);
  while (!tsGenerationFinished && genTaskSets.size() == 0) {
    gtsCond.wait(lck);
  }
  TsTaskSet* ts = NULL;
  if (genTaskSets.size() > 0) {
    ts = genTaskSets.front();
    genTaskSets.pop_front();
  }
  else if (tsGenerationFinished) {
    ts = NULL;
  }
  else {
    // should not happen
    abort();
  }
  return ts;
}


void runSingle() {
  //Random random(theSeed);
  //cout << "Seed: " << theSeed << endl;
  //TsTaskSet ts(*gcfg, random.getNumber(), &cout);
  TsTaskSet ts(*gcfg, theSeed, logSingle);
  executeTaskSet(&ts, logSingle);
}


void runMulti() {
  tGenerator = thread(generatorThread);
  tSimulator = new thread[theNThreads];
  for (int i = 0; i < theNThreads; ++i) {
    tSimulator[i] = thread(simulatorThread, i);
  }
  
  // Wait for simulations and aggreation to finish
  tGenerator.join();
  for (int i = 0; i < theNThreads; ++i) {
    tSimulator[i].join();
  }
}


void executeTaskSet(TsTaskSet *ts, ostream* ost) {
  assert(ost != NULL);
  *ost << "Seed: " << ts->getTasksSeed() << endl;
  *ost << *ts;
  ts->setDataLog(ost);
  Taskset* sts = ts->getSimTasks();
  Simulation sim(sts, new FPPScheduler);

  *ost << "Producer:" << endl;
  *ost << "jTS{N},{X} activationTime (LIB/LET/RTA)" << endl;
  
  *ost << "Consumer:" << endl;
  *ost << "jTC{N},{X} activationTime firstStep [Producer 1] ... [Producer n]" << endl;
  *ost << "\t[Producer X] = [(LIB/LIB), (LIB/LET), (LIB/RTA)]" << endl;
  
  Simulation::ExitCondition ec = sim.run(ts->getHyperPeriod());
  if (ec != 0) {
    tError() << "Failure occurred during simulations: " << ec;
  }
  const SimulationResults& results = sim.getResults();
  *ost << results;

  for (Taskset::iterator it = sts->begin(); it != sts->end(); ++it) {
    PConsumerTask* pt = dynamic_cast<PConsumerTask*>(*it);
    if (pt != NULL) {
      pt->calculateStatistics();
      *ost << *pt << endl;
      *ost << pt->getPrintableData();
    }
  }
}


void generatorThread() {
  outLock.lock();
  cout << "==INFO== Using generation parameters:" << endl;
  cout << "==INFO== \tnumberOfProducers: " << gcfg->getUInt32("numberOfProducers") << endl;
  cout << "==INFO== \tproducerUtilisation: " << gcfg->getDouble("producerUtilisation") << endl;
  cout << "==INFO== \tnumberOfConsumers: " << gcfg->getUInt32("numberOfConsumers") << endl;
  cout << "==INFO== \tconsumerUtilisation: " << gcfg->getDouble("consumerUtilisation") << endl;
  cout << "==INFO== \tmaxConsumerInputs: " << gcfg->getUInt32("maxConsumerInputs") << endl;
  cout << "==INFO== \tmaxWC: " << gcfg->getUInt32("maxWC") << endl;
  //cout << "==INFO== \t: " << gcfg->getUInt32("") << endl;
  outLock.unlock();
  Random random(theSeed);
  for (int i = 0; i < theNTaskSets; ++i) {
    TsTaskSet* ts = new TsTaskSet(*gcfg, random.getNumber(), NULL);
    assert(ts != NULL);
    putTaskSet(ts);
  }
  gtsLock.lock();
  tsGenerationFinished = true;
  gtsCond.notify_all();
  gtsLock.unlock();
}


void simulatorThread(__attribute__((unused)) int id) {
  bool finished = false;
  while (!finished) {
    TsTaskSet* ts = NULL;
    ts = fetchTaskSet();
    if (ts == NULL) { // no more task sets
      finished = true;
    }
    else {
      // prepare output
      ostringstream oss;
      oss << theLogPrefix << ts->getTasksSeed() << ".txt";
      ofstream* logfile = new ofstream(oss.str(), ios::out | ios::trunc);
      assert(logfile != NULL);
      ts->setDataLog(logfile);
      // now run simulation
      executeTaskSet(ts, logfile);
      logfile->close();
      delete logfile;
      std::this_thread::yield();
    }
  }
}
