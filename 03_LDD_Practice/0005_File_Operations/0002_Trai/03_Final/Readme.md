
# On Board
==============================

[root@BeagleBone ~]# mknod /dev/Infinite_first_driver c 240 0
[root@BeagleBone ~]# ls -l /dev/Infinite_first_driver 
crw-r--r--    1 root     root      240,   0 Jan  1 00:16 /dev/Infinite_first_driver
[root@BeagleBone ~]# cat /dev/Infinite_first_driver
cat: can't open '/dev/Infinite_first_driver': No such device or address
[root@BeagleBone ~]# mknod /dev/Infinite_first_ABC c 242 0
[root@BeagleBone ~]# cat /dev/Infinite_first_ABC
cat: can't open '/dev/Infinite_first_ABC': No such device
[root@BeagleBone ~]# 


# dmesg


[    4.553131] IPv6: ADDRCONF(NETDEV_UP): usb0: link is not ready                  
[    4.559260] IPv6: ADDRCONF(NETDEV_CHANGE): usb0: link becomes ready             
[   62.939358] first_char_driver: loading out-of-tree module taints kernel.        
[   62.946727] Hello: ofcd registered                                              
[   62.946738] <Major, Minor>: <240, 0>  




