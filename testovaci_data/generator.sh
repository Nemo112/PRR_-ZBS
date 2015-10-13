#!/bin/bash

n=4;
t="AD";

while [[ $n -lt 20 ]];do
	k=3
	while [[ $k -lt $(($n-1)) ]];do
		./generator -t $t -n $n -k $k -o "test"$n"x"$k".txt";
		./souvislost -s -i test$n$k.txt  -o "test"$n"x"$k"_s.txt";
		k=$(($k+1));
	done;
	n=$(($n+1));
done;
