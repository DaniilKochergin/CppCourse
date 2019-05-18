//
// Created by daniil on 16.05.19.
//
#include <cstring>
#include "my_vector.h"

my_vector::my_vector() {
    is_big = false;
    memset(union_data.small_data, 0, SMALL_SIZE * sizeof(uint32_t));
    _size = 0;
}

my_vector::~my_vector() {
    if (is_big) {
        union_data.big_data.~big();
    }
}

size_t my_vector::size() const {
    return _size;
}

uint32_t &my_vector::operator[](size_t i) {
    make_original();
    return start()[i];
}

uint32_t const &my_vector::operator[](size_t i) const {
    return start()[i];
}

my_vector::my_vector(size_t n) {
    _size = n;
    if (_size > SMALL_SIZE) {
        is_big = true;
        auto tmp = new uint32_t[n];

        new(&union_data.big_data) big(_size, tmp);
    } else {
        is_big = false;
    }
    memset(start(), 0, _size * sizeof(uint32_t));

}

uint32_t *my_vector::start() const noexcept {
    if (is_big) {
        return union_data.big_data.p.get();
    } else {
        return const_cast<uint32_t *>(union_data.small_data);
    }
}

void my_vector::pop_back() {
    make_original();
    _size--;
    if (_size <= SMALL_SIZE && is_big){
        auto tmp = new uint32_t[_size];
        memcpy(tmp, start(), _size * sizeof(uint32_t));
        union_data.big_data.~big();
        memcpy(union_data.small_data, tmp, _size * sizeof(uint32_t));
        delete [] tmp;
        is_big = false;
    }
}

my_vector::my_vector(my_vector const &other) noexcept{
    _size = other.size();
    is_big = other.is_big;
    if (is_big) {
        new(&union_data.big_data) big(other.union_data.big_data);
    } else {
        memcpy(union_data.small_data, other.union_data.small_data, _size * sizeof(uint32_t));
    }
}

my_vector &my_vector::operator=(my_vector other) noexcept{
    swap(other);
    return *this;
}

uint32_t &my_vector::back(){
    make_original();
    return start()[_size - 1];
}


void my_vector::swap(my_vector &other) {

    std::swap(_size, other._size);
    std::swap(is_big, other.is_big);

    char *tmp[sizeof(union_data)];

    mempcpy(tmp, &union_data, sizeof(union_data));

    mempcpy(&union_data, &other.union_data, sizeof(union_data));

    mempcpy(&other.union_data, tmp, sizeof(union_data));
}


void my_vector::big::ensure_capacity(size_t size) {
    if (size == capacity) {
        capacity *= 2;
        auto tmp = new uint32_t[capacity];
        memcpy(tmp, p.get(), size*sizeof(uint32_t));
        p.reset(tmp, std::default_delete<uint32_t[]>());
    }
}

my_vector::big::big(size_t size, uint32_t *array) : capacity(size), p(array, std::default_delete<uint32_t[]>()) {}


void my_vector::push_back(uint32_t val) {
    make_original();
    if (!is_big && _size == SMALL_SIZE) {
        auto tmp = new uint32_t[_size + 1];
        memcpy(tmp, start(), _size * sizeof(uint32_t));
        tmp[_size++] = val;
        new(&union_data.big_data) big(_size,tmp);
        is_big = true;
    } else {
        if (is_big){
            union_data.big_data.ensure_capacity(_size);
        }
        start()[_size++] = val;
    }
}

bool operator==(my_vector const &a, my_vector const &b) {
    if (a.size() == b.size()) {
        for (size_t i = 0; i < a.size(); ++i) {
            if (a.start()[i] != b.start()[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void my_vector::make_original() {
    if (is_big && !union_data.big_data.p.unique()){
        auto tmp = new uint32_t[union_data.big_data.capacity];
        memcpy(tmp, start(), union_data.big_data.capacity * sizeof(uint32_t));
        union_data.big_data.p.reset(tmp, std::default_delete<uint32_t[]>());
    }
}




