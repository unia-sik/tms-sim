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
 * $Id: iwriteabletoxml.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file iwriteabletoxml.h
 * @brief Interface to allow objects to serialize themselves into a XML file
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef IWRITEABLETOXML_H
#define IWRITEABLETOXML_H 1

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

namespace tmssim {

  /**
   * Interface to allow objects to serialize themselves into a XML file
   */
  class IWriteableToXML {
    
  public:
    
    //IWriteableToXML() {}
    
    /**
     * Write an object to a xml file by using the given xmlTextWriter
     * @param writer Pointer to the xmlTextWriter that sould be used for serz
     */
    virtual void write(xmlTextWriterPtr writer) const = 0;
    
    /**
     * D'tor
     */
    virtual ~IWriteableToXML() {}
  };

} // NS tmssim

#endif /* IWRITEABLETOXML_H */
