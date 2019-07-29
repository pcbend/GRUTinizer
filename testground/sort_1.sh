#!/bin/bash

# 186Hf runs

COMMAND="grutinizer -qH "

INPUTS="e15130.val e15130.cal zcuts_run136.cuts /home/gretinaadmin/Programs/GRUTinizer/lib/libMakee15130_v4.so -o/dev/null " 

#FILES="/global/data1y/gretina/e15130/Run0108/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0109/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0110/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0111/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0112/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0113/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0114/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0115/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0116/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0117/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0118/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0119/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0120/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0121/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0122/Global.dat "
#FILES=$FILES"/global/data1y/gretina/e15130/Run0123/Global.dat "

FILES=$FILES"/global/data1y/gretina/e15130/Run0144/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0145/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0146/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0147/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0148/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0149/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0150/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0151/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0152/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0153/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0154/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0155/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0156/Global.dat "
FILES=$FILES"/global/data1y/gretina/e15130/Run0157/Global.dat "



#parallel -j2 $COMMAND ::: $INPUTS ::: $FILES

for i in $FILES
do
  grutinizer -qH $i /home/gretinaadmin/Programs/GRUTinizer/lib/libMakee15130_v3.so e15130.val e15130.cal zcuts_run136.cuts -o /dev/null
done
