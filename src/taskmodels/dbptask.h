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
 * $Id: dbptask.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file dbptask.h
 * @brief Task with distance-based priority assignment
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_DBPTASK_H
#define TASKMODELS_DBPTASK_H 1

#include <taskmodels/mktask.h>

namespace tmssim {

  /**
   * @brief Task with distance-based priority assignment.
   * 
   * This is an implementation of the (m,k)-firm task defined
   * in Moncef Hamdaoui, Parameswaran Ramanathan, "A Dynamic Priority
   * Assignment Technique for Streams with (m,k)-Firm Deadlines", IEEE
   * Transactions on Computers, Vol. 44, No. 12, December 1995
   */
  class DbpTask : public MkTask {
  public:
    /**
     * @param _id
     * @param _period
     * @param _et
     * @param _ct
     * @param _uc
     * @param _ua
     * @param _priority this parameter is actually not used as the priority
     * is assigned dynamically
     * @param _m
     * @param _k
     */
    DbpTask(unsigned int _id, TmsTimeInterval _period, TmsTimeInterval _et,
	    TmsTimeInterval _ct,
	    UtilityCalculator* _uc, UtilityAggregator* _ua,
	    TmsPriority _priority, unsigned int _m, unsigned int _k,
	    CompressedMkState _is = CMKS_ALL_SUCCESS);
    DbpTask(const DbpTask& rhs);
    DbpTask(const MkTask* rhs);
    virtual ~DbpTask();


    //virtual void write(xmlTextWriterPtr writer) const;
    virtual Task* clone() const;


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

    
  };


  /**
   * @brief Generic allocator function
   */
  MkTask* DbpTaskAllocator(MkTask* task);

} // NS tmssim

#endif // !TASKMODELS_DBPTASK_H
