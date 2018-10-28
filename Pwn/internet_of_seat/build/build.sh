make || exit
SRC=$PWD/main
TARGET=$PWD/../files/
cp main ../main
cd ~/core
cp $SRC ./sbin/httpd
chmod 755 ./sbin/httpd
chown root:root ./sbin/httpd

find . -print0 | cpio --null -ov --format=newc | gzip -9 > $TARGET/initramfs.cpio.gz
