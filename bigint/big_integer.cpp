#include "big_integer.h"
#include <string>
#include <vector>
#include <stdexcept>

typedef unsigned long long uint64;

big_integer::big_integer() {
    v.push_back(0);
    sign = false;
}

big_integer::big_integer(big_integer const &other) {
    this->sign = other.sign;
    this->v = other.v;
    make_fit();
}

big_integer::big_integer(unsigned a) {
    this->sign = 0;
    v.push_back(a);
}

big_integer::big_integer(int a) {
    this->sign = a < 0;
    v.push_back(a);
}

big_integer::big_integer(std::string const &str) {
    bool start = !(str.empty()) && str[0] == '-';
    v.push_back(0);
    sign = false;
    for (size_t i = start; i < str.size(); ++i) {
        (*this) *= 10;
        (*this) += (str[i] - '0');
    }
    if (start) *this = -*this;
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


big_integer &big_integer::operator+=(big_integer const &other) {
    uint64 carry = 0;
    std::vector<unsigned int> res(std::max(other.length(), length()) + 2);
    for (size_t i = 0; i < res.size(); ++i) {
        uint64 sum = carry + get_digit(i) + other.get_digit(i);
        res[i] = sum;
        carry = sum >> BASE_SIZE;
    }
    v = res;
    sign = v.back() & (1 << (BASE_SIZE - 1));
    make_fit();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &other) {
    return *this += (-other);
}


big_integer &big_integer::operator*=(big_integer const &other) {
    big_integer a(abs());
    big_integer b(other.abs());
    if (a == 0 || b == 0) {
        *this = 0;
        return *this;
    }
    big_integer res(0);
    res.v.resize(a.length() + b.length() + 1, 0);
    for (size_t i = 0; i < a.length(); ++i) {
        unsigned int carry = 0;
        for (size_t j = 0; j < b.length(); ++j) {
            uint64 sum = res.v[i + j] + static_cast<uint64>(a.v[i]) * b.v[j] + carry;
            res.v[i + j] = sum;
            carry = sum >> BASE_SIZE;
        }
        size_t x = b.length();
        while (carry != 0) {
            uint64 sum = static_cast<uint64>(res.v[i + x]) + carry;
            res.v[i + x] = sum;
            carry = sum >> BASE_SIZE;
            x++;
        }
    }
    if (sign ^ other.sign) {
        *this = -res;
    } else {
        *this = res;
    }
    make_fit();
    return *this;
}

big_integer &big_integer::operator/=(unsigned a) {
    uint64 carry = 0;
    std::vector<unsigned> v1;
    for (size_t i = length() - 1; i < length(); --i) {
        uint64 sum = v[i] + (carry << BASE_SIZE);
        v1.push_back(sum / a);
        carry = sum % a;
    }
    std::reverse(v1.begin(), v1.end());
    v = v1;
    make_fit();
    return *this;
}

big_integer &big_integer::operator/=(int val) {
    return (*this) /= big_integer(val);
}

big_integer &big_integer::operator/=(big_integer const &other) {
    big_integer a = abs();
    big_integer b = other.abs();
    if (a < b) {
        *this = 0;
        return *this;
    }
    if (b.length() == 1) {
        if (sign ^ other.sign) {
            return *this = -(a / (b.v[0]));
        }
        return *this = a / (b.v[0]);
    }
    big_integer mod(0);
    unsigned f = (BASE / (b.v.back() + static_cast<uint64 >(1)));
    a *= f;
    b *= f;
    size_t n = a.length(), m = b.length();
    std::vector<unsigned int> data(n - m + 1);
    mod = a >> ((n - m + 1) * BASE_SIZE);
    uint64 top = b.v.back();
    for (size_t i = n - m; i <= n - m; --i) {
        mod <<= BASE_SIZE;
        mod.v[0] = a.v[i];
        uint64 mod_top = mod.v.back();
        if (mod.length() > m) {
            mod_top <<= BASE_SIZE;
            mod_top += mod.v[mod.length() - 2];
        }
        unsigned guess = std::min(mod_top / top, BASE - 1);
        big_integer res_guess = guess * b;
        while (mod < res_guess) {
            guess--;
            res_guess -= b;
        }
        data[i] = guess;
        mod -= res_guess;
    }
    big_integer tmp(false, data);
    if (sign ^ other.sign) {
        tmp = -tmp;
    }
    *this = tmp;
    make_fit();
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &other) {
    for (size_t i = 0; i < length(); ++i) {
        v[i] &= other.get_digit(i);
    }
    sign &= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &other) {
    for (size_t i = 0; i < std::max(length(), other.length()); ++i) {
        if (i < length()) v[i] ^= other.get_digit(i);
    }
    sign ^= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &other) {
    for (size_t i = 0; i < std::max(length(), other.length()); ++i) {
        v[i] |= other.get_digit(i);
    }
    sign |= other.sign;
    make_fit();
    return *this;
}

big_integer &big_integer::operator<<=(int a) {
    if (a < 0) {
        return (*this) >>= a;
    }
    size_t div = a >> 5;
    size_t mod = a & (BASE_SIZE - 1);
    size_t new_size = length() + div + 1;
    std::vector<unsigned> tmp(new_size);
    tmp[div] = static_cast<unsigned >(static_cast<uint64>(get_digit(0)) << mod);
    for (size_t i = div + 1; i < new_size; i++) {
        unsigned x = static_cast<uint64 >(get_digit(i - div)) << mod;
        unsigned y = static_cast<uint64>(v[i - div - 1]) >> (BASE_SIZE - mod);
        tmp[i] = x | y;
    }
    v = tmp;
    make_fit();
    return *this;
}

big_integer &big_integer::operator>>=(int a) {
    if (a < 0) {
        return (*this) <<= a;
    }
    size_t div = a >> 5;
    unsigned mod = a & (BASE_SIZE - 1);
    size_t new_size = 0;
    if (div < (*this).length()) {
        new_size = (*this).length() - div;
    }
    std::vector<unsigned> tmp(new_size);
    for (size_t i = 0; i < new_size; i++) {
        unsigned x = static_cast<uint64>(v[i + div]) >> mod;
        unsigned y = static_cast<uint64>(get_digit(i + div + 1)) << (BASE_SIZE - mod);
        tmp[i] = x | y;
    }
    v = tmp;
    make_fit();
    return *this;
}


big_integer &big_integer::operator%=(big_integer const &other) {
    big_integer a = (*this / other) * other;
    return *this -= a;
}

big_integer big_integer::operator-() const {
    if (*this == 0) {
        return *this;
    }
    big_integer tmp(*this);
    tmp = ~tmp;
    ++tmp;
    tmp.sign = !sign;
    return tmp;
}

big_integer big_integer::operator+() const {
    big_integer tmp(*this);
    return tmp;
}

big_integer &big_integer::operator++() {
    return *this += 1;
}

big_integer big_integer::operator++(int) {
    big_integer tmp = *this;
    ++(*this);
    return tmp;
}

big_integer &big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer tmp(*this);
    --(*this);
    return tmp;
}

big_integer big_integer::operator~() const {
    big_integer tmp(*this);
    for (size_t i = 0; i < tmp.length(); ++i) {
        tmp.v[i] = ~v[i];
    }
    tmp.sign = !tmp.sign;
    return tmp;
}

big_integer big_integer::abs() const {
    if (sign) return -(*this);
    return *this;
}

void big_integer::swap(big_integer &b) {
    big_integer tmp(*this);
    sign = b.sign;
    v = b.v;
    b.sign = tmp.sign;
    b.v = tmp.v;
    make_fit();
}

const unsigned big_integer::get_digit(size_t i) const {
    if (i < length()) return v[i];
    if (sign) return MAX_VALUE;
    return 0;
}

bool big_integer::is_signed() const {
    return sign;
}

big_integer::big_integer(bool negate, std::vector<unsigned> const &data) {
    v = data;
    sign = negate;
    make_fit();
}

void big_integer::make_fit() {
    while (length() > 1 && ((v.back() == 0 && !sign) || (v.back() == MAX_VALUE && sign))) {
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

big_integer operator/(big_integer a, unsigned b) {
    return a /= b;
}

big_integer operator/(big_integer a, int b) {
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

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}


bool operator==(big_integer const &a, big_integer const &b) {
    return a.v == b.v && a.sign == b.sign;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) return a.sign;
    if (a.length() != b.length()) {
        return a.sign ^ (a.length() < b.length());
    }
    for (size_t i = a.length() - 1; i < b.length(); --i) {
        if (a.v[i] != b.v[i]) {
            return a.sign ^ (a.v[i] < b.v[i]);
        }
    }
    return true;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return !(a <= b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    return (a <= b) && (a != b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}


std::string to_string(big_integer const &value) {
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