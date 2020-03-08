#include <iostream>
#include <chrono>
#include <string>

#include "common.h"
#include "DimacsParser.h"
#include "CDCL.h"

// helper function to show a formula
std::string show_formula(const formula& phi) {
    std::stringstream ss;
    ss << "(and" << std::endl;
    for (const auto & clause : phi.clauses) {
        ss << "  (or";
        for (literal l : clause) {
            if (POSITIVE(l)) ss << " " << l;
            else ss << " (not " << VAR(l) << ")";
        }
        ss << ")" << std::endl;
    }
    ss << ")";

    return ss.str();
}

// entry
int main(int argc, char **argv) {
    /*
    for (int i = 0; i < 19; i++) {
        std::string fn = "/home/jxqhhh/DPLL/tests/test"+std::to_string(i)+".dimacs";
        std::string f(fn);
        // std::cout << f << std::endl;
        formula phi = DimacsParser::parse(f);
        // std::cout << show_formula(phi) << std::endl;

        // timer start
        auto start = std::chrono::steady_clock::now();
        DPLL solver(phi);
        bool sat = solver.check_sat();
        model m;
        if (sat) {
            m = solver.get_model();
        }
        auto end = std::chrono::steady_clock::now();
        // timer end
        std::cout<< i;
        if (sat) {
            std::cout << "  sat" << std::endl;
            if(! solver.sat()){
                std::cout<<"no@"<<std::endl;
            }
            //for (const auto &p : m) {
            //    std::cout << "    " << p.first << " -> " << p.second << std::endl;
            //}
        } else {
            std::cout << "  unsat" << std::endl;
        }

        auto duration = std::chrono::duration<float, std::milli>(end - start);
        std::cout << "  time: " << duration.count() << " ms" << std::endl;
    }

    return 0;
    */
    if (argc < 2) {
        std::cerr << "error: no input files" << std::endl;
        //return 1;
    }

    for (int i = 1; i < 2; i++) {
        std::string f("/home/jxqhhh/DPLL/tests/test0.dimacs");
        std::cout << f << std::endl;
        formula phi = DimacsParser::parse(f);
        // std::cout << show_formula(phi) << std::endl;

        // timer start
        auto start = std::chrono::steady_clock::now();
        CDCL::CDCL solver(phi);
        bool sat = solver.check_sat();
        model m;
        if (sat) {
            m = solver.get_model();
        }
        auto end = std::chrono::steady_clock::now();
        // timer end

        if (sat) {
            std::cout << "  sat" << std::endl;
            //for (const auto &p : m) {
            //    std::cout << "    " << p.first << " -> " << p.second << std::endl;
            //}
        } else {
            std::cout << "  unsat" << std::endl;
        }

        auto duration = std::chrono::duration<float, std::milli>(end - start);
        std::cout << "  time: " << duration.count() << " ms" << std::endl;
    }

    return 0;
}
