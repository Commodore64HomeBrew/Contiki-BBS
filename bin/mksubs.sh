#!/bin/bash


DIR="subs-test"
mkdir $DIR
SUBS=8
for ((s=1;s<=SUBS;s++)); do
	mkdir $DIR/$s
	for ((i=0;i<=9;i++)); do
		mkdir $DIR/$s/$i
		for ((j=0;j<=9;j++)); do
			mkdir $DIR/$s/$i/$j
		done
	done
done


for ((s=1;s<=SUBS;s++)); do
	for ((i=0;i<=9;i++)); do
		for ((j=0;j<=9;j++)); do
			for ((k=0;k<=9;j++)); do
				cp "$s-$i$j$k.prg" $DIR/$s/$i/.
			done
		done
	done
done