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
 * $Id: utilityaggregatorfactory.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file utilityaggregatorfactory.h
 * @brief Building of utility aggregators from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_UTILITYAGGREGATORFACTORY_H
#define XMLIO_UTILITYAGGREGATORFACTORY_H 1

/**
 * Includes
 */
#include <string>
#include <map>

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>
#include <xmlio/ielementfactory.h>

namespace tmssim {

  /**
   * Creates UtilityAggregator objects from a XML document
 * @todo Make this class a singleton.
 * @todo Load builders from dynamic library
   */
  class UtilityAggregatorFactory: public IElementFactory<UtilityAggregator> {
    
  public:
    /**
     * C'tor
     * Initializes the map
     */
    UtilityAggregatorFactory();

    ~UtilityAggregatorFactory();
    
    /**
     * Checks if the factory is able to construct a UtilityAggregator at the current document position
     * @param doc The XML document
     * @param cur The current document position
     * @return true, if this factory can construct a UtilityAggregator, otherwise false
     */
    bool accept(xmlDocPtr doc, xmlNodePtr cur);
    /**
     * Calls the build method of the proper builder to construct a UtilityAggregator for the given position within
     * the XML-document
     * @param doc The XML-document
     * @param cur The current position within the document
     * @return A pointer to a task object, in case of an Error NULL
     */
    UtilityAggregator* getElement(xmlDocPtr doc, xmlNodePtr cur);
    

  private:
    /**
     * Container that maps strings (XML tag names) to builder-objects,
     * which are able to construct the prober object
     */
    std::map<std::string, IElementBuilder<UtilityAggregator>*> mBuilder;
    
  };

} // NS tmssim

#endif /* !XMLIO_UTILITYAGGREGATORFACTORY_H */
