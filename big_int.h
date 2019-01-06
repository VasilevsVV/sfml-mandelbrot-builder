#ifndef BIG_INT_H
#define BIG_INT_H

struct BigInt {
  BigInt(int value);
  ~BigInt();

  BigInt operator+(const BigInt& y);
  //BigInt operator*(const BigInt& y);

  //bool operator==(BigInt& x);

  unsigned int *getValue() const;
  unsigned int getLength() const;

private:
  unsigned int *value;
  unsigned int length;

  BigInt();

  void normalize();
};

#endif
