# Windows Setup
The Microsoft Windows set up is similar to that of the Linux set up albeit a
little more hands free/off. You need to download and install the following:
- OpenOCD
- GNU Arm Toolchain
- ST-Link Utility & Driver
- Eclipse IDE

## Install OpenOCD
- Download the OpenOCD 0.9.0+
  [here](http://sysprogs.com/files/gnutoolchains/arm-eabi/openocd/OpenOCD-20160901.zip).
- What is contained in the zip file is **NOT** an installation file, rather an
  executable that we must add to you PATH environment variable. My suggestion is
  to make a directory where you won't easily forget such as C:\OpenOCD. Copy the
  .zip file to that directory, right-click the archive and extract it. 
- Once extracted right-click your start menu button go to **System > Advanced
  system settings**. In the pop-up window click **Environment Variables... **

- In the bottom window we are looking for the Variable called **PATH**. Locate
  it, click it and then click **Edit...** The idea here is we want to add the
  path the to OpenOCD binary so that we can open a command prompt and type
  `openocd` vs `C:\<really_long_windows_path>\openocd.exe`. 
- Click **New**. In the empty field type the path to where you just extracted
  your OpenOCD archive \bin directory.  For example, if you extracted it to
  `C:\OpenOCD` type `C:\OpenOCD\bin` in the field.  
- Click **Ok**, open a **new**
  powershell/command prompt and type

```
openocd --version
Open On-Chip Debugger 0.9.0 (2016-09-01) [https://github.com/sysprogs/openocd]
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
```

## GNU ARM Toolchain
- Download the toolchain
  [here](https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q3-update/+download/gcc-arm-none-eabi-5_4-2016q3-20160926-win32.exe)
- Accept the defaults for the installation wizard and you're good to go!

## ST-Link Utility & Driver
- This part is a pain becuase you're unfortunately at the mercy of ST Microelectronics.
- Go [here](http://www.st.com/en/development-tools/st-link-v2.html)
- Scroll to the section labelled **Tools and Software**
- CLick on **STSW-LINK004**. 
- Scroll to the bottom of the newly opened page and click **Get Software** under
  the **Get Software** section. 
- Accept the license Agreement and fill out your info to download the software.
  This sucks but needs to be done.
- Once you enter your e-mail you will be mailed a link with a link to click on.
  This link will start the download for the **ST-Link Utility**. 
- Once that is downloaded repeat the same process for the **STSW-LINK009**.
- Once you have donwloaded both the ST-Link Utility and USB driver it's pretty
  easy to install since both have installation wizards. Accept the defaults for
  both installations and you should be good to go!

## Install Eclipse
- Download from
  [here](http://eclipse.mirror.rafal.ca/oomph/epp/neon/R2a/eclipse-inst-win64.exe)
- Follow install wizard steps to install Eclipse.
- To setup Eclipse for ARM development follow the [Eclipse
  Setup](docs/eclipse_setup.md). **WARNING** There is one MAJOR difference
  between Linux vs Windows Eclipse environment. The step when you go to set up
  the **External Tool** is not well understood yet as I don't know which command
  is issued to flash our board. Hopefully this will be fixed in the future but
  for now you may flash your board with the ST-Link Utility. 
