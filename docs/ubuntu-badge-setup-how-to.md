# Prerequisites
For this How-To it is assumed that the user is using Ubuntu 16.04. The following
packages need to be installed:
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
ST-Microelectronics do not provide a GNU/Linux installation on their website.
Fortunately there exists an alternative on GitHub!! I'm not sure if this is
affiliated with ST Microelectronics or not but it works wonders under Ubuntu.
```
git clone https://github.com/texane/stlink.git
cd stlink
make release
cd build/Release
sudo make install
sudo ldconfig
whereis st-flash
```
Verify that the last command shows the location of the st-flash bin! If it does
not, consult the stlink README.md files.

# Install OpenOCD
This is simple enough because it is in the Ubuntu main repositories!
```
sudo apt-get install openocd
openocd --version
```
Verify that the version is 0.9.0

# Install the GNU ARM Toolchain
Go to [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded) and
read the awesomeness that the GNU Project is doing for hardware development
available on GNU/Linux.
## Option 1 (Preferred)
They officially have a PPA for their stuff! How convenient!
```
sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
sudo apt-get update
sudo apt-get install gcc-arm-embedded
```
This should now give you the required compilers. If you do not like adding PPAs
consider Option 2 else, move on to the next step!
## Option 2
```
wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
tar -xvf gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2
cp -r gcc-arm-none-eabi-5_4-2016q3 ~/gcc-arm-toolchain
```
This tarball gives you the headers, libraries and binaries. For what we are
doing we just need the binaries. You must alter you environment to look into
~/gcc-arm-toolchain. If you want to use this method consider adding the
following to your ~/.bashrc
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
Go through the installation wizard. I installed eclipse to ~/eclipse. Yes,
eclipse does exist in the Ubuntu repositories. I chose this route and this is
what worked for me.
## Setup Eclipse...
If you need assistance please use our [Eclipse Setup
Guide](eclipse_setup.md)

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
