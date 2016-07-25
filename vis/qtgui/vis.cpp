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
 * $Id: vis.cpp 1360 2016-02-19 09:34:18Z klugeflo $
 * @file vis.cpp
 * @brief Visualisation main
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <QtGui>

#include "displaywidget.h"
#include "viswindow.h"

/*
#include <iostream>
#include <string>
using namespace std;
#include <clocale>
*/

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  if (argc < 2) {
    qDebug() << "No log file parameter found, exiting...";
    return -1;
  }
  // TODO: allow both
  //else {
  /*
  qDebug() << "Using logfile " << argv[1];
  LogFileReader lfr(argv[1]);
  */
  //}

  /*
  string currentLocale = setlocale(LC_ALL, NULL);
  cout << "Current locale: " << currentLocale << endl;
 
  string newLocale = setlocale(LC_ALL, "C");
  cout << "New locale: " << newLocale << endl;

  string s1 = "1.57";
  string s2 = "2,75";
  cout << "s1: " << s1 << " s2: " << s2 << endl;
  cout << "stod(s1): " << stod(s1) << " stod(s2): " << stod(s2) << endl;

  string oldLocale = setlocale(LC_ALL, currentLocale.c_str());
  cout << "Current locale: " << oldLocale << endl;
  */
  
  QStringList cmdline_args = QCoreApplication::arguments();
  
  QStringList files;
  QStringList::iterator it = cmdline_args.begin();
  ++it; // ignore program name
  while (it != cmdline_args.end()) {
    files.push_back(*it);
    ++it;
  }


  VisWindow* w = new VisWindow(files);
  w->show();

  return app.exec();
}

/*
Locale problem:
s1: 1.57 s2: 2,75
stod(s1): 1 stod(s2): 2.75

 */
