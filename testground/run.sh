#!/bin/bash

for i in $(seq 132 157) 
do
  echo " grutinizer -qH ./data/Run0$i/Global.dat ./lib/libMakee15130_v4.so e15130.val e15130.cal zcuts_run136.cuts -o /dev/null "

done
