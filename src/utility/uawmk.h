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
 * $Id: uawmk.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file uawmk.h
 * @brief Utility Aggregator for utility-base (m,k)-firm scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILITY_UAWMK_H
#define UTILITY_UAWMK_H 1

#include <utility/uawindow.h>

namespace tmssim {

  class UAWMK : public UAWindow {
  public:
    UAWMK(unsigned int _m, unsigned int _k);
    UAWMK(const UAWMK& rhs);
    virtual ~UAWMK();

    unsigned int getM() const { return m; }
    unsigned int getK() const { return k; }

    virtual double getCurrentUtility(void) const;
    virtual double predictUtility(double u) const;

    virtual void write(xmlTextWriterPtr writer) const;
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
    
  private:
    unsigned int m;
    unsigned int k;

    double currentSum;

  };

} // NS tmssim

#endif // !UTILITY_UAWMK_H
