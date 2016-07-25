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
 * $Id: tmsexception.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file tmsexception.cpp
 * @brief General exception
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_TMSEXCEPTION_H
#define UTILS_TMSEXCEPTION_H 1

#include <string>

#define THROW_UNIMPLEMENTED throw tmssim::TMSException(__PRETTY_FUNCTION__ + string(" not yet implemented!"))

namespace tmssim {

  class TMSException {
  public:
    TMSException(std::string msg);
    virtual ~TMSException();

    const std::string& getMessage() const;

  private:
    std::string message;
  };

} // NS tmssim

#endif // !UTILS_TMSEXCEPTION_H
