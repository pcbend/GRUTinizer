#!/bin/bash

GRANAFILES=("sca" "hb" "def" "alias" "datatest" "gr_wtdc.hst" "las_wtdc.hst" "hist.def")
for linkey in "${GRANAFILES[@]}"; do
	echo "ln -s GRAnalyzer/$linkey $linkey"
	ln -s "GRAnalyzer/$linkey" "$linkey"
done
source thisgrut.sh
