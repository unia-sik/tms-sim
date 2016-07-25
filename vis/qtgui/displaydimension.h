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
 * $Id: displaydimension.h 1358 2016-02-17 16:18:43Z klugeflo $
 * @file displaydimension.h
 * @brief Dimension parameters for display of schedules
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef DISPLAYDIMENSION_H
#define DISPLAYDIMENSION_H



class DisplayDimension {
 public:
  /**
   * @param _barWidth width of schedule bar for one time step
   * @param _barHeight height of schedule bar
   * @param _taskHeight height of a task's schedule line
   * @param _arrowLength1 total length of an arrow
   * @param _arrowLength2 length of an arrow's flanks
   * @param _xOffset offset to left
   * @param _yOffset offset to top
   * @param _ySep Y distance between two tasks
   * @param _separatorHeight distance between two groups
   */
  DisplayDimension(int _barWidth = 8, int _barHeight = 16, int _taskHeight = 40,
		   int _arrowLengthTotal = 10, int _arrowLengthFlank = 3,
		   int _xOffset = 10, int _yOffset = 30, int _ySep = 10,
		   int _separatorHeight = 40, int _legendWidth = 250)
    : barWidth(_barWidth), barHeight(_barHeight), taskHeight(_taskHeight),
    arrowLengthTotal(_arrowLengthTotal), arrowLengthFlank(_arrowLengthFlank),
    xOffset(_xOffset), yOffset(_yOffset), ySep(_ySep),
    separatorHeight(_separatorHeight), legendWidth(_legendWidth)
    {}

  void setBarWidth(int _barWidth) {
    barWidth = _barWidth;
  }
  
  int getBarWidth() const {
    return barWidth;
  }
  
  void setBarHeight(int _barHeight) {
    barHeight = _barHeight;
  }
  
  int getBarHeight() const {
    return barHeight;
  }
  
  void setTaskHeight(int _taskHeight) {
    taskHeight = _taskHeight;
  }
  
  int getTaskHeight() const {
    return taskHeight;
  }
  
  void setArrowLengthTotal(int _arrowLengthTotal) {
    arrowLengthTotal = _arrowLengthTotal;
  }
  
  int getArrowLengthTotal() const {
    return arrowLengthTotal;
  }
  
  void setArrowLengthFlank(int _arrowLengthFlank) {
    arrowLengthFlank = _arrowLengthFlank;
  }

  int getArrowLengthFlank() const {
    return arrowLengthFlank;
  }
  
  void setXOffset(int _xOffset) {
    xOffset = _xOffset;
  }
  
  int getXOffset() const {
    return xOffset;
  }
  
  void setYOffset(int _yOffset) {
    yOffset = _yOffset;
  }
  
  int getYOffset() const {
    return yOffset;
  }
  
  void setYSep(int _ySep) {
    ySep = _ySep;
  }
  
  int getYSep() const {
    return ySep;
  }
  
  void setSeparatorHeight(int _separatorHeight) {
    separatorHeight = _separatorHeight;
  }
  
  int getSeparatorHeight() const {
    return separatorHeight;
  }

  void setLegendWidth(int _legendWidth) {
    legendWidth = _legendWidth;
  }

  int getLegendWidth() const {
    return legendWidth;
  }
  
 private:
  int barWidth;
  int barHeight;
  int taskHeight;
  int arrowLengthTotal;
  int arrowLengthFlank;
  int xOffset;
  int yOffset;
  int ySep;
  int separatorHeight;
  int legendWidth;
};


#endif // !DISPLAYDIMENSION_H
