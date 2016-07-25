/**
 * $Id: dependencymatrix.h 1407 2016-05-31 07:30:51Z klugeflo $
 * @file dependencymatrix.h
 * @brief Creation of the dependencies between sensor and processor tasks
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TSEVAL_DEPENDENCYMATRIX_H
#define TSEVAL_DEPENDENCYMATRIX_H 1

#include <utils/random.h>

#include <cstdint>
#include <cstddef>

#include <ostream>



namespace tmssim {

  #define MAX_COLUMNS 64
  #define MAX_ROWS 64
  
  class DependencyMatrix {
  public:
    /**
     * @brief Representation of one row of the matrix.
     * This class is introduced to enable the well-known access to the
     * #DependencyMatrix via offset operators ([row][col])
     */
    class Row {
    public:
      Row(size_t _columns, uint64_t _row);
      unsigned int operator[](size_t column) const;
    private:
      size_t columns;
      uint64_t row;
    };

    /**
     * @brief Create a new matrix.
     * @param _rows
     * @param _columns
     * @param _maxRowSum the actual row sum is from [1, _maxRowSum]
     * @param _random an initialised random number generator
     */
    DependencyMatrix(size_t _rows, size_t _columns, unsigned int _maxRowSum, Random& _random);
    //~DependencyMatrix();

    Row operator[](size_t row) const;
    uint64_t getRow(size_t row) const;

    size_t getColumns() const { return columns; }
    size_t getRows() const { return rows; }
    unsigned int getRowSum(size_t row) const;
    
    unsigned int getSeed() const { return seed; }

    friend std::ostream& operator<<(std::ostream& ost, const DependencyMatrix& dm);
  private:

    void generateMatrix(Random &random);
    bool checkColumnSums();

    static uint64_t binomialCoefficient(unsigned int n, unsigned int k);

    uint64_t getAssignment(size_t length, size_t nEntries, unsigned int num);
    void fillBuckets(uint64_t& bucket, size_t n, size_t offset, size_t theK, int& ctr);

    
    size_t rows;
    size_t columns;
    unsigned int maxRowSum;

    unsigned int seed;
    
    // actually, we might save some memory by using dynamic allocation, but
    // we'll rather go for speed
    //uint64_t* matrix;
    uint64_t matrix[MAX_ROWS];
    unsigned int rowSums[MAX_ROWS];
    
  };


  std::ostream& operator<<(std::ostream& ost, const DependencyMatrix& dm);
  
} // NS tmssim

#endif // !TSEVAL_DEPENDENCYMATRIX_H
