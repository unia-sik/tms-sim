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
 * $Id: guard.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file guard.h
 * @brief Makros to define a guard class for singleton classes
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_GUARD_H
#define UTILS_GUARD_H 1

#include <cstddef>

namespace tmssim {

    /**
     * Guard for cleaning up memory
     * idea plucked from http://www.oop-trainer.de/Themen/Singleton.html
     */
#define DECLARE_GUARD(GuardedClass)		\
  class Guard {					\
  public:					\
    ~Guard() {					\
      if (GuardedClass::_instance != NULL) {	\
	delete GuardedClass::_instance;		\
      }						\
    }						\
  };						\
  friend class Guard;

#define DEFINE_GUARD(GuardedClass)			\
  static GuardedClass::Guard guard;
  
  //#define DECLARE_GUARD(guarded_class) friend class Guard<guarded_class>;
  //#define DEFINE_GUARD(guarded_class) static guarded_class::Guard<guarded_class> guard;

} // NS tmssim

#endif // !UTILS_GUARD_H
