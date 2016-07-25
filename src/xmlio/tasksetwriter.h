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
 */

/**
 * $Id: tasksetwriter.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file tasksetwriter.h
 * @brief Writing of task sets
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_TASKSETWRITER_H
#define XMLIO_TASKSETWRITER_H 1

/**
 * Includes
 */
#include <string>
#include <libxml/parser.h>
#include <core/scobjects.h>
#include <vector>

/**
 * Constants
 */
//#define TASKSETWRITER_DEFAULT_SCHEMA "tasksets/taskset.xsd"
//#define TASKSETWRITER_DEFAULT_SCHEMA "share/tms-sim/xsd/taskset.xsd"

namespace tmssim {

/**
 * TasksetReader reads sets of tasks from a XML-file.
 * TasksetReader is a Singleton-Class and at the end of your program,
 * you have to call the clear()-method to free the dynamically allocated
 * memory.
 */
class TasksetWriter {
  
  /**
   * Member
   */
 private:
  /**
   * The schema (.xsd)-file that the taskset-XML files will be checked against.
   */
  std::string _schemafilename;
  
  /**
   * The singleton instance
   */
  static TasksetWriter* _instance;
  
  /**
   * Private Methods
   */
 private:
  /**
   * Creates a new instance of a TasksetReader with the default schema-file
   * (tasksets/taskset.xsd).
   */
  TasksetWriter();
  
  /**
   * Creates a new instance of a TasksetReader with the given schema-file
   * @param schemafilename The xsd Schema File
   */
  TasksetWriter(const std::string schemafilename);
  
  /**
   * Copy-Constructor. Creates a TasksetReader based on another.
   * @param otherReader The reader to copy
   */
  TasksetWriter(const TasksetWriter& otherWriter);
  
  /**
   * Destroys the instance and frees the memory. Is called in the clear()-method.
   */
  ~TasksetWriter();
  
  /**
   * Checks if the given doc-object matches the structure and specifications in the schemafile.
   * @param doc The read in xml file.
   * @return true, if the file is valid, otherwise false
   */
  bool isValid(const xmlDocPtr doc) const;
  
  /**
   * Guard for cleaning up memory
   * idea plucked from http://www.oop-trainer.de/Themen/Singleton.html
   */
  class Guard {
  public:
    ~Guard() {
      if (TasksetWriter::_instance != NULL) {
	delete TasksetWriter::_instance;
      }
    }
  };
  friend class Guard;
  
  
  /**
   * Public Methods
   */
 public:
  
  
  /**
   * Gets an instance of the class. If there is already one, you will get that.
   * @return an instance of the class.
   */
  static TasksetWriter* getInstance();
  
  /**
   * Initializes the Singleton class with the given xml schemafile (.xsd).
   * @param schemafilename The xml-schemafile (.xsd)
   */
  void setSchema(const std::string schemafilename);
  
  /**
   * Writes the given taskset to the given xml-file
   * @param filename The filename where to put the xml-representations of the taskset
   * @param taskset The taskset that should be written (must implement IWriteableToXML-Interface)
   * @return true, if the written document is valid, false on errors or if the written file is invalid with the xml-schema
   */
  //bool write(const std::string& filename, std::vector<Task*>& taskset) const;
  
  bool write(const std::string& filename, std::vector<Task*>& taskset) const;
};
 
} // NS tmssim

#endif /* !XMLIO_TASKSETWRITER_H */
