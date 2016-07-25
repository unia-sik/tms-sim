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
 * $Id: mkptask.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkptask.h
 * @brief (m,k)-firm real-time task with fixed (m,k)-patterns
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_MKCTASK_H
#define TASKMODELS_MKCTASK_H 1

#include <core/scobjects.h>
#include <taskmodels/mktask.h>
#include <taskmodels/mkmonitor.h>

namespace tmssim {

  /**
   * Implementation of a (m,k)-firm real-time task as defined
   * by Parameswaran Ramanathan in "Overload Management in Real-Time
   * Control Applications Using (m,k)-Firm Guarantee" (IEEE Transactions
   * on Parallel and Distributed Systems, vol. 10, No. 6, June 1999)
   */
  class MkpTask : public MkTask {
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
     * @param _spin
     * @param _useSpin
     * @param _is
     * @param _strict use strict schedulability condition, i.e. mandatory
     *                instances must be successful
     */
    MkpTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et,
	    TmsTimeInterval _ct,
	    UtilityCalculator* _uc, UtilityAggregator* _ua,
	    TmsPriority _priority, unsigned int _m, unsigned int _k,
	    unsigned int _spin = 0, bool _useSpin = false,
	    CompressedMkState _is = CMKS_ALL_SUCCESS, bool _relaxed = false);
    MkpTask(const MkpTask& rhs);
    MkpTask(const MkTask* rhs);
    virtual ~MkpTask();

    //virtual void write(xmlTextWriterPtr writer) const;
    virtual Task* clone() const;

    bool isJobMandatory(unsigned int jobId) const;

    void enableSpin();
    void disableSpin();

    void setRelaxed(bool _relaxed);
    bool getRelaxed() const;

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
    virtual Job* spawnHook(TmsTime now);
    virtual bool completionHook(Job *job, TmsTime now);
    virtual bool cancelHook(Job *job);
    virtual std::string getShortId(void) const;

  private:
    // this is the spin parameter actually used - it is only != 0 if _useSpin was set in the contstructor
    unsigned int actSpin;
    bool relaxed;
    //unsigned int m;
    //unsigned int k;
    //unsigned int a;

    //MKMonitor *monitor;

  };


  /**
   * @brief Generic allocator function
   * @return MkpTask that does not use the spin parameter
   */
  MkTask* MkpTaskAllocator(MkTask* task);


  /**
   * @brief Generic allocator function
   * @return MkpTask that uses the spin parameter
   */
  MkTask* MkpTaskWithSpinAllocator(MkTask* task);


  /**
   * @brief Generic allocator function
   * @return MkpTask that does not use the spin parameter, but implements
   *         relaxed schedulability (all mandatory instances must be successful)
   */
  MkTask* RelaxedMkpTaskAllocator(MkTask* task);


  /**
   * @brief Generic allocator function
   * @return MkpTask that uses the spin parameter and implements
   *         relaxed schedulability (all mandatory instances must be successful)
   */
  MkTask* RelaxedMkpTaskWithSpinAllocator(MkTask* task);

  
} // NS tmssim

#endif /* !TASKMODELS_MKCTASK_H */
