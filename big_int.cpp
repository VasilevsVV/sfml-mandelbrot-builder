#include "big_int.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <climits>
#include <cstring>
#include <cassert>

using namespace std;

static const int ONE = 1;
static const int ZERO = 0;

static const unsigned char INT_BITS = sizeof(int) * 8 - 1;

BigInt::BigInt() {}

BigInt::BigInt(int value) {
  this->value = new unsigned int[1];
  *((int*) this->value) = value;
  this->length = 1;
}

BigInt::BigInt(int value1, int value2) {
  this->value = new unsigned int[2];
  *((int*) &this->value[0]) = value1;
  *((int*) &this->value[1]) = value2;
  this->length = 2;
}

BigInt::BigInt(int value1, int value2, int value3) {
  this->value = new unsigned int[3];
  *((int*) &this->value[0]) = value1;
  *((int*) &this->value[1]) = value2;
  *((int*) &this->value[3]) = value2;
  this->length = 3;
}

BigInt::~BigInt() {
  delete[] this->value;
}

unsigned int *BigInt::getValue() const {
  return this->value;
}

unsigned int BigInt::getLength() const {
  return this->length;
}

/// Utils

inline unsigned int
getSign(unsigned int value) {
  asm ("sar %2, %0" : "=&g" (value) : "0" (value), "c" (INT_BITS));
  return value;
}

inline unsigned int
bigIntAddWithCarry(unsigned int *result, unsigned int value1, unsigned int value2, unsigned int carry) {
  asm("neg   %1\n\t"
      "adc   %4, %0\n\t"
      "mov   %6, %1\n\t"
      "cmovc %5, %1"
      : "=&r" (value1),  "=&r" (carry)
      : "1" (carry),
        "0" (value1), "r" (value2),
        "r" (ONE),    "r" (ZERO));
  (*result) = value1;
  return carry;
}

/// Arithmetic

BigInt BigInt::operator+(const BigInt& y) {
  BigInt res;
  int carry = 0;
  unsigned int limit1, limit2;
  unsigned int *value1, *value2;
  if (y.length > this->length) {
    limit1 = this->length;
    limit2 = y.length;
    value1 = this->value;
    value2 = y.value;
  } else {
    limit1 = y.length;
    limit2 = this->length;
    value1 = y.value;
    value2 = this->value;
  }

  res.length = limit2 + 1;
  res.value = new unsigned int[limit2 + 1];

  for (int i = 0; i < limit1; i++)
    carry = bigIntAddWithCarry(&res.value[i], value1[i], value2[i], carry);
  for (int i = limit1; i < limit2; i++)
    carry = bigIntAddWithCarry(&res.value[i], getSign(value1[limit1 - 1]), value2[i], carry);
  bigIntAddWithCarry(&res.value[limit2], getSign(value1[limit1 - 1]), getSign(value2[limit2 - 1]), carry);

  res.normalize();

  return res;
}

/// Normalization

void BigInt::normalize() {
  unsigned int nextDigit = this->value[this->length - 2];
  unsigned int signDigit = this->value[this->length - 1];
  while ((signDigit ^ getSign(nextDigit)) == 0) {
    this->length--;
    if (this->length == 1)
      return;
    signDigit = nextDigit;
    nextDigit = this->value[this->length - 2];
  }
}

/// Compare

bool BigInt::isNegative() const {
  return *((int*) &this->value[this->length - 1]) < 0;
}

int BigInt::compare(const BigInt& other) {
  bool isNeg1 = this->isNegative();
  bool isNeg2 = other.isNegative();
  if (isNeg1 != isNeg2) {
    return isNeg1 ? -1 : 1;
  } else if (this->length == other.length) {
    for (int i = this->length - 1; i >= 0; i--) {
      if (this->value[i] > other.value[i]) {
        return 1;
      } else if (this->value[i] < other.value[i]) {
        return -1;
      }
    }
    return 0;
  } else if (this->length > other.length) {
    return isNeg1 ? -1 : 1;
  } else {
    return isNeg1 ? 1 : -1;
  }
}

bool BigInt::operator==(const BigInt& other) {
  return this->compare(other) == 0;
}

bool BigInt::operator!=(const BigInt& other) {
  return this->compare(other) != 0;
}

bool BigInt::operator<(const BigInt& other) {
  return this->compare(other) == -1;
}

bool BigInt::operator>(const BigInt& other) {
  return this->compare(other) == 1;
}

bool BigInt::operator<=(const BigInt& other) {
  return this->compare(other) < 1;
}

bool BigInt::operator>=(const BigInt& other) {
  return this->compare(other) > -1;
}

/// Print

std::ostream& operator <<(std::ostream &os, const BigInt &m) {
  int length = m.getLength();
  os << "0x";
  for (int i = length - 1; i >= 0; i--) {
    os << hex << setw(sizeof(int) * 2) << setfill('0') << m.getValue()[i];
  }
  return os;
}

/// TESTS

#ifdef BIG_INT_TESTS

void testBigIntEqual1() {
  assert(BigInt(0) == BigInt(0));
  assert(BigInt(1) == BigInt(1));
  assert(BigInt(-1) == BigInt(-1));
  assert(!(BigInt(-1) == BigInt(1)));
  assert(BigInt(0,1) == BigInt(0,1));
  assert(BigInt(0,1) == BigInt(0,1));
  assert(!(BigInt(1) == BigInt(1,1)));
  assert(!(BigInt(1) == BigInt(0,1)));
}

void testBigIntCompare1() {
  assert(BigInt(1) > BigInt(0));
  assert(BigInt(0) < BigInt(1));
  assert(BigInt(-1) < BigInt(0));
  assert(BigInt(0) > BigInt(-1));
  assert(BigInt(1) < BigInt(0,1));
  assert(BigInt(0,1) > BigInt(1));
  assert(BigInt(0,1) < BigInt(1,1));
  assert(BigInt(1,1) > BigInt(1,0));
  assert(BigInt(1,1) > BigInt(1,-1));
  assert(BigInt(1,-1) < BigInt(1,1));
  assert(BigInt(1) >= BigInt(1));
  assert(BigInt(2) >= BigInt(1));
  assert(BigInt(1) <= BigInt(1));
  assert(BigInt(1) <= BigInt(2));
}

void testBigIntSum() {
  assert(BigInt(2) == (BigInt(1) + BigInt(1)));
  assert(BigInt(0,1) == (BigInt(INT_MAX) + BigInt(INT_MAX) + BigInt(2)));
  assert(BigInt(UINT_MAX-1,0) == (BigInt(INT_MAX) + BigInt(INT_MAX)));
  assert(BigInt(INT_MAX,UINT_MAX) == (BigInt(INT_MIN) + BigInt(-1)));
  assert(BigInt(0,UINT_MAX) == (BigInt(INT_MIN) + BigInt(INT_MIN)));
}

int main() {
  testBigIntEqual1();
  testBigIntCompare1();
  testBigIntSum();

  return 0;
}

#endif
