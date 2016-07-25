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
 * $Id: mkdse.cpp 1432 2016-07-15 10:43:04Z klugeflo $
 * @file mkdse.cpp
 * @brief Extensive DSE (check all possible task sets, log breakdown anomalies
 * and breakdown utilisations).
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/abstractmktaskset.h>
#include <mkeval/concretemktaskset.h>
#include <mkeval/fixedtimesimulation.h>
#include <mkeval/mkallocators.h>
#include <mkeval/mkdsesimulationset.h>
#include <mkeval/gstsimulation.h>
#include <mkeval/mkglobals.h>
#include <mkeval/mksimulation.h>

#include <mkeval/periodgenerator.h>
#include <mkeval/intervalperiodgenerator.h>
#include <mkeval/gmperiodgenerator.h>

#include <utils/bitstrings.h>
#include <utils/mtlgrunner.h>
#include <utils/tlogger.h>

#include <xmlio/tasksetwriter.h>

#include <bitset>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include <boost/program_options.hpp>


#include <signal.h>

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

/**
 * Generate all Tasksets
 */
//void generateAllTasksets();

/**
 * Calculate number of utilisations and initialise #mapLog file
 */
void finishInitialisation(void);

/// @name Simulation functions
/// @{
/// Taskset generation
list<MkSimulation*> generateTaskset();

void writeCtsToXml(ConcreteMkTaskset* cts, string path);

/// Taskset execution
MkSimulation* executeTaskset(size_t tid, MkSimulation* mkSimulation);
/// Result output
void processResult(MkSimulation* mkSimulation);
/// @}

void writeDssLog(MkDseSimulationSet* dss);
void writeDssToGlobalLogs(MkDseSimulationSet* dss);
void cleanupDss(MkDseSimulationSet* dss);

void signalHandler(int signo);


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
/// @brief if we want to simulate only a fixed time
TmsTime theSteps = 0;
/// @brief Period generator
PeriodGenerator* thePeriodGenerator;
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

list<MkSimulation*> allSimulations;

bool initialisationFinished = false;
std::mutex initLock;
std::condition_variable initCond;
thread tInit;

bool generationFinished = false;
std::mutex genLock;
std::condition_variable genCond;

unsigned nUtils;
double uMax = 0.0;

/// @}


/// @name Simulation
/// @{


MtLgRunner<MkSimulation,MkSimulation>* theSimulation;

MkSimulation** currentSim;


ofstream *bdfLog;
ofstream *bdlLog;
ofstream *mapLog;
ofstream *anLog; // anomalies

/// @}

/// @name Additional Management structures
/// @{
list<MkDseSimulationSet*> simulationSets;
map<ConcreteMkTaskset*, MkDseSimulationSet*> ctsToSs;
map<MkSimulation*, ConcreteMkTaskset*> mkSimToCts;
mutex mgtLock;
/// @}


/// Final stats
size_t nTasksets = 0;
size_t* nSuccesses;
size_t nSims = 0;
size_t finishedSims = 0;

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
  cout << "==INFO== " << "\tthePeriodGenerator: ";
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
  cout << "==INFO== " << "\tSchedulerconfig: "
       << "SCC EC: " << scc.execCancellations
       << " DLMC: " << scc.dlMissCancellations << endl;

  cout << "==INFO== " << "\ttheLogPrefix: " << theLogPrefix << endl;
  if (theToFile) {
    cout << "==INFO== " << "\ttheXmlPrefix: " << theXmlPrefix << endl;
  }
  
  cout << endl;


  // setup signal handler
  if (signal(SIGUSR1, signalHandler) == SIG_ERR) {
    cerr << "Cannot register handler" << endl;
  }

  tInit = thread(&finishInitialisation);  
  
  theSimulation = new MtLgRunner<MkSimulation,MkSimulation>(generateTaskset, executeTaskset, processResult, theNThreads);
  theSimulation->run();

  tInit.join();

  cout << "==INFO== " << "\tTotal concrete task sets: " << nSims << endl;
  cout << "==INFO== " << "\tUmax: " << uMax << " nUtils: " << nUtils << endl;

  ostringstream oss;
  oss << theLogPrefix << "-succ.log";
  ofstream succLog(oss.str(), ios::out | ios::trunc);
  succLog << "Tasksets: " << nTasksets << endl;
  for (size_t i = 0; i < nEvals; ++i) {
    succLog << theAllocators[i]->id << ": " << nSuccesses[i] << endl;
  }
  succLog.close();
  
  cleanup();
  
  return 0;
}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&poConfigFile)->required(), "KvFile with settings for the task set generator (mandatory)")
    ("econf,e", po::value<string>(&poEconfFile), "KvFile with settings for taskset execution")
    ("seed,S", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    ("seed-file", po::value<string>(&poSeedFile), "Seed file (one seed per line)")
    (",t", po::value<unsigned>(&theTasksetSize)->default_value(5), "TASKSETSIZE")
    (",T", po::value<unsigned>(&theNTasksets)->default_value(100), "NTASKSETS")
    ("gen-utilisation,U", po::value<double>(&theGenUtilisation)->required(), "Utilisation for initial task set")
    ("utilisation-step,u", po::value<double>(&theUtilisationStep)->default_value(0.1), "Utilisation increment for breakdown search")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(0.1), "Deviation of actual utilisation for minimum utilisation")
    (",n", po::value<TmsTime>(&theSteps)->default_value(0), "Number of steps to simulate (if 0, use exact schedulability tests)")
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

  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;


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
      //in >> sv;
      //cout << "\t" << sv << endl;
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
    unsigned int genSeed = theSeed;
    for (size_t i = 0; i < theNTasksets; ++i) {
      theSeeds.push_back(rand_r(&genSeed));
      //cout << "\t" << theSeeds.back() << endl;
    }
  }

  if (vm.count("-x")) {
    theToFile = true;
    if (theXmlPrefix == "") {
      theXmlPrefix = theLogPrefix;
    }
  }

  if (theSteps != 0) {
    FixedTimeSimulation::setSteps(theSteps);
  }
  
  // allocators
  for (const string& alloc: poAllocators) {
    const MkEvalAllocatorPair* ap = MkAllocators::instance().getAllocatorPair(alloc);
    if (ap == NULL) {
      tError() << "Unknown allocator: " << alloc;
      INITIALISE_FAIL;
    }
    if (theSteps != 0) {
      MkEvalAllocatorPair* nap = new MkEvalAllocatorPair(*ap);
      nap->simAlloc = FixedTimeSimulationAllocator;
      ap = nap;
    }
    vector<const MkEvalAllocatorPair*>::iterator it = theAllocators.begin();
    while (it != theAllocators.end() && (*it)->id < alloc) {
      ++it;
    }
    theAllocators.insert(it, ap);
  }
  // now we know how many evals must be performed:
  nEvals = theAllocators.size();
  nSuccesses = new size_t[nEvals];
  for (size_t i = 0; i < nEvals; ++i) {
    allocatorIdMap[theAllocators[i]->id] = i;
    nSuccesses[i] = 0;
  }

  if (vm.count("restrict-periods")) {
    thePeriodGenerator = new GmPeriodGenerator(GmPeriodGenerator::LOWER);
  }
  else {
    thePeriodGenerator = new IntervalPeriodGenerator(gcfg->getUInt32("minPeriod"),
						     gcfg->getUInt32("maxPeriod"));
  }
    
  {
    ostringstream oss;
    oss << theLogPrefix << "-results.log";
    bdfLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *bdfLog << "Format: [seed];";
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      *bdfLog << " [" << ap->id << "]";
    }
    *bdfLog << " { max }";
    *bdfLog << endl;
    *bdfLog << "\t[sched] = [ u_breakdown ; u_real ; u_mk ]" << endl;
  }

  {
    ostringstream oss;
    oss << theLogPrefix << "-bdl.log";
    bdlLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *bdlLog << "Format: [seed];";
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      *bdlLog << " [" << ap->id << "]";
    }
    *bdlLog << " { max }";
    *bdlLog << endl;
    *bdlLog << "\t[sched] = [ u_breakdown ; u_real ; u_mk ]" << endl;
  }

  {// Map log
    ostringstream oss;
    oss << theLogPrefix << "-maps.log";
    mapLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *mapLog << "Format: [seed]";
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      *mapLog << " [" << ap->id << "]";
    }
    *mapLog << endl;
  }

  {// Anomaly log
    ostringstream oss;
    oss << theLogPrefix << "-an.log";
    anLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *anLog << "Format: [seed]";
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      *anLog << " [" << ap->id << "]";
    }
    *anLog << endl;
  }

  currentSim = new MkSimulation*[theNThreads];
  for (size_t i = 0; i < theNThreads; ++i) {
    currentSim[i] = NULL;
  }
  
 initialise_end:
  return success;
}


void cleanup() {
  for (MkDseSimulationSet* dss: simulationSets) {
    tWarn() << "Deleting simulation set " << dss << " map: " << dss->getMapStr();
    delete dss;
  }
  if (econf)
    delete econf;
  delete gcfg;
  bdfLog->close();
  delete bdfLog;
  bdlLog->close();
  delete bdlLog;
  mapLog->close();
  delete mapLog;
  anLog->close();
  delete anLog;
  delete theSimulation;
  delete[] nSuccesses;
  if (theSteps != 0) {
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      delete ap;
    }
  }

  delete thePeriodGenerator;  
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}


void finishInitialisation(void) {
  {
    std::unique_lock<std::mutex> lck(genLock);
    while (!generationFinished) {
      genCond.wait(lck);
    }
  }
  //nSims = allSimulations.size();

  *mapLog << "Utilisations:";
  double util = theGenUtilisation;
  nUtils = 0;
  while (util <= uMax) {
    *mapLog << " " << util;
    util += theUtilisationStep;
    ++nUtils;
  }
  *mapLog << endl;
  {
    unique_lock<mutex> lck(initLock);
    initialisationFinished = true;
    initCond.notify_all();
    cout << "Init finished, have " << nUtils << " utilisations" << endl;
  }
}


list<MkSimulation*> generateTaskset() {
  list<MkSimulation*> mksims;
  if (theSeeds.size() != 0) {
    unsigned int cSeed = theSeeds.front();
    theSeeds.pop_front();
    AbstractMkTaskset* ats =
      new AbstractMkTaskset(cSeed, theTasksetSize, gcfg,
			    theUtilisationDeviation, theGenUtilisation,
			    thePeriodGenerator);
    
    MkDseSimulationSet* dss = new MkDseSimulationSet(ats, theGenUtilisation,
						     theUtilisationStep,
						     theAllocators, scc);
    {
      std::unique_lock<std::mutex> lck(mgtLock);
      simulationSets.push_back(dss);
      const std::vector<ConcreteMkTaskset*>& ctss = dss->getCtss();
      for(ConcreteMkTaskset* cts: ctss) {
	ctsToSs[cts] = dss;
	double uc = cts->getMkTaskset()->targetUtilisation;
	if (uc > uMax) {
	  //cout << "new umax: " << uc << endl;
	  uMax = uc;
	}
	const vector<MkSimulation*>& mkss = cts->getMkSimulations();
	for (MkSimulation* mksim: mkss) {
	  mkSimToCts[mksim] = cts;
	  mksims.push_back(mksim);
	  allSimulations.push_back(mksim);
	  ++nSims;
	}
	if (theToFile) {
	  ostringstream oss;
	  oss << theXmlPrefix << "-" << cts->getMkTaskset()->targetUtilisation << "-" << ats->getSeed() << "-raw.xml";
	  writeCtsToXml(cts, oss.str());
	}
      }
    }
  }

  if (theSeeds.size() == 0) {
    unique_lock<mutex> lck(genLock);
    generationFinished = true;
    genCond.notify_all();
  }
  
  return mksims;
}


void writeCtsToXml(ConcreteMkTaskset* cts, string path) {
  vector<Task*> wtasks;
  for (MkTask* t: cts->getMkTaskset()->tasks) {
    wtasks.push_back(t);
  }
  
  TasksetWriter* tsw = TasksetWriter::getInstance();
  bool result = tsw->write(path, wtasks);
  //TasksetWriter::clear();
  if (!result) {
    tError() << "Writing task set failed!";
  }
  wtasks.clear();
}


MkSimulation* executeTaskset(size_t tid, MkSimulation* mkSimulation) {
  mgtLock.lock();
  currentSim[tid] = mkSimulation;
  mgtLock.unlock();
  mkSimulation->simulate();
  mgtLock.lock();
  currentSim[tid] = NULL;
  mgtLock.unlock();
  return mkSimulation;
}


void processResult(MkSimulation* mkSimulation) {
  {
    std::unique_lock<std::mutex> lck(initLock);
    while (!initialisationFinished) {
      initCond.wait(lck);
    }
  }
  std::unique_lock<std::mutex> lck(mgtLock);
  ++finishedSims;
  //cout << "MkSim finished: " << mkSimulation << " (ms: " << mkSimToCts.size() << ")" << endl;
  ConcreteMkTaskset* cts = NULL;
  try {
    cts = mkSimToCts.at(mkSimulation);
  }
  catch (const out_of_range& oor) {
    tError() << "MkSimulation " << mkSimulation << " not found mkSimToCts Map!";
    throw oor;
  }

  /*
  cout << "MKSim finished: " << mkSimulation->getAllocId()
       << " U_T=" << cts->getMkTaskset()->targetUtilisation
       << " S=" << mkSimulation->getSuccess()
       << endl;
  */
  
  if (mkSimToCts.erase(mkSimulation) != 1) {
    tError() << "Erasing MkSimulation from map failed!";
  }
  cts->notifyFinished(mkSimulation);
  if (cts->isFinished()) {
    MkDseSimulationSet* dss;
    try {
      dss = ctsToSs.at(cts);
    }
    catch (const out_of_range& oor) {
      tError() << "MkSimulation not found ctsToSs Map!";
      throw oor;
    }
    if (ctsToSs.erase(cts) < 1) {
      tError() << "Erasing CTS from map failed!";
    }
    dss->notifyFinished(cts);
    if (dss->isFinished()) {
      writeDssLog(dss);
      writeDssToGlobalLogs(dss);
      cleanupDss(dss);
    }
  }
}


void writeDssLog(MkDseSimulationSet* dss) {
  ostringstream oss;
  const AbstractMkTaskset* ats = dss->getAts();
  oss << theLogPrefix << "-ts-" << ats->getSeed() << ".log";
  ofstream simLog(oss.str(), ios::out | ios::trunc);

  simLog << "Hyperperiod: " << ats->getHyperPeriod() << endl;
  simLog << "FeasibilityMultiplicator: " << ats->getMkFeasibilityMultiplicator() << endl;
  simLog << "MKP-FeasibilityPeriod: " << ats->getMkpFeasibilityPeriod() << endl;

  simLog << "Abstract tasks: " << *ats << endl;
  
  const std::vector<ConcreteMkTaskset*>& ctss = dss->getCtss();
  // @todo: handle "duplicate" CTS somehow!
  for(ConcreteMkTaskset* cts: ctss) {
    const MkTaskset* mkts = cts->getMkTaskset();
    simLog << endl;
    simLog << "Utilisation: " << mkts->targetUtilisation
	   << " U_r: " << mkts->realUtilisation
	   << " U_mk: " << mkts->mkUtilisation
	   << " S: " << mkts->suffMKSched
	   << endl;
    
    const vector<MkSimulation*>& mkss = cts->getMkSimulations();
    //const std::vector<MkTask*>& tasks = mkts->tasks;
    for (MkSimulation* mksim: mkss) {
      simLog << mksim->getSimulationMessage() << endl;

      const std::list<MkTask*>& tasks = mksim->getMkTasks();
      GstSimulation* mkets = dynamic_cast<GstSimulation*>(mksim);
      if (mkets != NULL) {
	simLog << "\t";
	const CompressedMkState* states = mkets->getLastMkStates();
	size_t i = 0;
	for (MkTask* task: tasks) {
	  simLog << " ";
	  printBitString(simLog, states[i], task->getK());
	  ++i;
	}
	simLog << endl;
      }

      if (mksim->getSuccess() && theToFile) {
	ostringstream oss;
	oss << theXmlPrefix << "-" << mkts->targetUtilisation << "-" << ats->getSeed() << "-" << mksim->getAllocId() << ".xml";
	
	vector<Task*> wtasks;
	for (MkTask* t: tasks) {
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
    }
  }
}


void writeDssToGlobalLogs(MkDseSimulationSet* dss) {

  // @todo handling of "duplicates"
  const std::vector<ConcreteMkTaskset*>& ctss = dss->getCtss();
  size_t nCts = ctss.size();
  assert(nCts <= nUtils);

  MkSimulation* allsims[nEvals][nCts];
  //bitmap_t maps[nEvals];
  Bitmap* maps[nEvals];
  bitmap_t anomalies = 0;
  map<MkSimulation*,ConcreteMkTaskset*> simToCts;

  for (size_t i = 0; i < nEvals; ++i) {
    maps[i] = new Bitmap(nUtils);
  }

  for (size_t ui = 0; ui < nCts; ++ui) {
    ConcreteMkTaskset* cts = ctss[ui];
    const vector<MkSimulation*>& sims = cts->getMkSimulations();
    assert(sims.size() == nEvals);
    for (size_t ei = 0; ei < sims.size(); ++ei) {
      MkSimulation* sim = sims[ei];
      allsims[ei][ui] = sim;
      simToCts[sim] = cts;
      if (sim->getSuccess()) {
	//maps[ei] |= ((bitmap_t)1) << (nUtils - 1 - ui);
	(*maps[ei])[ui] = 1;
	++nSuccesses[ei];
      }
    }
  }

  *bdfLog << dss->getAts()->getSeed() << " ;";
  *bdlLog << dss->getAts()->getSeed() << " ;";

  double bdfMax = 0.0;
  double bdlMax = 0.0;
  list<string> maxfList;
  list<string> maxlList;

  
  // first BD
  for (size_t ei = 0; ei < nEvals; ++ei) {
    MkSimulation* first = NULL;
    for (size_t ui = 0; ui < nCts; ++ui) {
      if (allsims[ei][ui]->getSuccess()) {
	first = allsims[ei][ui];
      }
      else {
	break;
      }
    }
    if (first != NULL) {
      ConcreteMkTaskset* cts = simToCts[first];
      const MkTaskset* mkts = cts->getMkTaskset();

      if (bdfMax < mkts->targetUtilisation) {
	maxfList.clear();
	bdfMax = mkts->targetUtilisation;
	maxfList.push_back(theAllocators[ei]->id);
      }
      else if (bdfMax == mkts->targetUtilisation) {
	maxfList.push_back(theAllocators[ei]->id);
      }
            
      *bdfLog << " [ " << mkts->targetUtilisation
	      << " ; " << mkts->realUtilisation
	      << " ; " << mkts->mkUtilisation
	      << " ]";
   
    }
    else {
      *bdfLog << " [ 0 ; 0 ; 0 ]";
    }

    // last BD
    MkSimulation* last = NULL;
    for (size_t ui = 0; ui < nCts; ++ui) {
      if (allsims[ei][ui]->getSuccess()) {
	last = allsims[ei][ui];
      }
    }
    if (last != NULL) {
      ConcreteMkTaskset* cts = simToCts[last];
      const MkTaskset* mkts = cts->getMkTaskset();

      if (bdlMax < mkts->targetUtilisation) {
	maxlList.clear();
	bdlMax = mkts->targetUtilisation;
	maxlList.push_back(theAllocators[ei]->id);
      }
      else if (bdlMax == mkts->targetUtilisation) {
	maxlList.push_back(theAllocators[ei]->id);
      }
            
      *bdlLog << " [ " << mkts->targetUtilisation
	      << " ; " << mkts->realUtilisation
	      << " ; " << mkts->mkUtilisation
	      << " ]";
    }
    else {
      *bdlLog << " [ 0 ; 0 ; 0 ]";
    }

    // Anomalies?
    if (first != last) {
      anomalies |= 1 << (nEvals - 1 - ei);
      //cout << "Anomaly at ei=" << ei << endl;
    }
    
  }
  
  *bdfLog << " {";
  for (const string& s: maxfList) {
    *bdfLog << " " << s;
  }
  *bdfLog << " }";
  *bdfLog << endl;

  *bdlLog << " {";
  for (const string& s: maxlList) {
    *bdlLog << " " << s;
  }
  *bdlLog << " }";
  *bdlLog << endl;


  *mapLog << dss->getAts()->getSeed();
  for (size_t i = 0; i < nEvals; ++i) {
    *mapLog << " ";
    //printBitString(*mapLog, maps[i], nUtils);
    *mapLog << maps[i]->str();
  }
  *mapLog << endl;

  *anLog << dss->getAts()->getSeed() << " " << strBitString(anomalies, nEvals) << endl;;

  for (size_t i = 0; i < nEvals; ++i) {
    delete maps[i];
  }

}


void signalHandler(int signo) {
  std::unique_lock<std::mutex> lck(mgtLock);
  if (signo != SIGUSR1) {
    cerr << "Not configured to handle signal " << signo << endl;
  }
  for (size_t i = 0; i < theNThreads; ++i) {
    cout << "T" << i << ": ";
    MkSimulation* sim = currentSim[i];
    if (sim == NULL) {
      cout << "NULL" << endl;
    }
    else {
      ConcreteMkTaskset* cts = mkSimToCts.at(sim);
      MkDseSimulationSet* mss = ctsToSs.at(cts);
      cout << mss->getAts()->getSeed() << "-" << sim->getAllocId()
	   << "@" << cts->getMkTaskset()->targetUtilisation
	   << " (C: " << sim->getSimulation()->getTime() << ")"
	   << endl;
    }
  }
  cout << "Finished: " << finishedSims << "/" << nSims << endl;
  cout << endl;
}


void cleanupDss(MkDseSimulationSet* dss) {
  const std::vector<ConcreteMkTaskset*>& ctss = dss->getCtss();
  for(ConcreteMkTaskset* cts: ctss) {
    
    const vector<MkSimulation*>& mkss = cts->getMkSimulations();
    for (MkSimulation* mksim: mkss) {
      mkSimToCts.erase(mksim);
    }
    ctsToSs.erase(cts);
  }
  list<MkDseSimulationSet*>::iterator it = simulationSets.begin();
  while (it != simulationSets.end() && *it != dss) {
    ++it;
  }
  if (*it == dss) {
    simulationSets.erase(it);
    delete dss;
  }
  else {
    tError() << "Could not delete simulation set " << dss;
  }
  
}

