# **Temperature driver using TMP100 sensor for educational purposes**

Used materials:

1. Original driver lm75.c
2. Essential Linux Device Drivers
3. TI Documentation for TMP100 sensor

How to test:

1. Clone beaglebone core
2. Edit dts file, add driver in Kbuild if needed, insert the module
3. Build the code and flash the board
4. Adjust your Bread board, temperature sensor according to the documentation (find pins for SDA and SCLK)
5. Read from fs -> check dmesg if results are valid or make a simple test user application

How to integrate driver:
1. Add chardriver.c in drivers/hwmon/.
2. Add the respective node in arch/arm/boot/dts/am335x-bone-common.dtsi.

tmp100: tmp100@49 {
               compatible = "ti,tmp100";
               reg = <0x49>;
};

3. Add to arch/arm/configs/bb.org_defconfig.

CONFIG_SENSORS_TMP100=y

4. Add to drivers/hwmon/Kconfig.

config SENSORS_TMP100
       tristate "Test tmp100 driver"
       depends on I2C
       select REGMAP_I2C
       help
         If you say yes here you get support for one common type of
         temperature sensor chip.

5. Add to drivers/hwmon/Makefile.

obj-$(CONFIG_SENSORS_TMP100)   += chardriver.o
