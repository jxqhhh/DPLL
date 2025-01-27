//
// Created by jxqhhh on 2020/3/6.
//

#include "DPLL.h"
#include <unordered_map>
#include <queue>
#include <unordered_set>

DPLL::~DPLL() {
    if(_graph!=nullptr){
        delete _graph;
        _graph = nullptr;
    }
}

DPLL::DPLL(const formula& _phi): phi(_phi){
    _graph = new graph(phi.num_variable);
    num_original_clauses = phi.clauses.size();
}

model DPLL::get_model() {
    model m;
    for(int i=1;i<=phi.num_variable;i++){
        if(_graph->nodes[i].value == _true){
            m[i] = true;
        }else{
            m[i] = false;
        }
    }
    return m;
}

bool DPLL::check_sat() {
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

bool DPLL::exists_unit() {

    int counter=0; // counter for outer loop
    // outer loop:
    for(auto &clause: phi.clauses){
        counter++; // update counter for outer loop
        int num_undefined = 0; // num of undefined literals in the clause
        int undefinedliteral; // the undefined literal in the clause
        // inner loop:
        for (auto &_literal: clause) {
            auto &n = _graph->nodes[VAR(_literal)];
            if (!n.assigned) {
                num_undefined++;
                if (num_undefined == 1) {
                    undefinedliteral = _literal;
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

        if (num_undefined == 1) { // The clause is a unit!
            const int antecedent = counter;
            auto &n = _graph->nodes[VAR(undefinedliteral)];
            n.value = (undefinedliteral > 0) ? _true:_false;
            n.assigned = true;
            n.antecedent = antecedent;
            int max_decision_level = -1;
            for(auto& _literal:phi.clauses[antecedent-1]){
                if(_literal!=undefinedliteral){
                    auto &partner = _graph->nodes[VAR(_literal)];
                    int level = partner.decision_level;
                    if(level>max_decision_level){
                        max_decision_level = level;
                    }
                    _graph->add_edge(VAR(_literal),VAR(undefinedliteral));
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

void DPLL::decide(){
    for(int i=1;i<=phi.num_variable;i++){
        auto &n = _graph->nodes[i];
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

bool DPLL::conflict() {
    int counter = 0; // counter for outer loop
    for(auto it=phi.clauses.begin(),it2=phi.clauses.end();it!=it2;it++){
        auto &_clause = *it;
        counter++;
        bool detected=true;
        for (auto &_literal: _clause) {
            bool isPositiveLiteral = _literal > 0;
            auto &n = _graph->nodes[VAR(_literal)];

            bool isPositiveInterpretation = (n.value==_true);
            if (!n.assigned) {
                detected = false;
                goto Goto1; // undefined variable found, indicating no conflict
            }
            if (isPositiveLiteral & isPositiveInterpretation) {
                detected = false;
                goto Goto1;
            }
            if (!(isPositiveLiteral || isPositiveInterpretation)) {
                detected = false;
                goto Goto1;
            }
        }
        Goto1:

        if(detected) {
            //A conflict detected!
            auto &conflictNode = _graph->nodes[0];
            conflictNode.assigned = true;
            conflictNode.antecedent = counter;
            int max_decision_level = -1;
            for (auto &_literal: _clause) {
                auto &n = _graph->nodes[VAR(_literal)];
                if (n.decision_level > max_decision_level) {
                    max_decision_level = n.decision_level;
                }
                _graph->add_edge(VAR(_literal), 0);
            }
            conflictNode.decision_level = max_decision_level;
            return true;
        }

    }
    return false;
}

bool DPLL::has_decision() {
    bool decision_found = false;
    for(int i=1;i<=phi.num_variable;i++){
        auto &n = _graph->nodes[i];
        if(n.antecedent==0 && n.assigned){
            decision_found = true;
            break;
        }
    }
    if(!decision_found){
        return false;
    }else{
        if(_graph->nodes[0].decision_level < 0){ // special case: unavoidable conflict!
            return false;
        }

        // generate and include the new clause
        std::unordered_set<literal> _generated_clause_set;
        bool* processed = new bool[phi.num_variable+1];
        memset(processed, 0, sizeof(bool)*(phi.num_variable+1));

        _graph->generate_clause(0, _generated_clause_set, _graph->nodes[0].decision_level, processed, 0);
        delete[] processed;
        processed = nullptr;
        clause generated_clause;
        for(auto it=_generated_clause_set.begin(),it2=_generated_clause_set.end();it!=it2;it++){
            generated_clause.push_back(-*it);
        }
        phi.clauses.push_back(generated_clause);

        // apply the backjump rule
        int literal_of_max_decision_level_in_new_clause;
        int max_decision_level_in_new_clause=-2;
        for(auto &_literal: generated_clause){
            int dl = _graph->nodes[VAR(_literal)].decision_level;
            if(dl>max_decision_level_in_new_clause){
                max_decision_level_in_new_clause = dl;
                literal_of_max_decision_level_in_new_clause = _literal;
            }
        }


        /*if(max_decision_level_in_new_clause == -1){ // Special case:
            return false;
        }*/

        // Update the literal node in the leanrt clause with maximum decision level:
        auto &_node = _graph->nodes[VAR(literal_of_max_decision_level_in_new_clause)];
        auto v = (_node.value == _true)?(_false):(_true);
        _graph->remove_node(VAR(literal_of_max_decision_level_in_new_clause), true, false);
        _node.assigned = true;
        _node.value = v;
        _node.antecedent = phi.clauses.size();
        int max_decision_level = -1;
        for(auto &_literal:generated_clause){
            if(_literal!=literal_of_max_decision_level_in_new_clause){
                _graph->add_edge(VAR(_literal),VAR(literal_of_max_decision_level_in_new_clause));
                auto dl = _graph->nodes[VAR(_literal)].decision_level;
                if(dl>max_decision_level){
                    max_decision_level = dl;
                }
            }
        }
        //for(int i=0;i<_graph->num_nodes;i++){
        //    _graph->remove_edge(i,VAR(literal_of_max_decision_level_in_new_clause));
        //}
        _node.decision_level = max_decision_level;


        // Remove child nodes of the literal node in the leanrt clause with maximum decision level:
        std::queue<int> child_nodes_index_queue;
        for(int i = 0, base=_graph->num_nodes*VAR(literal_of_max_decision_level_in_new_clause);i<_graph->num_nodes;i++,base++) {
            if(_graph->edges[base]) {
                child_nodes_index_queue.push(i);
            }
        }
        while(!child_nodes_index_queue.empty()){
            auto index = child_nodes_index_queue.front();
            child_nodes_index_queue.pop();
            _graph->remove_node(index, true, false);
            int base = index*_graph->num_nodes;
            for(int i = 0;i< _graph->num_nodes;i++, base++){
                if(_graph->edges[base]){
                    if(_graph->nodes[i].assigned) {
                        child_nodes_index_queue.push(i);
                    }else{ // to avoid repeat enqueue
                        _graph->remove_edge(index, i);
                    }
                }
            }
        }
        return true;
    }
}

bool DPLL::sat(){
    for(int i=1;i<=phi.num_variable;i++){
        auto &n=_graph->nodes[i];
        if((!n.assigned)){
            return false;
        }
    }
    for(int i = 0;i<num_original_clauses;i++){
        auto &_clause = phi.clauses[i];
        // TODO: skip new leanred clauses
        for(auto &_literal: _clause){
            auto &n = _graph->nodes[VAR(_literal)];
            bool isPositiveLiteral = _literal > 0;
            bool isPositiveAssignment = (n.value==_true);
            if (isPositiveAssignment && isPositiveLiteral) {
                goto NextClause;
            }else if (!(isPositiveAssignment || isPositiveLiteral)) {
                goto NextClause;
            }
        }
        return false;
        NextClause:
            ;
    }
    return true;
}