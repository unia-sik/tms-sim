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
 * $Id: genseeds.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file genseeds.cpp
 * @brief Generate seed files for more parallel evals
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <utils/tlogger.h>

#include <boost/program_options.hpp>

using namespace std;
using namespace tmssim;
namespace po = boost::program_options;


const unsigned long DEFAULT_N_SEEDS = 100;
const unsigned long DEFAULT_N_FILES = 1;
const string DEFAULT_FILE_PREFIX = "seeds";

/**
 * @brief Initialise the program_options object
 */
void initialiseProgramOptions();

/**
 * @brief Initialise the program variables from the program_options
 */
bool initialise();

/**
 * @brief Cleanup all global memory
 */
void cleanup();

/**
 * @brief Print the usage message that describes all parameters
 */
void printUsage(char* prog);

/**
 * @brief Prepare output files
 */
void prepareFiles();

/**
 * @brief Actual seed generation
 */
void generateSeeds();

/**
 * @name Program parameters
 * @{
 */

po::options_description desc;
po::variables_map vm;

/// @brief seed from command line
unsigned poSeed;
/// @brief actual seed
unsigned theSeed;
/// @brief Number of seeds to generate
unsigned theNSeeds;
/// @brief Number of seed files
unsigned theNFiles;
/// @brief Seed file prefix
string theFilePrefix;

/**
 * @}
 */


ofstream* outfiles = NULL;

int main(int argc, char* argv[]) {
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
    return 1;
  }
  
  if (!initialise()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return 1;
  }

  cout << "== INFO == " << "\ttheSeed: " << theSeed << endl;
  cout << "== INFO == " << "\ttheNSeeds: " << theNSeeds << endl;
  cout << "== INFO == " << "\ttheNFiles: " << theNFiles << endl;
  cout << "== INFO == " << "\ttheFilePrefix: " << theFilePrefix << endl;

  prepareFiles();
  generateSeeds();
  cleanup();
  
  return 0;
}


void initialiseProgramOptions() {  
  desc.add_options()
    ("help,h", "produce help message")
    ("seed,S", po::value<unsigned>(&poSeed), "Seed [default=time(NULL)]")
    (",n", po::value<unsigned>(&theNSeeds)->default_value(DEFAULT_N_SEEDS), "Number of seed values")
    (",f", po::value<unsigned>(&theNFiles)->default_value(DEFAULT_N_FILES), "Number of seed files")
    ("prefix,p", po::value<string>(&theFilePrefix)->default_value(DEFAULT_FILE_PREFIX), "File prefix (if a file already exists, it will be overwritten!)")
    ;
}


void printUsage(char* prog) {
  cout << "Usage: " << prog << " [arguments] \nArguments:" << endl;
  cout << desc << "\n";
}

/*
#define INITIALISE_FAIL success = false;	\
  goto initialise_end;
*/

bool initialise() {
  cout << "Initialising program..." << endl;
  bool success = true;

  // seeds
  if (vm.count("seed")) {
    theSeed = poSeed;
  }
  else {
    theSeed = time(NULL);
  }

  
  //initialise_end:
  return success;
}


void cleanup() {
  for (unsigned i = 0; i < theNFiles; ++i) {
    outfiles[i].close();
  }
  delete[] outfiles;
}


void prepareFiles() {
  outfiles = new ofstream[theNFiles];
  for (unsigned i = 0; i < theNFiles; ++i) {
    ostringstream oss;
    oss << theFilePrefix << "-" << i << ".txt";
    outfiles[i].open(oss.str(), ios::out | ios::trunc);
  }
}


void generateSeeds() {
  unsigned actSeed = theSeed;    
  for (unsigned i = 0; i < theNSeeds; ++i) {
    outfiles[i % theNFiles] << rand_r(&actSeed) << endl;
  }
}
