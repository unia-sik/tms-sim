/**
 * $Id: uanone.h 1407 2016-05-31 07:30:51Z klugeflo $
 * @file uanone.cpp
 * @brief An empty implementation of a utility aggregator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILITY_UANONE_H
#define UTILITY_UANONE_H 1

#include <core/utilityaggregator.h>

namespace tmssim {

  class UANone : public UtilityAggregator {
  public:
    UANone();
    
    virtual double getCurrentUtility(void) const;
    
    /**
     * @brief Predict the HCUF value for the next step.
     *
     * This function calculates the value of the HCUF under the assumption
     * that a certain utility value is added to the history.
     * @param u Utility value to be added
     * @return the possible HCUF value
     */
    virtual double predictUtility(double u) const;
    

    /**
     *
     */
    virtual UtilityAggregator* clone() const;

        /**
     * @name XML
     * @{
     */

    virtual int writeData(xmlTextWriterPtr writer);
    virtual const std::string& getClassElement();

    /**
     * @}
     */

  protected:
    virtual void addHook(double u);
	
  };


} // NS tmssim

#endif // !UTILITY_UANONE_H
