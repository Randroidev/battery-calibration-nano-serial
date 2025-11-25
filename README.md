# battery-calibration-nano-serial
 
## Read Laptop Battery from Linux

sudo apt update && sudo apt install i2c-tools
i2cdetect -l
sudo i2cdetect -y -r 9
