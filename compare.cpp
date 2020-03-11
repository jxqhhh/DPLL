//
// Created by jxqhhh on 2020/3/10.
//

#include <iostream>
#include <chrono>
#include <string>
#include <vector>

#include "common.h"
#include "DimacsParser.h"
#include "DPLL.h"
#include "CHRONOLOGICAL_BACKTRACK_DPLL.h"


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

    if (argc < 2) {
        std::cerr << "error: no input files" << std::endl;
        return 1;
    }

    std::vector<int> chronological_running_time;
    std::vector<int> dpll_running_time;

    for(int i = 1; i < argc; i++) {
        std::string f(argv[i]);
        std::cout << f << std::endl<<"----------------------------------------"<<std::endl;
        formula phi = DimacsParser::parse(f);

        {
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

            std::cout << "DPLL solver: " << std::endl;
            auto duration = std::chrono::duration<float, std::milli>(end - start);
            std::cout << "time: " << duration.count() << " ms" << std::endl<<std::endl;
        }

        {
            // timer start
            auto start = std::chrono::steady_clock::now();
            CHRONOLOGICAL_BACKTRACK_DPLL solver(phi);
            bool sat = solver.check_sat();
            model m;
            if (sat) {
                m = solver.get_model();
            }
            auto end = std::chrono::steady_clock::now();
            // timer end

            std::cout << "Chronological DPLL solver: " << std::endl;
            auto duration = std::chrono::duration<float, std::milli>(end - start);
            std::cout << "time: " << duration.count() << " ms" << std::endl << std::endl << std::endl;
        }

    }

    return 0;
}
