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
 * $Id: tasksetwriter.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file tasksetwriter.cpp
 * @brief Write task sets to XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <typeinfo>

#include <iostream>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <vector>
#include <sstream>

#include <utils/tlogger.h>

#include <xmlio/tasksetwriter.h>
#include <xmlio/ielementfactory.h>
#include <xmlio/taskfactory.h>


using namespace std;

namespace tmssim {
#define TASKSETWRITER_DEFAULT_SCHEMA XSD_INSTALL_DIR "/taskset.xsd"

  //#define TASKSETWRITER_DEFAULT_SCHEMA "tasksets/taskset.xsd"
#define TASKSETWRITER_DEFAULT_SCHEMA XSD_INSTALL_DIR "/taskset.xsd"
// FIXME: this method will probably not work on Gentoo Linux where
// portage first installs into the sandbox

  /**
   * Initialize Singleton
   */
  TasksetWriter* TasksetWriter::_instance = 0;
  
  TasksetWriter::TasksetWriter() :
    _schemafilename(TASKSETWRITER_DEFAULT_SCHEMA) {
    tDebug() << "TasksetWriter default c'tor called";
  }
  
  TasksetWriter::TasksetWriter(const std::string schemafilename) :
    _schemafilename(schemafilename) {
    tDebug() << "TasksetWriter custom c'tor called";
  }
  
  TasksetWriter::TasksetWriter(const TasksetWriter& otherWriter) {
    this->_schemafilename = otherWriter._schemafilename;
  }
  
  TasksetWriter::~TasksetWriter() {
    tDebug() << "TasksetWriter d'tor called";
  }
  
  TasksetWriter* TasksetWriter::getInstance() {
    static TasksetWriter::Guard guard;
    if (_instance == NULL) {
      _instance = new TasksetWriter();
    }
    return _instance;
  }

  /*
  void TasksetWriter::clear() {
    if(_instance != NULL) {
      delete _instance;
    }
  }
  */
  
  void TasksetWriter::setSchema(const std::string schemafilename) {
    this->_schemafilename = schemafilename;
  }
  
  bool TasksetWriter::isValid(const xmlDocPtr doc) const {
    xmlDocPtr schema_doc = xmlReadFile(this->_schemafilename.c_str(), NULL, XML_PARSE_PEDANTIC);
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
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt); // TODO: cleanup memory!
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
  
  bool TasksetWriter::write(const std::string& filename, vector<Task*>& taskset) const {
    xmlDocPtr doc;
    tDebug() << "Writing Taskset to XML file: " << filename;
    
    xmlTextWriterPtr writer;
    writer = xmlNewTextWriterDoc(&doc, 0); // TODO: cleanup
    // actually, cleanup IS performed below, but it seems this fails (according to valgrind)
    xmlTextWriterSetIndent(writer, 1);
    if (xmlTextWriterSetIndentString(writer, (const xmlChar*) "  ") != 0) {
      tError() << "Error on settind indenting!";
    }
    
    xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
    
    xmlTextWriterWriteComment(writer, (xmlChar*) "Here come the tasks");
    
    xmlTextWriterStartElement(writer, (xmlChar*) "taskset");
    xmlTextWriterWriteAttributeNS(writer, (xmlChar*) "xsi", (xmlChar*) "schemaLocation", (xmlChar*) "http://www.w3.org/2001/XMLSchema-instance", (xmlChar*) "http://www.tmsxmlns.com taskset.xsd");
    xmlTextWriterWriteAttribute(writer, (xmlChar*) "xmlns", (xmlChar*) "http://www.tmsxmlns.com");
    
    xmlTextWriterWriteRaw(writer, (xmlChar*) "\n");
    
    for (size_t i = 0; i < taskset.size(); i++) {
      xmlTextWriterWriteRaw(writer, (xmlChar*) "\n");
      //taskset[i]->write(writer);
      taskset[i]->writeToXML(writer);
      xmlTextWriterWriteRaw(writer, (xmlChar*) "\n");
    }
    
    xmlTextWriterEndElement(writer); // close Taskset
    
    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
    xmlSaveFile(filename.c_str(), doc);
    
    xmlNodePtr cur = xmlDocGetRootElement(doc);
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
      tDebug() << "Written document is valid";
      
    } else {
      tError() << "Written document is invalid";
      xmlFreeDoc(doc);
      return false;
    }
    
    xmlFreeDoc(doc);
    //xmlFreeTextWriter(writer);
    
    return true;
    
  }
  
} // NS tmssim
