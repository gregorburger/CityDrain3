#!/usr/bin/env python
from cd3modelgen import *
import sys

max_nesting = 10
s=Simulation()

def to_tree(parent_node, nesting = 0):
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
        to_tree(s1, nesting+1)
        to_tree(s2, nesting+1)

def main():
    f = FileOut("tmp/tree-out.txt")
    to_tree(f)
    s.nodes += [f]
    s.render()
    pass

if __name__ == "__main__":
    main()
