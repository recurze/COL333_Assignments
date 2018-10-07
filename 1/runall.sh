#!/bin/bash
# File              : runall.sh
# Author            : recurze
# Date              : 15:40 07.10.2018
# Last Modified Date: 15:55 07.10.2018


./compile.sh

for tc in testcases/*
do
    ./run.sh $tc output/output${tc: -5}
done;
