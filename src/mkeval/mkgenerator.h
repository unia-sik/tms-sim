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
 * $Id: mkgenerator.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mkgenerator.h
 * @brief Generator of (m,k)-tasksets
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKGENERATOR_H
#define MKEVAL_MKGENERATOR_H 1

#include <mkeval/mkglobals.h>

#include <taskmodels/mkptask.h>
#include <vector>

namespace tmssim {

  /**
   * @brief Generation of (m,k) task sets
   * @todo generalise this class to simple periodic task sets!
   *
   * A Task set is generated in the following manner:
   * - the parameters period \f$p_i\f$ and \f$k\f$ are chosen randomly
   *   from the respective intervals specified in the class's constructor
   * - \f$m\f$ is chosen from \f$[1,k]\f$
   * - a execution time weight \f$w_i\f$ is chosen for each task from
   *   \f$[1,maxWC]\f$
   * - calculation of the actual \f$c_i\f$:
   *   - Problem: find \f$c_i\f$ such that \f$\sum\frac{c_i}{p_i}=U\f$
   *   - def. \f$u_i=\frac{c_i}{p_i}\f$
   *   - def. \f$W=\sum w_i\f$
   *   - require: \f$\frac{w_i}{W}=\frac{u_i}{U}\f$
   *   - \f$\Leftrightarrow u_i=\frac{w_iU}{W}\Leftrightarrow\frac{c_i}{p_i} = \frac{w_iU}{W}\Leftrightarrow c_i=w_i p_i\frac{U}{W}\f$
   *   - if \f$c_i=0\f$ after rounding, then it is set to 1
   */
  class MkGenerator {
  public:
    /**
     * @brief C'tor
     *
     * @param _seed seed for random number generator
     * @param _size Size of each generated task set
     * @param _minPeriod minimum period of a task
     * @param _maxPeriod maximum period of a task
     * @param _minK minimum k value of a task
     * @param _maxK maximum k value of a task
     * @param _utilisation target utilisation of th the task set (ignoring m/k)
     * @param _utilisationDeviation maximum deviation of the actual utilisation
     * from the target utilisation
     * @param _maxWC maximum weight for calculation of task execution time
     * @param _ucAlloc Allocator function for UtilityCalculator
     * @param _uaAlloc Allocator function for UtilityAggregator
     */
    MkGenerator(unsigned int _seed, unsigned int _size = 5,
		TmsTimeInterval _minPeriod = 5, TmsTimeInterval _maxPeriod = 15,
		unsigned int _minK = 2, unsigned int _maxK = 10,
		float _utilisation = 1.0, float _utilisationDeviation = 0.1,
		unsigned int _maxWC = 100,
		UCMKAllocator _ucAlloc = DefaultUCMKAllocator,
		UAMKAllocator _uaAlloc = DefaultUAMKAllocator);

    /**
     * @brief D'tor
     */
    virtual ~MkGenerator();

    /**
     * @brief Generates a new task set.
     *
     * Task priorities are assigned according to the rate monotonic
     * algorithm (Liu & Layland 1976).
     * @return the task set; the calling program muss free the task set's
     * memory after usage!
     */
    MkTaskset* nextTaskset();

    /**
     * @brief Generates a new task set.
     *
     * This method can be used to reproduce certain results
     * @return the task set; the calling program muss free the task set's
     * memory after usage!
     */
    MkTaskset* nextTaskset(unsigned int tsSeed);


    /**
     * @brief Calculation of rotation values for (m,k)-patterns.
     *
     * Implementation of Algorithm 2 from Quan and Hu, "Enhanced
     * Fixed-Priority Scheduling with (m,k)-Firm Guarantee", RTSS 2000
     *
     * @param [inout] tasks the taskset for which the rotation values
     * shall be calculated. The Algorithm sets the tasks' spin value.
     *
     */
    static void calcRotationValues(std::vector<MkTask*>& tasks);

    /**
     * @brief Sufficient schedulability test.
     *
     * Implements the sufficient schedulability test by Jia et al.,
     * "Task Handler Based on (m,k)-firm Constraint Model for Managing a
     * Set of Real-Time Controllers", RTNS 2007 (Theorem 1).
     * For harmonic task sets, this condition is also necessary.
     *
     * @param tasks the tasks of the taskset sorted by increasing period
     * (shortest first)
     *
     * @return true, if the taskset passed the schedulability test
     */
    static bool testSufficientSchedulability(std::vector<MkTask*>& tasks);


  protected:

    /**
     * @brief Get a new seed.
     *
     * Each task set is generated using a different seed. Thus, task sets
     * can be identified by their seed and simulations can be reproduced.
     * @return a new seed base on the baseSeed of the class
     */
    unsigned int newSeed();

    /**
     * @brief Get a random number.
     *
     * The number is chosen from the closed interval [min, max]
     * @param seed the seed to use
     * @param min lower interval bound
     * @param max upper interval bound
     * @return the number
     */
    unsigned int produceInt(unsigned int* seed,
			    unsigned int min, unsigned int max);


    /*
     * @brief Calculate \f$n_{ij}\f$ for testSufficientSchedulability.
     *
     * \f$n_{ij}=\left\lceil\frac{m_j}{k_j}\left\lceil\frac{T_i}{T_j}\right\rceil\right\rceil\f$
     */
    //static int calcNij(MkTask* ti, MkTask* tj);


    /**
     * @brief Calculation of execution interference between two tasks.
     *
     * Implementation of Algorithm 1 from Quan and Hu, "Enhanced
     * Fixed-Priority Scheduling with (m,k)-Firm Guarantee", RTSS 2000
     * Interference that \tau_i experiences from \tau_h. h<i!
     *
     * @param ti
     * @param th
     *
     * @return F_i^h
     */
    static long calcExecutionInterference(const MkpTask& ti, const MkpTask& th);


    /**
     * @brief Calculate greatest common divisor
     */
    static unsigned long calcGCD(unsigned long a, unsigned long b);

    /**
     * @function calcLhij
     * Calculate \f$l^h_{ij}\f$ for #calcExecutionInterference. This
     * value represents the number of mandatory instances of a task
     * that fall entirely in a certain interval.
     * @param task the task whose mandatory instances shall be counted
     * @param low lower bound of the interval
     * @param high higher bound of the interval
     * @return the number of mandatory instances
     * @deprecated
     */
    /*static unsigned int calcLhij(const MkpTask& task,
      unsigned int low, unsigned int high);*/

    /**
     * @brief temporary container for task set generation
     */
    struct TempTask {
    TempTask() : period(0), m(0), k(0), wc(0), c(0), priority(0) {}
      /*TempTask(unsigned int _period, unsigned int _m, unsigned int _k)
	: period(_period), m(_m), k(_k), wc(0), c(0) {}*/
      TmsTimeInterval period;
      unsigned int m;
      unsigned int k;
      unsigned int wc;
      TmsTimeInterval c;
      unsigned int priority;
    };

  private:
    unsigned int baseSeed;

    unsigned int size;
    TmsTimeInterval minPeriod;
    TmsTimeInterval maxPeriod;
    unsigned int minK;
    unsigned int maxK;
    float utilisation;
    float utilisationDeviation;
    unsigned int maxWC;

    UCMKAllocator ucAlloc;
    UAMKAllocator uaAlloc;
    
  };

} // NS tmssim

#endif // !MKEVAL_MKGENERATOR_H
