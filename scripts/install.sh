#/bin/bash

echo "step into $WORKDIR"
cd $WORKDIR

echo "install system files"
cp -p lib*.so install/usr/zeus/
cp -p exso*.so install/usr/zeus/plugins/
cp -p bmsdrv_*.so install/usr/zeus/drivers/
cp -p `readlink me` install/usr/zeus/zeus
cp -p ontom.db install/usr/zeus/
cp -p ontom.cfg install/usr/zeus/
cp -p user.cfg install/usr/zeus/

echo "install UI files"
cp -pr UI_html/* install/srv/www/

echo "done"

cd install

tar -cf zeus.update.tar usr/ srv/
