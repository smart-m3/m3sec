#!/bin/bash
ps -eaf | grep "afsfuse_client /mnt/afs" | awk '{ print $2 }' | xargs kill
ps -eaf | grep "afsfuse_server" | awk '{ print $2 }' | xargs kill

make
./afsfuse_server &
umount /mnt/afs
./afsfuse_client /mnt/afs -d &
