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
 * $Id: eval.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file eval.cpp
 * @brief Evaluations for comparison of (m,k)-firm schedulers
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 *
 * This eval yiels only approximate results, as the simulations are performed
 * only for a fixed number of time steps.
 * Nevertheless, we keep it to keep the results from the ARCS paper
 * reproducible.
 */


#include <utils/logger.h>
//#define TLOGLEVEL TLL_WARN
#include <utils/tlogger.h>
#include <utils/kvfile.h>
#include <utils/mtrunner.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <fstream>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <mkeval/mkallocators.h>
#include <mkeval/mkeval.h>
#include <mkeval/mkgenerator.h>
#include <mkeval/utilisationstatistics.h>


#include <xmlio/tasksetwriter.h>

#include <boost/program_options.hpp>


namespace po = boost::program_options; 
using namespace std;
using namespace tmssim;


/**
 * @brief Initialise the program_options object
 */
void initialiseProgramOptions();

/**
 * @brief Initialise the program variables from the program_options
 */
bool initialise();

/// Cleanup all global memory
void cleanup();

/**
 * Print the usage message that describes all parameters
 */
void printUsage(char* prog);

/**
 * @brief Check wether the config file contains all relevant data
 */
bool checkGCfg(void);


/// @name New multithreading
/// @{

/// Taskset generation
MkTaskset* generateTaskset();
/// Taskset execution
MkEval* executeTaskset(MkTaskset* ats);
/// Result output
void processResult(MkEval* rs);

MtRunner<MkTaskset,MkEval>* theSimulation;

/// @}


/**
 * Output results
 */
void printResults();

string binString(unsigned int n, unsigned int width);

/// @name Program options
/// @{
po::options_description desc;
po::variables_map vm;

/// @brief path to config file
string poConfigFile;
/// @brief path to econf file
string poEconfFile;
/// @brief seed parameter
unsigned poSeed;
/// @brief taskset size
unsigned theTasksetSize;
/// @brief how many taskset to generate
unsigned theNTasksets;
/// @brief number of simulation steps
TmsTime theSimulationSteps;
/// @brief number of simulation threads parameter (-m)
unsigned theNThreads;
/// @brief target utilisation
double theUtilisation;
/// @brief maximum allowed deviation from generation utilisation
double theUtilisationDeviation;
/// @brief log prefix
string theLogPrefix = "";
/// @brief allocators (= scheduling scenarios)
vector<string> poAllocators;
/// @brief configuration of logger
vector<string> poLog;
/// @brief write TS to file?
bool theToFile = false;
/// @brief TS prefix
string theXmlPrefix = "";
/// @}


/// @name Actual parameters
/// @{

const int DEFAULT_SIMULATION_STEPS = 100;
const int DEFAULT_TASKSET_SIZE = 5;
const int DEFAULT_N_TASKSETS = 100;
const double DEFAULT_UTILISATION = 1.0;
const double DEFAULT_UTILISATION_DEVIATION = 0.1;

/// @brief Seed used for taskset generation
unsigned int theSeed;

/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;

/// @brief Econf file
KvFile* econf = NULL;
SchedulerConfiguration scc;

vector<const MkEvalAllocatorPair*> theAllocators;
unsigned nEvals;

/// @brief logfiles for tasks/scheduler
ofstream** tsLogs;
/// @brief logfile task success maps
ofstream* taskSuccessLog;

/// @}


/// @name Results
/// @name Results
/// @{

/// @brief Accumulated results of all task sets
/// @todo make dynamic
MkResults* results = NULL;
/// @brief Accumulated results of all successful task sets
/// @todo make dynamic
MkResults* successResults = NULL;
/// @brief Discarded Task sets
unsigned int discardedTasksets = 0;
/// @brief How many task sets passed the sufficient schedulability test (Jia et al.)
unsigned int schedTestPass = 0;
/**
 * @brief Count specific success combinations.
 *
 * Each index bit stands for one scheduler evaluation using the respective
 * scheduler from the #theAllocators.
 * @todo make dynamic
 */
unsigned* resultBucket = NULL;
/**
 * @brief Count specific success combinations including schedulability test
 *
 * Highest index bit stands for schedulability test.
 * @see #resultBucket
 * @todo make dynamic
 */
unsigned* resultSchedBucket = NULL;
/// @brief Collect statistics about the task sets' utilisation
UtilisationStatistics uStats;

/// @}

int main(int argc, char* argv[]) {
  // INIT
  logger::setClass(0);

  initialiseProgramOptions();

  try {
    store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      printUsage(argv[0]);
      return 1;
    }
    notify(vm);
  }
  catch (po::error& e) {
    tError() << "ERROR: " << e.what();
    tError() << "Use '--help' for further information.";
    return -1;
  }
  
  if (!initialise()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }


  cout << "==INFO== Seed: " << theSeed << endl;
  cout << "==INFO== Taskset size: " << theTasksetSize << endl;
  cout << "==INFO== Utilisation: " << theUtilisation << endl;
  cout << "==INFO== UDeviation: " << theUtilisationDeviation << endl;
  cout << "==INFO== Execution steps: " << theSimulationSteps << endl;
  cout << "==INFO== CfgFile: " << poConfigFile << endl;
  cout << "==INFO== N Tasksets: " << theNTasksets << endl;
  cout << "==INFO== N Threads: " << theNThreads << endl;
  cout << "==INFO== " << "\ttheAllocators:";
  for (const MkEvalAllocatorPair* a: theAllocators) {
    cout << " " << a->id;
  }
  cout << endl;
  if (vm.count("prefix")) {
    cout << "==INFO== Log prefix: " << theLogPrefix << endl;
  }
  if (theToFile) {
    cout << "==INFO== " << "\ttheXmlPrefix: " << theXmlPrefix << endl;
  }

  cout << "==INFO== Using generation parameters:" << endl;
  cout << "==INFO== \tminPeriod: " << gcfg->getUInt32("minPeriod") << endl;
  cout << "==INFO== \tmaxPeriod: " << gcfg->getUInt32("maxPeriod") << endl;
  cout << "==INFO== \tminK: " << gcfg->getUInt32("minK") << endl;
  cout << "==INFO== \tmaxK: " << gcfg->getUInt32("maxK") << endl;
  cout << "==INFO== \tmaxWC: " << gcfg->getUInt32("maxWC") << endl << endl;

  // RUN
  theSimulation = new MtRunner<MkTaskset,MkEval>(generateTaskset, executeTaskset, processResult, theNThreads);
  theSimulation->run();


  // FINISH
  // now create final data and output results
  uStats.evaluate();
  printResults();

  cleanup();
  return 0;
}


void cleanup() {
  delete econf;
  delete gcfg;

  if (vm.count("prefix")) { // flush and close log files
    for (unsigned i= 0; i < nEvals; ++i) {
      tsLogs[i]->close();
      delete tsLogs[i];
    }
    delete[] tsLogs;
    taskSuccessLog->close();
    delete taskSuccessLog;
  }

  if (results != NULL)
    delete[] results;
  if (successResults != NULL)
    delete[] successResults;
  if (resultBucket != NULL)
    delete[] resultBucket;
  if (resultSchedBucket != NULL)
    delete[] resultSchedBucket;

}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&poConfigFile)->required(), "KvFile with settings for the task set generator (mandatory)")
    ("econf,e", po::value<string>(&poEconfFile), "KvFile with settings for taskset execution")
    ("seed,s", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    (",t", po::value<unsigned>(&theTasksetSize)->default_value(DEFAULT_TASKSET_SIZE), "Taskset size")
    (",n", po::value<TmsTime>(&theSimulationSteps)->default_value(DEFAULT_SIMULATION_STEPS), "Simulation steps")
    (",m", po::value<unsigned>(&theNThreads)->default_value(thread::hardware_concurrency()), "Simulation threads")
    (",T", po::value<unsigned>(&theNTasksets)->default_value(DEFAULT_N_TASKSETS), "Number of tasksets to simulate")
    (",u", po::value<double>(&theUtilisation)->default_value(DEFAULT_UTILISATION), "Target utilisation")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(DEFAULT_UTILISATION_DEVIATION), "Deviation of actual utilisation for target utilisation")
    (",a", po::value<vector<string>>(&poAllocators)->required(), "Scheduler/Task allocators, for valid options see below")
    ("prefix,p", po::value<string>(&theLogPrefix)->required(), "Log prefix")
    (",x", po::value<string>(&theXmlPrefix)->implicit_value(""), "Write successful tasksets to xml file (default prefix is log prefix")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ;
}


#define INITIALISE_FAIL success = false;	\
  goto initialise_end;

bool initialise() {
  cout << "Initialising program..." << endl;
  bool success = true;

  // config file:
  try {
    gcfg = new KvFile(poConfigFile);
    if (!checkGCfg()) {
      tError() << "Invalid configuration file!";
      INITIALISE_FAIL;
    }
    else {
      tInfo() << "Configuration file read successfully!";
    }
  }
  catch (KvFileException& e) {
    tError() << "Error when reading config file " << poConfigFile
	     << ": " << e.error;
    INITIALISE_FAIL;
  }

  // econf
  if (vm.count("econf")) {
    try {
      econf = new KvFile(poEconfFile);
      tInfo() << "EConf file read successfully!";
    }
    catch (KvFileException& e) {
    tError() << "Error when reading econf file " << poEconfFile
	     << ": " << e.error;
    INITIALISE_FAIL;
    }
    scc = SchedulerConfiguration(econf);
  }

  // seed
  if (vm.count("seed")) {
    theSeed = poSeed;
  }
  else {
    theSeed = time(NULL);
  }

  // allocators
  if (poAllocators.size() == 0) {
    tError() << "No allocators given!";
    INITIALISE_FAIL;
  }
  for (const string& alloc: poAllocators) {
    const MkEvalAllocatorPair* ap = MkAllocators::instance().getAllocatorPair(alloc);
    if (ap == NULL) {
      tError() << "Unknown allocator: " << alloc;
      INITIALISE_FAIL;
    }
    vector<const MkEvalAllocatorPair*>::iterator it = theAllocators.begin();
    while (it != theAllocators.end() && (*it)->id < alloc) {
      ++it;
    }
    theAllocators.insert(it, ap);
  }
  // now we know how many evals must be performed:
  nEvals = theAllocators.size();

  // Memory
  results = new MkResults[nEvals];
  successResults = new MkResults[nEvals];
  resultBucket = new unsigned[1 << nEvals];
  resultSchedBucket =new unsigned[1 << (nEvals + 1)];
  
  // just to be sure
  for (unsigned i = 0; i < nEvals; ++i) {
    resultBucket[i] = 0;
    resultSchedBucket[i] = 0;
  }

  // log prefix
  if (vm.count("prefix")) {
    tsLogs = new ofstream*[nEvals];
    for (unsigned i = 0; i < nEvals; ++i) {
      ostringstream oss;
      oss << theLogPrefix;
      oss << "-" << theAllocators[i]->id << ".txt";
      tsLogs[i] = new ofstream(oss.str(), ios::out | ios::trunc);
      *tsLogs[i] << "tsid ; succ ; act ; compl ; canc ; ecanc ; miss ; preempt ; usum ; esum ; mksuccs" << endl;
    }
    ostringstream oss;
    oss << theLogPrefix << "-success.txt";
    taskSuccessLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *taskSuccessLog << "TSID ; STEST";
    for (int i = nEvals - 1; i >= 0; --i) {
      *taskSuccessLog << " | " << theAllocators[i]->id;
    }
    *taskSuccessLog << endl;
  }

  if (vm.count("-x")) {
    theToFile = true;
    if (theXmlPrefix == "") {
      theXmlPrefix = theLogPrefix;
    }
  }

  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;


 initialise_end:
  return success;
}


void printUsage(char* prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  /*
    TODO: nicer output, e.g. with output class from 
    http://www.radmangames.com/programming/how-to-use-boost-program_options
  */
  cout << desc << "\n";

  cout << "Valid parameters for the -a option are:" << endl;
  list<string> al = MkAllocators::instance().listAllocators();
  for (const string& alloc: al) {
    cout << "\t" << alloc << endl;
  }

  cout << "Valid parameters for the -l option are:";
  for (size_t i = 0; i < LOG_N_PREFIX; ++i) {
    cout << " " << tmssim::detail::LOG_PREFIX[i];
  }
  cout << endl;

  cout << "The -a and -l options may be specified multiple times." << endl;
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}


MkTaskset* generateTaskset() {
  static MkGenerator generator(theSeed, theTasksetSize,
			       gcfg->getUInt32("minPeriod"),
			       gcfg->getUInt32("maxPeriod"),
			       gcfg->getUInt32("minK"),
			       gcfg->getUInt32("maxK"),
			       theUtilisation, theUtilisationDeviation,
			       gcfg->getUInt32("maxWC"));
  static unsigned genCtr = 0;

  ++genCtr;
  if (genCtr <= theNTasksets) {
    MkTaskset* mkts = generator.nextTaskset();

    if (theToFile) {
      ostringstream oss;
      oss << theXmlPrefix << "-" << mkts->seed << ".xml";
      
      vector<Task*> wtasks;
      for (MkTask* t: mkts->tasks) {
	wtasks.push_back(new MkTask(t));
      }
      
      TasksetWriter* tsw = TasksetWriter::getInstance();
      bool result = tsw->write(oss.str(), wtasks);
      //TasksetWriter::clear();
      if (!result) {
	tError() << "Writing task set failed!";
      }
      for (Task* t: wtasks) {
	delete t;
      }
      wtasks.clear();
    }
    
    return mkts;
  }
  else
    return 0;
}


MkEval* executeTaskset(MkTaskset* ts) {
  MkEval* eval = new MkEval(ts, nEvals, theAllocators, scc, theSimulationSteps);
  eval->run();
  return eval;
}


void processResult(MkEval* eval) {
  if (eval == NULL) {
    return;
  }
  if (eval->getSuccessMap() == EVAL_MAP_FULL(nEvals) ) {
    // all schedulers successful
    unsigned int rb = 0;
    unsigned int rsb = 0;
    for (unsigned int i = 0; i < nEvals; ++i) {
      const MKSimulationResults& sres = eval->getResults()[i];
      results[i].successes += sres.success ? 1 : 0;
      results[i].activations += sres.activations;
      results[i].completions += sres.completions;
      results[i].cancellations += sres.cancellations;
      results[i].execCancellations += sres.execCancellations;
      results[i].misses += sres.misses;
      results[i].preemptions += sres.preemptions;
      results[i].usum += sres.usum;
      results[i].esum += sres.esum;
      if (sres.mkfail) {
	results[i].mkfails += 1;
      }
      else {
	results[i].mksuccs += 1;
	rb |= 1 << i;
      }
      if (vm.count("prefix")) {
	*tsLogs[i] << eval->getTaskset()->seed << " ; "
		   << sres.success << " ; "
		   << sres.activations << " ; "
		   << sres.completions << " ; "
		   << sres.cancellations << " ; "
		   << sres.execCancellations << " ; "
		   << sres.misses << " ; "
		   << sres.preemptions << " ; "
		   << sres.usum << " ; "
		   << sres.esum << " ; "
		   << (sres.mkfail ? 0 : 1) << endl;
	tsLogs[i]->flush();
      }
    }

    // aggregate fully successfull tasksets
    if (rb == EVAL_MAP_FULL(nEvals)) {
      for (unsigned int i = 0; i < nEvals; ++i) {
	const MKSimulationResults& sres = eval->getResults()[i];
	successResults[i].successes += sres.success ? 1 : 0;
	successResults[i].activations += sres.activations;
	successResults[i].completions += sres.completions;
	successResults[i].cancellations += sres.cancellations;
	successResults[i].execCancellations += sres.execCancellations;
	successResults[i].misses += sres.misses;
	successResults[i].preemptions += sres.preemptions;
	successResults[i].usum += sres.usum;
	successResults[i].esum += sres.esum;
	if (sres.mkfail) {
	  successResults[i].mkfails += 1;
	}
	else {
	  successResults[i].mksuccs += 1;
	  rb |= 1 << i;
	}
      }
    }
    
    if (eval->getTaskset()->suffMKSched) {
      ++schedTestPass;
      rsb = 1 << nEvals;
    }
    uStats.addUtilisation(eval->getTaskset()->realUtilisation);
    rsb |= rb;
    ++resultBucket[rb];
    ++resultSchedBucket[rsb];
    
    if (vm.count("prefix")) {
      *taskSuccessLog << eval->getTaskset()->seed << " ; "
		      << binString(rsb, nEvals + 1) << endl;
    }
  }
  else {
    // This should not happen, any scheduler should run until the end!
    tError() << "Aggregator detected unfinished taskset!";
    ++discardedTasksets;
  }
  delete eval;
}


void printResults() {
  /*
  cout << "RB:" << endl;
  for (int i = 0; i < (1 << nEvals); ++i) {
    //cout << " " << resultBucket[i];
    cout << binString(i, nEvals) << ": " << resultBucket[i] << endl;
  }
  cout << endl;
  */
  //cout << "RSB:" << endl;
  cout << "==RSBI==; schedTestPass";
  for (int i = nEvals - 1; i >= 0; --i) {
    cout << " ; " << theAllocators[i]->id;
  }
  cout << endl;
  for (unsigned i = 0; i < (unsigned)(1 << (nEvals + 1)); ++i) {
    //cout << " " << resultSchedBucket[i];
    cout << "==RSB==; " << i << " ; " << binString(i, nEvals + 1) << " ; " << resultSchedBucket[i] << endl;
  }
  cout << endl;

  cout << "==STAT==; Sched ; succ ; act ; compl ; canc ; ecanc ; miss ; preempt ; usum ; esum ; mksuccs ; succ%" << endl;
  for (unsigned int i = 0; i < nEvals; ++i) {
    cout << "==STAT==; " << theAllocators[i]->id
	 << " ; " << round(results[i].successes / theNTasksets)
	 << " ; " << round(results[i].activations / theNTasksets)
	 << " ; " << round(results[i].completions / theNTasksets)
	 << " ; " << round(results[i].cancellations / theNTasksets)
	 << " ;" << round(results[i].execCancellations / theNTasksets)
	 << " ; " << round(results[i].misses / theNTasksets)
	 << " ; " << round(results[i].preemptions / theNTasksets)
	 << " ; " << round(results[i].usum / theNTasksets)
	 << " ; " << round(results[i].esum / theNTasksets)
	 << " ; " << results[i].mksuccs
	 << " ; " << (double)results[i].mksuccs / theNTasksets << endl;
  }
  cout << "==STA2==; schedTestPass ; " << schedTestPass 
       << " ; " << (double)schedTestPass / theNTasksets << endl;
  //cout << "Task sets that passed the sufficient schedulability test: " << schedTestPass << endl;
  cout << "==STA2==; discarded ; " << discardedTasksets << endl;
  //cout << "Discarded " << (discardedTasksets/4) << " tasksets from results" << endl;

  for (unsigned int i = 0; i < nEvals; ++i) {
    cout << "==SUCC==; " << theAllocators[i]->id
	 << " ; " << round(successResults[i].successes / theNTasksets)
	 << " ; " << round(successResults[i].activations / theNTasksets)
	 << " ; " << round(successResults[i].completions / theNTasksets)
	 << " ; " << round(successResults[i].cancellations / theNTasksets)
	 << " ;" << round(successResults[i].execCancellations / theNTasksets)
	 << " ; " << round(successResults[i].misses / theNTasksets)
	 << " ; " << round(successResults[i].preemptions / theNTasksets)
	 << " ; " << round(successResults[i].usum / theNTasksets)
	 << " ; " << round(successResults[i].esum / theNTasksets)
	 << " ; " << successResults[i].mksuccs
	 << " ; " << (double)successResults[i].mksuccs / theNTasksets << endl;
  }


  cout << "==UTIL== "
       << "[" << uStats.getMin() << ";" << uStats.getMax() << "] "
       << uStats.getMean()
       << " +/- " << uStats.getSigma()
       << " m=" << uStats.getMedian()
       << " 50% [" << uStats.getMedianIntervalLower(50)
       << ";" << uStats.getMedianIntervalUpper(50) << "]" << endl;

}


string binString(unsigned int n, unsigned int width) {
  assert(width <= sizeof(unsigned int) * 8);
  ostringstream oss;
  for (int i = width - 1; i >= 0; --i) {
    if (n & (1 << i))
      oss << 1;
    else
      oss << 0;
  }
  return oss.str();
}


