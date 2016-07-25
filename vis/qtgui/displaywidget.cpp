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
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holder.
 */

/**
 * $Id: displaywidget.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file displaywidget.cpp
 * @brief Display schedule
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "displaywidget.h"

#include <QDebug>
//#include <QMouseEvent>
#include <QPainter>

#include <iostream>
#include <typeinfo>


using namespace std;

//#define STEP_WIDTH 8
//#define BAR_HEIGHT 16
//#define LINE_HEIGHT 40
//#define ARROW_LEN 10
//#define ARROW_AR 3
//#define COORD_OFFSET 40
//#define LEGEND_WIDTH 30
//#define Y_OFFSET 30
//#define Y_SEP 10


DisplayWidget::DisplayWidget(const DisplayDimension& _dDim, const DisplayModel* _dModel, QWidget* parent)
  : QWidget(parent), dDim(_dDim), dModel(_dModel), groupSize(-1),
    maxX(0), maxY(0), xBar(0) {
  //setMinimumSize(0, 0);
  //setStep(0);
}


void DisplayWidget::update() {
  //qDebug() << "DW::update()";
  adjustGeometry();
  QWidget::update();
}


void DisplayWidget::setStep(unsigned long step) {
  if (step > dModel->getTMax())
    return;

  xBar = step;
  stepSelected(xBar);
  update();
}


unsigned long DisplayWidget::getXBar() const {
  return xBar;
}


void DisplayWidget::setModel(const DisplayModel* _dModel) {
  dModel = _dModel;
}


void DisplayWidget::adjustGeometry() {
  adjustTMax();
  adjustMaxX();
  adjustMaxY();
  setMinimumSize(maxX, maxY);
}


void DisplayWidget::adjustMaxX() {
  maxX = dDim.getXOffset() + (tMax+2) * dDim.getBarWidth();
  //qDebug() << "maxX=" << maxX;
}


void DisplayWidget::adjustMaxY() {
  int y = 0;
  for (DisplayModelEntry* dme : *dModel) {
    switch (dme->type) {
    case DisplayModelEntry::HEAD:
      y += dDim.getYOffset();
      break;
      
    case DisplayModelEntry::SEP:
      y += dDim.getSeparatorHeight();
      break;

    case DisplayModelEntry::TASK:
      y += dDim.getTaskHeight();
      break;
      
    default:
      qDebug() << "Invalid DisplayModelEntry:" << dme->type;
    }
  }
  maxY = y;
  //qDebug() << "maxY=" << maxY;

}


void DisplayWidget::adjustTMax() {
  tMax = dModel->getTMax();
  //qDebug() << "tMax=" << tMax;
}


void DisplayWidget::mouseMoveEvent(QMouseEvent* event) {
  int x = event->pos().x();
  long step = (x - dDim.getXOffset()) / dDim.getBarWidth();
  unsigned long realStep = (step > 0) ? step : 0;
  //qDebug() << "Mouse clicked at x=" << x << "t=" << step << "real_t=" << realStep;

  setStep(realStep);
}


void DisplayWidget::paintEvent(QPaintEvent *event) {
  //qDebug() << "DW::paintEvent";
  Q_UNUSED(event);
  QPainter qp(this);

  if (xBar >= 0) {
    QPen pen(Qt::blue);
    pen.setWidth(2);
    qp.setPen(pen);
    int x = dDim.getXOffset() + xBar * dDim.getBarWidth();
    qp.drawLine(x, dDim.getYOffset(), x, maxY);
  }
  
  int yb = 0;
  for (const DisplayModelEntry* dme : *dModel) {
    switch (dme->type) {
    case DisplayModelEntry::HEAD:
      //qDebug() << "\tHEAD";
      yb += paintHead(&qp);
      break;
      
    case DisplayModelEntry::SEP:
      //qDebug() << "\tSEP";
      yb += dDim.getSeparatorHeight();
      // TODO: Write Group name
      break;

    case DisplayModelEntry::TASK:
      //qDebug() << "\tTASK";
      // TODO: exclude label
      paintSchedule(&qp, dme, yb);
      yb += dDim.getTaskHeight();
      break;
      
    default:
      qDebug() << "Invalid DisplayModelEntry:" << dme->type;
    }
  }
  //qDebug() << "DW y:" << yb;
}


int DisplayWidget::paintHead(QPainter* qp) {
  int yb = dDim.getYOffset();
  qp->setPen(Qt::darkGray);
  for (unsigned int i = 0; i < tMax; i += 10) {
    int x = dDim.getXOffset() + i * dDim.getBarWidth();
    qp->drawLine(x, yb, x, maxY); //yb + tasks.size() * dDim.getTaskHeight());
    qp->drawText(x, yb - 10, QString::number(i));
  }
  return yb;// + dDim.getSeparatorHeight();
}


#define BOOL2B(b) ((b)?1:0)
QDebug& operator << (QDebug& ost, JobInfo ji) {
  ost << "{" << ji.jobId << "@" << ji.time
      << " ";
  for (size_t i = 0; i < JobInfo::LAST; ++i) {
    ost << BOOL2B(ji.event[i]);
  }
  ost << " "
      << ji.execState
      << " \""
      << ji.taskState.c_str()
      << "\"}";
  return ost;
}

// TODO: change interface: only pass painter, DisplayModelEntry and y0
void DisplayWidget::paintSchedule(QPainter *qp, const DisplayModelEntry* dme, int y0) {

  const JobInfoList& trace = *dme->trace;
  
  //qDebug() << "Painting task " << dme->label;
  //qp->setPen(Qt::black);
  //qp->drawText(10, y0+10, QString(taskId.c_str()));
  int x = dDim.getXOffset();
  for (JobInfoList::const_iterator it = trace.begin(); it != trace.end(); ++it) {

    const JobInfo* ji = *it;
    //qDebug() << "\t" << ji;

    // first paint events

    if (ji->getEvent(JobInfo::ACTIVATE)) {
      drawActivation(qp, x, y0);
    }
    if (ji->getEvent(JobInfo::START)) {
      drawStart(qp, x, y0);
    }
    if (ji->getEvent(JobInfo::PREEMPT)) {
      drawPreemption(qp, x, y0);
    }
    if (ji->getEvent(JobInfo::RESUME)) {
      drawResume(qp, x, y0);
    }
    if (ji->getEvent(JobInfo::FINISH)) {
      drawCompletion(qp, x, y0);
    }
    if (ji->getEvent(JobInfo::CANCEL)) {
      drawCancel(qp, x, y0);
    }

    // then paint states
    JobInfoList::const_iterator next = it;
    next++;

    int tEnd;
    if (next == trace.end())
      tEnd = tMax;
    else
      tEnd = (*next)->time;

    int width = (tEnd - ji->time) * dDim.getBarWidth();

    // need only to paint relevant states
    if (ji->execState != JobInfo::NONE) {
      QColor color;
      if (ji->execState == JobInfo::READY) {
	color = Qt::yellow;
	//qDebug() << "\tReadying " << taskId.c_str() << "," << ji->jobId << " until " << tEnd << "(x=" << x << " w=" << width << "[" << ji->time << "," << tEnd << "])";
      }
      else if (ji->execState == JobInfo::EXECUTE) {
	color = Qt::green;
	//qDebug() << "\tExecuting " << taskId.c_str() << "," << ji->jobId << " until " << tEnd << "(x=" << x << " w=" << width << "[" << ji->time << "," << tEnd << "])";
      }
      else {
	color = Qt::red;
	qDebug() << "Unknown state " << ji->execState << " in task " << dme->label;
      }
      
      qp->fillRect(x, y0, width, dDim.getBarHeight(), color);
    }
    x += width;
  }
}


void DisplayWidget::drawActivation(QPainter *qp, int x, int y) {
  qp->setPen(Qt::black);
  qp->drawLine(x, y, x, y - dDim.getArrowLengthTotal());
  qp->drawLine(x, y, x - dDim.getArrowLengthFlank(), y - dDim.getArrowLengthFlank());
  qp->drawLine(x, y, x + dDim.getArrowLengthFlank(), y - dDim.getArrowLengthFlank());
}


void DisplayWidget::drawStart(__attribute__((unused)) QPainter *qp, __attribute__((unused)) int x, __attribute__((unused)) int y) {
}


void DisplayWidget::drawPreemption(__attribute__((unused)) QPainter *qp, __attribute__((unused)) int x, __attribute__((unused)) int y) {
}


void DisplayWidget::drawResume(__attribute__((unused)) QPainter *qp, __attribute__((unused)) int x, __attribute__((unused)) int y) {
}


void DisplayWidget::drawCompletion(QPainter *qp, int x, int y) {
  qp->setPen(Qt::black);
  int yu = y - dDim.getArrowLengthTotal();
  qp->drawLine(x, y, x, y - dDim.getArrowLengthTotal());
  qp->drawLine(x, yu, x - dDim.getArrowLengthFlank(), yu + dDim.getArrowLengthFlank());
  qp->drawLine(x, yu, x + dDim.getArrowLengthFlank(), yu + dDim.getArrowLengthFlank());
}


void DisplayWidget::drawCancel(QPainter *qp, int x, int y) {
  qp->setPen(Qt::red);
  int xu = x + dDim.getArrowLengthTotal();
  int yu = y - dDim.getArrowLengthTotal();
  qp->drawLine(x, y, xu, yu);
  qp->drawLine(xu, yu, xu - dDim.getArrowLengthFlank(), yu);
  qp->drawLine(xu, yu, xu, yu + dDim.getArrowLengthFlank());
}


