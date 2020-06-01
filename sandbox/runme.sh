#!/bin/bash

currentrundir="/global/data1y/gretina/e15130"
expnumber="e15130"
valfile="~/Programs/GRUTinizer/${expnumber}.val"
calfile="~/Programs/GRUTinizer/${expnumber}.cal"
histlib="~/packages/GRUTinizer6/lib/libMake${expnumber}.so"


runnumber=`echo $currentrundir | sed -e s/Run//g`

ls $currentrundir
