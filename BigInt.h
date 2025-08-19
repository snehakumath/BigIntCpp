#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <vector>
#include <string>

class BigInt {
private:
    static const int BASE = 10000;
    static const int WIDTH = 4;
    std::vector<int> digits;  // Least significant digit first
    bool negative;

    void removeLeadingZeros();
    static bool absLess(const BigInt &a, const BigInt &b);


public:
    BigInt(long long num = 0);
    BigInt(const std::string &str);

    friend std::istream &operator>>(std::istream &in, BigInt &x);
    friend std::ostream &operator<<(std::ostream &out, const BigInt &x);

    BigInt operator+(const BigInt &b) const;
    BigInt operator-(const BigInt &b) const;
    BigInt operator-() const;   
    BigInt operator*(const BigInt &b) const;

    BigInt operator/(int b) const;  // only int division
    bool operator<(const BigInt &b) const;
};

#endif




