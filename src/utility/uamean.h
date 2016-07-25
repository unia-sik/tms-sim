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
 * $Id: uamean.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file uamean.h
 * @brief Aggregator with equal weight
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#ifndef UTILITY_UAMEAN_H
#define UTILITY_UAMEAN_H 1

#include <core/scobjects.h>

namespace tmssim {

  class UAMean : public virtual UtilityAggregator {
  public:
    UAMean();
    virtual ~UAMean(void);
    
    virtual double getCurrentUtility(void) const;
    virtual double predictUtility(double u) const;
    
    /**
     * Serializes this object with the given xmlTextWriter to a xml-document
     * @param writer A pointer to the xmlWriter that should be used for serialization
     */
    virtual void write(xmlTextWriterPtr writer) const;
    
    /**
     * Creates a deep-copy of the current object
     * @return A pointer to a new object with the same properties of this object
     */
    virtual UtilityAggregator* clone() const;

    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */
    
  protected:
    virtual void addHook(double u);
  };

} // NS tmssim

#endif /* !UTILITY_UAMEAN_H */
