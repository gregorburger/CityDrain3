set terminal png enhanced large
set output
set yrange [0:]
set ylabel "runtime"
set xlabel "number of threads"
plot 	'time-avg.txt' using 1:2 with lines title "wall"
#	'time-avg.txt' using 1:3 with lines title "sys", \
#	'time-avg.txt' using 1:4 with lines title "real"
