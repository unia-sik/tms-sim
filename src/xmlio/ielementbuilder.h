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
 * $Id: ielementbuilder.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file ielementbuilder.h
 * @brief
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef IELEMENTBUILDER_H
#define IELEMENTBUILDER_H 1

/**
 * Includes
 */
#include <libxml/parser.h>

namespace tmssim {

/**
 * Template interface class to build build a generic object parsed out of a XML-document
 */
template <class T>
class IElementBuilder {

public:

	/**
	 * Builds an element
	 * @param doc The parsed document
	 * @param cur The current position within the document
	 * @return A pointer to the created element
	 */
	virtual T* build(xmlDocPtr doc, xmlNodePtr cur) = 0;

	/**
	 * D'tor
	 */
	virtual ~IElementBuilder() {}
};

} // NS tmssim

#endif /* IELEMENTBUILDER_H */
