/**
 * $Id: dummymkcts.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file dummymkcts.h
 * @brief Dummy CTS that does not perform any simulation, but uses the
 * results of another CTS.
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_DUMMYMKCTS_H
#define MKEVAL_DUMMYMKCTS_H 1

#include <mkeval/concretemktaskset.h>

#include <utils/tmsexception.h>

namespace tmssim {

  class DummyConcreteMkTaskset : public ConcreteMkTaskset {
  public:
    DummyConcreteMkTaskset(MkTaskset* _mkTaskset,
			   const vector<const MkEvalAllocatorPair*>& _allocators,
			   ConcreteMkTaskset* _cts);
    
    ~DummyConcreteMkTaskset();

    const vector<MkSimulation*>& getMkSimulations() const {
      if (!cts->isFinished()) {
	throw TMSException("Tried to get data before base simulation is finished!");
      }
      return cts->getMkSimulations();
    }


  private:

    ConcreteMkTaskset* cts;
    //MkSimulation* mySimulation;
  };

} // NS tmssim

#endif // !MKEVAL_DUMMYMKCTS_H
