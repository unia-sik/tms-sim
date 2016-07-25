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
 * $Id: tswconstants.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file tswconstants.h
 * @brief Constants for width management in task state widgets
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

namespace Tsw {

  enum TswWidgetId {
    TaskId = 0,

    DefaultState = 1,

    //MkState = 1,
    MkMk = 1,
    MkStream = 2,
    MkDist = 3,
    MkUtil = 4
    
  };
  
} // namespace Tsw
