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
