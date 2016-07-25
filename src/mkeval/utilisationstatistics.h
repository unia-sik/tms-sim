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
 * $Id: utilisationstatistics.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file utilisationstatistics.h
 * @brief Statistics on task sets' processor utilisation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_UTILISATIONSTATISTICS_H
#define MKEVAL_UTILISATIONSTATISTICS_H 1

#include <list>
#include <vector>

namespace tmssim {

  /**
   * @brief Collect and evaluates task sets' processor utilisation statistics
   */
  class UtilisationStatistics {
  public:
    UtilisationStatistics();
    ~UtilisationStatistics();

    /**
     * @brief Add a new utilisation value
     */
    void addUtilisation(double utilisation);

    void evaluate();

    /**
     * @return number of utilisation values
     */
    double getSize() const;
    /// @return mean utilisation
    double getMean() const;
    /// @return standard deviation of utilisations
    double getSigma() const;
    /// @return median utilisation (average of middle values if an even number of measurements was performed)
    double getMedian() const;
    /// @return minimum utilisation
    double getMin() const;
    /// @return maximum utilisation
    double getMax() const;

    /**
     * Calculate the interval I around the median such that a certain
     * percentage of the data lies inside the interval
     * @param percent percentage
     * @return lower end of the interval
     */
    double getMedianIntervalLower(int percent) const;
    /**
     *
     * @param percent percentage
     * @return upper end of the interval
     * @see getMedianIntervalLower
     */
    double getMedianIntervalUpper(int percent) const;

  private:
    /// Stores all utilisation values
    std::list<double> uList;
    /**
     * Used for median calculations - after evaluation, this vector holds
     * all data sorted in non-decreasing order
     */
    std::vector<double> data;

    /// set to true, if evaluation has been performed.
    bool evaluated;

    /// mean utilisation
    double mean;
    /// utilisation variance
    double var;
    /// standard deviation of utilisation
    double sigma;
    /// median utilisation
    double median;

  };


} // NS tmssim

#endif // !MKEVAL_UTILISATIONSTATISTICS_H
