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
 * $Id: ielementfactory.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file ielementfactory.h
 * @brief Factory interface
 * @authors Peter ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_IELEMENTFACTORY_H
#define XMLIO_IELEMENTFACTORY_H 1

/**
 * Includes
 */
#include <libxml/parser.h>

namespace tmssim {

  /**
   * Template class the construct objects out of an XML-document
   */
  template <class T>
    class IElementFactory {
    
  public:
    /**
     * Gets a pointer to an element out of the factory, which is at the given position in the document
     * @param doc The XML-document
     * @param cur The current position within the XML-document
     * @return A pointer to an object that has been created
     */
    virtual T* getElement(xmlDocPtr doc, xmlNodePtr cur) = 0;
    
    /**
     * Checks if the factory is able to construct an element at the current document position
     * @param doc The XML document
     * @param cur The current document position
     * @return true, if this factory can construct an object, otherwise false
     */
    virtual bool accept(xmlDocPtr doc, xmlNodePtr cur) = 0;
    
    /**
     * D'tor
     */
    virtual ~IElementFactory() {}
  };
  
} // NS tmssim

#endif /* !XMLIO_IELEMENTFACTORY_H */
