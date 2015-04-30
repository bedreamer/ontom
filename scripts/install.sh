#/bin/bash

echo "step into $WORKDIR"
cd $WORKDIR

echo "install system files"
mkdir -p install/usr/zeus
mkdir -p install/usr/zeus/plugins
mkdir -p install/usr/zeus/drivers
mkdir -p install/srv/www

cp lib*.so install/usr/zeus/
cp exso*.so install/usr/zeus/plugins/
cp bmsdrv_*.so install/usr/zeus/drivers/
cp `readlink me` install/usr/zeus/zeus
cp ontom.db install/usr/zeus/
cp ontom.cfg install/usr/zeus/
cp user.cfg install/usr/zeus/

echo "install UI files"
rsync -r UI_html/* install/srv/www/

echo "done"

cd install

tar --exclude-vcs -cf zeus.update.tar usr/ srv/
