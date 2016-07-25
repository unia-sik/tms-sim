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
 * $Id: writeabletoxml.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file writeabletoxml.h
 * @brief Abstract class for serialising objects to XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_WRITEABLETOXML_H
#define CORE_WRITEABLETOXML_H 1

#include <string>
#include <libxml/xmlwriter.h>

#include <xmlio/xmlutils.h> // include for convenience - needed only in cpp

namespace tmssim {

  /**
   * @brief Generic abstract class for serialising objects to XML.
   *
   * This class defines an interface that you should implement if you
   * want your classes to be serialisable to XML.
   */
  class WriteableToXML {
  public:
    
    /**
     * D'tor
     */
    virtual ~WriteableToXML();

    /**
     * @brief This method manages writing an object to XML.
     *
     * Call this method to write your object to an XML file.
     * @param writer Pointer to the xmlTextWriter that sould be used
     * for serialisation
     * @return
     */
    int writeToXML(xmlTextWriterPtr writer);


    /**
     * @brief Get the class's XML element name.
     *
     * Overwrite this method in any subclass you want to be able to
     * serialise to XML.
     * @return The class's XML element name.
     */
    virtual const std::string& getClassElement() = 0;


    /**
     * @brief Write the element's data to XML.
     *
     * Implement this function in any subclass you want to serialise.
     * The implementation must only write the subclass' data to XML,
     * superclass data is written by additionally calling the
     * the superclass's writeData method. Do this before you write any other
     * data to XML! Make sure you do this in any
     * class not directly derived from WritableToXML.
     * @param writer Pointer to the xmlTextWriter that sould be used
     * for serialisation
     * @return
     */
    virtual int writeData(xmlTextWriterPtr writer) = 0;



  };

} // NS tmssim

#endif // !CORE_WRITEABLETOXML_H
