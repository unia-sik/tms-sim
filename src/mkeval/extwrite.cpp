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
 * $Id: extwrite.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file extwrite.cpp
 * @brief Write taskset XML for a single ext mkeval (abstract task set
 * with one specified utilisation)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/logger.h>
#include <utils/tlogger.h>
#include <utils/kvfile.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <fstream>
#include <sstream>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <mkeval/mkallocators.h>
#include <mkeval/abstractmktaskset.h>
#include <mkeval/utilisationstatistics.h>

#include <schedulers/schedulers.h>

#include <taskmodels/mktask.h>
#include <taskmodels/mkptask.h>
#include <taskmodels/dbptask.h>

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


/// @name Program options
/// @todo remove simple copies
/// @{
po::options_description desc;
po::variables_map vm;

string poConfigFile;
unsigned poSeed;
unsigned poTasksetSize;
double poGenUtilisation;
double poRunUtilisation;
double poUtilisationDeviation;
string poOutput;
/// @}


/// @name Actual parameters
/// @{
/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;
/// @brief Seed used for taskset generation
unsigned int theSeed;
unsigned theTasksetSize;
/// @brief Task set target utilisation
double theGenUtilisation;
double theRunUtilisation;
double theUtilisationDeviation;
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
    cerr << "ERROR: " << e.what() << endl;
    cerr << "Use '--help' for further information" << endl;
    return -1;
  }
  
  if (!initialise()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }
  
  cout << "==INFO== " << "Initialising program options with boost succeeded:" << endl;
  //cout << "\ttheConfigFile: " << theConfigFile << endl;		
  cout << "==INFO== " << "\ttheSeed: " << theSeed << endl;		
  cout << "==INFO== " << "\ttheTasksetSize: " << theTasksetSize << endl;		
  cout << "==INFO== " << "\ttheGenUtilisation: " << theGenUtilisation << endl;
  cout << "==INFO== " << "\ttheRunUtilisation: " << theRunUtilisation << endl;
  cout << "==INFO== " << "\ttheUtilisationDeviation: " << theUtilisationDeviation << endl;
  cout << endl;

  AbstractMkTaskset* ats= new AbstractMkTaskset(theSeed, theTasksetSize, gcfg,
						theUtilisationDeviation, theGenUtilisation);

  ats->setUtilisation(theRunUtilisation);
  MkTaskset* mkts = ats->getMkTasks();

  vector<Task*> tasks;
  for (MkTask* t: mkts->tasks) {
    tasks.push_back(t);
  }
  TasksetWriter* tsw = TasksetWriter::getInstance();
  bool result = tsw->write(poOutput, tasks);
  //TasksetWriter::clear();
  if (!result) {
    tError() << "Writing task set failed!";
  }

  delete ats;

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
    ("seed,S", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    (",t", po::value<unsigned>(&poTasksetSize)->default_value(5), "TASKSETSIZE")
    (",U", po::value<double>(&poGenUtilisation)->required(), "Utilisation for task set generation")
    (",u", po::value<double>(&poRunUtilisation)->required(), "Utilisation for execution")
    (",d", po::value<double>(&poUtilisationDeviation)->default_value(0.1), "Deviation of actual utilisation for minimum utilisation")
    (",o", po::value<string>(&poOutput)->required(), "Where to write the task set")
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

  // seed
  if (vm.count("seed")) {
    theSeed = poSeed;
  }
  else {
    theSeed = time(NULL);
  }

  // taskset size
  theTasksetSize = poTasksetSize;

  // utilisations
  if (poRunUtilisation < poGenUtilisation) {
    tError() << "Run utilisation must be >= genUtilisation!";
    INITIALISE_FAIL;
  }
  else {
    theGenUtilisation = poGenUtilisation;
    theRunUtilisation = poRunUtilisation;
  }

  // utilisation deviation
  theUtilisationDeviation = poUtilisationDeviation;

  

 initialise_end:
  return success;
}


void cleanup() {
  delete gcfg;
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}

