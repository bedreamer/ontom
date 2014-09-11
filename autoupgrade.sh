#!/bin/sh
# just usded on a develop board that has ONLY ONE usb interface.
# the device should be named /dev/sda1, /dev/sda2, ...

check_dog=5
mount_dir=""
updatefile=on

# used for check a usb device.
while [ true ]; do
	devs=`ls /dev/sda[1-9] 2>/dev/null`
	if [ "$?" -ne 0 ]; then
		sleep $check_dog;
		continue
	fi

	echo "detect a usb device pluged in."

	# for each device
	for dev in $devs; do

		dir=`cat /proc/mounts | grep "$dev" 2>/dev/null | awk '{print $2}' 2>/dev/null`
		echo "dir is $dir"
		if [ 0 = ${#dir} ]; then
			echo "$dev not mounted."
			continue;
		fi
		echo "$dev is mounted at $dir"
		mount_dir=$dir
		return 0;

	done

	# block until this usb device pluged out.
	# we support /dev/sda device only
	while [ -e "/dev/sda" ];do
		sleep 1
	done
	#echo "$this_usb pluged out."
done
