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
 * $Id: dbptaskbuilder.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file dbptaskbuilder.h
 * @brief
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_DBPTASKBUILDER_H
#define XMLIO_DBPTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a MBPTask object from the given XML-document.
   */
  class DbpTaskBuilder: public IElementBuilder<Task> {
    
  public:

    /**
     * Builds a SporadicTask object
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };

} // NS tmssim

#endif // !XMLIO_DBPTASKBUILDER_H
