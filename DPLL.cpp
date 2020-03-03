//
// DPLL algorithm.
//

#include "DPLL.h"

bool DPLL::exists_unit() {
    for(auto & _clause: phi.clauses){
        if(I.isUnitClause(_clause)){
            return true;
        }
    }
    return false;
}

bool DPLL::confilict() {
    for(auto & _clause: phi.clauses){
        if(I.conflict(_clause)){
            return true;
        }
    }
    return false;
}

bool DPLL::has_decision(bool use_backjump){
    for(auto & _clause: phi.clauses){
        if(I.has_decision(_clause, use_backjump)){
            return true;
        }
    }
    return false;
}

bool DPLL::sat() {
    for(auto & _clause: phi.clauses){
        if(not I.sat(_clause)){
            return false;
        }
    }
    return true;
}


bool DPLL::check_sat() {
    I.reset(phi.num_variable);
    while(1){
        while(exists_unit()){};
        I.decide();
        if(confilict()){
            if(not has_decision()){
                return false;
            }
        }else if(sat()){
            return true;
        }
    }
}

model DPLL::get_model() {
    model m;
    I.getModel(m);
    return m;
}