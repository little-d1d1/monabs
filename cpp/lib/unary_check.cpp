#include <z3++.h>
#include <vector>

std::vector<int> unary_check(z3::expr precond, std::vector<z3::expr> cnt_list) {
    std::vector<int> results;
    z3::solver solver(precond.ctx());

    solver.add(precond);  // Add the precondition

    for (const auto& cnt : cnt_list) {
        solver.push();  // Save the current state
        solver.add(cnt);  // Add the current constraint
        auto res = solver.check();
        if (res == z3::sat) {
            results.push_back(1);
        } else if (res == z3::unsat) {
            results.push_back(0);
        } else {
            results.push_back(2);
        }

        solver.pop();  // Restore the state
    }

    return results;
}

std::vector<int> unary_check_cached(z3::expr precond, std::vector<z3::expr> cnt_list) {
    std::vector<int> results(cnt_list.size(), -1);
    z3::solver solver(precond.ctx());

    solver.add(precond);  // Add the precondition

    for (size_t i = 0; i < cnt_list.size(); ++i) {
        if (results[i] != -1) {
            continue;
        }

        solver.push();  // Save the current state
        solver.add(cnt_list[i]);  // Add the current constraint
        auto res = solver.check();

        if (res == z3::sat) {
            z3::model model = solver.get_model();
            results[i] = 1;
            for (size_t j = 0; j < cnt_list.size(); ++j) {
                if (results[j] == -1 && z3::eq(model.eval(cnt_list[j], true), precond.ctx().bool_val(true))) {
                    results[j] = 1;
                }
            }
        } else if (res == z3::unsat) {
            results[i] = 0;
        } else {
            results[i] = 2;
        }

        solver.pop();  // Restore the state
    }

    return results;

}