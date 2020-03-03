//
// Common data structures.
//

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

#ifndef DPLL_COMMON_H
#define DPLL_COMMON_H

// A literal is a atomic formula (that contains a variable). Like in dimacs,
// + positive numbers denote the corresponding variables;
// - negative numbers denote the negations of the corresponding variables.
// Variables are numbered from 1.
typedef int literal;
#define POSITIVE(x) ((x) > 0)
#define NEGATIVE(x) ((x) < 0)
#define VAR(x) (((x) > 0) ? (x) : (-(x)))

// A clause is a list of literals (meaning their disjunction).
typedef std::vector<literal> clause;

// A formula is a list of clauses (meaning their conjunction).
// We also specify the total number of variables, as some of them may not occur in any clause!
struct formula {
    int num_variable;
    std::vector<clause> clauses;

    formula(int n, const std::vector<clause> &clauses) : num_variable(n), clauses(clauses) {}
};

#include <vector>

using std::vector;

// A variableAssignment records how a variable is assigned within some interpretation.
struct variableAssignment {
    bool isDecided; // whether produced by decision rule
    int index; // range: 1-num_variable
    bool isPositive; // true if assigned true; false if assigned false
    variableAssignment() : isDecided(true), isPositive(true), index(0) {} // default constructor
};


// A satisfying model (interpretation).
// e.g. `model[i] = false` means variable `i` is assigned to false.
typedef std::unordered_map<int, bool> model;

struct Interpretation {
    Interpretation(int num_variables) : _lastDecisionIndex(-1), _num_varibales(num_variables) {
        if (num_variables < 0) {
            num_variables = -num_variables;
        }
        _assignments.reserve(num_variables);
        _model.reserve(num_variables);
    }

    Interpretation() = delete;

    void reset(int num_variables) {
        _lastDecisionIndex = -1;
        _assignments.clear();
        _model.clear();
        _num_varibales = num_variables;
    }

    bool isUnitClause(const clause &c) {
        /**
         * Check if the input clause is unit under current interpretation.
         * If a unit is found, update the interpretation.
         */
        int num_undefined = 0;
        int undefinedliteral;
        for (auto _literal: c) {
            auto pos = _model.find(abs(_literal));
            if (pos == _model.end()) {
                num_undefined++;
                if (num_undefined == 1) {
                    undefinedliteral = _literal;
                } else {
                    return false; // too many undefined varibales in the clause, indicating the clause is not a unit
                }
            } else {
                bool isPositiveliteral = _literal > 0;
                bool isPositiveInterpretation = pos->second;
                if (isPositiveliteral and (not isPositiveInterpretation)) {
                    continue;
                } else if ((not isPositiveliteral) and isPositiveInterpretation) {
                    continue;
                } else {
                    return false; // the clause is true under current interpretation, indicating the clause is not a unit
                }
            }
        }
        if (num_undefined == 1) {
            // The clause is a unit! Update the interpretation now:
            variableAssignment a;
            a.isDecided = false;
            a.isPositive = undefinedliteral > 0;
            a.index = abs(undefinedliteral);
            _assignments.push_back(a);
            _model[a.index] = a.isPositive;
            return true;
        }
        return false;
    }

    void decide() {
        /**
         * Apply the decision rule to update the interpretation.
         */
        for (int i = 1; i <= _num_varibales; i++) {
            if (_model.find(i) == _model.end()) {
                _lastDecisionIndex = _assignments.size();
                _model[i] = true;
                variableAssignment a;
                a.isDecided = true;
                a.isPositive = true;
                a.index = i;
                _assignments.push_back(a);
                return;
            }
        }
    }

    bool conflict(const clause &c) const {
        for (auto &_literal: c) {
            bool isPositiveLiteral = _literal > 0;
            auto pos = _model.find(abs(_literal));
            if (pos == _model.end()) {
                return false; // undefined variable found, indicating no conflict
            }
            bool isPositiveInterpretation = pos->second;
            if (isPositiveLiteral and isPositiveInterpretation) {
                return true;
            }
            if (not(isPositiveLiteral or isPositiveInterpretation)) {
                return true;
            }
        }
        return false;
    }

    bool has_decision(const clause &c, bool use_backjump) {
        /**
         * Check if there exists any decision variable and apply the backtrack/backjump rule if exists.
         * @param use_backjump: apply the backjump rule if the use_backjump argument is true; else apply the backtrack rule
         */
        if (_lastDecisionIndex < 0) {
            return false;
        }
        if (use_backjump) {
            //TODO
            exit(1);
        } else {
            variableAssignment &v = _assignments[_lastDecisionIndex];

            // Update _model:
            auto pos = _model.find(v.index);
            if (pos == _model.end()) {
                exit(1);
            } else {
                pos->second = false;
            }
            for (auto it = _assignments.begin() + _lastDecisionIndex + 1, it2 = _assignments.end(); it < it2; it++) {
                _model.erase(it->index);
            }

            // Update _assignments:
            v.isPositive = false;
            v.isDecided = false;
            _assignments.resize(_lastDecisionIndex + 1);
            _lastDecisionIndex = -1;
            int i = 0;
            for (auto it = _assignments.begin(), it2 = _assignments.end(); it < it2; it++, i++) {
                if (it->isDecided) {
                    _lastDecisionIndex = i;
                }
            }
            _assignments.reserve(_num_varibales);
            return true;
        }
    }

    bool sat(const clause &c) const {
        /**
         * Check if the clause c is satisfied by current interpretation.
         */
        for (auto &_literal: c) {
            auto pos = _model.find(abs(_literal));
            if (pos == _model.end()) {
                return false;
            }
            bool isPositiveLiteral = _literal > 0;
            bool isPositiveAssignment = pos->second;
            if (isPositiveAssignment and isPositiveLiteral) {
                return true;
            }
            if (not(isPositiveAssignment or isPositiveLiteral)) {
                return true;
            }
        }
        return false;
    }

private:
    vector<variableAssignment> _assignments; // One way to express the interpretation
    model _model; // Another way to express the interpretation; we will keep it identical to _assignments all the time
    int _lastDecisionIndex; // index for the last decision variable in Interpretation.assignments, starting from 0
    int _num_varibales;
};


#endif //DPLL_COMMON_H
