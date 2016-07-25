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
 * $Id: uawindow.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file uawindow.h
 * @brief Abstract aggregator over a sliding window
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#ifndef UTILITY_UAWINDOW_H
#define UTILITY_UAWINDOW_H 1

#include <core/scobjects.h>

namespace tmssim {

  /**
   * This abstract class manages the contents of a sliding window for utility
   * aggregation.
   * The actual calculation of HC utilities must be performed in subclasses.
   */    
  class UAWindow : public virtual UtilityAggregator {
  public:
    /**
     * @param _size size of the window
     * @param initValue initial value for the entries
     */
    UAWindow(size_t _size, double initValue=1);

    /**
     * Copy C'tor
     */
    UAWindow(const UAWindow& rhs);

    /**
     * D'tor
     */
    virtual ~UAWindow(void);
    
    //virtual double getCurrentUtility(void) const = 0;
    //virtual double predictUtility(double u) const = 0;
    
    /**
     * Offset operator for accessing the single entries of the monitoring
     * window.
     * @param offset 0 returns newest value, UAWindow::size-1 oldest value;
     * for other values of offset the return value is undefined
     */
    double operator [] (size_t offset) const;

    /**
     * @return size of the monitoring window
     */
    size_t getSize(void) const;

    /**
     * @return oldest utility entry in window
     */
    double oldest(void) const;

    //virtual UtilityAggregator* clone() const = 0;
    //virtual void write(xmlTextWriterPtr writer) const = 0;
    
    /**
     * @name XML
     * @{
     */

    virtual int writeData(xmlTextWriterPtr writer);

    /**
     * @}
     */

  protected:
    /**
     * This function records the utility value in the values array.
     * You may want to overwrite this function. If you do so, make sure
     * you call this function (UAWindow::addHook) first (or second, if you need
     * to save the oldest value for some calculation) in your implementation
     * to keep the data structures consistent and not loose any values.
     */
    virtual void addHook(double u);
    
    //const size_t& fill;
    
  private:
    /**
     * size of window
     */
    size_t size;

    /**
     * actual utility values
     */
    double* values;

    /**
     * "pointer" to last stored element
     */
    size_t current;

    /**
     * How many values are currently stored
     */
    //size_t _fill;
  };
  
} // NS tmssim

#endif /* !UTILITY_UAWINDOW_H */
