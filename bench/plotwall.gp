set terminal png enhanced large
#set terminal postscript enhanced "Helvetica" 16
set output
#set title "`uname -m`"
set yrange [0:]
plot 	'time-avg.txt' using 1:2 with lines title "wall"
#	'time-avg.txt' using 1:3 with lines title "sys", \
#	'time-avg.txt' using 1:4 with lines title "real"
