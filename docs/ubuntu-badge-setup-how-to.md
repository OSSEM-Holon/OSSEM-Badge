# Prerequisites
For this How-To it is assumed that the user is using Ubuntu 16.04. The following packages need to be installed:
- git
- make
- cmake
- build-essential
- libusb-1.0
- libgtk-3-dev
```
sudo apt-get install git make cmake build-essential libusb-1.0 libgtk-3-dev
```

# Install ST-Link software
ST-Microelectronics do not provide a GNU/Linux installation on their website. Fortunately there exists an alternative on GitHub!! I'm not sure if this is affiliated with ST Microelectronics or not but it works wonders under Ubuntu.
```
git clone https://github.com/texane/stlink.git
cd stlink
make release
cd build/Release
sudo make install
sudo ldconfig
whereis st-flash
```
Verify that the last command shows the location of the st-flash bin! If it does not, consult the stlink README.md files.

# Install OpenOCD
This is simple enough because it is in the Ubuntu main repositories!
```
sudo apt-get install openocd
openocd --version
```
Verify that the version is 0.9.0

# Install the GNU ARM Toolchain
Go to [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded) and read the awesomeness that the GNU Project is doing for hardware development available on GNU/Linux.
## Option 1 (Preferred)
They officially have a PPA for their stuff! How convenient!
```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt-get update
sudo apt-get install gcc-arm-embedded
```
This should now give you the required compilers. If you do not like adding PPAs consider Option 2 else, move on to the next step!
## Option 2
```
wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
tar -xvf gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
cp -r gcc-arm-none-eabi-5_4-2016q3 ~/gcc-arm-toolchain
```
This tarball gives you the headers, libraries and binaries. For what we are doing we just need the binaries. You must alter you environment to look into ~/gcc-arm-toolchain. If you want to use this method consider adding the following to your ~/.bashrc
```
PATH = ~/gcc-arm-toolchain:$PATH
export PATH
```

# Install Eclipse Neon IDE
For the whole development process we will be using Eclipse Neon.
```
wget http://mirror.cc.columbia.edu/pub/software/eclipse/oomph/epp/neon/R2a/eclipse-inst-linux64.tar.gz
tar -xvf eclipse-inst-linux64.tar.gz
cd eclipse-installer/
./eclipse-inst

```
Go through the installation wizard. I installed eclipse to ~/eclipse. Yes, eclipse does exist in the Ubuntu repositories. I chose this route and this is what worked for me.

## Setup Eclipse Neon IDE
Start Eclipse by going to ~/eclipse and choose your workspace to be ~/workspace. Once Eclipse is loaded up go to Help > Eclipse. Welcome to the Eclipse marketplace! In the find search bar we are going to type GNU. We need to install GNU ARM Eclipse 3.1.1. This will install the necessary libraries in Eclipse to allow use to compile STM32 software within Eclipse. Once finished installing and restarting Eclipse go to File > New > C/C++ Project. Click C++ Managed Build and then Next. Give your project a name and then in the Project type column select STM32F10x C/C++ Project and click Next. Our flash size should be 64kB. Verify that it is. This was the only field that I had to change on mine. Verify that Content is Blinky (blink a led) and click next. Accept the defaults and click next on the next two windows. For the Cross GNU ARM Toolchain ensure that your Toolchain path is correct. If you installed via the package manager it should be /usr/bin. If you are unsure of this open a terminal and type
```
whereis arm-none-eabi-gcc
```
This will show you where all of the toolchain binaries exist. Once you have the path entered in, click Finish and we're ready to start programming/flashing our chip!

The first thing you should do is right-click the project and go to Properties. Click on C Build and then Settings. Don't panic, this may take a while to load. On the Tool Settings tab scroll down to where you see Cross ARM GNU Create Flash Image and click General. I have found that you **MUST** Change the output format from Intel HEX to Raw Binary. Click Apply and OK.

We are going to make an LED blink. Of course the typical STM32f10x ships with the blinkLED project or something similar on it. Here we are assuming it doesn't but even if it does, we will over write it. Go to Run > External Tools > External Tools Configuration. This is where we will configure the "st-slaher" (the thing that burns the binary to our device!). In the Location field type
```
/usr/local/bin/st-flash
```
In working directory type
```
${project_loc}/Release
```
In the Arguments box type
```
write ${project_name}.bin 0x8000000
```
Basically this is telling Eclipse to use this command when we click on this tool.
```
/usr/local/bin/st-flash /path/to/your/eclipse/project/Release/<name_of_your_project>.bin 0x8000000
```
Once that is set up you can now easily access that tool (think of it as your flash tool) by clicking the play button with the toolbox just below it (Don't do this now though. We haven't compiled anything!).

We are now going to edit the source code to get the correct pin for our LED. Open the BlinkLed.h and on the line
```c
#define BLINK_PIN_NUMBER                (12)
```
Change the 12 to a 13. Save the file. Make sure you are in Debug mode. Click the hammer drop-down and click Debug. Debug mode is now compiled. Click the drop down menu again and click Release. **You can only flash is release mode!!!!!!** Click the hammer and then Release mode will be built. Now, ensure your ST-Link flasher is hooked up to your computer and to your STM32F10x development board and click the play button with the toolbox. This will burn the .bin file to your device. If all was well your device should have it's status LED blinking. Congrats!

# Notes
- If Eclipse complains about not having any room (0kB) on the device try this:
```
sudo ~/tmp/stlink/st-info --probe
```
Should show 0kB. If so open two terminals. The first terminal type:
```
sudo /usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/local/share/openocd/scripts/target/stm32f1x.cfg
```
In the second terminal type:
```
telnet localhost 4444
>flash probe 0
>stm32f1x unlock 0
```
This happened with a brand new board from China. I guess it was a bad flash *shrug*
