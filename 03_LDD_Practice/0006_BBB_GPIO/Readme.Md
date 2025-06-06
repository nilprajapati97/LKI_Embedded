
#				0006 GPIO

================================================================================
===============================================================================


 
Beagle Bone Black Reference Manual
      Link -: https://cdn-shop.adafruit.com/datasheets/BBB_SRM.pdf
	
	
	

	LED  |	GPIO SIGNAL | PROC PIN

	USR0 |   GPIO1_21   |  V15
	USR1 |   GPIO2_22   |  U15
	USR2 |   GPIO2_23   |  T15 
	USR3 |   GPIO2_24   |  V16
	
# APIs
--------------------------------------------------------------------------------------------------
	                    To set
					gpio_set_valu(56, 0)   =>> LED0 should OFF
					gpio_set_valu(56, 1)   =>> LED0 should ON	

	
	                    To get
					gpio_get_valu(56, 0)   =>> LED0 should OFF
					gpio_get_valu(56, 1)   =>> LED0 should ON	

----------------------------------------------------------------------------------------------------


	
