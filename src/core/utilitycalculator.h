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
 * $Id: utilitycalculator.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file utilitycalculator.h
 * @brief Calculation of single-job utilities
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_UTILITYCALCULATOR_H
#define CORE_UTILITYCALCULATOR_H 1

#include <core/writeabletoxml.h>
//#include <core/iwriteabletoxml.h>
#include <core/icloneable.h>
#include <core/primitives.h>

namespace tmssim {

  class Job;

  /**
   * @brief Calculation of single-job utilities
   */
  class UtilityCalculator
    : //public IWriteableToXML,
    public WriteableToXML,
    public ICloneable<UtilityCalculator>
    {
  public:
    /*
    UtilityCalculator();
    UtilityCalculator(const UtilityCalculator& rhs);
    */
    virtual ~UtilityCalculator(void) = 0;
    
    /**
      Calculate the single-execution utility
      @param job the job whose utility shall be calculated
      @param complTime actual or estimated completion time
      @return a utility value
    */
    virtual double calcUtility(const Job *job, TmsTime complTime) const = 0;
    virtual UtilityCalculator* clone() const = 0;

    /**
     * @name XML
     * @{
     */

    virtual int writeData(xmlTextWriterPtr writer);

    /**
     * @}
     */

  };

} // NS tmssim

#endif // !CORE_UTILITYCALCULATOR_H
