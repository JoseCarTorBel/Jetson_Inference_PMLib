#!/bin/bash

#for network in "alexnet" "googlenet" "googlenet-12" "resnet-18" "resnet-50" "resnet-101" "resnet-152" "vgg-16" "vgg-19" "inception-v4";
#for network in "alexnet" "resnet-50";
#for network in "vgg-19" "inception-v4" ;
for network in "inception-v4" "alexnet" "googlenet-12" "vgg-19";
do	
	for mode in 0 1 2 3 4;
	do
		nvpmodel -m $mode
		#for batch in "1" "4" "16";
		for batch in "1" "4" "16" "32" "64" "128"; 
		do
			#for((i=0;i<5;i++));
			for((i=0;i<1;i++));
			do
				./imagenet-console model=$network batch_size=$batch &> time.out 
				time=$(cat time.out | grep Accumulated | sed 's/ /#/g' | cut -f7 -d# | awk '{ total+=$1; count++ } END { print total/count }') 
				#energy=$(cat time.out | grep pmlib | cut -f2- -d:) 
				energy=$(cat time.out | grep pmlib | cut -f2- -d: | awk -F: '{for (i=1; i<=NF; i++){ printf $i/20; if(i!=NF){printf ":"} else printf "\n" } }') 
				echo $network:$mode:$precision:$batch:$i:$time:$energy >> resultados_energia_20reps_all.txt
				rm time.out
			done
		done
	done
        find networks/ -name "*cache*" -exec rm -f {} \; 
done
