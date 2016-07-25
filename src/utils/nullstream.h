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
 * $Id: nullstream.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file nullstream.h
 * @brief Stream with no output
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_NULLSTREAM_H
#define UTILS_NULLSTREAM_H 1

#include <iostream>
#include <mutex>

namespace tmssim {

  /**
   * we need a null stream, therefore we use the implementation by James Kanze,
   * see http://stackoverflow.com/questions/8243743/is-there-a-null-stdostream-implementation-in-c-or-libraries
   */
  class NullStreambuf : public std::streambuf {
  protected:
    virtual int overflow(int c);
  private:
    char dummyBuffer[ 64 ];
  };

  extern std::ostream nullStream;
  
  /*
  class _NullOStream : private NullStreambuf, public std::ostream {
  public:
    _NullOStream() : std::ostream(this) {}
    NullStreambuf* rdbuf() const { return this; }
  };
  */


} // NS tmssim

#endif // !UTILS_NULLSTREAM_H
