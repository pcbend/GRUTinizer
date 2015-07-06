#!/bin/bash
#Source me!!!

# this should be sourced, not executed, to work properly
if [[ "$_" == $0 ]]; then
    echo "Try sourcing me!";
    exit 0;
fi

SOURCEDFILE=`readlink -f $BASH_SOURCE`

export GRUTSYS=`dirname $SOURCEDFILE`
echo "GRUTSYS is now $GRUTSYS"

if [ ! -d $ROOTSYS ]; then
    echo "ROOTSYS=$ROOTSYS does not point to a directory. Please rectify before installing GRSISort."
    return 0;
fi

export PATH=$GRUTSYS/bin:$PATH
echo "Added $GRUTSYS/bin to PATH"

export LD_LIBRARY_PATH=$GRUTSYS/libraries:$LD_LIBRARY_PATH
echo "Added $GRUTSYS/libraries to PATH."

echo
echo
echo "To avoid running this script every session add the following to your ~/.bashrc file"
echo "export GRUTSYS=$GRUTSYS"
echo "export PATH=\$GRUTSYS/bin:\$PATH" 
echo "export LD_LIBRARY_PATH=\$GRSISYS/libraries:\$LD_LIBRARY_PATH"


# clean up variables
unset SOURCEDFILE
