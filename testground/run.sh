#!/bin/bash

for i in $(seq 108 126) 
do
  echo " grutinizer -qH ./data/Run0$i/Global.dat ./lib/libMakee15130_v6.so e15130.val e15130.cal zcuts_run108.cuts zcuts_noReactions.cuts zcuts_ChargeStates.cuts -o /dev/null "

done
