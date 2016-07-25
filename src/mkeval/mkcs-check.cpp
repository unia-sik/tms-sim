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
 * $Id: mkcs-check.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file runts.cpp
 * @brief Check the spin parameter calculation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/logger.h>
#include <utils/tlogger.h>
#include <utils/tmsmath.h>

//#include <mkeval/mkeval.h>
#include <mkeval/mkgenerator.h>

#include <taskmodels/mktask.h>

#include <xmlio/tasksetreader.h>
#include <xmlio/tasksetwriter.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;
using namespace tmssim;

#include <boost/program_options.hpp>
namespace po = boost::program_options; 


const int DEFAULT_SIMULATION_STEPS = 100;

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


void printTaskset();
TmsTime calcFeasibilityPeriod();
void performCalculation();


/// @name Program options
/// @{
po::options_description desc;
po::variables_map vm;
/// @brief input file (-i)
string theInputFile = "";
/// @brief configuration of logger
vector<string> poLog;
/// @}


/// @brief Input data
vector<MkTask*> mkTasks;


int main(int argc, char* argv[]) {
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

  printTaskset();
  cout << "FeasibilityPeriod: " << calcFeasibilityPeriod() << endl;
  cout << "Resetting spin..." << endl;
  size_t idx = 0;
  unsigned int spins[mkTasks.size()];
  for (MkTask* t: mkTasks) {
    spins[idx++] = t->getSpin();
    t->setSpin(0);
  }
  //printTaskset();

  bool sc = MkGenerator::testSufficientSchedulability(mkTasks);
  cout << "Sufficient Schedulability: " << sc << endl;

  performCalculation();

  cout << "After spin calculation:" << endl;
  printTaskset();

  bool spinChanged = false;
  for (idx = 0; idx < mkTasks.size(); ++idx) {
    if (mkTasks[idx]->getSpin() != spins[idx]) {
      spinChanged = true;
      break;
    }
  }
  cout << "Changed: " << spinChanged << endl;
  if (spinChanged) {
    ostringstream ost;
    ost << "adj-" << theInputFile;
    vector<Task*> tasks;
    for (MkTask* t: mkTasks) {
      tasks.push_back(t);
    }
    TasksetWriter* tsw = TasksetWriter::getInstance();
    bool result = tsw->write(ost.str(), tasks);
    if (!result) {
      tError() << "Writing task set failed!";
    }
    else {
      cout << "Changed taskset written to " << ost.str() << endl;
    }
  }
  if (vm.count("-2")) {
    cout << "Second calc!" << endl;
    performCalculation();
    cout << "After 2nd spin calculation:" << endl;
    printTaskset();
  }
}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("infile,i", po::value<string>(&theInputFile)->required(), "xml input file")
    (",2", "Perform 2nd calculation of spin values based on first results")
    (",l", po::value<vector<string>>(&poLog), "Activate execution logs, for valid options see below")
    ;
}


#define INITIALISE_FAIL success = false;	\
  goto initialise_end;

bool initialise() {
  cout << "Initialising program..." << endl;
  bool success = true;

  TasksetReader* reader = TasksetReader::getInstance();
  Taskset tmpTasks;
  if (reader->read(theInputFile, tmpTasks)) {
    for (Task* t: tmpTasks) {
      MkTask* mkt = dynamic_cast<MkTask*>(t);
      if (mkt != NULL) {
	mkTasks.push_back(mkt);
      }
      else {
	tError() << "Unable to convert task " << *t;
	INITIALISE_FAIL;
      }
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
  //cout << __FUNCTION__ << " not implemented yet" << endl;
  
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  /*
    TODO: nicer output, e.g. with output class from 
    http://www.radmangames.com/programming/how-to-use-boost-program_options
  */
  cout << desc << "\n";

  cout << "Valid parameters for the -l option are:";
  for (size_t i = 0; i < LOG_N_PREFIX; ++i) {
    cout << " " << tmssim::detail::LOG_PREFIX[i];
  }
  cout << endl;

  cout << "The -a, -l and -u options may be specified multiple times." << endl;

}


void printTaskset() {
  for (MkTask* t: mkTasks) {
    cout << *t << endl;
  }
}


void performCalculation() {
  cout << "Calculating spin values..." << endl;
  MkGenerator::calcRotationValues(mkTasks);
}


TmsTime calcFeasibilityPeriod() {
  TmsTime lcm = 1;
  for (MkTask* t: mkTasks) {
    lcm = calculateLcm(lcm, t->getPeriod() * t->getK());
  }
  return lcm;
}
