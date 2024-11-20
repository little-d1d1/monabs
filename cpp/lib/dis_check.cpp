#include <z3++.h>
#include <vector>

void compact_check_misc(z3::expr precond, std::vector<z3::expr>& cnt_list, std::vector<int>& res_label) {
    z3::expr f = precond.ctx().bool_val(false);
    for (size_t i = 0; i < res_label.size(); ++i) {
        if (res_label[i] == 2) {
            f = z3::or_(f, cnt_list[i]);
        }
    }
    if (z3::eq(f, precond.ctx().bool_val(false))) {
        return;
    }

    z3::solver sol(precond.ctx());
    z3::expr g = z3::and_(precond, f);
    sol.add(g);
    auto s_res = sol.check();
    if (s_res == z3::unsat) {
        for (size_t i = 0; i < res_label.size(); ++i) {
            if (res_label[i] == 2) {
                res_label[i] = 0;
            }
        }
    } else if (s_res == z3::sat) {
        z3::model m = sol.get_model();
        for (size_t i = 0; i < res_label.size(); ++i) {
            if (res_label[i] == 2 && z3::eq(m.eval(cnt_list[i], true), precond.ctx().bool_val(true))) {
                res_label[i] = 1;
            }
        }
    } else {
        return;
    }
    compact_check_misc(precond, cnt_list, res_label);
}

std::vector<int> disjunctive_check(z3::expr precond, std::vector<z3::expr> cnt_list) {
    std::vector<int> res(cnt_list.size(), 2); // Initialize with 2 (unknown)
    compact_check_misc(precond, cnt_list, res);
    return res;
}

void compact_check_misc_incremental(z3::solver& solver, z3::expr precond, std::vector<z3::expr>& cnt_list, std::vector<int>& res_label) {
    z3::expr f = precond.ctx().bool_val(false);
    for (size_t i = 0; i < res_label.size(); ++i) {
        if (res_label[i] == 2) {
            f = z3::or_(f, cnt_list[i]);
        }
    }
    if (z3::eq(f, precond.ctx().bool_val(false))) {
        return;
    }

    solver.push();
    solver.add(f);
    auto s_res = solver.check();

    if (s_res == z3::unsat) {
        for (size_t i = 0; i < res_label.size(); ++i) {
            if (res_label[i] == 2) {
                res_label[i] = 0;
            }
        }
    } else if (s_res == z3::sat) {
        z3::model m = solver.get_model();
        for (size_t i = 0; i < res_label.size(); ++i) {
            if (res_label[i] == 2 && z3::eq(m.eval(cnt_list[i], true), precond.ctx().bool_val(true))) {
                res_label[i] = 1;
            }
        }
    }

    solver.pop();
    compact_check_misc_incremental(solver, precond, cnt_list, res_label);
}

std::vector<int> disjunctive_check_incremental(z3::expr precond, std::vector<z3::expr> cnt_list) {
    std::vector<int> res(cnt_list.size(), 2);
    z3::solver solver(precond.ctx());
    solver.add(precond);
    compact_check_misc_incremental(solver, precond, cnt_list, res);
    return res;
}
