/**
 * $Id: bitmap.h 1421 2016-06-22 07:46:32Z klugeflo $
 * @file bitmap.h
 * @brief Bitmap with arbitrary size
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_BITMAP_H
#define UTILS_BITMAP_H 1

#include <cstddef>
#include <cstdint>
#include <string>


namespace tmssim {

#define ELEM_EXP 6
#define ELEM_SIZE (1 << ELEM_EXP) // 64
#define ELEM_SIZE_MASK (ELEM_SIZE - 1) // 63
  

  class Bitmap {
  public:
    /**
     * convenient manipulation of bits
     */
    class BitmapManipulator {
    public:
      BitmapManipulator(Bitmap* _bitmap, size_t _offset);
      //~BitmapManipulator();
      unsigned operator=(unsigned bit);
      operator unsigned();
    private:
      Bitmap* bitmap;
      size_t offset;
    };

    Bitmap(size_t _size);
    ~Bitmap();

    size_t getSize() const { return size; };

    unsigned operator[](size_t offset) const;

    BitmapManipulator operator[](size_t offset);


    bool all() const;
    
    std::string str() const;

  protected:
    void setBit(size_t offset, unsigned value);
    
  private:
    size_t size; ///< number of bits
    size_t length; ///< length of storage array
    uint64_t* bits; ///< storage array; if changed, adjust also #BITS_PER_ELEMENT
  };
  

} // NS tmssim


#endif // !UTILS_BITMAP_H
