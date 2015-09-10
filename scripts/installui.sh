#!/bin/bash

printf "选择需要安装的界面类型:\n"
printf "ID|界面名\n"
sqlite3 scripts/install.db "select ui_id,ui_name from UI"
read -p "输入需要选择的界面代码:" ID

BRANCHES=`sqlite3 scripts/install.db "SELECT ui_branch FROM UI WHERE ui_id=$ID"`
VERSION=`sqlite3 scripts/install.db "SELECT ui_VERSION FROM UI WHERE ui_id=$ID"`

mkdir install/www
svn co svn://192.168.1.8/$BRANCHES -r $VERSION install/srv/www
