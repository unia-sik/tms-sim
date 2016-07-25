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
 * $Id: mktask.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file mktask.h
 * @brief General task with (m,k)-firm real-time constraints
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_MKTASK_H
#define TASKMODELS_MKTASK_H 1

#include <core/scobjects.h>
#include <taskmodels/periodictask.h>
#include <taskmodels/mkmonitor.h>

namespace tmssim {

  /**
   * @brief General task with (m,k)-firm real-time constraints
   *
   * This class acts as superclass for the implementation of task models
   * with (m,k)-firm real-time constraints.
   *
   * @todo
   * - add constructor from tmssim::PeriodicTask
   * - add mktaskbuilder (?), extend xml schema by mktask
   */
  class MkTask : public PeriodicTask {
  public:
    /**
     * @param _id
     * @param _period
     * @param _et
     * @param _ct
     * @param _uc
     * @param _ua
     * @param _priority
     * @param _m
     * @param _k
     * @param _spin The spin parameter used by the Pattern-Spinning algorithm
     * for (m,k)-tasks by Semprebom et al. 2013, earlier by Quan & Hu 2000
     */
    MkTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et,
	   TmsTimeInterval _ct,
	   UtilityCalculator* _uc, UtilityAggregator* _ua,
	   TmsPriority _priority, unsigned int _m, unsigned int _k,
	   unsigned int _spin = 0, CompressedMkState _is = CMKS_ALL_SUCCESS);
    MkTask(const MkTask& rhs);
    MkTask(const MkTask* rhs);
    virtual ~MkTask();

    bool operator==(const MkTask& rhs) const;
    bool operator!=(const MkTask& rhs) const;
    
    virtual std::ostream& print(std::ostream& ost) const;

    /**
     * @brief (m,k)-constrained tasks can have a failure-state distance
     * different from 1.
     *
     * Overrides the Task::getDistance() method. This method implements
     * the distance calculation for (m,k)-constrained real-time tasks
     * as defined by Hamdaoui & Ramanathan (1995) in equation 3.1:
     * \f$\mbox{priority}_{i+1}^j=k-l_j(m_j,s)+1\f$
     *
     * @return The distance to a failure-state.
     */
    int getDistance() const;


    /**
     * @brief Find position of nth deadline meet.
     *
     * Implements function \f$l_j(n,s)\f$ from Hamdaoui & Ramanathan (1995).
     *
     * @param n which deadline meet should be found
     * @param vals ring buffer length k
     * @param pos current head of buffer
     * @return a number between 1 and k, k+1 if no nth meet exists,
     *         and -1 if n>k
     */
    int calcL(unsigned int n, const unsigned int* vals, unsigned int pos) const;


    /**
     * @brief simplified calculation of l
     * @param n which deadline meet should be found
     * @return a number between 1 and k, k+1 if no nth meet exists,
     *         and -1 if n>k
     */
    int calcL(int n) const;


    /**
     * @brief get the monitor of this task
     * @return the task's MkMonitor
     */
    const MkMonitor& getMonitor() const;


    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */

    unsigned int getM() const { return m; }
    unsigned int getK() const { return k; }
    unsigned int getSpin() const { return spin; }
    void setSpin(unsigned int _spin) { spin = _spin; }

    virtual std::string strState() const;

  protected:
    /**
     * If you overwrite this function in your implementation, make sure to
     * still call it!
     */
    virtual bool completionHook(Job *job, TmsTime now);

    /**
     * If you overwrite this function in your implementation, make sure to
     * still call it!
     */
    virtual bool cancelHook(Job *job);

    virtual std::string getShortId(void) const;


    unsigned int m;
    unsigned int k;
    unsigned int spin;

    MkMonitor monitor;

  };

  /**
   * @brief Generic allocator function
   */
  MkTask* MkTaskAllocator(MkTask* task);


} // NS tmssim

#endif // !TASKMODELS_MKTASK_H
