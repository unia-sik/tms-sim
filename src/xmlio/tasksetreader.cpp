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
 * $Id: tasksetreader.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file tasksetreader.cpp
 * @brief Read task sets from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <vector>
#include <sstream>

//#define TLOGLEVEL TLL_DEBUG
#include <utils/tlogger.h>

#include <xmlio/tasksetreader.h>
#include <xmlio/ielementfactory.h>
#include <xmlio/taskfactory.h>


using namespace std;

namespace tmssim {

  /**
 * Constants
 */
  //#define TASKSETREADER_DEFAULT_SCHEMA "tasksets/taskset.xsd"
#define TASKSETREADER_DEFAULT_SCHEMA XSD_INSTALL_DIR "/taskset.xsd"
// FIXME: this method will probably not work on Gentoo Linux where
// portage first installs into the sandbox

  //#define MK_SCHEMA_PATH(base, file) base  file


  /**
   * Initialize Singleton
   */
  TasksetReader* TasksetReader::_instance = 0;
  
  TasksetReader::TasksetReader() :
    _schemafilename(TASKSETREADER_DEFAULT_SCHEMA) {
    //_schemafilename(MK_SCHEMA_PATH(XSD_DIR, TASKSETREADER_DEFAULT_SCHEMA)) {
    tDebug() << "TasksetReader default c'tor called";
  }
  
  TasksetReader::TasksetReader(const std::string schemafilename) :
    _schemafilename(schemafilename) {
    tDebug() << "TasksetReader custom c'tor called";
  }
  
  TasksetReader::TasksetReader(const TasksetReader& otherReader) {
    this->_schemafilename = otherReader._schemafilename;
  }
  
  TasksetReader::~TasksetReader() {
    tDebug() << "TasksetReader d'tor called";
  }
  
  TasksetReader* TasksetReader::getInstance() {
    static TasksetReader::Guard guard;
    if (_instance == NULL) {
      _instance = new TasksetReader();
    }
    return _instance;
  }
  
  void TasksetReader::setSchema(const std::string schemafilename) {
    this->_schemafilename = schemafilename;
  }

  /*
  void TasksetReader::clear() {
    if(_instance != NULL) {
      delete _instance;
    }
  }
  */
  
  bool TasksetReader::isValid(const xmlDocPtr doc) const {
    xmlDocPtr schema_doc = xmlReadFile(this->_schemafilename.data(), NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
      // the schema cannot be loaded or is not well-formed
      return -1;
    }
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
      // unable to create a parser context for the schema
      xmlFreeDoc(schema_doc);
      return -2;
    }
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
      // the schema itself is not valid
      xmlSchemaFreeParserCtxt(parser_ctxt);
      xmlFreeDoc(schema_doc);
      return -3;
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
      // unable to create a validation context for the schema
      xmlSchemaFree(schema);
      xmlSchemaFreeParserCtxt(parser_ctxt);
      xmlFreeDoc(schema_doc);
      return -4;
    }
    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    // force the return value to be non-negative on success
	return is_valid ? 1 : 0;
  }
  
  // Note that taskset is an output parameter
  bool TasksetReader::read(const std::string& filename, vector<Task*>& taskset) {
    tDebug() << "Reading Taskset out of XML file: " << filename;
    
    xmlDocPtr doc;
    xmlNodePtr cur;
    doc = xmlParseFile(filename.c_str());
    
    if (doc == NULL) {
      tError() << "Document not parsed successfully.";
      return false;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
      tError() << "Empty document.";
      xmlFreeDoc(doc);
      return false;
    }
    if (xmlStrcmp(cur->name, (const xmlChar *) "taskset")) {
      tError() << "Document of the wrong type, root node != taskset";
      xmlFreeDoc(doc);
      return false;
    }
    
    if (isValid(doc) > 0) {
      tDebug() << "Document is valid";
      
      // Factory bauen
      TaskFactory fact;
      
      cur = cur->xmlChildrenNode;
      while (cur != NULL) {
	if(fact.accept(doc,cur)) {
	  Task* taskPtr = fact.getElement(doc, cur);
	  if (taskPtr != NULL) {
	    taskset.push_back(taskPtr);
	  }
	}
	cur = cur->next;
      }
      
    } else {
      tError() << "Document is invalid";
      xmlFreeDoc(doc);
      return false;
    }
    
    xmlFreeDoc(doc);
    
    // Debug output
    //tDebug() << "Document is valid";
    tDebug() << "Number of tasks: " << taskset.size();
    
    return true;
    
  }
  
} // NS tmssim
