//
// Created by jxqhhh on 2020/3/6.
//

#ifndef DPLL_CDCL_H
#define DPLL_CDCL_H

#include "common.h"
#include <vector>

namespace CDCL {

    enum assignment { // possible value assignment for propositional variables
        _true, _false, _undefined
    };

    struct node {
        bool assigned = false; // indicating whether the node has been assigned; this attribute must be set to false if you undo the assignment
        enum assignment value = _undefined;
        int decision_level;
        int antecedent = 0; // range: 0 - num_clauses (0 corresponds to decision variables or unassigned variables)
        std::vector<int> antecedent_node_index; // range of elements of antecedent_node_index: 1 - num_clause
        std::vector<int> descendant_node_index; // range of elements of descendant_node_index: 0 - num_clause (0 corresponds to the special conflict node)
        node() : assigned(false), value(_undefined), antecedent(0) {
            descendant_node_index.clear();
            antecedent_node_index.clear();
        }
    };

    class CDCL {
        CDCL(const CDCL &) = delete;

        CDCL(CDCL &&) = delete;

        CDCL() = delete;

        ~CDCL();

        CDCL(const formula &_phi);

        /**
         * Check if the formula is satisfiable.
         *
         * @return true if satisfiable, and false if unsatisfiable
         * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
         */
        bool check_sat();

        /**
         * Get a satisfying model (interpretation) of the formula, the model must be *complete*, that is,
         * it must assign every variable a truth value.
         * This function will be called if and only if `check_sat()` returns true.
         *
         * @return an arbitrary (if there exist many) satisfying model
         * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
         */
        model get_model();

    private:
        node *graph = nullptr; // graph[0] corresponds to the special conflict node
        formula phi;
        int current_decision_level = 0;

        /**
         * Detect the case that the unit propogation rule can be applied; apply the rule if it is the case
         * @return true iff at least one unit detected
         */
        bool exists_unit();

        /**
         * If any decision variable exists within current interpretation, apply the backjump rule.
         * @return true if has any decision variable else false
         */
        bool has_decision();

        /**
         * Apply the decision rule to assign a variable.
         */
        void decide();

        /**
         * @return true if a conflict is detected else false
         **/
        bool conflict();
    };
}


#endif //DPLL_CDCL_H
