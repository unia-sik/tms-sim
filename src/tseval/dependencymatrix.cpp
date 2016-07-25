/**
 * $Id: dependencymatrix.cpp 1407 2016-05-31 07:30:51Z klugeflo $
 * @file dependencymatrix.cpp
 * @brief Creation of the dependencies between sensor and processor tasks
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <tseval/dependencymatrix.h>
#include <utils/tmsexception.h>

#include <cassert>

using namespace std;

namespace tmssim {

  DependencyMatrix::Row::Row(size_t _columns, uint64_t _row)
    : columns(_columns), row(_row) {
    if (_columns > MAX_COLUMNS)
      throw TMSException("Columns limit exceeded");
  }

  
  unsigned int DependencyMatrix::Row::operator[](size_t column) const {
    if (column >= columns)
      throw TMSException("Columns limit exceeded");
    else {
      return ((row & (1LL << column)) != 0);
    }
  }


  DependencyMatrix::DependencyMatrix(size_t _rows, size_t _columns, unsigned int _maxRowSum, Random& _random)
    : rows(_rows), columns(_columns), maxRowSum(_maxRowSum)
  {
    if ( (_rows > MAX_ROWS) || (_columns > MAX_COLUMNS)
	 || (_maxRowSum > _columns) || (_maxRowSum * _rows < _columns) )
      throw TMSException(string("Invalid Parameters on Construction in") + __PRETTY_FUNCTION__);

    generateMatrix(_random);
  }

  
  DependencyMatrix::Row DependencyMatrix::operator[](size_t row) const {
    if (row >= rows)
      throw TMSException("Rows limit exceeded");
    else {
      return Row(columns, matrix[row]);
    }
  }

  
  uint64_t DependencyMatrix::getRow(size_t row) const {
    if (row >= rows)
      throw TMSException("Rows limit exceeded");
    else {
      return matrix[row];
    }
  }


  unsigned int DependencyMatrix::getRowSum(size_t row) const {
    if (row >= rows)
      throw TMSException("Rows limit exceeded");
    else
      return rowSums[row];
  }

  
  void DependencyMatrix::generateMatrix(Random& random) {
    uint64_t relevant_bits = (1LL << columns) - 1;
    //printf("Relevant bits: %lx\n", relevant_bits);
    // generate the matrix row-wise
    int colSumOk = 0;
    unsigned ctr = 0;
    while (!colSumOk) {
      //putchar('*');
      ++ctr;
      seed = random.getCurrentSeed();
      for (size_t r = 0; r < rows; ++r) {
	rowSums[r] = random.getIntervalRand(1, maxRowSum);
	unsigned long long nCombinations = binomialCoefficient(columns, rowSums[r]);
	unsigned int assignmentNum = random.getIntervalRand(0, nCombinations - 1);
	uint64_t theAssignment = 0;
	theAssignment = getAssignment(columns, rowSums[r], assignmentNum);
	//printf("rowSum: %2u nCombinations: %5llu ass.Num: %5u ass.: ",
	//rowSum, nCombinations, assignmentNum);
	//printBuckets(theAssignment, columns);
	//putchar('\n');
	assert((theAssignment & relevant_bits) != 0);
	matrix[r] = theAssignment;
      }
      // we don't adjust the columns - if a matrix fails the test, simply a new one is generated
      colSumOk = checkColumnSums();
    }
    //putchar('\n');
    //printf("[%u]\n", ctr);
  }

  
  bool DependencyMatrix::checkColumnSums() {
    unsigned int csum[columns];
    size_t i;
    for (i = 0; i < columns; ++i)
      csum[i] = 0;
    
    size_t r, c;
    for (r = 0; r < rows; ++r) {
      uint64_t curRow = matrix[r];
      for (c = 0; c < columns; ++c) {
	if (curRow & (1LL << c))
	  ++csum[c];
      }
    }
    bool ok = true;
    for (c = 0; c < columns; ++c) {
      if (csum[c] == 0)
	ok = false;
    }
    return ok;
  }
  
  
  uint64_t DependencyMatrix::binomialCoefficient(unsigned int n, unsigned int k) {
    if (k > n)
      return 0;
    else if (k ==0 || k == n)
      return 1;
    else {
      uint64_t result = n - k + 1;
      unsigned int i;
      for (i = 2; i <= k; ++i) {
	result *= n - k + i;
	result /= i;
      }
      return result;
    }
  }
  

  uint64_t DependencyMatrix::getAssignment(size_t length, size_t nEntries, unsigned int num) {
    uint64_t assignment = 0;
    int counter = num;
    assert(num < binomialCoefficient(length, nEntries));
    fillBuckets(assignment, length, 0, nEntries, counter);
    return assignment;
  }

  
  void DependencyMatrix::fillBuckets(uint64_t& bucket, size_t n, size_t offset, size_t theK, int& ctr) {
    //printf("Call to fillBuckets @ offset %lu K %lu\n", offset, theK);
    unsigned int i;
    for (i = offset; i <= n - theK; ++i) {
      bucket |= 1LL << i;
      if (theK > 1) {
	fillBuckets(bucket, n, i+1, theK - 1, ctr);
	if (ctr == -2)
	  return;
      }
      else {
	if (ctr > 0) {
	  ctr = ctr - 1;
	}
	else if (ctr == 0) {
	  //puts("Found: ");
	  //printBuckets(*bucket, n);
	  ctr = -2;
	  return;
	}
	else if (ctr == -1) {
	  //printBuckets(*bucket, n);
	}
      }
      bucket &= ~(1LL << i);
    }
  }


  ostream& operator<<(ostream& ost, const DependencyMatrix& dm) {
    for (size_t r = 0; r < dm.rows; ++r) {
      size_t c;
      ost << "(";
      for (c = 0; c < dm.columns; ++c) {
	ost << " " << ((dm.matrix[r] & (1LL << c)) ? 1 : 0);
      }
      ost << " )" << endl;
    }
    return ost;
  }

  
} // NS tmssim
