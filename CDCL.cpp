//
// Created by jxqhhh on 2020/3/6.
//

#include "CDCL.h"

CDCL::CDCL::~CDCL() {
    if(graph!=nullptr){
        delete[] graph;
        graph = nullptr;
    }
}

CDCL::CDCL::CDCL(const formula& _phi): phi(_phi){
    //TODO
    graph = new node[phi.num_variable];
}

model CDCL::CDCL::get_model() {
    model m;
    for(int i=0;i<phi.num_variable;i++){
        if(graph[i].value = _true){
            m[i] = true;
        }else{
            m[i] = false;
        }
    }
    return m;
}

bool CDCL::CDCL::check_sat() {
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

bool CDCL::CDCL::exists_unit() {
    for(auto &clause: phi.clauses){
        
        for(auto &literal: clause){

        }
    }
    return false;
}