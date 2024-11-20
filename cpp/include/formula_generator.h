#ifndef FORMULA_GENERATOR_H
#define FORMULA_GENERATOR_H

#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <z3++.h>

class FormulaGenerator {
public:
    FormulaGenerator(std::vector<z3::expr> init_vars, bool bv_signed = true,
                     bool bv_no_overflow = false, bool bv_no_underflow = false);

    std::string generate_formula_as_str();

private:
    std::vector<z3::expr> bools;
    std::vector<z3::expr> ints;
    std::vector<z3::expr> reals;
    std::vector<z3::expr> bvs;
    std::vector<z3::expr> hard_bools;

    bool bv_signed;
    bool bv_no_overflow;
    bool bv_no_underflow;
    bool use_int;
    bool use_real;
    bool use_bv;

    static int random_int(int min, int max);

    z3::expr random_int_expr();
    z3::expr random_real_expr();
    z3::expr generate_formula();

    void int_from_int();
    void real_from_real();
    void bv_from_bv();
    void bool_from_int();
    void bool_from_real();
    void bool_from_bv();
    void bool_from_bool();

    template <typename T>
    std::vector<T> random_sample(const std::vector<T>& vec, size_t n);

    double random_probability();
};

#endif // FORMULA_GENERATOR_H
