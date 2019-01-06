#ifndef BIG_INT_H
#define BIG_INT_H

struct BigInt {
  BigInt(int value);
  BigInt(int value1, int value2);
  BigInt(int value1, int value2, int value3);
  ~BigInt();

  BigInt operator+(const BigInt& y);
  //BigInt operator*(const BigInt& y);

  bool operator==(const BigInt& x);
  bool operator!=(const BigInt& x);
  bool operator<(const BigInt& x);
  bool operator>(const BigInt& x);
  bool operator<=(const BigInt& x);
  bool operator>=(const BigInt& x);

  bool isNegative() const;

  unsigned int *getValue() const;
  unsigned int getLength() const;

private:
  unsigned int *value;
  unsigned int length;

  BigInt();

  void normalize();
  int compare(const BigInt& x);
};

#endif
