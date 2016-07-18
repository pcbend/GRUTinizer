# Source this script to set up the grutSort build that this script is part of.
#
# Conveniently an alias like this can be defined in .bashrc:
#   alias thisgrut=". bin/thisgrut.sh"
#
# This script if for the bash like shells, see thisgrut.csh for csh like shells.
#
# Author: Fons Rademakers, 18/8/2006
# Adapted by Ryan Dunlop, 8/9/2015

drop_from_path()
{
   # Assert that we got enough arguments
   if test $# -ne 2 ; then
      echo "drop_from_path: needs 2 arguments"
      return 1
   fi

   p=$1
   drop=$2

   newpath=`echo $p | sed -e "s;:${drop}:;:;g" \
                          -e "s;:${drop};;g"   \
                          -e "s;${drop}:;;g"   \
                          -e "s;${drop};;g"`
}

if [ -n "${GRUTSYS}" ] ; then
   old_grutsys=${GRUTSYS}
fi

if [ "x${BASH_ARGV[0]}" = "x" ]; then
    if [ ! -f thisgrut.sh ]; then
        echo ERROR: must "cd where/grut/is" before calling ". thisgrut.sh" for this version of bash!
        GRUTSYS=; export GRUTSYS
        return 1
    fi
    GRUTSYS="$PWD"; export GRUTSYS
else
    # get param to "."
    thisgrut=$(dirname ${BASH_ARGV[0]})
    GRUTSYS=$(cd ${thisgrut};pwd); export GRUTSYS
fi

if [ -n "${old_grutsys}" ] ; then
   if [ -n "${PATH}" ]; then
      drop_from_path "$PATH" ${old_grutsys}/bin
      PATH=$newpath
   fi
   if [ -n "${LD_LIBRARY_PATH}" ]; then
      drop_from_path $LD_LIBRARY_PATH ${old_grutsys}/lib
      LD_LIBRARY_PATH=$newpath
   fi
   if [ -n "${DYLD_LIBRARY_PATH}" ]; then
      drop_from_path $DYLD_LIBRARY_PATH ${old_grutsys}/lib
      DYLD_LIBRARY_PATH=$newpath
   fi
   if [ -n "${MANPATH}" ]; then
      drop_from_path $MANPATH ${old_grutsys}/man
      MANPATH=$newpath
   fi
fi

if [ -z "${MANPATH}" ]; then
   # Grab the default man path before setting the path to avoid duplicates 
   if `which manpath > /dev/null 2>&1` ; then
      default_manpath=`manpath`
   else
      default_manpath=`man -w 2> /dev/null`
   fi
fi

if [ -z "${PATH}" ]; then
   PATH=$GRUTSYS/bin; export PATH
else
   PATH=$GRUTSYS/bin:$PATH; export PATH
fi

if [ -z "${LD_LIBRARY_PATH}" ]; then
   LD_LIBRARY_PATH=$GRUTSYS/lib; export LD_LIBRARY_PATH       # Linux, ELF HP-UX
else
   LD_LIBRARY_PATH=$GRUTSYS/lib:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
fi

if [ -z "${DYLD_LIBRARY_PATH}" ]; then
   DYLD_LIBRARY_PATH=$GRUTSYS/lib; export DYLD_LIBRARY_PATH   # Mac OS X
else
   DYLD_LIBRARY_PATH=$GRUTSYS/lib:$DYLD_LIBRARY_PATH; export DYLD_LIBRARY_PATH
fi

if [ -z "${MANPATH}" ]; then
   MANPATH=`dirname $GRUTSYS/man/man1`:${default_manpath}; export MANPATH
else
   MANPATH=`dirname $GRUTSYS/man/man1`:$MANPATH; export MANPATH
fi

#if [ "x`grut-config --arch | grep -v win32gcc | grep -i win32`" != "x" ]; then
#  GRUTSYS="`cygpath -w $GRUTSYS`"
#fi

#if [ "x`grut-config --arch | grep -v win64gcc | grep -i win64`" != "x" ]; then
#  GRUTSYS="`cygpath -w $GRUTSYS`"
#fi

unset old_grutsys
unset thisgrut

