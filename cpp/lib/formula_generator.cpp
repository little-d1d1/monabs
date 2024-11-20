#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <z3++.h>

class FormulaGenerator {
public:
    FormulaGenerator(std::vector<z3::expr> init_vars, bool bv_signed = true,
                     bool bv_no_overflow = false, bool bv_no_underflow = false)
        : bv_signed(bv_signed), bv_no_overflow(bv_no_overflow),
          bv_no_underflow(bv_no_underflow), use_int(false), use_real(false), use_bv(false) {

        for (const auto& var : init_vars) {
            if (var.is_int()) {
                ints.push_back(var);
            } else if (var.is_real()) {
                reals.push_back(var);
            } else if (var.is_bv()) {
                bvs.push_back(var);
            }
        }

        if (!ints.empty()) {
            use_int = true;
            for (int i = 0; i < random_int(3, 6); ++i) {
                ints.push_back(random_int_expr());
            }
        }

        if (!reals.empty()) {
            use_real = true;
            for (int i = 0; i < random_int(3, 6); ++i) {
                reals.push_back(random_real_expr());
            }
        }

        if (!bvs.empty()) {
            use_bv = true;
            int bv_size = bvs[0].get_sort().bv_size();
            for (int i = 0; i < random_int(3, 6); ++i) {
                bvs.push_back(z3::to_expr(bvs[0].ctx(),
                Z3_mk_numeral(bvs[0].ctx(), std::to_string(random_int(1, 100)).c_str(),
                bvs[0].get_sort())));
            }
        }
    }

    std::string generate_formula_as_str() {
        z3::expr formula = generate_formula();
        z3::solver sol(formula.ctx());
        sol.add(formula);
        return sol.to_smt2();
    }

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

    static int random_int(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    z3::expr random_int_expr() {
        return z3::to_expr(z3::context(), Z3_mk_int(z3::context(),
        random_int(-100, 100), z3::context().int_sort()));
    }

    z3::expr random_real_expr() {
        return z3::to_expr(z3::context(), Z3_mk_real(z3::context(),
        random_int(-100, 100), 1, z3::context().real_sort()));
    }

    z3::expr generate_formula() {
        // Implement logic similar to the Python version
        std::vector<z3::expr> res;

        for (int i = 0; i < random_int(3, 8); ++i) {
            if (use_int) bool_from_int();
            if (use_real) bool_from_real();
            if (use_bv) bool_from_bv();
        }

        for (int i = 0; i < 8; ++i) {
            if (random_int(0, 100) < 33) {
                if (use_int) int_from_int();
                if (use_real) real_from_real();
                if (use_bv) bv_from_bv();
            }

            if (random_int(0, 100) < 33) {
                if (use_int) bool_from_int();
                if (use_real) bool_from_real();
                if (use_bv) bool_from_bv();
            }

            if (random_int(0, 100) < 33) {
                bool_from_bool();
            }
        }

        int max_assert = random_int(5, 30);
        assert(!bools.empty());
        for (int i = 0; i < max_assert; ++i) {
            int clen = random_int(1, 8);
            z3::expr cls = (clen == 1) ? bools[random_int(0, bools.size() - 1)]
                                        : z3::mk_or(std::vector<z3::expr>(bools.begin(), bools.begin() + std::min(clen, static_cast<int>(bools.size()))));
            res.push_back(cls);
        }

        if (hard_bools.size() > 1) {
            res.insert(res.end(), hard_bools.begin(), hard_bools.end());
        }

        return (res.size() == 1) ? res[0] : z3::mk_and(res);
    }

    void int_from_int() {
        if (ints.size() >= 2) {
            std::vector<z3::expr> data = random_sample(ints, 2);
            z3::expr i1 = data[0];
            z3::expr i2 = data[1];
            double prob = random_probability();
            if (prob <= 0.2) {
                ints.push_back(i1 + i2);
            } else if (prob <= 0.4) {
                ints.push_back(i1 - i2);
            } else if (prob <= 0.6) {
                ints.push_back(i1 * i2);
            } else if (prob <= 0.8) {
                ints.push_back(i1 / i2);
            } else {
                ints.push_back(z3::mod(i1, i2));
            }
        }
    }

    void real_from_real() {
        if (reals.size() >= 2) {
            std::vector<z3::expr> data = random_sample(reals, 2);
            z3::expr r1 = data[0];
            z3::expr r2 = data[1];
            double prob = random_probability();
            if (prob <= 0.25) {
                reals.push_back(r1 + r2);
            } else if (prob <= 0.5) {
                reals.push_back(r1 - r2);
            } else if (prob <= 0.75) {
                reals.push_back(r1 * r2);
            } else {
                reals.push_back(r1 / r2);
            }
        }
    }

    void bv_from_bv() {
        if (bvs.size() >= 2) {
            std::vector<z3::expr> data = random_sample(bvs, 2);
            z3::expr r1 = data[0];
            z3::expr r2 = data[1];
            double prob = random_probability();
            if (prob <= 0.25) {
                bvs.push_back(r1 + r2);
                if (bv_no_overflow) {
                    hard_bools.push_back(z3::bvadd_no_overflow(r1, r2, bv_signed));
                }
                if (bv_no_underflow) {
                    hard_bools.push_back(z3::bvadd_no_underflow(r1, r2));
                }
            } else if (prob <= 0.5) {
                bvs.push_back(r1 - r2);
                if (bv_no_underflow) {
                    hard_bools.push_back(z3::bvsub_no_overflow(r1, r2));
                }
                if (bv_no_underflow) {
                    hard_bools.push_back(z3::bvsub_no_underflow(r1, r2, bv_signed));
                }
            } else if (prob <= 0.75) {
                bvs.push_back(r1 * r2);
                if (bv_no_underflow) {
                    hard_bools.push_back(z3::bvmul_no_overflow(r1, r2, bv_signed));
                }
                if (bv_no_underflow) {
                    hard_bools.push_back(z3::bvmul_no_underflow(r1, r2));
                }
            } else {
                bvs.push_back(r1 / r2);
                if (bv_signed) {
                    hard_bools.push_back(z3::bvsdiv_no_overflow(r1, r2));
                }
            }
        }
    }

    void bool_from_int() {
        if (ints.size() >= 2) {
            std::vector<z3::expr> data = random_sample(ints, 2);
            z3::expr i1 = data[0];
            z3::expr i2 = data[1];
            double prob = random_probability();
            z3::expr new_bool;
            if (prob <= 0.16) {
                new_bool = i1 < i2;
            } else if (prob <= 0.32) {
                new_bool = i1 <= i2;
            } else if (prob <= 0.48) {
                new_bool = i1 == i2;
            } else if (prob <= 0.62) {
                new_bool = i1 > i2;
            } else if (prob <= 0.78) {
                new_bool = i1 >= i2;
            } else {
                new_bool = i1 != i2;
            }
            bools.push_back(new_bool);
        }
    }

    void bool_from_real() {
        if (reals.size() >= 2) {
            std::vector<z3::expr> data = random_sample(reals, 2);
            z3::expr i1 = data[0];
            z3::expr i2 = data[1];
            double prob = random_probability();
            z3::expr new_bool;
            if (prob <= 0.16) {
                new_bool = i1 < i2;
            } else if (prob <= 0.32) {
                new_bool = i1 <= i2;
            } else if (prob <= 0.48) {
                new_bool = i1 == i2;
            } else if (prob <= 0.62) {
                new_bool = i1 > i2;
            } else if (prob <= 0.78) {
                new_bool = i1 >= i2;
            } else {
                new_bool = i1 != i2;
            }
            bools.push_back(new_bool);
        }
    }

    void bool_from_bv() {
        if (bvs.size() >= 2) {
            std::vector<z3::expr> data = random_sample(bvs, 2);
            z3::expr bv1 = data[0];
            z3::expr bv2 = data[1];
            double prob = random_probability();
            z3::expr new_bv;
            if (prob <= 0.16) {
                new_bv = bv_signed ? bv1 < bv2 : z3::ult(bv1, bv2);
            } else if (prob <= 0.32) {
                new_bv = bv_signed ? bv1 <= bv2 : z3::ule(bv1, bv2);
            } else if (prob <= 0.48) {
                new_bv = bv1 == bv2;
            } else if (prob <= 0.62) {
                new_bv = bv_signed ? bv1 > bv2 : z3::ugt(bv1, bv2);
            } else if (prob <= 0.78) {
                new_bv = bv_signed ? bv1 >= bv2 : z3::uge(bv1, bv2);
            } else {
                new_bv = bv1 != bv2;
            }
            bools.push_back(new_bv);
        }
    }

    void bool_from_bool() {
        if (bools.size() >= 2) {
            if (random_probability() < 0.22) {
                z3::expr b = bools[random_int(0, bools.size() - 1)];
                bools.push_back(!b);
                return;
            }

            std::vector<z3::expr> data = random_sample(bools, 2);
            z3::expr b1 = data[0];
            z3::expr b2 = data[1];
            double prob = random_probability();
            if (prob <= 0.25) {
                bools.push_back(b1 && b2);
            } else if (prob <= 0.5) {
                bools.push_back(b1 || b2);
            } else if (prob <= 0.75) {
                bools.push_back(z3::xor_(b1, b2));
            } else {
                bools.push_back(z3::implies(b1, b2));
            }
        }
    }

    template <typename T>
    std::vector<T> random_sample(const std::vector<T>& vec, size_t n) {
        std::vector<T> copy = vec;
        std::random_shuffle(copy.begin(), copy.end());
        return std::vector<T>(copy.begin(), copy.begin() + n);
    }

    double random_probability() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(gen);
    }
};

int main() {
    z3::context ctx;
    z3::expr w = ctx.int_const("w");
    z3::expr x = ctx.int_const("x");
    z3::expr y = ctx.int_const("y");
    z3::expr z = ctx.int_const("z");

    std::vector<z3::expr> vars = {w, x, y, z};
    FormulaGenerator generator(vars);
    std::cout << generator.generate_formula_as_str() << std::endl;

    return 0;
}
