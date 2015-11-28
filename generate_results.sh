#!/bin/bash
ptht=`pwd`;
arrl=`ls ZBS*/main`;
tsar=`ls testovaci_data/*.txt`;
#for i in ${arrl[@]};do
	# i="./ZBS_var/main";
	a=10;
	while [[ $a -lt 20 ]];do
		for j in ${tsar[@]};do
			k=0;
			while [[ k -lt 2 ]];do
				if [[ ! -f outputs/`echo $j | cut -d'/' -f 2 | cut -d. -f 1`_$a.txt ]];then
					{
						(time ./ZBS_final/main $ptht/$j $a) |& tee outputs/`echo $j | cut -d'/' -f 2 | cut -d. -f 1`_$a.txt 1>/dev/null;
						echo outputs/`echo $j | cut -d'/' -f 2 | cut -d. -f 1`_$a.txt; 
					} &
				fi
				k=$(($k+1));
			done;
			wait;
		done;
		a=$(($a+1));
	done;
#done
