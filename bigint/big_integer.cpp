#include "big_integer.h"
#include <string>
#include <vector>
#include <stdexcept>

typedef long long int64;
typedef unsigned long long uint64;

big_integer::big_integer() {
    sign = false;
}

big_integer::big_integer(big_integer const &other) {
    this->sign = other.sign;
    this->v = other.v;
    make_fit();
}

big_integer::big_integer(int64 a) {
    this->sign = a < 0;
    if (a < 0) a = -a;
    v.push_back(a % BASE);
    v.push_back(a / BASE);
    make_fit();
}

big_integer::big_integer(unsigned a) {
    this->sign = 0;
    v.push_back(a);
}
big_integer::big_integer(int a) {
    this->sign = a < 0;
    if (a < 0) a = -a;
    v.push_back(a);
}

big_integer::big_integer(std::string const &str) {
    sign = !str.empty() && str[0] == '-';
    v.push_back(0);
    for (size_t i = sign; i < str.size(); ++i) {
        (*this) *= 10;
        (*this) += (str[i] - '0');
    }
    make_fit();
}

big_integer::~big_integer() {
    v.clear();
}

big_integer &big_integer::operator=(big_integer const &other) {
    v = other.v;
    sign = other.sign;
    return *this;
}

bool big_integer::operator==(big_integer const &other) const {
    return this->v == other.v && this->sign == other.sign;
}

bool big_integer::operator!=(big_integer const &other) const {
    return !(*this == other);
}

bool big_integer::operator<=(big_integer const &other) const {
    if (this->sign != other.sign) return this->sign;
    if (v.size() != other.v.size()) {
        return sign ^ (v.size() < other.v.size());
    }
    for (size_t i = v.size() - 1; i < v.size(); --i) {
        if (v[i] != other.v[i]) {
            return sign ^ (v[i] < other.v[i]);
        }
    }
    return true;
}

bool big_integer::operator>(big_integer const &other) const {
    return !(*this <= other);
}

bool big_integer::operator<(big_integer const &other) const {
    return (*this <= other) && (*this != other);
}

bool big_integer::operator>=(big_integer const &other) const {
    return !(*this < other);
}

big_integer &big_integer::operator+=(big_integer const &other) {
    if (sign != other.sign) {
        *this -= (-other);
    } else {
        uint64 carry = 0;
        std::vector<unsigned int> res(std::max(other.v.size(), v.size()));
        for (size_t i = 0; i < res.size(); ++i) {
            uint64 sum = carry + (i < v.size() ? v[i] : 0) + (i < other.v.size() ? other.v[i] : 0);
            res[i] = sum;
            carry = sum / BASE;
        }
        if (carry != 0) {
            res.push_back(carry);
        }
        v = res;
    }
    make_fit();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &other) {
    if (sign != other.sign) {
        *this += (-other);
    } else {
        uint64 carry = 0;
        std::vector<unsigned int> res(std::max(other.v.size(), v.size()));
        if ((!sign && *this >= other) || (sign && *this <= other)) {
            for (size_t i = 0; i < res.size(); ++i) {
                int64 sub = static_cast<int64>(v[i]) - (i < other.v.size() ? other.v[i] : 0) - carry;
                if (sub < 0) {
                    sub += BASE;
                    carry = 1;
                } else {
                    carry = 0;
                }
                res[i] = sub;
            }
        } else {
            for (size_t i = 0; i < res.size(); ++i) {
                int64 sub = static_cast<int64>(other.v[i]) - (i < v.size() ? v[i] : 0) - carry;
                if (sub < 0) {
                    sub += BASE;
                    carry = 1;
                } else {
                    carry = 0;
                }
                res[i] = sub;
            }
            sign = !sign;
        }
        v = res;
    }
    make_fit();
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &other) {
    big_integer res(0);
    for (size_t i = 0; i < other.v.size(); ++i) {
        unsigned int carry = 0;
        big_integer tmp;
        tmp.v.resize(i, 0);
        for (size_t j = 0; j < v.size(); ++j) {
            uint64 sum = carry + static_cast<uint64>(v[j]) * other.v[i];
            tmp.v.push_back(sum);
            carry = sum / BASE;
        }
        if (carry != 0) {
            tmp.v.push_back(carry);
        }
        res += tmp;
    }
    v = res.v;
    sign ^= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &other) {
    if (other == 0) {
        throw std::runtime_error("Division by zero");
    }
    big_integer a = abs();
    big_integer b = other.abs(); //39045157200000000000000000
    if (a < b) {
        *this = 0;
        return *this;
    }
    if (a == b) {
        *this = 1;
        return *this;
    }
    big_integer res(0), mod(0);
    unsigned int f = (BASE / (other.v.back() + 1ll));
    a *= f;
    b *= f;
    size_t n = a.v.size(), m = b.v.size();
    v.resize(n, 0);
    std::vector<unsigned int> data(n - m + 1);
    mod = a >> ((n - m + 1) * BASE_SIZE);
    uint64 top = b.v.back();
    for (size_t i = 0; i <= n - m; ++i) {
        size_t idx = n - m - i;
        mod <<= BASE_SIZE;
        mod.v[0] = a.v[idx];
        uint64 mod_top = mod.v.back();
        if (mod.v.size() > m) {
            mod_top <<= BASE_SIZE;
            mod_top += mod.v[mod.v.size() - 2];
        }
        unsigned int guess = std::min(mod_top / top, BASE - 1);
        big_integer res_guess = guess * b;
        while (mod < res_guess) {
            guess--;
            res_guess -= b;
        }
        data[idx] = guess;
        mod -= res_guess;
    }
    big_integer tmp(false, data);
    *this = tmp;
    if (sign != other.sign) {
        sign = !sign;
    }
    make_fit();
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &other) {
    for (size_t i = 0; i < v.size(); ++i) {
        v[i] &= (i < other.v.size() ? other.v[i] : 0);
    }
    sign &= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &other) {
    for (size_t i = 0; i < std::max(v.size(), other.v.size()); ++i) {
        if (i < v.size()) v[i] ^= (i < other.v.size() ? other.v[i] : 0);
        else v.push_back(other.v[i]);
    }
    sign ^= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &other) {
    for (size_t i = 0; i < std::max(v.size(), other.v.size()); ++i) {
        if (i < v.size()) v[i] |= (i < other.v.size() ? other.v[i] : 0);
        else v.push_back(other.v[i]);
    }
    sign |= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator<<=(unsigned int a) {
    unsigned int skip = a / BASE_SIZE;
    unsigned move = a % BASE_SIZE;
    a = 1 << move;
    *this *= a;
    std::vector<unsigned> tmp = v;
    v.clear();
    while (skip != 0) {
        v.push_back(0);
        --skip;
    }
    for (size_t i = 0; i < tmp.size(); ++i) {
        v.push_back(tmp[i]);
    }
    make_fit();
    return *this;
}

big_integer &big_integer::operator>>=(unsigned int a) {
    unsigned int skip = a / BASE_SIZE;
    unsigned move = a % BASE_SIZE;
    a = 1 << move;
    if (v.size() <= skip) {
        v.clear();
        v.push_back(0);
        return *this;
    }
    std::vector<unsigned> tmp;
    for (size_t i = skip; i < v.size(); ++i) {
        tmp.push_back(v[i]);
    }
    v = tmp;
    *this /= a;
    make_fit();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &other) {
    make_fit();
    return *this = *this - (*this / other) * other;
}

big_integer big_integer::operator-() const {
    big_integer tmp(*this);
    tmp.sign ^= true;
    return tmp;
}

big_integer big_integer::operator+() const {
    big_integer tmp(*this);
    return tmp;
}

big_integer &big_integer::operator++() {
    make_fit();
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    make_fit();
    big_integer tmp = *this;
    ++(*this);
    return tmp;
}

big_integer &big_integer::operator--() {
    make_fit();
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    make_fit();
    big_integer tmp(*this);
    --(*this);
    return tmp;
}

big_integer big_integer::operator~() const {
    big_integer tmp(*this);
    for (size_t i = 0; i < tmp.v.size(); ++i) {
        tmp.v[i] = ~v[i];
    }
    tmp.sign = !tmp.sign;
    return tmp;
}

big_integer big_integer::abs() const {
    return big_integer(false, v);
}

void big_integer::swap(big_integer &b) {
    big_integer temp(*this);
    sign = b.sign;
    v = b.v;
    b.sign = temp.sign;
    b.v = temp.v;
    make_fit();
}

const unsigned big_integer::get_digit(size_t i) const {
    return v[i];
}

bool big_integer::is_signed() const {
    return sign;
}

big_integer::big_integer(bool negate, std::vector<unsigned int> const &data) {
    v = data;
    sign = negate;
    make_fit();
}

void big_integer::make_fit() {
    while (v.size() > 1 && v.back() == 0) {
        v.pop_back();
    }
}

size_t big_integer::length() const {
    return v.size();
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator<<(big_integer a, unsigned int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, unsigned int b) {
    return a >>= b;
}

std::string to_string(big_integer const &value) {
    if (value.length() == 0) {
        throw std::runtime_error("Call function to_string from empty number");
    }
    big_integer a = value.abs();
    if (value == 0) {
        return "0";
    }
    std::string s;
    while (a != 0) {
        unsigned val = (a % 10).get_digit(0);
        s.push_back(val + '0');
        a /= 10;
    }
    if (value.is_signed()) {
        s.push_back('-');
    }
    std::reverse(s.begin(), s.end());
    return s;
}
