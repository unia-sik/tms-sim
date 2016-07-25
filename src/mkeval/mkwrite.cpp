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
 * $Id: mkwrite.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mkwrite.cpp
 * @brief Write (m,k)-tasksets to XML.
 * Do not use this program, as it seems to produce an output that seems to be
 * different from the tasksets used by mkrun.
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkgenerator.h>
#ifndef TLOGLEVEL
#define TLOGLEVEL TLL_DEBUG
#endif
#include <utils/tlogger.h>
#include <utils/kvfile.h>
#include <xmlio/tasksetwriter.h>

#include <cstring>


using namespace std;
using namespace tmssim;

#include <boost/program_options.hpp>
namespace po = boost::program_options; 


/**
 * @brief Initialise the program_options object
 */
void initialiseProgramOptions();

/**
 * @brief Initialise the program variables from the program_options
 */
bool initialise();

/**
 * Print the usage message that describes all parameters
 */
void printUsage(char* prog);

void cleanup();

/**
 * @brief Check wether the config file contains all relevant data
 */
bool checkGCfg(void);


/// @name Program options
/// @{
po::options_description desc;
po::variables_map vm;
/// @brief path to config file
string poConfigFile;
/// @brief seed parameter
unsigned poSeed;
/// @brief taskset size
unsigned theTasksetSize;
/// @brief target utilisation
double theUtilisation;
/// @brief maximum allowed deviation from generation utilisation
double theUtilisationDeviation;
/// @ brief Output file
string theOutput;
/// @}


/// @name Actual parameters
/// @{

const int DEFAULT_TASKSET_SIZE = 5;
const double DEFAULT_UTILISATION = 1.0;
const double DEFAULT_UTILISATION_DEVIATION = 0.1;

/// @brief Seed used for taskset generation
unsigned int theSeed;

/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;

/// @}


/// The tasks, stored globally for easier cleanup
Taskset* tasks = NULL;


int main(int argc, char *argv[]) {

  cerr << "THIS PROGRAM SEEMS TO PRODUCE TASK SETS THAT ARE DIFFERENT FROM THE ONES USED BY MKRUN. USE mkrun -o TO GET THE REAL TASK SET" << endl;
  
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

  // Everything's fine, so create the task set
  tDebug() << "Using seed " << theSeed;
  MkGenerator generator(theSeed, theTasksetSize,
			gcfg->getUInt32("minPeriod"),
			gcfg->getUInt32("maxPeriod"),
			gcfg->getUInt32("minK"),
			gcfg->getUInt32("maxK"),
			theUtilisation, theUtilisationDeviation,
			gcfg->getUInt32("maxWC"));
  
  MkTaskset* ts = generator.nextTaskset();
  vector<Task*> tasks;
  for (MkTask* t: ts->tasks) {
    tasks.push_back(t);
  }
  TasksetWriter* tsw = TasksetWriter::getInstance();
  bool result = tsw->write(theOutput, tasks);
  //TasksetWriter::clear();
  if (!result) {
    tError() << "Writing task set failed!";
  }
  
  cleanup();
  return 0;
}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&poConfigFile)->required(), "KvFile with settings for the task set generator (mandatory)")
    ("outfile,o", po::value<string>(&theOutput)->required(), "xml output file, if the file exists it will be truncated (mandatory)")
    ("seed,s", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    (",t", po::value<unsigned>(&theTasksetSize)->default_value(DEFAULT_TASKSET_SIZE), "Taskset size")
    (",u", po::value<double>(&theUtilisation)->default_value(DEFAULT_UTILISATION), "Target utilisation")
    (",d", po::value<double>(&theUtilisationDeviation)->default_value(DEFAULT_UTILISATION_DEVIATION), "Deviation of actual utilisation for target utilisation")
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

  // seed
  if (vm.count("seed")) {
    theSeed = poSeed;
  }
  else {
    theSeed = time(NULL);
  }

 initialise_end:
  return success;
}


void printUsage(char* prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  cout << desc << "\n";
}


void cleanup() {
  if (gcfg != NULL) {
    delete gcfg;
  }
  if (tasks != NULL) {
    for (Task* t: *tasks) {
      delete t;
    }
    delete tasks;
  }
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}
