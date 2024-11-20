#ifndef Z3_UTILS_H
#define Z3_UTILS_H

#include <z3++.h>
#include <vector>

void compact_check_misc(z3::expr precond, std::vector<z3::expr>& cnt_list, std::vector<int>& res_label);

std::vector<int> disjunctive_check(z3::expr precond, std::vector<z3::expr> cnt_list);

void compact_check_misc_incremental(z3::solver& solver, z3::expr precond, std::vector<z3::expr>& cnt_list, std::vector<int>& res_label);

std::vector<int> disjunctive_check_incremental(z3::expr precond, std::vector<z3::expr> cnt_list);

#endif // Z3_UTILS_H
