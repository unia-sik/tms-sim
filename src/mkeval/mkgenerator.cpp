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
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: mkgenerator.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file mkgenerator.cpp
 * @brief Generation of (m,k)-tasksets
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkgenerator.h>

#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <list>

#include <limits.h>

#include <utils/logger.h>
//#define TLOGLEVEL TLL_DEBUG
#include <utils/tlogger.h>

using namespace std;

namespace tmssim {

  MkGenerator::MkGenerator(unsigned int _seed, unsigned int _size,
			   TmsTimeInterval _minPeriod, TmsTimeInterval _maxPeriod,
			   unsigned int _minK, unsigned int _maxK,
			   float _utilisation, float _utilisationDeviation,
			   unsigned int _maxWC,
			   UCMKAllocator _ucAlloc, UAMKAllocator _uaAlloc)
    : baseSeed(_seed), size(_size), minPeriod(_minPeriod), maxPeriod(_maxPeriod),
      minK(_minK), maxK(_maxK), utilisation(_utilisation),
      utilisationDeviation(_utilisationDeviation), maxWC(_maxWC),
      ucAlloc(_ucAlloc), uaAlloc(_uaAlloc)
  {
  }
  

  MkGenerator::~MkGenerator() {
  }


  MkTaskset* MkGenerator::nextTaskset() {
    tDebug() << "Generating next taskset using seed " << baseSeed;
    unsigned int seed = newSeed();
    return nextTaskset(seed);
  }
  
  MkTaskset* MkGenerator::nextTaskset(unsigned int tsSeed) {
    unsigned int sumWC = 0;
    float sumU = 0.0;
    MkTaskset *ts = NULL;// = new MkTaskset();
    while ( sumU < (utilisation - utilisationDeviation)
    	    || sumU > (utilisation + utilisationDeviation) ) {
      list<TempTask *> tmpTS;
      sumU = 0;
      sumWC = 0;
      if (ts != NULL)
	delete ts;
      ts = new MkTaskset();
      ts->seed = tsSeed;
      ts->targetUtilisation = utilisation;
      // generate basic parameters
      for (unsigned int i = 0; i < size; ++i) {
	TempTask * tt = new TempTask;
	tt->period = produceInt(&tsSeed, minPeriod, maxPeriod);
	tt->k = produceInt(&tsSeed, minK, maxK);
	tt->m = produceInt(&tsSeed, 1, tt->k);
	tt->wc = produceInt(&tsSeed, 1, maxWC);
	sumWC += tt->wc;
	tmpTS.push_back(tt);
      }
      
      // Sort & perform rate-monotonic priority assignment
      list<TempTask *> tmpPri;
      for (list<TempTask *>::iterator it = tmpTS.begin();
	   it != tmpTS.end(); ++it) {
	list<TempTask *>::iterator ins = tmpPri.begin();
	while (ins != tmpPri.end() && (*ins)->period < (*it)->period)
	  ++ins;
	tmpPri.insert(ins, *it);
      }
      unsigned int i = 0;
      for (list<TempTask *>::iterator it = tmpPri.begin();
	   it != tmpPri.end(); ++it, ++i) {
	(*it)->priority = i;
      }
      
      
      float uw = utilisation / sumWC; // $\frac{U}{W}$ to speed up calculation
      i = 0;
      
      for (list<TempTask *>::iterator it = tmpPri.begin();
	   it != tmpPri.end(); ++it, ++i) {
	TempTask *tt = *it; // only for convenience
	float fci = uw * tt->period * tt->wc;
	tt->c = lround(fci);
	if (tt->c == 0)
	  tt->c = 1;
	sumU += (float)tt->c / (float)tt->period;
	MkTask* task = new MkTask(i, tt->period, tt->c, tt->period, ucAlloc(),
				  uaAlloc(tt->m, tt->k), tt->priority, tt->m, tt->k);
	ts->tasks.push_back(task);
	delete tt;
      }
      tmpPri.clear();
      tmpTS.clear();
    }
    ts->realUtilisation = sumU;
    ts->suffMKSched = testSufficientSchedulability(ts->tasks);
    if (ts->suffMKSched)
      MkGenerator::calcRotationValues(ts->tasks);

    return ts;    
  }

  
  bool MkGenerator::testSufficientSchedulability(vector<MkTask*>& tasks) {
    for (unsigned int i = 0; i < tasks.size(); ++i) {
      TmsTimeInterval sum = 0;
      for (unsigned int j = 0; j < i; ++j) {

	double nij = ceil( double(tasks[j]->getM()) / double(tasks[j]->getK())
			   * ceil( double(tasks[i]->getPeriod())
				   / double(tasks[j]->getPeriod()) ));
	sum += tasks[j]->getExecutionTime() * round(nij);
	//sum += tasks[j]->getExecutionTime() * calcNij(tasks[i], tasks[j]);
      }
      sum += tasks[i]->getExecutionTime();
      if (sum > tasks[i]->getPeriod())
	return false;
    }
    return true;
  }


  /*
  int MkGenerator::calcNij(MkTask* ti, MkTask* tj) {
    double mj = tj->getM();
    double kj = tj->getK();
    double pi = ti->getPeriod();
    double pj = tj->getPeriod();

    double nij = ceil( mj / kj * ceil(pi / pj) );
    return round(nij);
  }
  */

  unsigned int MkGenerator::newSeed() {
    return rand_r(&baseSeed);
  }

  unsigned int MkGenerator::produceInt(unsigned int* seed, unsigned int min, unsigned int max) {
    assert(min < max);
    unsigned int len = max - min + 1; // due to modulo arithmetics
    unsigned long long int interval = RAND_MAX / len;
    unsigned long long int rnd = rand_r(seed);
    unsigned int number = (uint32_t) ((uint64_t)rnd / interval);
    return number + min;
  }


  /// @todo check implementation
  /// @todo check variable ranges!
  // "//=" lines denote original algorithm text
  void MkGenerator::calcRotationValues(vector<MkTask*>& tasks) {
    tDebug() << "Calculating rotation values...";
    list<MkTask*> psi; // \Psi = \emptyset
    //= \Psi contains the tasks whose s_i values have been determined
    list<MkTask*> ts; // \mathcal{T} in original algorithm
    for (vector<MkTask*>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
      ts.push_back(*it);
    }
    //= while \mathcal{T} is not empty do
    while (ts.size() > 0) {
      // ts is not empty, so use first task for init (no need for neutral)
      list<MkTask*>::iterator min = ts.begin();
      //= find t_i = task in T with smallest k_i
      for (list<MkTask*>::iterator it = ++ts.begin(); it != ts.end(); ++it) {
	if ( (*it)->getK() < (*min)->getK() )
	  min = it;
      }
      // while condition above ensures that ts.begin() != ts.end()
      MkTask* ti = *min;
      LOG(LOG_CLASS_EVAL_MK) << " ... for task " << *ti;
      if (psi.size() > 0) { //= if \Psi \not= \emptyset
	list<MkTask*> omega = psi; //= \Omega = \Psi
	MkTask* tj = NULL; // needed after loop
	unsigned long g = 0; // needed after loop
	bool found = false; // make sure rotation value is only calculated
	                    // if a candidate task was found
	while (omega.size() > 0) { //= while \Omega \not= \emptyset
	  //= \tau_j = task in \Omega such that \mathcal{F}_i^j is maximum,
	  //= where \mathcal{F}_i^j is defined in (4)
	  unsigned long maxval = 0;
	  list<MkTask*>::iterator max = omega.begin();
	  for (list<MkTask*>::iterator it = omega.begin();
	       it != omega.end(); ++it) {
	    unsigned long fji;
	    LOG(LOG_CLASS_EVAL_MK) << "\tCheck interference from " << **it;
	    // interference can only happen if the candidate task's priority
	    // is greater or equal (lower value is higher priority!)
	    // FAK 17.12.2015: Think we should regard any interference:
	    // Even though it might have lower priority - it has already got
	    // its spin value
	    /*
	    if (ti->getPriority() < (*it)->getPriority())
	      continue;
	    */
	    
	    fji = calcExecutionInterference(MkpTask(ti), MkpTask(*it));
	    LOG(LOG_CLASS_EVAL_MK) << "\tChecked interference from " << **it
				   << ": " << fji;
	    if (fji > maxval) {
	      max = it;
	      maxval = fji;
	      LOG(LOG_CLASS_EVAL_MK) << "New max F from " << **it << ": Fij=" << maxval;
	    }
	  }
	  tj = *max;
	  //= g = gcd(k_i \times T_i, k_j \time T_j)
	  g = calcGCD(ti->getK() * ti->getPeriod(),
		      (*max)->getK() * (*max)->getPeriod());
	  if (g == 1) { //= if g = 1
	    LOG(LOG_CLASS_EVAL_MK) << "GCD == 1, ignore " << *tj;
	    omega.erase(max); //= remove \tau_j from \Omega
	    found = false; // might be omitted, as 'else' terminates loop
	  }
	  else { //= else break;
	    LOG(LOG_CLASS_EVAL_MK) << "Use interference from " << *tj
				   << " with g=" << g;
	    found = true;
	    break;
	  }
	}
	// here might be another problem: what if GCD==1 for all tasks in omega?
	// this seems not to concern the current problem cases, but still
	// might happen!?
	if (found) {
	  tDebug() << "Found tj = " << *tj;
	  LOG(LOG_CLASS_EVAL_MK) << "\tUsing " << *tj;
	  //= O_j' = O_j + s_j \times Tj
	  // Actually, s_j shifts in the other direction, so we need to
	  // subtract. Add k_j to stay positive.
	  int ojp = tj->getOffset() + ((tj->getK() - tj->getSpin()) % tj->getK()) * tj->getPeriod();
	  // next is just for debugging
	  int ojpo = tj->getOffset() + tj->getSpin() * tj->getPeriod();
	  LOG(LOG_CLASS_EVAL_MK) << "\tojp: " << ojp << " ojpo: " << ojpo;
	  //= s_i = l such that 0 \leq l \leq k_i
	  //= and |l \times T_i + 0_i - O_j'| is nearest to one of
	  //= (2q+1) \times g / 2, q\in Z
	  int lOpt;
	  float distance = FLT_MAX;
	  for (unsigned int l = 0; l < ti->getK(); ++l) {
	    LOG(LOG_CLASS_EVAL_MK) << "l=" << l << " Ti=" << ti->getPeriod()
		     << " Oi=" << ti->getOffset() << " Oj'=" << ojp;
	    // like above for O_j', we need to heed the shift direction
	    int interm = ((ti->getK() - l) % ti->getK()) * (int)ti->getPeriod() + (int)ti->getOffset() - ojp;
	    //int interm = l * (int)ti->getPeriod() + (int)ti->getOffset() - ojp;
	    int val = abs(interm);
	    //int val = abs(l * (int)ti->getPeriod() + (int)ti->getOffset() - ojp);
	    //LOG(LOG_CLASS_EVAL_MK) << "interm = " << interm;
	    // check only those q that define the bounds of the interval
	    // inside which val resides
	    int q = floor((float)val / g - 0.5);
	    float lb = ((float)q * 2 + 1) * g / 2;
	    float ub = ((float)(q+1) * 2 + 1) * g / 2;
	    tDebug() << "\tval: " << val
		     << " q: " << q
		     << " lb: " << lb
		     << " ub: " << ub;
	    assert( lb <= val);
	    assert( ub >= val);
	    float dl = val - lb;
	    float du = ub - val;
	    LOG(LOG_CLASS_EVAL_MK) << "\tl: " << l
				   << " val: " << val
				   << " q: " << q
				   << " lb: " << lb
				   << " ub: " << ub
				   << " dl: " << dl
				 << " du: " << du;
	    if (dl < distance) {
	      lOpt = l;
	      distance = dl;
	    }
	    if (du < distance) {
	      lOpt = l;
	      distance = du;
	    }
	  }
	  ti->setSpin(lOpt);
	  LOG(LOG_CLASS_EVAL_MK) << "\t=> spin: " << lOpt;
	  /*
	    }
	    else {
	    LOG(LOG_CLASS_EVAL_MK) << "\tno spin calculated as no tj was found";
	    }
	  */
	} // found
	// else there's no need to calculate a rotation value
      }
      else {
	// s_i = 0
	(*min)->setSpin(0);
      }
      psi.push_back(*min); // Add \tau_i to \Psi
      ts.erase(min); // Remove \tau_i from \mathcal{T}
    }
  }

  //$ time bin/mkeval -c ../cfg/default.mkg -t 5 -u 1.55 -d 0.05 -n1000000 -m 8 -T 100 -a MKC -a MKC-S -p logs/fast

  //// @todo check implementation
  long MkGenerator::calcExecutionInterference(const MkpTask& ti, const MkpTask& th) {
    //if (ti.getPriority() > th.getPriority()) return 0;
    // F_i^h = 0
    long fhi = 0;
    // g = gcd(k_i T_i, k_h T_h)
    unsigned long g = calcGCD(ti.getK() * ti.getPeriod(),
			     th.getK() * th.getPeriod());
    // CAUTION: jobs in tms-sim are counted from 0 
    // for j from 1 to k_i do
    for (unsigned int j = 0; j < ti.getK(); ++j) {
      // if \pi_{ij} = 1 then
      if (ti.isJobMandatory(j)) {
	// x = (O_i + (j-1)T_i - O_h) mod g
	// FIXED: j -> j-1
	unsigned int x = ( ti.getOffset() + j * ti.getPeriod() - th.getOffset() ) % g;
	LOG(LOG_CLASS_EVAL_MK)  << "j: " << j << " x: " << x << " g: " << g;
	// while x  < k_h T_h do
	while (x < th.getK() * th.getPeriod()) {
	  long s = floor((double)x/(double)th.getPeriod());
	  long t = floor((double)(x+ti.getPeriod()) / (double)th.getPeriod());
	  if (x == 0) {
	    // in this case we have to go one step back, else instance s
	    // would start exactly at rijm1 (see below) but still be ignored
	    // by calculation of l_ij
	    s = -1;
	  }
	  LOG(LOG_CLASS_EVAL_MK) << "\t-- x: " << x << " s: " << s << " t: " << t;
	  // code commented out below resulted from a misunderstanding
	  // of the (quite incompletely described) algorithm
	  /*
	  // F_{ij}^h is calculated according to (3)
	  // (3) F_{ij}^h = e_s + l_{ij}^h \times C_i + e_t

	  // l^h_{ij} number of mandatory jobs of \tau_h that fall
	  // entirely in the interval
	  // [(j-1)T_i+O_i, jT_i+O_i]
	  // Caution again: j in tms-sim starts at 0!
	  TmsTime lIntervalLow = j * ti.getPeriod() + ti.getOffset();
	  TmsTime lIntervalHigh = (j + 1) * ti.getPeriod() + ti.getOffset();

	  // Find first and last jobs in the interval. This is done slightly
	  // indirectly to accomodate for certain pathologic cases (e.g.
	  // only one job in interval, or none at all but overlapping)

	  // first job of \tau_h that is released after or at lIntervalLow
	  int firstAfterLow = ceil( ( (double) lIntervalLow
				      - (double) th.getOffset() )
				    / (double) th.getPeriod() );
	  assert(firstAfterLow >= 0);
	  
	  // last job of \tau_h that is released before or at lIntervalHigh
	  // This job does not finish inside the interval!
	  int lastBeforeHigh = floor ( ( (double) lIntervalHigh
					- (double) th.getOffset() )
				      / (double) th.getPeriod() );
	  LOG(LOG_CLASS_EVAL_MK) << "\tx: " << x
				 << " lo: " << lIntervalLow
				 << " hi: " << lIntervalHigh
				 << " first: " << firstAfterLow
				 << " last: " << lastBeforeHigh;


	  unsigned int lhij = 0;
	  //long es = 0;
	  //long et = 0;
	  long s = -1; // job that was started before interval and finishes inside
	  long t = -1; // job that is started inside interval and finished outside

	  if (firstAfterLow < lastBeforeHigh) {
	    // Real interval
	    LOG(LOG_CLASS_EVAL_MK) << "\t\tReal Interval";
	    for (int i = firstAfterLow; i < lastBeforeHigh; ++i) {
	      LOG(LOG_CLASS_EVAL_MK) << "\t\tchecking " << th << " for mandatory job " << i;
	      if (th.isJobMandatory(i)) {
		LOG(LOG_CLASS_EVAL_MK) << "\t\tmandatory!";
		++lhij;
	      }
	    }
	    s = firstAfterLow - 1;
	    t = lastBeforeHigh;
	  }
	  else if (firstAfterLow == lastBeforeHigh) {
	    // last happens inside or at end of interval
	    LOG(LOG_CLASS_EVAL_MK) << "\t\tPoint in interval";
	    lhij = 0;
	    s = firstAfterLow - 1;
	    t = lastBeforeHigh;
	  }
	  else { // firstAfterLow > lastBeforeHigh
	    // interval completely covered by period of \tau_h
	    LOG(LOG_CLASS_EVAL_MK) << "\t\tInterval completely covered";
	    lhij = 0;
	    s = firstAfterLow - 1;
	    t = -1;
	  }
	  */

	  unsigned int lhij = 0;
	  for (int i = s+1; i < t; ++i) {
	    //LOG(LOG_CLASS_EVAL_MK) << "\t\tchecking " << th << " for mandatory job " << i;
	    if (th.isJobMandatory(i)) {
	      LOG(LOG_CLASS_EVAL_MK) << "\t" << th.getIdString() << " job " << i << " mandatory!";
	      ++lhij;
	    }
	  }

	  // e_s = \pi_{hs} min\{C_h + r_{hs} - r_{i(j-1)}, 0\}
	  // e_s is amount of \tau_h instance s that falls inside
	  // [r_{i(j-1)},r_ij] with r_{hs} < r_{i(j-1)}
	  // FAK: WHY min? shouldn't it be rather max?
	  // -> r_hs < r_{i(j-1)}, so whole term gives amount of C_h that will
	  // will be after r_{i(j-1)}. If negative, then discard!
	  // -> use max!!!

	  long es = 0;
	  if ( s >= 0 && th.isJobMandatory((unsigned)s)) {
	    long rhs = /*th.getOffset() +*/ s * th.getPeriod();
	    long rijm1 = x;//ti.getOffset() + j * ti.getPeriod();
	    es = max((long)th.getExecutionTime() + rhs - rijm1, 0L);
	  }
	  assert(es >= 0);

	  // e_t = \pi_{ht} min\{C_h, r_{ij} - r_{ht}\}
	  // e_t is amount of \tau_h instance t that falls inside
	  // [r_{i(j-1)},r_ij] with r_{ht} > r_{ij}

	  // t is instance that is still released in the interval,
	  // but not finished inside. The special case where t starts
	  // exactly at lIntervalHigh is automatically covered by the
	  // calculation below
	  long et = 0;
	  if ( t >= 0 && th.isJobMandatory(t)) {
	    TmsTimeInterval rij = x + ti.getPeriod(); //ti.getOffset() + (j + 1) * ti.getPeriod();
	    TmsTimeInterval rht = /*th.getOffset() +*/ t * th.getPeriod();
	    assert(rij >= rht);
	    et = min(th.getExecutionTime(), rij - rht);
	    // if job t starts at lIntervalHigh, rij = rht.
	  }
	  assert(et >= 0);

	  // F^h_{ij} = e_s + l^h_{ij} C_i + e_t
	  // this should be *C_h*
	  unsigned int fhij = es + lhij * th.getExecutionTime() + et;
	  LOG(LOG_CLASS_EVAL_MK) << "\tes: " << es
				 << " et: " << et
				 << " lhij: " << lhij
				 << " fhij: " << fhij;
	  // if F_i^h < F_{ij}^h
	  if (fhi < fhij) {
	    // F_i^h = F_{ij}^h
	    fhi = fhij;
	  }
	  // x = x + g
	  x += g;
	} // end while
      } // end if
    } // end for
    assert(fhi >= 0);
    return fhi;
  }


  /// @todo move to utils
  unsigned long MkGenerator::calcGCD(unsigned long _a, unsigned long _b) {
    // use euclid's algorithm
    unsigned long a = max(_a, _b);
    unsigned long b = min(_a, _b);
    unsigned long tmp;
    if (b == 0)
      return a;

    while (b != 0) {
      tmp = a % b;
      a = b;
      b = tmp;
    }
    return a;
  }

  // this function is not used anymore, and its implementation is wrong anyway
  // -> remove in future!
  /*
  unsigned int MkGenerator::calcLhij(const MkpTask& task, unsigned int low, unsigned int high) {

    unsigned int first = floor( ( (double) low - (double) task.getOffset() )
				/ (double) task.getPeriod() ) + 1;
    unsigned int last = floor ( ( (double) high - (double) task.getOffset() )
				/ (double) task.getPeriod() );

    unsigned int ctr = 0;

    for (unsigned int i = first; i <= last; ++i) {
      if (task.isJobMandatory(i))
	++ctr;
    }
    return ctr;
  }
  */

} // NS tmssim
