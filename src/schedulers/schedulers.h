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
 * $Id: schedulers.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file schedulers.h
 * @brief Single include file for all TMS schedulers
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_SCHEDULERS_H
#define SCHEDULERS_SCHEDULERS_H 1

#include <schedulers/ald.h>
#include <schedulers/beedf.h>
#include <schedulers/dbpedf.h>
#include <schedulers/dummy.h>
#include <schedulers/dvdedf.h>
#include <schedulers/edf.h>
#include <schedulers/fpp.h>
//#include <schedulers/fppnat.h>
#include <schedulers/gdpa.h>
#include <schedulers/gdpas.h>
#include <schedulers/gmuamk.h>
#include <schedulers/hcedf.h>
#include <schedulers/mkuedf.h>
#include <schedulers/oedfmax.h>
#include <schedulers/oedfmin.h>
#include <schedulers/phcedf.h>

#endif /* SCHEDULERS_SCHEDULERS_H */
