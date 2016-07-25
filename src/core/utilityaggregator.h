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
 * $Id: utilityaggregator.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file utilityaggregator.h
 * @brief Interface definition of history-cognisant utility functions.
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_UTILITYAGGREGATOR_H
#define CORE_UTILITYAGGREGATOR_H 1

#include <core/writeabletoxml.h>
//#include <core/iwriteabletoxml.h>
#include <core/icloneable.h>

namespace tmssim {

  /**
   * @brief History-cognisant utility function.
   *
   * A HCUF aggregates the utilities that are achieved by the execution
   * of multiple jobs of the same task. This class defines the interface
   * for a HCUF and contains basic functionality for utility aggregation.
   */
  class UtilityAggregator
    : //public IWriteableToXML,
    public WriteableToXML,
    ICloneable<UtilityAggregator> {
  public:
    /**
     * @brief Create a new utility aggregator.
     *
     * Make sure to call this constructor also in your subclass
     * implementations!
     */
    UtilityAggregator();

    /**
     * D'tor
     */
    virtual ~UtilityAggregator(void);
    
    /**
     * @brief Add a new utility value to execution history.
     *
     * Do NOT overwrite this function in your implementations. This
     * function records the utility value in the utility sum. Then
     * the UtilityAggregator::addHook method is called.
     * @param u Utility value 0 <= u <= 1
     */
    void addUtility(double u);

    /**
     * @brief Get the number of utility values that have been recorded so far.
     * @return length of execution history
     */
    int getCount(void) const;
    
    /**
     * @brief Get the sum of all utility values that have been recorded so far.
     * @return totally accumulated utility
     */
    double getTotal(void) const;
    
    /**
     * @brief Get the mean utility that was collected in each execution step
     * @return mean utility
     */
    double getMeanUtility(void) const;
    
    /**
     * @brief Get the current history-cognisant utility.
     *
     * Implement your HCUF methric in this function in your subclasses.
     * @return the current HCUF value
     */
    virtual double getCurrentUtility(void) const = 0;
    
    /**
     * @brief Predict the HCUF value for the next step.
     *
     * This function calculates the value of the HCUF under the assumption
     * that a certain utility value is added to the history.
     * @param u Utility value to be added
     * @return the possible HCUF value
     */
    virtual double predictUtility(double u) const = 0;
    

    /**
     *
     */
    virtual UtilityAggregator* clone() const = 0;
    
    /**
     *
     */
    friend std::ostream& operator << (std::ostream& ost, const UtilityAggregator& ua);

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
     * @brief Implement this to notify your metric about new utility values.
     *
     * This function is called by the UtilityAggregator::addUtility method
     * whenever a new utility value is added to the execution history.
     *
     * @param u the utility value
     */
    virtual void addHook(double u) = 0;
    
  private:
    /**
     * Sum of the utility values accumulated so far
     */
    double utilitySum;

    /**
     * Number of utility values accumulated so far
     */
    int utilityCount;
  };

} // NS tmssim

#endif // !CORE_UTILITYAGGREGATOR_H
