#/bin/bash

PACKFILE=zeus.update.tar

cd $WORKDIR

echo "install system files...."
mkdir -p install/usr/zeus
mkdir -p install/usr/zeus/plugins
mkdir -p install/usr/zeus/drivers
mkdir -p install/srv/www

cp lib*so install/usr/zeus/$f
cp exso*.so install/usr/zeus/plugins/
cp bmsdrv_*.so install/usr/zeus/drivers/
cp `readlink me` install/usr/zeus/zeus
cp ontom.db install/usr/zeus/
cp ontom.cfg install/usr/zeus/
cp user.cfg install/usr/zeus/

echo "installing UI files..."
rsync -r UI_html/* install/srv/www/

cd install

tar --exclude-vcs -cf $PACKFILE `ls`

printf "`date` \033[31minstall/zeus.update.tar\033[0m packed.\n"
