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
 * $Id: defaulttaskstatedecoder.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file defaulttaskstatedecoder.h
 * @brief A simple default decoder
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MODEL_DEFAULTTASKSTATEDECODER_H
#define MODEL_DEFAULTTASKSTATEDECODER_H

#include "taskstatedecoder.h"


class DefaultTaskStateDecoder: virtual public TaskStateDecoder {
 public:
  DefaultTaskStateDecoder(const std::string& __taskDescription);
  DefaultTaskStateDecoder(const DefaultTaskStateDecoder& rhs);

  virtual void setState(const std::string& _state);

};


#endif // !MODEL_DEFAULTTASKSTATEDECODER_H
