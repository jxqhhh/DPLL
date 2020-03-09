//
// CHRONOLOGICAL_BACKTRACK_DPLL algorithm.
//

#ifndef CHRONOLOGICAL_BACKTRACK_DPLL_H
#define CHRONOLOGICAL_BACKTRACK_DPLL_H

#include "common.h"

class CHRONOLOGICAL_BACKTRACK_DPLL {
public:
    /**
     * Constructor.
     *
     * @param phi the formula to be checked
     * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
     */
    CHRONOLOGICAL_BACKTRACK_DPLL(const formula &phi) : phi(phi), I(phi.num_variable) {}

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
    formula phi;
    Interpretation I;

    /**
     * If a unit is detected, a corresponding propagation will be done inside the function.
     * @return false if not exists else true
     */
    bool exists_unit();

    /**
     * @return true if a conflict is detected else false
     **/
    bool confilict();

    /**
     * @param use_backjump: apply the backjump rule if the use_backjump argument is true; else apply the backtrack rule
     * If any decision variable exists within current interpretation, apply the backtrack/backjump rule.
     * @return true if has any decision variable else false
     */
    bool has_decision(bool use_backjump = false);

    /**
     * @return true if this->I satisfies this->phi
     */
    bool sat();
};


#endif //CHRONOLOGICAL_BACKTRACK_DPLL_H
