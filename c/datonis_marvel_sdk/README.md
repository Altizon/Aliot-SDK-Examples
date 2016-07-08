# Marvell's Starter SDK for Datonis IoT Service

* [Setup your development environment](https://github.com/marvell-iot/aws_starter_sdk/wiki/Development%20Host%20Setup)
* Create a Thing template and a Thing after [signing up on Datonis](https://www.datonis.io)
* Download your default key pair (Access and Secret key) and mention that in sample_apps/datonis_demo/src/main.c
* Also update the Thing Key of the thing you created on Datonis
* make
* Open a terminal and connect to the Serial Console at /dev/ttyUSB1
* sudo ./sdk/tools/OpenOCD/ramload.py ./bin/mw302_rd/datonis_demo.axf (To load in SRAM)
