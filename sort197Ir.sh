#!/bin/bash

COMMAND="grutinizer -qH "

INPUTS="e15130.val e15130.cal blobs.cuts -o/dev/null" 

FILES="/global/data1y/gretina/e15130/Run0096/Global.dat /global/data1y/gretina/e15130/Run0097/Global.dat "



parallel -j4 $COMMAND ::: $INPUTS ::: $FILES



