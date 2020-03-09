# Author: Zhang Xinwei
# Modify: Wang Yuanbiao
# Validation for the 18 test cases with TLE detection


import os
import re
from functools import partial


unsat_indices = [0, 4, 7, 10, 12, 14, 15, 16, 18]
num_vars = [1, 5, 4, 5, 20, 12, 20, 21, 70, 57, 90, 42, 35, 45, 80, 50, 275, 163, 157]


def execute(i):   
    with os.popen('./release/dpll tests/test{}.dimacs'.format(i)) as f:
        res = f.readlines()
    return res


def parseDimacs(i):
    filename = 'tests/test{}.dimacs'.format(i)
    clauses = []
    with open(filename) as f:
        lines = f.readlines()
    for line in lines:
        line = line.strip()
        if len(line) == 0: continue
        if line[0] in ['c', 'p']: continue
        literals = list(map(int, line.split()[:-1]))
        clauses.append(literals)
    return clauses


def parseExecuteResult(res):
    reg = re.compile(r'(time:\s*)(\S*)(\s*ms)')
    dplltime = float(reg.findall(res[-1])[0][1])
    sat = not 'unsat' in res[1]
    if not sat: 
        return True, dplltime
    res = map(lambda x: x.strip(), res[2:-1])
    interpretation = {}
    for r in res:
        literal, _, value = r.split()
        literal = int(literal)
        value = int(value) * 2 - 1
        interpretation[literal] = value
    return interpretation, dplltime


def check(clauses, interpretation, index, dplltime):
    if dplltime > 5000:
        return False
    if interpretation is True:
        return (index in unsat_indices)
    else:
        condition1 = all(map(any, map(partial(map, lambda x: x * interpretation[abs(x)] > 0), clauses)))
        condition2 = (len(interpretation) == num_vars[index])
        return (condition1 and condition2)


def test(i):
    res = execute(i)
    inter, dplltime = parseExecuteResult(res)
    clauses = parseDimacs(i)
    
    if inter is True:
        print(i, 'unsat', end=' ')
    else:
        print(i, 'sat for', len(inter), 'vars', end=' ')

    if check(clauses, inter, i, dplltime):
        print('pass')
        return True
    else:
        print('fail')
        return False


total = 19
score = 0
for i in range(total):
    if test(i) is True:
        score += 1
print('\ntotal score: %d / %d' % (score, total))