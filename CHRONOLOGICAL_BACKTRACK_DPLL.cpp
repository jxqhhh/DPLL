//
// CHRONOLOGICAL_BACKTRACK_DPLL algorithm.
//

#include "CHRONOLOGICAL_BACKTRACK_DPLL.h"

bool CHRONOLOGICAL_BACKTRACK_DPLL::exists_unit() {
    for(auto & _clause: phi.clauses){
        if(I.isUnitClause(_clause)){
            return true;
        }
    }
    return false;
}

bool CHRONOLOGICAL_BACKTRACK_DPLL::confilict() {
    for(auto & _clause: phi.clauses){
        if(I.conflict(_clause)){
            return true;
        }
    }
    return false;
}

bool CHRONOLOGICAL_BACKTRACK_DPLL::has_decision(bool use_backjump){
    for(auto & _clause: phi.clauses){
        if(I.has_decision(_clause, use_backjump)){
            return true;
        }
    }
    return false;
}

bool CHRONOLOGICAL_BACKTRACK_DPLL::sat() {
    if(I.num_assigned()<phi.num_variable){
        return false;
    }
    for(auto & _clause: phi.clauses){
        if(! I.sat(_clause)){
            return false;
        }
    }
    return true;
}


bool CHRONOLOGICAL_BACKTRACK_DPLL::check_sat() {
    I.reset(phi.num_variable);
    while(1){
        while(exists_unit()){};
        I.decide();
        if(confilict()){
            if(! has_decision()){
                return false;
            }
        }else if(sat()){
            return true;
        }
    }
}

model CHRONOLOGICAL_BACKTRACK_DPLL::get_model() {
    model m;
    I.getModel(m);
    return m;
}