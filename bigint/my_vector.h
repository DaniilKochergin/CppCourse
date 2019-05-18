//
// Created by daniil on 16.05.19.
//

#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#include <memory>

struct my_vector {

    my_vector();

    ~my_vector();

    explicit my_vector(size_t n);

    my_vector(my_vector const &other) noexcept;

    size_t size() const;

    uint32_t &operator[](size_t i);

    uint32_t const &operator[](size_t i) const;

    void push_back(uint32_t val);

    void pop_back();

    uint32_t &back();

    void swap(my_vector &other);

    friend bool operator==(my_vector const &a, my_vector const &b);

    my_vector &operator=(my_vector other) noexcept;

    uint32_t *start() const noexcept;

private:
    struct big {
        size_t capacity;
        std::shared_ptr<uint32_t> p;

        big() : capacity(0), p(nullptr) {};

        void ensure_capacity(size_t size);

        big(size_t size, uint32_t *array);
    };

    size_t _size;
    static const size_t SMALL_SIZE = 2;

    bool is_big;

    union any_data {
        big big_data;
        uint32_t small_data[SMALL_SIZE];

        ~any_data() {};

    } union_data{};


    void make_original();

};


#endif //BIGINT_MY_VECTOR_H
