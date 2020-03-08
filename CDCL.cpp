//
// Created by jxqhhh on 2020/3/6.
//

#include "CDCL.h"
#include <unordered_map>
#include <queue>
#include <unordered_set>

CDCL::CDCL::~CDCL() {
    if(_graph!=nullptr){
        delete _graph;
        _graph = nullptr;
    }
}

CDCL::CDCL::CDCL(const formula& _phi): phi(_phi){
    _graph = new graph(phi.num_variable);
}

model CDCL::CDCL::get_model() {
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

void CDCL::CDCL::decide(){
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

bool CDCL::CDCL::conflict() {
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
            int max_decision_level = 0;
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

bool CDCL::CDCL::has_decision() {
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
        /*
        // generate the conflict clause:
        bool changed;
        //std::unordered_map<int, bool> learnt_clause;
        std::unordered_set<literal> learnt_clause;
        learnt_clause.insert(0);
        //learnt_clause[0]=true;
        int decision_level = _graph->nodes[0].decision_level;
        do{
            changed = false;
            /*
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
            }*/

        /*
            for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
                auto &n=graph[VAR(*it)];
                if(n.decision_level==decision_level && n.antecedent!=0){
                    changed = true;
                    for(auto lit=n.antecedent_node_index.begin(),lit2=n.antecedent_node_index.end();lit!=lit2;lit++){
                        bool posiitve = (n.value == _true);
                        if(posiitve) {
                            learnt_clause.insert(-*lit);
                        }else{
                            learnt_clause.insert(*lit);
                        }
                    }
                    learnt_clause.erase(*it);
                    goto NextWhileLoop;
                }
            }
            NextWhileLoop:
            ;
        }while(changed);*/

        // generate and include the new clause
        clause generated_clause;
        _graph->generate_clause(0, generated_clause, _graph->nodes[0].decision_level);
        for(auto it=generated_clause.begin(),it2=generated_clause.end();it!=it2;it++){
            *it = (-*it);
        }
        phi.clauses.push_back(generated_clause);
        phi.num_variable ++;

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

        // Update the literal node in the leanrt clause with maximum decision level:
        auto &_node = _graph->nodes[VAR(literal_of_max_decision_level_in_new_clause)];
        _node.value = (_node.value == _true)?(_false):(_true);
        _node.antecedent = phi.clauses.size();
        for(int i = 0;i<_graph->num_nodes;i++){
            _graph->remove_edge(i, VAR(literal_of_max_decision_level_in_new_clause));
        }
        for(auto &_literal:generated_clause){
            if(_literal!=literal_of_max_decision_level_in_new_clause){
                _graph->add_edge(VAR(_literal),VAR(literal_of_max_decision_level_in_new_clause));
            }
        }

        // Remove child nodes of the literal node in the leanrt clause with maximum decision level:
        std::queue<int> child_nodes_index_queue;
        child_nodes_index_queue.push(VAR(max_decision_level_in_new_clause));
        while(!child_nodes_index_queue.empty()){
            auto index = child_nodes_index_queue.front();
            child_nodes_index_queue.pop();
            _graph->remove_node(index);
            int base = index*_graph->num_nodes;
            for(int i = 0;i< _graph->num_nodes;i++, base++){
                if(_graph->edges[base]){
                    child_nodes_index_queue.push(i);
                }
            }
        }
        return true;
        /*
        for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
            generated_clause.push_back(*it);
        }*/

        /*
        // apply the backjump rule:
        std::unordered_set<int> to_be_erased_node_index;
        //int size = 0;
        for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
            if(graph[VAR(*it)].decision_level==decision_level){
                auto &_node = graph[VAR(*it)];
                _node.assigned = true;
                _node.value = _false;
                _node.antecedent = phi.clauses.size();

                _node.decision_level = -1;
                int max_dl = -1;
                for(auto lit = learnt_clause.begin(),lit2=learnt_clause.end();lit!=lit2;lit++){
                    if(graph[VAR(*lit)].decision_level > max_dl){
                        max_dl = graph[VAR(*lit)].decision_level;
                    }
                }
                _node.decision_level = max_dl;

                for(auto lit=learnt_clause.begin(),lit2=learnt_clause.end();lit!=lit2;lit++){
                    if(*it!=*lit) {
                        _node.antecedent_node_index.insert(VAR(*it));
                        graph[VAR(*it)].descendant_node_index.insert(*it);
                    }
                }
                for(auto &index: _node.descendant_node_index){
                    to_be_erased_node_index.insert(index);
                }
                _node.descendant_node_index.clear();
                break;
            }
        }

        while(!to_be_erased_node_index.empty()){
            auto _literal =*(to_be_erased_node_index.begin());
            auto &_node = graph[VAR(_literal)];
            for(auto &index: _node.descendant_node_index){
                to_be_erased_node_index.insert(index);
            }
            for(auto &index: _node.antecedent_node_index){
                graph[VAR(index)].descendant_node_index.erase(index);
            }
            _node.assigned = false;
            _node.value = _undefined;
            _node.descendant_node_index.clear();
            _node.antecedent_node_index.clear();
            _node.antecedent = 0;
            to_be_erased_node_index.erase(VAR(_literal));
        }
        return true;

        /*
        int size = to_be_erased_node_index.size();
        do{
            int n = to_be_erased_node_index.front();
            to_be_erased_node_index.pop();
            auto &_node=graph[n];

            for(auto &index: _node.descendant_node_index){
                to_be_erased_node_index.push(index);
                _node.descendant_node_index
            }
            if(dl_size == 1){
                _node.assigned = true;
                _node.value = _false;
                _node.antecedent = phi.clauses.size();
                _node.decision_level = decision_level;
                _node.antecedent_node_index.clear();
                _node.descendant_node_index.clear();
                for(auto it=learnt_clause.begin(),it2=learnt_clause.end();it!=it2;it++){
                    if(VAR(*it)!=n){
                        _node.antecedent_node_index.push_back(VAR(*it));
                        graph[VAR(*it)].descendant_node_index.push_back(n);
                    }
                }
            }
            if(size<=0){

            }
            size -- ;
            if(_node.decision_level == decision_level ){
                dl_size--;
            }
        }while(!to_be_erased_node_index.empty());
    */
    }
}

bool CDCL::CDCL::sat(){
    for(int i=1;i<=phi.num_variable;i++){
        auto &n=_graph->nodes[i];
        if((!n.assigned)){
            return false;
        }
    }
    for(auto &_clause: phi.clauses){
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