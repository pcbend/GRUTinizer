#!/bin/bash

for i in $(seq 177 188) 
do
  echo " grutinizer -qH ./data/Run0$i/Global.dat ./lib/libMakee15130_v4.so e15130.val e15130.cal zcuts_run163.cuts -o /dev/null "

done
