BEGIN {print "\\begin{tabular}{lll}"; print "\\hline\\hline"; print "Testing System & Speedup nonshared & Speedup shared \\\\"}
/-shared-/ {print $1 " & " NONSHARED " & "$2"\\\\"}
/-nonshared-/ {NONSHARED=$2}
END {print "\\end{tabular}"}

