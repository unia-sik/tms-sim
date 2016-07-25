/**
 * $Id: bitmap.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file bitmap.cpp
 * @brief Test of bitmap class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <iostream>
#include <utils/bitmap.h>

using namespace std;
using namespace tmssim;


int main() {
  Bitmap map(77);

  cout << map.str() << endl;

  for (size_t i = 0; i < 77; i += 2) {
    map[i] = 1;
  }

  cout << map.str() << endl;
  
  
  return 0;
}

