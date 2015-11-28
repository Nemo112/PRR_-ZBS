#!/bin/bash
function fixsort(){
	[[ ! -f $1.dat ]] && return 1;
	tm=`mktemp`;
	cat $1.dat | sort -n > $tm;
	cat $tm > $1.dat;
	rm $tm;
}
function mergj(){
	tm=`mktemp`;
	cat $1 > $tm;
	shift;
	while [[ $1 ]];do
		mt=`mktemp`;
		join $tm $1 > $mt;
		rm $tm;
		tm=$mt;
		shift;
	done
	mv $tm final.dat;
}
lsd=(`echo a*n*logs`);
#lss=(`echo ${lsd[@]} | sed 's/ /\n/g' | sort -n`);
for i in ${lsd[@]};do
	a=`echo $i | grep -o '^a[0-9]\+' | grep -o '[0-9]\+'`;
	n=`echo $i | grep -o 'n[0-9]\+' | grep -o '[0-9]\+'`;
	echo $a $n;
	if [[ $a == 3 ]]; then
		echo $n `cat $i/parallel_job*.sh.o* | grep '[0-9]\+\.[0-9]\+'` >> $a.dat;
	fi
	if [[ $a == 4 ]]; then
		echo $n `cat $i/parallel_job*.sh.o* | grep '[0-9]\+\.[0-9]\+'` >> $a.dat;
	fi
	if [[ $a == 5 ]]; then
		echo $n `cat $i/parallel_job*.sh.o* | grep '[0-9]\+\.[0-9]\+'` >> $a.dat;
	fi
done
fixsort 3;
fixsort 4;
fixsort 5;
mergj 3.dat 4.dat 5.dat;
rm 3.dat 4.dat 5.dat;
#join 3.dat 4.dat;
