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
 * $Id: tasksetreader.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file tasksetreader.h
 * @brief Read task sets from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_TASKSETREADER_H
#define XMLIO_TASKSETREADER_H 1

/**
 * Includes
 */
#include <string>
#include <vector>
#include <libxml/parser.h>
#include <core/scobjects.h>

namespace tmssim {
  
  /**
   * TasksetReader reads sets of tasks from a XML-file.
   * TasksetReader is a Singleton-Class and at the end of your program,
   * you have to call the clear()-method to free the dynamically allocated
   * memory.
   */
  class TasksetReader {
    
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
    static TasksetReader* _instance;
    
    /**
     * Private Methods
     */
  private:
    /**
     * Creates a new instance of a TasksetReader with the default schema-file
     * (tasksets/taskset.xsd).
     */
    TasksetReader();
    
    /**
     * Creates a new instance of a TasksetReader with the given schema-file
     * @param schemafilename The xsd Schema File
     */
    TasksetReader(const std::string schemafilename);
    
    /**
     * Copy-Constructor. Creates a TasksetReader based on another.
     * @param otherReader The reader to copy
     */
    TasksetReader(const TasksetReader& otherReader);
    
    /**
     * Destroys the instance and frees the memory. Is called in the clear()-method.
     */
    ~TasksetReader();
    
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
	if (TasksetReader::_instance != NULL) {
	  delete TasksetReader::_instance;
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
    static TasksetReader* getInstance();
    
    /**
	 * Initializes the Singleton class with the given xml schemafile (.xsd).
	 * @param schemafilename The xml-schemafile (.xsd)
	 */
    void setSchema(const std::string schemafilename);
    
    /**
     * Reads in the given XML-file, will parse and validate the file and creates the appropriate Task-objects.
     * This will usually allocate memory, which can be released by calling clean().
     * @param filename The name of the taskset file.
     * @param[out] taskset Reference to the taskset vector, in which pointers to tasks will be stored.
     *             The caller must provide this object and care about its memory management.
	 * @return true, if the taskset was read successful, false otherwise.
	 */
    bool read(const std::string& filename, std::vector<Task*>& taskset);
  };

} // NS tmssim

#endif /* !XMLIO_TASKSETREADER_H */
