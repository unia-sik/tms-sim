/**
 * $Id: excdse.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file excdse.cpp
 * @brief Design space exploration using exact schedulability test (GST),
 * only meaningful for DBP and MKU!
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo Merge changes to mkdse, then REMOVE again, this file is deprecated.
 */

// bin/mkexcdse -c ../cfg/long.mkg -U 1.05 -d 0.05 -u 0.1 -M 1.5 -m8 -p results/dse -t 5 -T 10 -a DBP -a MKU

// $ grep -v [A-Za-z] dse-maps.log | sed -e "s/^[0-9]*//" | grep -n 01
// need to add 2 to line number to find original line

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
#include <mkeval/mkexttaskset.h>

#include <xmlio/tasksetwriter.h>

#include <boost/program_options.hpp>

using namespace std;
using namespace tmssim;
//using namespace boost::program_options; 
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
DseResultSet* executeTaskset(AbstractMkTaskset* ats);
/// Actual simulation
DseResultSet* simulate(AbstractMkTaskset* ats);
/// Result output
void processResult(DseResultSet* rs);
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
/// @brief maximum utilisation
double theMaxUtilisation;
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
unsigned nUtils;
/// @}


/// @name Simulation
/// @{

MtRunner<AbstractMkTaskset,DseResultSet>* theSimulation;

//ofstream *resultLog;
ofstream *mapLog;

/// @}


int main(int argc, char* argv[]) {
  // INIT
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
  cout << "==INFO== " << "\ttheTasksetSize: " << theTasksetSize << endl;		
  cout << "==INFO== " << "\ttheNTasksets: " << theNTasksets << endl;		
  cout << "==INFO== " << "\ttheGenUtilisation: " << theGenUtilisation << endl;
  cout << "==INFO== " << "\ttheUtilisationDeviation: " << theUtilisationDeviation << endl;
  cout << "==INFO== " << "\ttheUtilisationStep: " << theUtilisationStep << endl;
  cout << "==INFO== " << "\ttheMaxUtilisation: " << theMaxUtilisation << endl;
  cout << "==INFO== " << "\tUtilisations: ";
  double util = theGenUtilisation;
  while (util < theMaxUtilisation) {
    cout << " " << util;
    util += theUtilisationStep;
  }
  cout << endl;
  cout << "==INFO== " << "\tnUtils: " << nUtils << endl;

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

  /*
  AbstractMkTaskset* generateTaskset();
  BdResultSet* executeTaskset(AbstractMkTaskset* ats);
  void processResult(BdResultSet* rs);
  */
  
  theSimulation = new MtRunner<AbstractMkTaskset,DseResultSet>(generateTaskset, executeTaskset, processResult, theNThreads);
  theSimulation->run();
  /*
  // RUN
  tGenerator = thread(generatorThread);
  tSimulator = new thread[theNThreads];
  for (size_t i = 0; i < theNThreads; ++i) {
    tSimulator[i] = thread(simulatorThread, i);
  }
  
  // Wait for simulations and aggreation to finish
  tGenerator.join();

  for (int i = 0; i < theNThreads; ++i) {
    tSimulator[i].join();
  }
  */
  
  //writeResults();
  
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
    ("utilisation-step,u", po::value<double>(&theUtilisationStep)->default_value(0.1), "Deviation of actual utilisation for minimum utilisation")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(0.1), "Deviation of actual utilisation for minimum utilisation")
    (",M", po::value<double>(&theMaxUtilisation)->required(), "Maximum utilisation to check")
    (",m", po::value<unsigned>(&theNThreads)->default_value(thread::hardware_concurrency()), "Simulation threads")
    (",a", po::value<vector<string>>(&poAllocators)->required(), "Scheduler/Task allocators, for valid options see below")
    ("prefix,p", po::value<string>(&theLogPrefix)->required(), "Log prefix")
    (",x", po::value<string>(&theXmlPrefix)->implicit_value(""), "Write successful tasksets to xml file (default prefix is log prefix")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
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
    scc = SchedulerConfiguration(econf);
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
    unsigned actSeed = theSeed;    
    for (size_t i = 0; i < theNTasksets; ++i) {
      theSeeds.push_back(rand_r(&actSeed));
      //cout << "\t" << theSeeds.back() << endl;
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


  // Logger
  for (string logClass: poLog) {
    logger::activateClass(logClass);
  }
  cout << "Log class: " << hex << logger::getCurrentClass() << dec << endl;

  /*
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
  */
  
  {
    ostringstream oss;
    oss << theLogPrefix << "-maps.log";
    mapLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *mapLog << "Format: [seed]";
    for (const MkEvalAllocatorPair* ap: theAllocators) {
      *mapLog << " [" << ap->id << "]";
    }
    *mapLog << endl;
    *mapLog << "Utilisations:";
    double util = theGenUtilisation;
    nUtils = 0;
    while (util < theMaxUtilisation) {
      *mapLog << " " << util;
      util += theUtilisationStep;
      ++nUtils;
    }
    *mapLog << endl;
  }
  
 initialise_end:
  return success;
}


void cleanup() {
  if (econf)
    delete econf;
  delete gcfg;
  /*
  resultLog->close();
  delete resultLog;
  */
  mapLog->close();
  delete mapLog;
  delete theSimulation;
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
			    theUtilisationDeviation, theGenUtilisation);
    return ats;
  }
  /* 
  static size_t tsCount = 0;
  
  if (tsCount >= theNTasksets) {
    return NULL;
  }
  else {
    AbstractMkTaskset* ats =
      new AbstractMkTaskset(rand_r(&theSeed), theTasksetSize, gcfg,
			    theUtilisationDeviation, theGenUtilisation);
    ++tsCount;
    return ats;
  }
  */
}


DseResultSet* executeTaskset(AbstractMkTaskset* ats) {
  DseResultSet* rs = simulate(ats);
  delete ats;
  return rs;
}


DseResultSet* simulate(AbstractMkTaskset* ats) {
  //BdResultSet* rs = new BdResultSet(ats->getSeed(), nEvals);
  DseResultSet* drs = new DseResultSet(ats->getSeed(), nEvals, nUtils);

  double currentUtilisation = theGenUtilisation;

  ostringstream oss;
  oss << theLogPrefix << "-ts-" << ats->getSeed() << ".log";
  ofstream simLog(oss.str(), ios::out | ios::trunc);

  simLog << "TS: " << *ats << endl;
  simLog << "Hyperperiod: " << ats->getHyperPeriod() << endl;
  simLog << "FeasibilityMultiplicator: " << ats->getMkFeasibilityMultiplicator() << endl;

  unsigned ctr = 0;
  MkTaskset* lastMkts = NULL;

  ostringstream olog;
  
  while (currentUtilisation < theMaxUtilisation) {
    //assert(ctr < 8*sizeof(bitmap_t));
    ats->setUtilisation(currentUtilisation);
    double ur = ats->getRealUtilisation();
    double umk = ats->getMkUtilisation();

    simLog << endl;
    simLog << "Utilisation: " << currentUtilisation
	   << " U_r: " << ur
	   << " U_mk: " << umk
	   << endl;

    MkTaskset* mkts = ats->getMkTasks();
    if (lastMkts != NULL && *lastMkts == *mkts) {
      simLog << olog.str();
      for (const MkEvalAllocatorPair* ap: theAllocators) {
	size_t allocatorId = allocatorIdMap.at(ap->id);
	unsigned val = (*drs->data[allocatorId])[ctr - 1];
	(*drs->data[allocatorId])[ctr] = val;
      }
    }
    else {
      olog.str("");
      olog.clear();
      
      for (MkTask* t: mkts->tasks) {
	olog << "\t" << *t << endl;
      }
      list<string> failedScenarios;
      for (const MkEvalAllocatorPair* ap: theAllocators) {
	bool success;
	size_t allocatorId = allocatorIdMap.at(ap->id);
	
	list<MkTask*> tasks;
	for (MkTask* t: mkts->tasks) {
	  tasks.push_back(ap->taskAlloc(t));
	}
	
	MkExtTaskset* mkets = new MkExtTaskset(tasks, ap->schedAlloc(scc), ap->id);
	olog << ap->id << ":";
	
	if (umk <= 1.0) { // perform simulation only if necessary condition fulfiled
	  success = mkets->simulate();
	}
	else {
	  success = false;
	}
	
	if (success) {
	  
	  (*drs->data[allocatorId])[ctr] = 1;// |= 1 << (nUtils - ctr); //TODO!
	  
	  olog << " success";
	  if (mkets->stateRecurred())
	    olog << " due to recurring state";
	  
	  olog <<" after " << mkets->getHpCount()-1
	       << " hyperperiods in cycle "
	       << mkets->getSimulation()->getTime() << "."
	       << " (EC: " << const_cast<Simulation*>(mkets->getSimulation())->getResults().execCancellations << ")";
	  if (mkets->reducedStateRecurred()) {
	    olog << " [RS @ " << mkets->getReducedStateHyperPeriod()
		 << " / " << mkets->getReducedStateCycle() << "]";
	  }
	  olog << endl;
	  
	  if (theToFile) {
	    ostringstream oss;
	    oss << theXmlPrefix << "-" << currentUtilisation << "-" << ats->getSeed() << "-" << ap->id << ".xml";
	    
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
	else { // (!success)
	  failedScenarios.push_back(ap->id);
	  olog << " failed after " << mkets->getHpCount()-1
	       << " hyperperiods in cycle "
	       << mkets->getSimulation()->getTime() << "."
	       << " (EC: " << const_cast<Simulation*>(mkets->getSimulation())->getResults().execCancellations << ")"
	       << endl;
	}
	const CompressedMkState* states = mkets->getLastMkStates();
	if (states != NULL && umk <= 1.0) {
	  olog << "\t";
	  size_t i = 0;
	  for (MkTask* task: tasks) {
	    olog << " ";
	    printBitString(olog, states[i], task->getK());
	    ++i;
	  }
	  olog << endl;
	}
	delete mkets;
	/*// no need to delete these, simulation takes care of this
	  for (MkTask* t: tasks) {
	  delete t;
	  }
	*/
      }
      
      simLog << olog.str();
      failedScenarios.clear();
    }
    
    if (lastMkts != NULL)
      delete lastMkts;
    lastMkts = mkts;
    currentUtilisation += theUtilisationStep;
    ++ctr;
  }

  return drs;
}


void processResult(DseResultSet* drs) {
  /*
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
  */

  *mapLog << drs->seed;
  for (size_t i = 0; i < nEvals; ++i) {
    *mapLog << " " << drs->data[i]->str();
    //printBitString(*mapLog, drs->data[i], nUtils);
  }
  *mapLog << endl;
  delete drs;
}


