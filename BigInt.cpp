
#include "BigInt.h"
#include <iomanip>
#include <algorithm>

std::vector<int> karatsubaMultiply(const std::vector<int> &a, const std::vector<int> &b) {
    int n = a.size(), m = b.size();
    if (n < m) return karatsubaMultiply(b, a);
    if (n == 0 || m == 0) return {};
    if (n <= 32) { // Fallback to normal multiplication for small sizes
        std::vector<int> res(n + m);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                res[i + j] += a[i] * b[j];
        return res;
    }

    int k = n / 2;

    std::vector<int> a1(a.begin(), a.begin() + k);
    std::vector<int> a2(a.begin() + k, a.end());
    std::vector<int> b1(b.begin(), b.begin() + std::min((int)b.size(), k));
    std::vector<int> b2(b.begin() + std::min((int)b.size(), k), b.end());

    auto a1b1 = karatsubaMultiply(a1, b1);
    auto a2b2 = karatsubaMultiply(a2, b2);

    std::vector<int> a1a2 = a1;
    std::vector<int> b1b2 = b1;
    for (int i = 0; i < a2.size(); ++i)
        if (i < a1a2.size()) a1a2[i] += a2[i];
        else a1a2.push_back(a2[i]);

    for (int i = 0; i < b2.size(); ++i)
        if (i < b1b2.size()) b1b2[i] += b2[i];
        else b1b2.push_back(b2[i]);

    auto r = karatsubaMultiply(a1a2, b1b2);
    for (int i = 0; i < a1b1.size(); ++i) r[i] -= a1b1[i];
    for (int i = 0; i < a2b2.size(); ++i) r[i] -= a2b2[i];

    std::vector<int> res(a1b1.size() + 2 * (n - k), 0);
    for (int i = 0; i < a1b1.size(); ++i) res[i] += a1b1[i];
    for (int i = 0; i < r.size(); ++i) res[i + k] += r[i];
    for (int i = 0; i < a2b2.size(); ++i) res[i + 2 * k] += a2b2[i];

    return res;
}


BigInt::BigInt(long long num) : negative(false) {
    if (num < 0) {
        negative = true;
        num = -num;
    }
    do {
        digits.push_back(num % BASE);
        num /= BASE;
    } while (num > 0);
}

BigInt::BigInt(const std::string &str) : negative(false) {
    digits.clear();
    int len = str.length();
    int start = 0;

    if (str[0] == '-') {
        negative = true;
        start = 1;
    }

    for (int i = len; i > start; i -= WIDTH) {
        int end = i;
        int begin = std::max(start, i - WIDTH);
        digits.push_back(std::stoi(str.substr(begin, end - begin)));
    }

    removeLeadingZeros();
}

void BigInt::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0)
        digits.pop_back();

    if (digits.size() == 1 && digits[0] == 0)
        negative = false;
}

std::istream &operator>>(std::istream &in, BigInt &x) {
    std::string s;
    in >> s;
    x = BigInt(s);
    return in;
}

std::ostream &operator<<(std::ostream &out, const BigInt &x) {
    if (x.negative && !(x.digits.size() == 1 && x.digits[0] == 0))
        out << '-';

    out << x.digits.back(); // most significant digit (no padding)

    for (int i = x.digits.size() - 2; i >= 0; --i)
        out << std::setw(BigInt::WIDTH) << std::setfill('0') << x.digits[i];

    return out;
}

bool BigInt::operator<(const BigInt &b) const {
    if (digits.size() != b.digits.size())
        return digits.size() < b.digits.size();
    for (int i = digits.size() - 1; i >= 0; --i)
        if (digits[i] != b.digits[i])
            return digits[i] < b.digits[i];
    return false;
}

BigInt BigInt::operator-() const {
    BigInt result = *this;
    if (!(digits.size() == 1 && digits[0] == 0))  // don't flip sign for zero
        result.negative = !negative;
    return result;
}

BigInt BigInt::operator-(const BigInt &b) const {
    // Case 1: Different signs → subtraction becomes addition
    if (this->negative != b.negative) {
        return *this + (-b);
    }

    // Case 2: Same sign → compare absolute values
    if (absLess(*this, b)) {
        // |a| < |b| → result is negative
        return -(b - *this);
    }

    // Case 3: |a| >= |b|, perform digit-wise subtraction
    BigInt result;
    result.digits.clear();
    int borrow = 0;

    for (size_t i = 0; i < digits.size(); ++i) {
        int diff = digits[i] - borrow - (i < b.digits.size() ? b.digits[i] : 0);
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits.push_back(diff);
    }

    result.negative = this->negative;
    result.removeLeadingZeros();
    return result;
}

bool BigInt::absLess(const BigInt &a, const BigInt &b) {
    if (a.digits.size() != b.digits.size())
        return a.digits.size() < b.digits.size();

    for (int i = a.digits.size() - 1; i >= 0; --i) {
        if (a.digits[i] != b.digits[i])
            return a.digits[i] < b.digits[i];
    }
    return false; // equal
}


BigInt BigInt::operator+(const BigInt &b) const {
    // Case 1: Same sign → regular addition
    if (this->negative == b.negative) {
        BigInt result;
        result.negative = this->negative;  // both signs same, result has the same sign
        result.digits.clear();

        int carry = 0;
        for (size_t i = 0; i < digits.size() || i < b.digits.size() || carry; ++i) {
            int sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < b.digits.size()) sum += b.digits[i];
            result.digits.push_back(sum % BASE);
            carry = sum / BASE;
        }

        result.removeLeadingZeros();
        return result;
    }

    // Case 2: Signs differ → do subtraction: a + (-b) => a - b
    if (this->negative) {
        // (-a) + b == b - a
        return b - (-(*this));
    } else {
        // a + (-b) == a - b
        return *this - (-b);
    }
}



BigInt BigInt::operator*(const BigInt &other) const {
    BigInt result;
    result.digits = karatsubaMultiply(this->digits, other.digits);

    // Handle carry
    long long carry = 0;
    for (int i = 0; i < result.digits.size(); ++i) {
        long long temp = (long long)result.digits[i] + carry;
        result.digits[i] = temp % BASE;
        carry = temp / BASE;
    }

    while (carry) {
        result.digits.push_back(carry % BASE);
        carry /= BASE;
    }

    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator/(int b) const {
    BigInt result;
    result.digits.resize(digits.size());
    long long remainder = 0;
    for (int i = digits.size() - 1; i >= 0; --i) {
        long long current = digits[i] + remainder * BASE;
        result.digits[i] = current / b;
        remainder = current % b;
    }
    result.removeLeadingZeros();
    return result;
}

