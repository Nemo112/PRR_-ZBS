#!/bin/bash
[[ ! -r "$1" ]] && { exit 1; };
fdls=`head -n 1 "$1" | awk '{print NF}'`;
out="\begin{tabular}{";
i=1;
out+=" l |"
while [[ $i -lt $fdls-1 ]];do
	out+=" c |"
	i=$(($i+1));
done
out+=" r }
"
i=1;
line="`sed -n "${i}p" < $1`";
while [[ "$line" != "" ]];do
	k=0;
	for l in $line;do
		if [[ $k -eq 0 ]];then
			out+="\textbf{"$l"} & ";
		elif [[ $k -eq $fdls-1 ]];then
			out+=$l" \\\\ \hline
";
		else
			out+=$l" & ";
		fi

		k=$(($k+1));
	done;
	i=$(($i+1));
	line="`sed -n "${i}p" < $1`";
done;

out+="\end{tabular}";
echo "$out";

