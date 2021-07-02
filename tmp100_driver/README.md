# **Temperature driver using TMP100 sensor for educational purposes**

Used materials:
-Original driver lm75.c
-Essential Linux Device Drivers
-TI Documentation for TMP100 sensor

How to test:

1. Clone beaglebone core
2. Edit dts file, add driver in Kbuild if needed, insert the module
3. Build the code and flash the board
4. Adjust your Bread board, temperature sensor according to the documentation (find pins for SDA and SCLK)
5. Read from fs -> check dmesg if results are valid or make a simple test user application
