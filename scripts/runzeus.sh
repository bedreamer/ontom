#!/bin/sh

SCRIPTSIZE=`awk '/./{s = s + length($0);} /$/ {s = s + 1;}  /ONPW$/ {print s;exit 0;}' $0`
TOTAL=`ls -l $0 |awk '{print $5}'`
ELFSIZE=`expr $TOTAL - $SCRIPTSIZE`

dd if=$0 of=/tmp/zeus skip=$SCRIPTSIZE bs=1 count=$ELFSIZE
exit 0
##ONPW
