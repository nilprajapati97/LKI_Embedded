# LDD_BBB
*******************************************




If you are using the Beaglebone boar.You can cross-compile the Linux device driver using the below command.
	
		-->> sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

Then copy the driver to the Beaglebone using the below command.

		-->> sudo rsync -avz *.ko root@192.168.7.2: 
		-->> sudo scp  *.ko root@192.168.7.2:
		
		
