#!/bin/sh

echo "APKFT ^o^"

# blue led on
echo 49 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio49/direction
echo 1 > /sys/class/gpio/gpio49/value

stage=`cat /etc/apkft/stage`
echo "stage = $stage"

if [ "$stage" == "iperf" ]; then
    echo "test iperf..."
    cd /etc/apkft

    sleep 1
    ifconfig wlan0 up
    sleep 1
    wpa_supplicant -D wext -i wlan0 -c  /etc/conf/wpa.conf -B > /dev/null 2>&1 &
    udhcpc -i wlan0 -p /var/run/udhcpc.pid &

    echo "turn on wifi..."
    /opt/network/wifi_cmd.sh connect I8Iperf "I8Iperf888" WPA2 &

    iperf3 -s &
    /etc/apkft/appApkft iperf &
fi

if [ "$stage" == "all" ]; then
    echo "test all"
    cd /etc/apkft

    sleep 1
    ifconfig wlan0 up
    sleep 1
    wpa_supplicant -D wext -i wlan0 -c  /etc/conf/wpa.conf -B > /dev/null 2>&1 &
    udhcpc -i wlan0 -p /var/run/udhcpc.pid &

    echo "turn on wifi..."
    /opt/network/wifi_cmd.sh connect I8Test "I8Test888" WPA2 &

    echo "turn on usb rndis..."
    cd /lib/modules/
    insmod libcomposite.ko
    insmod u_ether.ko
    insmod usb_f_ecm.ko
    insmod usb_f_ecm_subset.ko
    insmod usb_f_rndis.ko
    insmod g_ether.ko
    ifconfig usb0 192.168.1.111 up
    cd -

    /etc/apkft/carrier-server --st=jxh62 &
    
    /etc/apkft/appApkft all &
fi

if [ "$stage" == "aging" ]; then
    echo "test aging"
    cd /etc/apkft

    /etc/apkft/appApkft aging &
fi
