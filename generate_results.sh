#!/bin/bash
ptht=`pwd`;
arrl=`ls ZBS*/main`;
tsar=`ls testovaci_data/*.txt`;
#for i in ${arrl[@]};do
	i="./ZBS_var/main";
	a=3;
	while [[ $a -lt 20 ]];do
		for j in ${tsar[@]};do
			k=0;
			while [[ k -lt 4 ]];do
				eval "./$i $ptht\"/\"$j $a" > outputs/`echo $j | cut -d'/' -f 2 | cut -d. -f 1`_$a.txt &
				k=$(($k+1));
			done;
			wait;
		done;
		a=$(($a+1));
	done;
#done
