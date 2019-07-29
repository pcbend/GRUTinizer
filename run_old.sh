#!/bin/bash


for i in $(seq -f "%04g" 108 126 ) 
do
  echo " grutinizer -qH ./data/Run$i/Global.dat ./lib/libMakee15130_v5.so e15130.val e15130.cal zcuts_run110.cuts -o /dev/null "

done
