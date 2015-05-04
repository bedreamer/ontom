#/bin/bash

PACKFILE=zeus.update.tar
INSTALLDIRS="/usr/zeus /usr/zeus/plugins /usr/zeus/drivers /srv/www"
CFGS="ontom.db ontom.cfg user.cfg"


cd $WORKDIR

echo "create system directories...."
for d in $INSTALLDIRS;do
	mkdir -p install$d
	echo "  create ./install$d"	
done

echo "copy depends librarys...."
for f in `ls lib*.so`;do
	cp $f 'install/usr/zeus/'
	echo "  update ./install/usr/zeus/$f"
done

echo "copy extern librarys...."
for f in `ls exso*.so`;do
	cp $f 'install/usr/zeus/plugins/'
	echo "  update ./install/usr/zeus/plugins/$f"
done

echo "copy bms drivers....."
for f in `ls bmsdrv_*.so`;do
	cp $f 'install/usr/zeus/drivers/'
	echo "  update ./install/usr/zeus/drivers/$f"
done

echo "copy configuration database and files...."
for f in $CFGS;do
	cp $f 'install/usr/zeus/'
	echo "  update ./install/usr/zeus/$f"
done

echo "copy main program file `readlink me`"
cp `readlink me` 'install/usr/zeus/zeus'

echo "copy UI files..."
rsync -r UI_html/* 'install/srv/www/'

cd install

echo "CREATE INSTALL/UPDATE PACKET: $PACKFILE"
tar --exclude-vcs -cf $PACKFILE `ls`
cp zeus.update.tar /media/sf_E_DRIVE/www/
printf "`date` \033[31minstall/$PACKFILE\033[0m packed.\n"
