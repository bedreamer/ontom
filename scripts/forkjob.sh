#!/bin/sh

TSP=`date +%s`
CID="root"
GUN="0"
REAMIN="999.9"
PSWD="000000"
CMODE="auto"
BMODE="auto"

URL="job/create.json?t=$TSP&cid=$CID&gun=$GUN&remain=$REAMIN&passwd=$PSWD&c_mode=$CMODE&b_mode=$BMODE"

curl "http://127.0.0.1:8081/$URL"

