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
 * $Id: mktaskstatewidget.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file mktaskstatewidget.h
 * @brief Display of (m,k)-task state
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include "mktaskstatewidget.h"

#include "tswconstants.h"


MkTaskStateWidget::MkTaskStateWidget(const MkTaskStateDecoder& mkdecoder, QWidget* parent)
  : TaskStateDecoder(mkdecoder), MkTaskStateDecoder(mkdecoder), TaskStateWidget(mkdecoder, parent)
{
  //adjustStateString();
  //lState = new QLabel("No state yet");
  //addWidget(lState);

  QString mk = QString("(").append(QString::number(getM())).append(",")
    .append(QString::number(getK())).append(")");
  lMk = new QLabel(mk);
  addWidget(lMk);
  lStream = new QLabel;
  addWidget(lStream);
  lDist = new QLabel;
  addWidget(lDist);
  lUtil = new QLabel;
  addWidget(lUtil);
}


void MkTaskStateWidget::setState(const std::string& _state) {
  //qDebug() << "New state for MkTaskStateWidget of task" << getTaskId().c_str();
  MkTaskStateDecoder::setState(_state);
  /*adjustStateString();
    lState->setText(stateString);*/
  lStream->setText(getMkStream().c_str());
  lDist->setText(QString::number(getDistance()));
  lUtil->setText(QString::number(getUCurrent(), 'g'));
  update();
  /*
  qDebug() << "mk:" << lMk->geometry() << lMk->sizeHint();
  qDebug() << "stream:" << lStream->geometry() << lStream->sizeHint();
  qDebug() << "dist:" << lDist->geometry() << lDist->sizeHint();
  qDebug() << "util:" << lUtil->geometry() << lUtil->sizeHint();
  */
}


void MkTaskStateWidget::findMaxWidths(QMap<int,int>& maxWidths) const {
  TaskStateWidget::findMaxWidths(maxWidths);
  //findMaxWidth(maxWidths, lState, Tsw::MkState);
  findMaxWidth(maxWidths, lMk, Tsw::MkMk);
  findMaxWidth(maxWidths, lStream, Tsw::MkStream);
  findMaxWidth(maxWidths, lDist, Tsw::MkDist);
  findMaxWidth(maxWidths, lUtil, Tsw::MkUtil);
}


void MkTaskStateWidget::adjustWidths(const QMap<int,int>& maxWidths) {
  TaskStateWidget::adjustWidths(maxWidths);
  //adjustWidth(lState, maxWidths[Tsw::MkState]);
  adjustWidth(lMk, maxWidths[Tsw::MkMk]);
  adjustWidth(lStream, maxWidths[Tsw::MkStream]);
  adjustWidth(lDist, maxWidths[Tsw::MkDist]);
  adjustWidth(lUtil, maxWidths[Tsw::MkUtil]);
}


void MkTaskStateWidget::adjustStateString() {
  stateString = "(";
  stateString.append(QString::number(getM()));
  stateString.append(",");
  stateString.append(QString::number(getK()));
  stateString.append(")");
  stateString.append(getMkStream().c_str());
  //qDebug() << "\t" << stateString;
}
