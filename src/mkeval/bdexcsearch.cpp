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
 * $Id: bdexcsearch.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file bdexcsearch.cpp
 * @brief Search for breakdown utilisation using exact schedulability
 * condition from Goossens. This works only for DBP and similar, but
 * may not for fixed (m,k)-patterns!
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 *
 */

#include <utils/bitstrings.h>
#include <utils/logger.h>
#include <utils/mtrunner.h>
#include <utils/tlogger.h>
#include <utils/kvfile.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <mkeval/mkallocators.h>
#include <mkeval/abstractmktaskset.h>
#include <mkeval/mkeval.h>
#include <mkeval/gstsimulation.h>

#include <mkeval/periodgenerator.h>
#include <mkeval/intervalperiodgenerator.h>
#include <mkeval/gmperiodgenerator.h>

#include <xmlio/tasksetwriter.h>

#include <boost/program_options.hpp>

using namespace std;
using namespace tmssim;
namespace po = boost::program_options; 


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

/// @name Simulation functions
/// @{
/// Taskset generation
AbstractMkTaskset* generateTaskset();
/// Taskset execution
BdResultSet* executeTaskset(AbstractMkTaskset* ats);
/// Actual simulation
BdResultSet* simulate(AbstractMkTaskset* ats);
/// Result output
void processResult(BdResultSet* rs);
/// @}


/// @name Program options
/// @todo remove simple copies
/// @{
po::options_description desc;
po::variables_map vm;

/// @brief path to config file
string poConfigFile;
/// @brief path to econf file
string poEconfFile;
/// @brief seed parameter
unsigned poSeed;
/// @brief seed file parameter
string poSeedFile;
/// @brief taskset size
unsigned theTasksetSize;
/// @brief how many taskset to generate
unsigned theNTasksets;
/// @brief utilisation for task set generation (will also be start utilisation)
double theGenUtilisation;
/// @brief maximum allowed deviation from generation utilisation
double theUtilisationDeviation;
/// @brief increment of utilisation for search for breakdown utilisation
double theUtilisationStep;
/// @brief configuration of logger
vector<string> poLog;
/// @brief number of simulation threads parameter (-m)
unsigned theNThreads;
/// @brief allocators (= scheduling scenarios)
vector<string> poAllocators;
/// @brief loggin prefix
string theLogPrefix;
/// @brief write TS to file?
bool theToFile = false;
/// @brief TS prefix
string theXmlPrefix = "";
/// @}


/// @name Actual parameters (only for those that need preprocessing)
/// @{
/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;

/// @brief Econf file
KvFile* econf = NULL;
SchedulerConfiguration scc;

/// @brief Seed used for taskset generation
unsigned int theSeed;
vector<const MkEvalAllocatorPair*> theAllocators;
map<string, size_t> allocatorIdMap;
unsigned nEvals;
list<unsigned int> theSeeds;
/// @brief Period generator
PeriodGenerator* thePeriodGenerator;

/// @}


/// @name Simulation
/// @{

MtRunner<AbstractMkTaskset,BdResultSet>* theSimulation;

ofstream *resultLog;

/// @}


int main(int argc, char* argv[]) {
  logger::setClass(0);

  initialiseProgramOptions();
  
  try {
    po::store(parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
      printUsage(argv[0]);
      return 1;
    }
    po::notify(vm);
  }
  catch (po::error& e) {
    cerr << "ERROR: " << e.what() << endl;
    cerr << "Use '--help' for further information" << endl;
    return -1;
  }
  
  if (!initialise()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }
  
  cout << "==INFO== " << "Initialising program options with boost succeeded:" << endl;
  if (poSeedFile != "") {
    cout << "==INFO== " << "\tseedFile: " << poSeedFile << endl;		
  }
  else {
    cout << "==INFO== " << "\ttheSeed: " <<	theSeed << endl;
  }
  if (vm.count("restrict-periods")) {
    cout << "restricted periods";
  }
  else {
    cout << "interval [" << gcfg->getUInt32("minPeriod")
	 << "," << gcfg->getUInt32("maxPeriod") << "]";
  }
  cout << endl;

  cout << "==INFO== " << "\ttheTasksetSize: " << theTasksetSize << endl;		
  cout << "==INFO== " << "\ttheNTasksets: " << theNTasksets << endl;		
  cout << "==INFO== " << "\ttheGenUtilisation: " << theGenUtilisation << endl;
  cout << "==INFO== " << "\ttheUtilisationDeviation: " << theUtilisationDeviation << endl;
  cout << "==INFO== " << "\ttheUtilisationStep: " << theUtilisationStep << endl;
  cout << "==INFO== " << "\ttheNThreads: " << theNThreads << endl;	
  cout << "==INFO== " << "\ttheAllocators:";
  for (const MkEvalAllocatorPair* a: theAllocators) {
    cout << " " << a->id;
  }
  cout << endl;
  cout << "==INFO== " << "\ttheLogPrefix: " << theLogPrefix << endl;
  if (theToFile) {
    cout << "==INFO== " << "\ttheXmlPrefix: " << theXmlPrefix << endl;
  }
  cout << endl;

  theSimulation = new MtRunner<AbstractMkTaskset,BdResultSet>(generateTaskset, executeTaskset, processResult, theNThreads);
  theSimulation->run();
  
  cleanup();

  return 0;
}


/*
  Good overview of how to use boost::program_options:
  http://www.radmangames.com/programming/how-to-use-boost-program_options
*/
void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&poConfigFile)->required(), "KvFile with settings for the task set generator (mandatory)")
    ("econf,e", po::value<string>(&poEconfFile), "KvFile with settings for taskset execution")
    ("seed,S", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    ("seed-file", po::value<string>(&poSeedFile), "Seed file (one seed per line)")
    (",t", po::value<unsigned>(&theTasksetSize)->default_value(5), "TASKSETSIZE")
    (",T", po::value<unsigned>(&theNTasksets)->default_value(100), "NTASKSETS")
    ("gen-utilisatione,U", po::value<double>(&theGenUtilisation)->required(), "Utilisation for initial task set")
    ("utilisation-step,u", po::value<double>(&theUtilisationStep)->default_value(0.1), "Utilisation increment for breakdown search")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(0.1), "Deviation of actual utilisation for minimum utilisation")
    (",m", po::value<unsigned>(&theNThreads)->default_value(thread::hardware_concurrency()), "Simulation threads")
    (",a", po::value<vector<string>>(&poAllocators)->required(), "Scheduler/Task allocators, for valid options see below")
    ("prefix,p", po::value<string>(&theLogPrefix)->required(), "Log prefix")
    (",x", po::value<string>(&theXmlPrefix)->implicit_value(""), "Write successful tasksets to xml file (default prefix is log prefix")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ("restrict-periods", "Use period generator by Goossens & Macq (periods with many common divisors")
    ;
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
  cout << "The -a, -l and -u options may be specified multiple times." << endl;
  cout << "If a the --seed-file option is specified, the --seed option is ignored." << endl;
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
    scc = SchedulerConfiguration(*econf);
  }
  
  // seeds
  if (vm.count("seed-file")) {
    // read seeds from file
    ifstream in(poSeedFile);
    if (!in.is_open()) {
      tError() << "Could not open seed file " << poSeedFile;
      INITIALISE_FAIL;
    }
    while (!in.eof()) {
      #define BS 255
      char buffer[BS];
      in.getline(buffer, BS);
      if (buffer[0] == '\0') continue;
      unsigned int sv = atol(buffer);
      theSeeds.push_back(sv);
    }
  }
  else {
    // generate
    if (vm.count("seed")) {
      theSeed = poSeed;
    }
    else {
      theSeed = time(NULL);
    }
    for (size_t i = 0; i < theNTasksets; ++i) {
      theSeeds.push_back(rand_r(&theSeed));
    }
  }

  if (vm.count("-x")) {
    theToFile = true;
    if (theXmlPrefix == "") {
      theXmlPrefix = theLogPrefix;
    }
  }
  
  // allocators
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
  for (size_t i = 0; i < nEvals; ++i) {
    allocatorIdMap[theAllocators[i]->id] = i;
  }

  if (vm.count("restrict-periods")) {
    thePeriodGenerator = new GmPeriodGenerator(GmPeriodGenerator::LOWER);
  }
  else {
    thePeriodGenerator = new IntervalPeriodGenerator(gcfg->getUInt32("minPeriod"),
						     gcfg->getUInt32("maxPeriod"));
  }

  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;

  {
    ostringstream oss;
    oss << theLogPrefix << "-results.log";
    resultLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *resultLog << "Format: [seed];";
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      *resultLog << " [" << ap->id << "]";
    }
    *resultLog << " { max }";
    *resultLog << endl;
    *resultLog << "\t[sched] = [ u_breakdown ; u_real ; u_mk ]" << endl;
  }
  
 initialise_end:
  return success;
}


void cleanup() {
  if (econf)
    delete econf;
  delete gcfg;
  resultLog->close();
  delete resultLog;
  delete theSimulation;
  delete thePeriodGenerator;  
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}


AbstractMkTaskset* generateTaskset() {
  if (theSeeds.size() == 0)
    return NULL;
  else {
    unsigned int cSeed = theSeeds.front();
    theSeeds.pop_front();
    AbstractMkTaskset* ats =
      new AbstractMkTaskset(cSeed, theTasksetSize, gcfg,
			    theUtilisationDeviation, theGenUtilisation,
			    thePeriodGenerator);
    return ats;
  }
}


BdResultSet* executeTaskset(AbstractMkTaskset* ats) {
  BdResultSet* rs = simulate(ats);
  delete ats;
  return rs;
}


BdResultSet* simulate(AbstractMkTaskset* ats) {
  BdResultSet* rs = new BdResultSet(ats->getSeed(), nEvals);

  double currentUtilisation = theGenUtilisation;
  vector<const MkEvalAllocatorPair*> myAllocators;
  for (const MkEvalAllocatorPair* ap: theAllocators) {
    myAllocators.push_back(ap);
  }

  ostringstream oss;
  oss << theLogPrefix << "-ts-" << ats->getSeed() << ".log";
  ofstream simLog(oss.str(), ios::out | ios::trunc);

  simLog << "Hyperperiod: " << ats->getHyperPeriod() << endl;
  simLog << "FeasibilityMultiplicator: " << ats->getMkFeasibilityMultiplicator() << endl;
  
  while (myAllocators.size() > 0) {
    
    ats->setUtilisation(currentUtilisation);
    double ur = ats->getRealUtilisation();
    double umk = ats->getMkUtilisation();

    simLog << endl;
    simLog << "Utilisation: " << currentUtilisation
	   << " U_r: " << ur
	   << " U_mk: " << umk
	   << endl;

    MkTaskset* mkts = ats->getMkTasks();
    list<string> failedScenarios;
    for (const MkEvalAllocatorPair* ap: myAllocators) {
      list<MkTask*> tasks;
      for (MkTask* t: mkts->tasks) {
	tasks.push_back(ap->taskAlloc(t));
      }
      
      GstSimulation* mkets = new GstSimulation(tasks, ap->schedAlloc(scc), ap->id);
      simLog << ap->id << ":";

      bool success = mkets->simulate();

      
      if (success) {
	BdResultData& rd = rs->data[allocatorIdMap.at(ap->id)];
	rd.breakdownUtilisation = currentUtilisation;
	rd.realUtilisation = ur;
	rd.mkUtilisation = umk;
	rd.success = true;
	simLog << " success";
	if (mkets->stateRecurred())
	  simLog << " due to recurring state";
	
	simLog <<" after " << mkets->getHpCount()-1
	       << " hyperperiods in cycle "
	       << mkets->getSimulation()->getTime() << "."
	       << " (EC: " << const_cast<Simulation*>(mkets->getSimulation())->getResults().execCancellations << "/" << const_cast<Simulation*>(mkets->getSimulation())->getResults().ecPerformanceLost << ")";
	if (mkets->reducedStateRecurred()) {
	  simLog << " [RS @ " << mkets->getReducedStateHyperPeriod()
		 << " / " << mkets->getReducedStateCycle() << "]";
	}
	simLog << endl;

	if (theToFile) {
	  ostringstream oss;
	  oss << theXmlPrefix << "-" << currentUtilisation << "-" << ats->getSeed() << "-" << ap->id << ".xml";

	  vector<Task*> wtasks;
	  for (MkTask* t: tasks) {
	    wtasks.push_back(new MkTask(t));
	  }
	  
	  TasksetWriter* tsw = TasksetWriter::getInstance();
	  bool result = tsw->write(oss.str(), wtasks);
	  if (!result) {
	    tError() << "Writing task set failed!";
	  }
	  for (Task* t: wtasks) {
	    delete t;
	  }
	  wtasks.clear();
	}
	
      }
      else { // (!success)
	failedScenarios.push_back(ap->id);
	simLog << " failed after " << mkets->getHpCount()-1
	       << " hyperperiods in cycle "
	       << mkets->getSimulation()->getTime() << "."
	       << " (EC: " << const_cast<Simulation*>(mkets->getSimulation())->getResults().execCancellations << ")"
	       << endl;
      }
      simLog << "\t";
      const CompressedMkState* states = mkets->getLastMkStates();
      size_t i = 0;
      for (MkTask* task: tasks) {
	simLog << " ";
	printBitString(simLog, states[i], task->getK());
	++i;
      }
      simLog << endl;
      delete mkets;
      // no need to delete these, simulation takes care of this
    }

    for (string sc: failedScenarios) {
      vector<const MkEvalAllocatorPair*>::iterator it = myAllocators.begin();
      while ((*it)->id != sc && it != myAllocators.end()) {
	++it;
      }
      if (it != myAllocators.end()) {
	myAllocators.erase(it);
      }
      else {
	tError() << "Have stored " << sc << " for removal, but cannot find in myAllocators!";
      }
    }
    failedScenarios.clear();
    delete mkts;
    currentUtilisation += theUtilisationStep;
  }

  return rs;
}


void processResult(BdResultSet* rs) {
  double umax = 0;
  list<string> maxList;
  *resultLog << rs->seed << " ;";
  for (size_t i = 0; i < theAllocators.size(); ++i) {
    *resultLog << " [ " << rs->data[i].breakdownUtilisation
	<< " ; " << rs->data[i].realUtilisation
	<< " ; " << rs->data[i].mkUtilisation
	<< " ]";
    if (rs->data[i].breakdownUtilisation > umax) {
      maxList.clear();
      umax = rs->data[i].breakdownUtilisation;
	maxList.push_back(theAllocators[i]->id);
    }
    else if (rs->data[i].breakdownUtilisation == umax) {
      maxList.push_back(theAllocators[i]->id);
      }
  }
  *resultLog << " {";
  for (const string& s: maxList) {
    *resultLog << " " << s;
  }
  *resultLog << " }";
  *resultLog << endl;
  delete rs;
  maxList.clear();
}


