/**
 * $Id: bitmap.cpp 1421 2016-06-22 07:46:32Z klugeflo $
 * @file bitmap.cpp
 * @brief Bitmap with arbitrary size
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/bitmap.h>

#include <utils/tmsexception.h>

#include <sstream>

using namespace std;

namespace tmssim {

  /*
  Bitmap::BitmapManipulator::~BitmapManipulator() {
  }
  */
  
  unsigned Bitmap::BitmapManipulator::operator=(unsigned bit) {
    bitmap->setBit(offset, bit);
    return bit != 0;
  }

  
  Bitmap::BitmapManipulator::BitmapManipulator(Bitmap* _bitmap, size_t _offset)
    : bitmap(_bitmap), offset(_offset) {
  }


  Bitmap::BitmapManipulator::operator unsigned() {
    return (*(const_cast<const Bitmap*>(bitmap)))[offset];
  }
  

  Bitmap::Bitmap(size_t _size)
    : size(_size) {
    length = ( (size + ELEM_SIZE_MASK) & ~ELEM_SIZE_MASK ) >> ELEM_EXP;

    bits = new uint64_t[length];
    for (size_t i = 0; i < length; ++i) {
      bits[i] = 0LL;
    }

    if ( (size & ELEM_SIZE_MASK) != 0) {
      unsigned upper = size & ELEM_SIZE_MASK;
      uint64_t mask = (1LL << upper) - 1;
      bits[length - 1] = ~mask;
    }
  }

  
  Bitmap::~Bitmap() {
    delete[] bits;
  }

  
  unsigned Bitmap::operator[](size_t offset) const {
    size_t realOffset = offset >> ELEM_EXP;
    size_t bitOffset = offset & ELEM_SIZE_MASK;
    uint64_t bit = 1LL << bitOffset;
    return ( (bits[realOffset] & bit) != 0 );
  }
  
  
  Bitmap::BitmapManipulator Bitmap::operator[](size_t offset) {
    return BitmapManipulator(this, offset);
  }
  

  bool Bitmap::all() const {
    uint64_t full = -1ULL;
    for (size_t i = 0; i < length; ++i) {
      if (bits[i] != full)
	return false;
    }
    return true;
  }
  

  string Bitmap::str() const {
    ostringstream oss;

    if (length > 1) {
      for (size_t i = 0; i < length - 1; ++i) {
	for (size_t j = 0; j < ELEM_SIZE; ++j) {
	  uint64_t bit = 1LL << j;
	  if (bits[i] & bit) {
	    oss << "1";
	  }
	  else {
	    oss << "0";
	  }
	}
      }
    }
    for (size_t j = 0; j < (size & ELEM_SIZE_MASK); ++j) {
	  uint64_t bit = 1LL << j;
	  if (bits[length - 1] & bit) {
	    oss << "1";
	  }
	  else {
	    oss << "0";
	  }
    }
    
    return oss.str();
  }


  void Bitmap::setBit(size_t offset, unsigned value) {
    if (offset >= size) {
      ostringstream oss;
      oss << "Invalid offset in bitmap access: " << offset;
      throw TMSException(oss.str());
    }
    else {
      size_t realOffset = offset >> ELEM_EXP;
      size_t bitOffset = offset & ELEM_SIZE_MASK;
      uint64_t bit = 1LL << bitOffset;
      if (value == 0) {
	bits[realOffset] &= ~bit;
      }
      else {
	bits[realOffset] |= bit;
      }
    }
  }


} // NS tmssim
