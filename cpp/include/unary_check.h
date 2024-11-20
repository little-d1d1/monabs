#ifndef Z3_UNARY_CHECK_H
#define Z3_UNARY_CHECK_H

#include <z3++.h>
#include <vector>

std::vector<int> unary_check(z3::expr precond, std::vector<z3::expr> cnt_list);

std::vector<int> unary_check_cached(z3::expr precond, std::vector<z3::expr> cnt_list);

#endif // Z3_UNARY_CHECK_H