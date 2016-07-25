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
 * $Id: mktaskstatedecoder.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file mktaskstatedecoder.h
 * @brief Decoding of (m,k)-tasks
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MODEL_MKTASKSTATEDECODER_H
#define MODEL_MKTASKSTATEDECODER_H

#include "taskstatedecoder.h"

class MkTaskStateDecoder: virtual public TaskStateDecoder {
 public:
  MkTaskStateDecoder(std::string __taskDescription);
  MkTaskStateDecoder(const MkTaskStateDecoder& rhs);
  virtual ~MkTaskStateDecoder();
  
  virtual void setState(const std::string& _state);

  unsigned getPeriod() const { return period; }
  unsigned getExecutionTime() const { return executionTime; }
  unsigned getDeadline() const { return deadline; }
  unsigned getM() const { return m; }
  unsigned getK() const { return k; }
  unsigned getSpin() const { return spin; }

  const std::string& getMkStream() const { return mkstream; }
  unsigned getStreamSum() const { return streamSum; }
  unsigned getDistance() const { return distance; }
  double getUCurrent() const { return uCurrent; }
  double getUPredict() const { return uPredict; }
  
 protected:

 private:
  unsigned period;
  unsigned executionTime;
  unsigned deadline;
  unsigned m;
  unsigned k;
  unsigned spin;
  
  std::string mkstream;
  unsigned streamSum;
  unsigned distance;
  double uCurrent;
  double uPredict;
};


#endif // !MODEL_MKTASKSTATEDECODER_H
