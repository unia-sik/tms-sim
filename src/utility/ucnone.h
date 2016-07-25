/**
 * $Id: ucnone.h 1407 2016-05-31 07:30:51Z klugeflo $
 * @file ucnone.h
 * @brief An empty implementation of a utility calculator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILITY_UCNONE_H
#define UTILITY_UCNONE_H 1

#include <core/utilitycalculator.h>

namespace tmssim {

  class UCNone : public UtilityCalculator {
  public:
    virtual double calcUtility(const Job *job, TmsTime complTime) const;
    virtual UtilityCalculator* clone() const;

    /**
     * @name XML
     * @{
     */

    virtual int writeData(xmlTextWriterPtr writer);

    virtual const std::string& getClassElement();

    /**
     * @}
     */

  };

} // NS tmssim

#endif // !UTILITY_UCNONE_H
