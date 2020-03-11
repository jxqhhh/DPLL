import os
import random
import re
from functools import partial
import argparse

PROBABILITY = 0.05


def generate_testing_data(testing_data_fn:str, num_variables, num_clauses):

    global PROBABILITY

    assert (num_clauses>0) and isinstance(num_clauses, int)
    assert (num_variables > 0) and isinstance(num_variables, int)

    if os.path.exists(testing_data_fn):
        print("File \"{}\" alreasy exists. Try another filename for generated testing data.".format(testing_data_fn))
        return False
    f = open(testing_data_fn,'w')

    # write header:
    f.write("p cnf {} {}\n".format(num_variables, num_clauses))

    # write clauses:
    for i in range(num_clauses):
        clause = ""
        okay = False
        while not okay:
            for literal in range(1,num_variables+1):
                if random.uniform(0,1) > PROBABILITY:
                    if random.uniform(0,1) > 0.5:
                        clause += str(literal)
                    else:
                        clause += str(-literal)
                    clause += " "
            if len(clause) > 0:
                okay = True
        else:
            clause += '0\n'
            f.write(clause)

    f.flush()
    f.close()

    return True

def my_solver(solver_path, testing_data_fn):
    with os.popen("{} {}".format(solver_path, testing_data_fn)) as f:
        res = f.readlines()
    for r in res:
        if 'UNSAT' in r.upper():
            return (False, None, res[-1])
    assignment = {}
    for r in res:
        if '->' in r:
            literal = abs(int(r.split('->')[0]))
            value = (int(r.split('->')[1])==1)
            assignment[literal] = value
    return (True, assignment, res[-1])



def test_satisfying_assignment(assignment:dict, testing_data_fn):
    with open(testing_data_fn, 'r') as f:
        cnf = f.readlines()
    for clause in cnf:
        if clause.startswith("p"):
            num_variables = int(clause.replace("p cnf ","").split(" ")[0])
            if num_variables!= len(assignment.keys()):
                return False, "num of assigned varibales({}) not equal to num of variables({})".format(len(assignment.keys()), num_variables)
            for i in range(1, num_variables+1):
                if not (i in assignment.keys()):
                    return False, "variable {} not assigned".format(i)
            continue
        if clause.startswith('c'):
            continue
        literals = [int(l) for l in clause[:-3].split(" ")]
        clause_sat = False
        for literal in literals:
            if abs(literal) in assignment.keys():
                value = assignment[abs(literal)]
                if (literal > 0) and value:
                    clause_sat = True
                elif (literal < 0) and (not value):
                    clause_sat = True
        if not clause_sat:
            return False, "current assignment unsatisfies the clause: \n{}\ncurrent assignment: \n{}\n".format(str(literals), str(assignment))
    return True, ""


def minisat(testing_data_fn):
    # return true if satisfiable; return false else
    with os.popen('minisat {}'.format(testing_data_fn)) as f:
        res = f.readlines()
    if res[-1].upper().replace("\n","")=="UNSATISFIABLE":
        return False
    else:
        return True

def error_handler(epoch:int, testing_data_fn, info:str):
    print("Epoch {}: Error with testing sample {}\n".format(epoch, testing_data_fn))
    print("Error info: {}\n".format(info))
    exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-p","--path", help="path to executable SAT solver",required=True)
    parser.add_argument("-e","--epochs", help="epochs to test", required=True, type=int)
    parser.add_argument("--min_num_variables", help="minimum number of variables", default=1, type=int)
    parser.add_argument("--max_num_variables", help="maximum number of variables", default=40, type=int)
    parser.add_argument("--min_num_clauses", help="minimum number of clauses", default=1, type=int)
    parser.add_argument("--max_num_clauses", help="maximum number of clauses", default=10000, type=int)
    FLAGS = parser.parse_args()
    solver_path = FLAGS.path

    for epoch in range(FLAGS.epochs):
        fn = "".join([random.choice('abcdefgHIJKLMNopqRSTuvwXYZ1234567890') for length in range(random.randint(30,50))])+".dimacs"
        num_variables = random.randint(FLAGS.min_num_variables, FLAGS.max_num_variables)
        num_clauses = random.randint(FLAGS.min_num_clauses, FLAGS.max_num_clauses)
        if generate_testing_data(testing_data_fn=fn, num_variables=num_variables, num_clauses=num_clauses):
            my_solver_success = True
            minisat_result = minisat(testing_data_fn=fn)
            my_solver_result, my_solver_assignment, runtime_info = my_solver(solver_path=solver_path,testing_data_fn=fn)

            if my_solver_result and minisat_result:
                result, error_info = test_satisfying_assignment(assignment=my_solver_assignment, testing_data_fn=fn)
                if result:
                    pass
                else:
                    error_handler(epoch, fn, "My solver generates a wrong assignment for a satisfiable CNF\n"+error_info)
            elif my_solver_result and (not minisat_result):
                error_handler(epoch, fn, "My solver think it's satisfiable but minisat does not agree")
            elif (not my_solver_result) and (not minisat_result):
                pass
            else:
                error_handler(epoch, fn, "My solver think it's unsatisfiable but minisat does not agree")

            os.remove(fn)
            print("Epoch {}: num_variables={}, num_clauses={}".format(epoch, num_variables, num_clauses))
            print(runtime_info)
        else:
            pass

