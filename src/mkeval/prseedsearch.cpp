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
 * $Id: prseedsearch.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkdse.cpp
 * @brief Extensive DSE (check all possible task sets)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */


#include <mkeval/abstractmktaskset.h>
#include <mkeval/mkglobals.h>

#include <mkeval/periodgenerator.h>
#include <mkeval/gmperiodgenerator.h>

#include <utils/bitstrings.h>
#include <utils/random.h>
#include <utils/tlogger.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <set>
#include <thread>

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


void startThreads();
void joinThreads();

void workerThread(size_t tid);
void signalHandler(int signo);


/// @name Program options
/// @todo remove simple copies
/// @{
po::options_description desc;
po::variables_map vm;

/// @brief path to config file
string poConfigFile;
/// @brief seed parameter
unsigned poSeed;
/// @brief taskset size
unsigned theTasksetSize;
/// @brief how many taskset to generate
unsigned theNTasksets;
/// @brief utilisation for task set generation (will also be start utilisation)
double theGenUtilisation;
/// @brief maximum allowed deviation from generation utilisation
double theUtilisationDeviation;
/// @brief number of simulation threads parameter (-m)
unsigned theNThreads;

string theOutputFile;
/// @}

/// @name Actual parameters (only for those that need preprocessing)
/// @{
/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;

/// @brief Seed used for taskset generation
unsigned int theSeed;

/// @}


/// @name Execution and results
/// @{

thread* workers;
size_t* seedsFound;
size_t* tids;

Random myRandom;
mutex seedLock;

set<unsigned> seeds;
ofstream* seedFile;
size_t nSeeds = 0;
mutex outLock;

/// @}




int main(int argc, char* argv[]) {
  
  // INIT
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
  cout << "==INFO== " << "\ttheSeed: " <<	theSeed << endl;
  cout << "==INFO== " << "\ttheTasksetSize: " << theTasksetSize << endl;	
  cout << "==INFO== " << "\ttheNTasksets: " << theNTasksets << endl;		
  cout << "==INFO== " << "\ttheGenUtilisation: " << theGenUtilisation << endl;
  cout << "==INFO== " << "\ttheUtilisationDeviation: " << theUtilisationDeviation << endl;
  cout << "==INFO== " << "\ttheNThreads: " << theNThreads << endl;	
  cout << endl;


  startThreads();
  
  // setup signal handler
  if (signal(SIGUSR1, signalHandler) == SIG_ERR) {
    cerr << "Cannot register handler" << endl;
  }

  joinThreads();
  
  cleanup();
  
  return 0;
}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&poConfigFile)->required(), "KvFile with settings for the task set generator (mandatory)")
    ("seed,S", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    (",t", po::value<unsigned>(&theTasksetSize)->default_value(5), "TASKSETSIZE")
    (",T", po::value<unsigned>(&theNTasksets)->default_value(100), "NTASKSETS")
    ("gen-utilisatione,U", po::value<double>(&theGenUtilisation)->required(), "Utilisation for initial task set")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(0.1), "Deviation of actual utilisation for minimum utilisation")
    (",m", po::value<unsigned>(&theNThreads)->default_value(thread::hardware_concurrency()), "Simulation threads")
    (",o", po::value<string>(&theOutputFile)->required(), "Where to write the seeds")
    ;
}

void printUsage(char* prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  cout << desc << "\n";
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

  // seeds
  if (vm.count("seed")) {
    theSeed = poSeed;
  }
  else {
    theSeed = time(NULL);
  }
  myRandom = Random(theSeed);

  seedFile = new ofstream(theOutputFile, ios::out | ios::trunc);
  
 initialise_end:
  return success;
}


void startThreads() {
  workers = new thread[theNThreads];
  seedsFound = new size_t[theNThreads];
  tids = new size_t[theNThreads];
  for (size_t i = 0; i < theNThreads; ++i) {
    tids[i] = i;
    seedsFound[i] = 0;
    workers[i] = thread(workerThread, tids[i]);
  }
}


void joinThreads() {
  for (size_t i = 0; i < theNThreads; ++i) {
    workers[i].join();
  }
}


void cleanup() {

  delete[] workers;
  delete[] seedsFound;
  delete[] tids;
  
  seedFile->close();
  delete seedFile;
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}


void workerThread(size_t tid) {
  GmPeriodGenerator myPeriodGenerator(GmPeriodGenerator::HIGH);
  bool finished = false;
  
  while (!finished) {
    unsigned seed = 0;
    {
      unique_lock<mutex> lck(seedLock);
      seed = myRandom.getNumber();
    }
    
    
    AbstractMkTaskset* ats =
      new AbstractMkTaskset(seed, theTasksetSize, gcfg,
			    theUtilisationDeviation, theGenUtilisation,
			    &myPeriodGenerator);
    
    unsigned tss = ats->getSeed();
    {
      unique_lock<mutex> lcs(outLock);
      if (seeds.find(tss) == seeds.end()) {
	// record!
	seeds.insert(tss);
	++nSeeds;
	++seedsFound[tid];
	*seedFile << tss << endl;
	if (nSeeds >= theNTasksets) {
	  finished = true;
	}
      }
    }
  }
  tids[tid] = -1;
}


void signalHandler(int signo) {
  //cout << "Received signal " << signo << endl;
  if (signo != SIGUSR1) {
    cerr << "Not configured to handle signal " << signo << endl;
  }
  for (size_t i = 0; i < theNThreads; ++i) {
    cout << "T" << i << ": " << seedsFound[i]
	 << " (" << (tids[i] == i) << ")"
	 << endl;
  }
  cout << endl;
}


