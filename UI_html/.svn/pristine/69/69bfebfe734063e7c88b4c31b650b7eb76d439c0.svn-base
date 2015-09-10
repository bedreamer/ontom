#!/bin/sh
FILELIST=file.list
HTMLLIST=html.list
JSLIST=js.list
CSSLIST=css.list
IMGSLIST=imgs.list
SRVIP=''

all='yes'
html='no'
js='no'
css='no'
imgs='no'

if [ $# -lt 1 ]; then
        echo "need tftp host SRVIP";
        echo "usage: update.sh \$SRVIP"
        exit
fi

SRVIP=192.168.1.8

while getopts ahjcis: param; do
	case $param in
	a)
		all='yes'
		html='no'
		js='no'
		css='no'
		imgs='no'
		break;
	;;
	h)
		all='no'
		html='yes'
	;;
	j)
		all='no'
		js='yes'
	;;
	c)
		all='no'
		css='yes'
	;;
	i)
		all='no'
		imgs='yes'
	;;
	s)
		SRVIP=$OPTARG
	;;
	esac
done
shift $(( OPTIND - 1 ));

if [ 'yes' == $all ];then
	printf "fetching $FILELIST...."
	tftp -g -r $FILELIST -l /tmp/$FILELIST $SRVIP >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "          [ DONE]"
	else
		echo "          [FAILE]"
	fi

	f=''
	for f in `cat /tmp/$FILELIST`; do
		printf "get $f from $SRVIP"
		tftp -g -r $f -l $f $SRVIP >/dev/null 2>&1
		if [ $? -eq 0 ]; then
				echo "          [ DONE]"
		else
				echo "          [FAILE]"
		fi
	done
	exit 0
fi

if [ 'yes' == $html ];then
	printf "fetching $HTMLLIST...."
	tftp -g -r $HTMLLIST -l /tmp/$FILELIST $SRVIP >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "          [ DONE]"
	else
		echo "          [FAILE]"
	fi

	f=''
	for f in `cat /tmp/$FILELIST`; do
		printf "get $f from $SRVIP"
		tftp -g -r $f -l $f $SRVIP >/dev/null 2>&1
		if [ $? -eq 0 ]; then
				echo "          [ DONE]"
		else
				echo "          [FAILE]"
		fi
	done
fi

if [ 'yes' == $js ];then
	printf "fetching $JSLIST...."
	tftp -g -r $JSLIST -l /tmp/$FILELIST $SRVIP >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "          [ DONE]"
	else
		echo "          [FAILE]"
	fi

	f=''
	for f in `cat /tmp/$FILELIST`; do
		printf "get $f from $SRVIP"
		tftp -g -r $f -l $f $SRVIP >/dev/null 2>&1
		if [ $? -eq 0 ]; then
				echo "          [ DONE]"
		else
				echo "          [FAILE]"
		fi
	done
fi

if [ 'yes' == $css ];then
	printf "fetching $CSSLIST...."
	tftp -g -r $CSSLIST -l /tmp/$FILELIST $SRVIP >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "          [ DONE]"
	else
		echo "          [FAILE]"
	fi

	f=''
	for f in `cat /tmp/$FILELIST`; do
		printf "get $f from $SRVIP"
		tftp -g -r $f -l $f $SRVIP >/dev/null 2>&1
		if [ $? -eq 0 ]; then
				echo "          [ DONE]"
		else
				echo "          [FAILE]"
		fi
	done
fi

if [ 'yes' == $imgs ];then
	printf "fetching $IMGSLIST...."
	tftp -g -r $IMGSLIST -l /tmp/$FILELIST $SRVIP >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "          [ DONE]"
	else
		echo "          [FAILE]"
	fi

	f=''
	for f in `cat /tmp/$FILELIST`; do
		echo "get $f from $SRVIP"
		tftp -g -r $f -l $f $SRVIP >/dev/null 2>&1
		if [ $? -eq 0 ]; then
				echo "          [ DONE]"
		else
				echo "          [FAILE]"
		fi
	done
fi
