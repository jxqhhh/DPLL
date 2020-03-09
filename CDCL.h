//
// Created by jxqhhh on 2020/3/6.
//

#ifndef DPLL_CDCL_H
#define DPLL_CDCL_H

#include "common.h"
#include <vector>
#include <unordered_set>
#include <queue>
#include <cstring>
namespace CDCL {

    enum assignment { // possible value assignment for propositional variables
        _true, _false
    };

    struct node {
        bool assigned = false; // indicating whether the node has been assigned; this attribute must be set to false if you undo the assignment
        enum assignment value;
        int decision_level; // range: -1,0,1,2,... (-1 corresponds to node produced by unit propagation rule but without any parent node)
        int antecedent = 0; // represents from which clause the node's value is derived; range: 0 - num_clauses (0 corresponds to decision variables or unassigned variables)
        node() : assigned(false), antecedent(0){}
    };

    struct graph {
        node* nodes=nullptr;
        bool* edges=nullptr; // edges[i*num_nodes+j] is true iff there is a edge from node i to node j
        const int num_nodes; // nodes include variable node and conflict node
        graph(int n_variables):num_nodes(n_variables+1){
            nodes = new node[num_nodes];
            int num_edges = num_nodes*num_nodes;
            edges = new bool[num_edges];
            std::memset(edges, 0, sizeof(bool)*num_edges);
        }
        graph() = delete;
        graph(const graph&) = delete;
        graph(const graph&&) = delete;
        ~graph() {
            delete[] nodes;
            delete[] edges;
            nodes = nullptr;
            edges = nullptr;
        }
        void remove_node(int index) {
            // We do not remove the edge starting from nodes[index] in this function! Because we will use these edges in CDCL::has_decision function!
            for(int i = index; i<num_nodes*num_nodes;i+=num_nodes){
                edges[i] = false;
            }

            auto& n = nodes[index];
            n.antecedent = 0;
            n.assigned=false;
        }
        void add_edge(int row, int column){
            if(edges[column*num_nodes+row] || (row == 10 && column==6) || (row == 6 && column==10)){
                //exit(1);;
                int i = 0;
            }
            edges[row*num_nodes+column] = true;
        }
        void remove_edge(int row, int column){
            edges[row*num_nodes+column] = false;
        }

        /**
         *
         * @param _literal the literal waiting to be processed
         * @param _clause the learnt new clause
         * @param decision_level decision level of the special conflict node
         */
        void generate_clause(literal _literal, std::unordered_set<literal>& _clause, const int decision_level, bool* processed, int depth){
            if(processed[VAR(_literal)]){
                return;
            }
            int column = VAR(_literal);
            bool has_parent_node=false;
            for(int i=column;i<num_nodes*num_nodes;i+=num_nodes){
                if(edges[i]) {
                    int row = i/num_nodes;
                    has_parent_node = true;
                    auto &n = nodes[row];
                    if(n.decision_level<decision_level){
                        _clause.insert(i/num_nodes);
                    }else{
                        generate_clause((n.value==_true)?(row):(-row), _clause, decision_level, processed, depth+1);
                    }
                }
            }
            if(!has_parent_node){
                _clause.insert(_literal);
            }
            processed[VAR(_literal)] = true;
        }
    };

    class CDCL {
    public:
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
        graph* _graph = nullptr;
        formula phi;
        int current_decision_level = 0;
        int num_original_clauses;
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

        /**
         * @return true if this->graph satisfies this->phi
         */
        bool sat();
    };
}


#endif //DPLL_CDCL_H
