//
// Created by jxqhhh on 2020/3/6.
//

#include "CDCL.h"
#include <unordered_map>
#include <queue>

CDCL::CDCL::~CDCL() {
    if(graph!=nullptr){
        delete[] graph;
        graph = nullptr;
    }
}

CDCL::CDCL::CDCL(const formula& _phi): phi(_phi){
    //TODO
    graph = new node[phi.num_variable+1];
}

model CDCL::CDCL::get_model() {
    model m;
    for(int i=1;i<=phi.num_variable;i++){
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
        decide();
        if(conflict()){
            if(! has_decision()){
                return false;
            }
        }else if(sat()){
            return true;
        }
    }
}

bool CDCL::CDCL::exists_unit() {
    // outer loop:
    for(auto &clause: phi.clauses){
        bool found = false;
        int num_undefined = 0;
        int antecedent;
        int undefinedliteral;

        // inner loop:
        int counter = 0;
        for (auto &_literal: clause) {
            auto &n = this->graph[VAR(_literal)];
            counter++;
            if (!n.assigned) {
                num_undefined++;
                if (num_undefined == 1) {
                    undefinedliteral = _literal;
                    antecedent = counter;
                } else {
                    goto NextOuterLoop; // too many undefined varibales in the clause, indicating the clause is not a unit
                }
            } else {
                bool isPositiveliteral = _literal > 0;
                bool isPositiveInterpretation = (n.value == _true);
                if ((isPositiveliteral) && (!isPositiveInterpretation)) {
                    goto NextInnerLoop;
                } else if ((!isPositiveliteral) && (isPositiveInterpretation)) {
                    goto NextInnerLoop;
                } else {
                    goto NextOuterLoop;; // the clause is true under current interpretation, indicating the clause is not a unit
                }
            }

            NextInnerLoop:
                ;
        }

        if (num_undefined == 1) {
            // The clause is a unit!
            auto &n = this->graph[VAR(undefinedliteral)];
            n.value = (undefinedliteral > 0) ? _true:_false;
            n.assigned = true;
            n.antecedent = antecedent;
            int max_decision_level = 0;
            for(auto& _literal:phi.clauses[antecedent-1]){
                if(_literal!=undefinedliteral){
                    auto &partner = graph[VAR(_literal)];
                    partner.descendant_node_index.push_back(VAR(undefinedliteral));
                    n.antecedent_node_index.push_back(VAR(_literal));
                    int level = this->graph[VAR(_literal)].decision_level;
                    if(level>max_decision_level){
                        max_decision_level = level;
                    }
                }
            }
            n.decision_level = max_decision_level;
            return true;
        }

        NextOuterLoop:
            ;

    }
    return false;
}

void CDCL::CDCL::decide(){
    for(int i=1;i<=phi.num_variable;i++){
        auto &n = graph[i];
        if(n.assigned){
            continue;
        }
        n.assigned=true;
        n.antecedent = 0;
        n.decision_level = current_decision_level++;
        n.value = _true;
        return;
    }
}

bool CDCL::CDCL::conflict() {
    int counter = 0;
    for(auto &_clause: phi.clauses){
        counter++;
        for (auto &_literal: _clause) {
            bool isPositiveLiteral = _literal > 0;
            auto &n = graph[VAR(_literal)];

            bool isPositiveInterpretation = (n.value==_true);
            if (!n.assigned) {
                goto NextOuterLoop; // undefined variable found, indicating no conflict
            }
            if (isPositiveLiteral & isPositiveInterpretation) {
                goto NextOuterLoop;
            }
            if (!(isPositiveLiteral || isPositiveInterpretation)) {
                goto NextOuterLoop;
            }
        }

        //A conflict detected!
        graph[0].assigned = true;
        graph[0].value = _true;
        graph[0].antecedent = counter;
        int max_decision_level = 0;
        for(auto &_literal: _clause){
            auto &n = graph[VAR(_literal)];
            n.descendant_node_index.push_back(0);
            graph[0].antecedent_node_index.push_back(VAR(_literal));
            if(n.decision_level>max_decision_level){
                max_decision_level=n.decision_level;
            }
        }
        graph[0].decision_level = max_decision_level;
        return true;

        NextOuterLoop:
            ;

    }
    return false;
}

bool CDCL::CDCL::has_decision() {
    bool decision_found = false;
    for(int i=1;i<=phi.num_variable;i++){
        auto &n = graph[i];
        if(n.antecedent==0 && n.assigned){
            decision_found = true;
            break;
        }
    }
    if(decision_found){

        // generate the conflict clause:
        bool changed;
        std::unordered_map<int, bool> learnt_clause;

        learnt_clause[0]=true;
        int decision_level = graph[0].decision_level;
        do{
            changed = false;
            for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
                auto &n=graph[it->first];
                if(n.decision_level==decision_level && n.antecedent!=0){
                    for(auto lit=n.antecedent_node_index.begin(),lit2=n.antecedent_node_index.end();lit<lit2;lit++){
                        if(learnt_clause.find(*lit)==learnt_clause.end()){
                            learnt_clause[*lit] = (graph[*lit].value == _true);
                            changed = true;
                        }else if(learnt_clause[*lit] != (graph[*lit].value==true)){
                            learnt_clause.erase(*lit);
                            changed = true;
                        }
                    }
                    break;
                }
            }
        }while(changed);
        clause generated_clause;
        for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
            if(it->second){
                generated_clause.push_back(it->first);
            }else{
                generated_clause.push_back(-it->first);
            }
        }

        // apply the backjump rule:
        phi.clauses.push_back(generated_clause);
        std::queue<int> to_be_erased_node_index;
        int size = 0;
        for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
            to_be_erased_node_index.push(it->first);
            if(graph[it->first].decision_level==decision_level){
                size++;
            }
        }
        do{
            int n = to_be_erased_node_index.front();
            to_be_erased_node_index.pop();
            auto &_node=graph[n];

            for(auto &index: _node.descendant_node_index){
                to_be_erased_node_index.push(index);
            }
            if(size == 1){
                _node.assigned = true;
                _node.decision_level = decision_level;
                _node.antecedent_node_index.clear();
                _node.descendant_node_index.clear();
                for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
                    if(graph[it->first].decision_level==decision_level){
                        if(VAR(learnt_clause[it->first])!=n){
                            _node.antecedent_node_index.push_back(VAR(learnt_clause[it->first]);
                        }
                    }
                }
            }
            if(size >= 1 && _node.decision_level == decision_level ){
                size--;
            }
        }while(!to_be_erased_node_index.empty());
    }else{
        return true;
    }
}