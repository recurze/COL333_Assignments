#!/bin/bash
# File              : checkall.sh
# Author            : recurze
# Date              : 15:53 07.10.2018
# Last Modified Date: 16:25 07.10.2018

fcjar=$1

for tc in testcases/*
do
    java -jar $fcjar $tc output/output${tc: -5} | grep score | grep -Eo '[0-9]+([.][0-9]+)?'>> result.txt
done;
