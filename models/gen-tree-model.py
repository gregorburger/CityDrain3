#!/usr/bin/env python
from cd3modelgen import *
import sys

#max_nesting = 10
s=Simulation("ParallelSimulation")

def to_tree(parent_node, max_nesting, nesting = 0):
    if nesting == max_nesting:
        c=ConstSource()
        s.nodes += [c]
        s.cons+=[Connection(c, parent_node)]
    else:
        m = Mixer()
        s1 = Sewer()
        s2 = Sewer()
        s.nodes+=[m, s1, s2]
        s.cons+=[Connection(m, parent_node),
                 Connection(s1, m, "out", "inputs[0]"),
                 Connection(s2, m, "out", "inputs[1]")]
        to_tree(s1, max_nesting, nesting+1)
        to_tree(s2, max_nesting, nesting+1)

def main():
    if (len(sys.argv) > 1):
	max_nesting = int(sys.argv[1])
    else:
	max_nesting = 10
    f = FileOut("tmp/tree-out.txt")
    to_tree(f, max_nesting)
    s.nodes += [f]
    s.render()
    sys.stderr.write("rendered with max_nesting %d\n" % max_nesting)

if __name__ == "__main__":
    main()
