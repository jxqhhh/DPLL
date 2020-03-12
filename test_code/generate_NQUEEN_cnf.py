import os

class NQUEEN_CNF_GENERATOR:
    def __init__(self, N: int):
        assert N>0
        self.N = N
        self.CNF = None

    def write_to_file(self, fn):
        if self.CNF is None:
            self.generate_CNF()
        self.num_clauses = len(self.CNF)
        if os.path.exists(fn):
            print("File {} already exists. Exiting...".format(fn))
            exit(1)
        with open(fn, 'w') as f:
            f.write("p cnf {} {}\n".format(self.N*self.N, self.num_clauses))
            string = ""
            for clause in self.CNF:
                for literal in clause:
                    string += str(literal)
                    string += " "
                string += "0\n"
            f.write(string)
            f.flush()
            f.close()


    def generate_CNF(self):
        if self.CNF is None:
            self.CNF = []
        else:
            return

        if self.N >= 2:
            # row constraint:
            for row in range(self.N):
                for column1 in range(self.N):
                    for column2 in range(column1+1, self.N):
                        self.CNF.append([-self.get_atom(row, column1), -self.get_atom(row, column2)])

            # column constraint:
            for column in range(self.N):
                for row1 in range(self.N):
                    for row2 in range(row1+1, self.N):
                        self.CNF.append([-self.get_atom(row1, column), -self.get_atom(row2, column)])

            # diagonal constraint:
            for sum in range(2*self.N-1):
                for row1 in range(0, self.N):
                    for row2 in range(row1+1, self.N):
                        column1 = sum - row1
                        column2 = sum - row2
                        if (0<=column1) and (0<=column2) and (column1<self.N) and (column2<self.N):
                            self.CNF.append([-self.get_atom(row1, column1), -self.get_atom(row2, column2)])

            for difference in range(-(self.N-1),self.N):
                for column1 in range(0, self.N):
                    for column2 in range(column1+1, self.N):
                        row1 = column1+difference
                        row2 = column2+difference
                        if (0<=row1) and (0<=row2) and (row1<self.N) and (row2<self.N):
                            self.CNF.append([-self.get_atom(row1, column1), -self.get_atom(row2, column2)])

            # number of queen constraint:
            for row in range(self.N):
                self.CNF.append([self.get_atom(row, column) for column in range(self.N)])

        else:
            self.CNF.append([1]) # a trivial case

    def get_atom(self, row:int, column:int):
        return row*self.N+column+1

if __name__ == '__main__':
    for i in range(5, 20, 5):
        queen=NQUEEN_CNF_GENERATOR(i)
        queen.write_to_file("queen{}.dimacs".format(i))


