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
 * $Id: mkmonitor.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkmonitor.h
 * @brief Monitoring of (m,k)-firm condition
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_MKMONITOR_H
#define TASKMODELS_MKMONITOR_H 1

#include <string>
#include <cstdint>

namespace tmssim {

  /**
   * @brief Compressed representation of a task's current (m,k)-state.
   *
   * LSB represents most recent job, k-th bit least recent job; further
   * bits shall be set to 0.
   */
  typedef uint64_t CompressedMkState;
  #define CMKS_MAX_SIZE 64
#define CMKS_ALL_SUCCESS ((uint64_t)-1LL)
  
  
  /**
   * @brief Monitoring of (m,k) constraint.
   *
   * This class implements a ring buffer to monitor the a sliding window
   * of \f$k\f$ job executions.
   *
   * @todo add check for (m,k) constraint, actually use m
   */
  class MkMonitor {
  public:
    /**
     * @brief C'tor
     *
     * @param _m \f$m\f$ of the (m,k) constraint (currently not used!)
     * @param _k \f$k\f$ of the (m,k) constraint
     * @param _is initial (m,k) state
     */
    MkMonitor(unsigned int _m, unsigned int _k, CompressedMkState _is = CMKS_ALL_SUCCESS);
    /**
     * @brief D'tor
     */
    virtual ~MkMonitor();

    /**
     * @brief Add a new value to the buffer.
     *
     * Pushing a new value into the buffer leads to the oldest value
     * being removed from the buffer.
     * @param _val the new value to be added
     */
    void push(unsigned int _val);

    /**
     * @brief Get the current sum of all buffer entries.
     *
     * If not yet enough values have been recorded, this function returns
     * a hypothetical sum. It ignores the initial values and assumes
     * missing values are 1 (successful execution).
     *
     * @return Current sum
     */
    unsigned int getCurrentSum() const;

    /**
     * @brief Get the complete buffer.
     *
     * Note: the returned array is not ordered! Use the getPos() method
     * to get the position of the oldest entry. The most recent entry is
     * located at \f$p-1 (\% k)\f$. The least recent entry is at \f$p\f$.
     * @return the complete buffer
     */
    const unsigned int* getVals() const;

    /**
     * @brief Get the position of the oldest entry in the buffer.
     *
     * @return position
     */
    size_t getPos() const;

    /**
     * @brief Get the number of violations of the (m,k)-constraint
     * @return the number
     */
    unsigned int getViolations() const;

    std::string printState() const;

    /**
     *
     */
    CompressedMkState getState() const;
    CompressedMkState getReducedState() const;

    bool isStateValid() const;

    class MkMonitorException {
    public:
      MkMonitorException(std::string _msg = "")
	: msg(_msg) {}
      const std::string& getMessage() const { return msg; }
    private:
      std::string msg;
    };
    
  private:
    unsigned int m;
    unsigned int k;
    /**
     * Points to next write field in buffer
     */
    size_t pos;

    /**
     * Ring buffer
     */
    unsigned int* vals;

    /**
     * Current sum of the values that are in the buffer. This value is
     * updated any time the buffer is changed by the push() method.
     */
    unsigned int sum;

    unsigned int recorded; ///!< number of the <b>next</b> value that is recorded
    
    unsigned int violations;
  };

} // NS tmssim

#endif /* !TASKMODELS_MKMONITOR_H */
