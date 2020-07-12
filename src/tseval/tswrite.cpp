/**
 * $Id: tswrite.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file tswrite.cpp
 * @brief Write taskset from seed to XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */


#include <tseval/tstaskset.h>

#include <utils/logger.h>
#ifndef TLOGLEVEL
#define TLOGLEVEL TLL_DEBUG
#endif
#include <utils/tlogger.h>
#include <xmlio/tasksetwriter.h>

#include <cstring>

using namespace std;
using namespace tmssim;


bool parseArgs(int argc, char *argv[]);
void printUsage(char* prog);
bool init();
void cleanup();


/// @name Command line parameters
/// @{

/// @brief generator configuration file parameter (-c)
string parmCfgFile = "";
bool haveCfgFile = false;
/// @brief output file parameter (-o)
string parmOutput = "";
bool haveOutput = false;
/// @brief seed parameter (-s)
unsigned int parmSeed = 0;
bool haveSeed = false;
/// @}


/// @name Actual parameters
/// @{

/// @brief Seed used for taskset generation
unsigned int theSeed;
/// @brief internal representation of generator configuration file
KvFile* gcfg = NULL;
/*
/// @brief output stream
ofstream* outFile = NULL;
*/
/// @}


/// The tasks, stored globally for easier cleanup
Taskset* tasks = NULL;


int main(int argc, char *argv[]) {
  //testDependencyMatrix();
  //logger::setLevel(-1);
  logger::setClass(0);

  int rv = 0;
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
    rv = -1;
  }

  if (rv == 0) {
    // Everything's fine, so create the task set
    tDebug() << "Using seed " << theSeed;
    TsTaskSet ts(*gcfg, theSeed, NULL);
    tasks = ts.getSimTasks();
    TasksetWriter* tsw = TasksetWriter::getInstance();
    bool result = tsw->write(parmOutput, *tasks);
    //TasksetWriter::clear();
    if (!result) {
      tError() << "Writing task set failed!";
    }
  }


  cleanup();
  return rv;
}


void printUsage(char* prog) {
  //cout << "Job: " << sizeof(Job) << endl;

  cout << "Usage: " << prog << " [arguments] \n\
  Arguments:\n\
  -c CFGFILE KvFile with settings for the task set generator (mandatory)\n\
  -s SEED [default=time(NULL)]\n\
  -o FILE xml output file, if the file exists it will be truncated (mandatory)\n\
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
	/*
      case 'f':
	logptr = new ofstream(str, ios::out | ios::trunc);
	logSingle = logptr;
	break;
	*/
	/*
      case 'm': // max. number of threads
	if (str == NULL) {
	  tError() << "You need to specify the number of threads!";
	  goto failure;
	} else {
	  parmNThreads = atoi(str);
	  haveNThreads = true;
	}
	break;
	*/
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
      case 'o': // Output file
	if (str == NULL) {
	  tError() << "Output file parameter given, but no file specified!";
	  goto failure;
	} else {
	  parmOutput = str;
	  haveOutput = true;
	}
	break;
	
	/*
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
	*/
	
      case 's': // Seed
	if (str == NULL) {
	  tError() << "You need to specify a seed!";
	  goto failure;
	} else {
	  parmSeed = atoi(str);
	  cout << "Found seed \"" << str << "\", as int: " << parmSeed << endl;
	  haveSeed = true;
	}
	break;

	/*
      case 'S': // Single run to stdout
	parmSingle = true;
	--p;
	break;
	*/
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
	/*
      case 'T': // Number of task sets
	if (str == NULL) {
	  tError() << "You need to specify a valid number of task sets!";
	  goto failure;
	} else {
	  parmNTaskSets = atoi(str);
	  haveNTaskSets = true;
	}
	break;
	*/
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
  if (!haveOutput) {
    tError() << "You must specify an output file for the task set!";
    rv = -1;
  }
 failure:
  return rv;
}


#define INIT_FAIL success = false;		\
  goto init_end;


bool init() {
  bool success = true;

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

  // output file
  if (!haveOutput) {
    // already checked in parseArgs
  }
  else {
    // filename is directly passed to xml lib
  }
  
  // seed
  if (haveSeed) {
    theSeed = parmSeed;
  }
  else {
    theSeed = time(NULL);
  }

 init_end:
  return success;
}


void cleanup() {
  if (gcfg != NULL) {
    delete gcfg;
  }
  /*
  if (outFile != NULL) {
    outFile->close();
    delete outFile;
  }
  */
  if (tasks != NULL) {
    for (Task* t: *tasks) {
      delete t;
    }
    delete tasks;
  }
}
