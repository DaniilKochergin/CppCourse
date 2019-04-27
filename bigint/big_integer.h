#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#ifndef BIG_INT
#define BIG_INT
typedef unsigned long long uint64;

struct big_integer {
    big_integer();

    big_integer(big_integer const &other);

    big_integer(unsigned a);

    big_integer(int a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &other);

    big_integer &operator-=(big_integer const &other);

    big_integer &operator*=(big_integer const &other);

    big_integer &operator/=(big_integer const &other);

    big_integer &operator/=(unsigned a);

    big_integer &operator/=(int a);

    big_integer &operator&=(big_integer const &other);

    big_integer &operator^=(big_integer const &other);

    big_integer &operator|=(big_integer const &other);

    big_integer &operator<<=(int a);

    big_integer &operator>>=(int a);

    big_integer &operator%=(big_integer const &other);

    big_integer operator-() const;

    big_integer operator+() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    big_integer operator~() const;

    big_integer abs() const;

    void swap(big_integer &b);

    const unsigned get_digit(size_t i) const;

    bool is_signed() const;

    size_t length() const;

    friend big_integer operator+(big_integer a, big_integer const &b);

    friend big_integer operator-(big_integer a, big_integer const &b);

    friend big_integer operator*(big_integer a, big_integer const &b);

    friend big_integer operator/(big_integer a, big_integer const &b);

    friend big_integer operator/(big_integer a, int b);

    friend big_integer operator/(big_integer a, unsigned b);

    friend big_integer operator%(big_integer a, big_integer const &b);

    friend big_integer operator&(big_integer a, big_integer const &b);

    friend big_integer operator^(big_integer a, big_integer const &b);

    friend big_integer operator|(big_integer a, big_integer const &b);

    friend big_integer operator>>(big_integer a, int b);

    friend big_integer operator<<(big_integer a, int b);


    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

private:
    void make_fit();

    big_integer(bool negate, std::vector<unsigned int> const &data);
    big_integer & short_divide(unsigned a);
    std::vector<unsigned int> v;
    bool sign;
    uint64 const BASE = 1ll << 32;
    size_t const BASE_SIZE = 32;
    unsigned MAX_VALUE = UINT32_MAX;
};

std::string to_string(big_integer const &value);

#endif // BIG_INTEGER_H