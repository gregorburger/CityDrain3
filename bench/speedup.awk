#!/bin/env awk
BEGIN {min = 0; base = 0}
$1==1	{ if (base == 0)
		base = $2
	  if ($2 < base)
	  	base = $2
	}
	
$1!=1	{ if (min == 0) 
		min = $2
	  if ($2 < min)
	  	min = $2
	  }
END {print ARGV[1] " "  base / min }
