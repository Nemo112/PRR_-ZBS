#!/usr/bin/gnuplot
set terminal png
set output "graf_all.png"
set title "Graf výpočtu"
set ylabel "Čas"
set xlabel "Počet procesů"
plot "final.dat" using 1:2 with lines title "a=3", \
"final.dat" using 1:3 with lines title "a=4", \
"final.dat" using 1:4 with lines title "a=5"
#(2.00894)/x with lines title "Lineární zrychlení pro a=3", \
#(32.9316)/x with lines title "Lineární zrychlení pro a=4", \
#(411.382)/x with lines title "Lineární zrychlení pro a=5"

set terminal png
set output "graf3.png"
set title "Graf výpočtu a=3"
set ylabel "Čas"
set xlabel "Počet procesů"
plot "final.dat" using 1:2 with lines title "a=3", \
(2.00894)/x with lines title "Lineární zrychlení pro a=3"

set terminal png
set output "graf4.png"
set title "Graf výpočtu a=4"
set ylabel "Čas"
set xlabel "Počet procesů"
plot "final.dat" using 1:3 with lines title "a=4", \
(32.9316)/x with lines title "Lineární zrychlení pro a=4"

set terminal png
set output "graf5.png"
set title "Graf výpočtu a=5"
set ylabel "Čas"
set xlabel "Počet procesů"
plot "final.dat" using 1:4 with lines title "a=5", \
(411.382)/x with lines title "Lineární zrychlení pro a=5"



