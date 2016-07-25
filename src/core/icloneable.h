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
 * $Id: icloneable.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file icloneable.h
 * @brief Interface for cloning of objects
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_ICLONEABLE_H
#define CORE_ICLONEABLE_H 1

namespace tmssim {

  /**
    Interface class for objects that have to provide replication functionality
    Template parameter T should be the name of the class that is going to
    provide this functionality
  */
  template <class T>
    class ICloneable {
  public:
    
    /**
     * Clones an object
     * @return A pointer to a deep-copied clone of the object
     */
    virtual T* clone() const = 0;
    
    /**
     * D'tor
     */
    virtual ~ICloneable() {}
  };

} // NS tmssim

#endif /* CORE_ICLONEABLE_H */

