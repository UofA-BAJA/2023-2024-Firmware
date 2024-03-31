# Required Software and Downloads

## Software Programs
To Convert VSC into Arduino IDE
1. Follow the tutorial on [this website](https://www.circuitstate.com/tutorials/how-to-use-vs-code-for-creating-and-uploading-arduino-sketches/)


To Get Setup for CAD
1. Make an account with autodesk with your school email
2. Install [Fusion360](https://www.autodesk.com/products/fusion-360/overview?term=1-YEAR&tab=subscription)

### GitHub Desktop

Link to [download](https://docs.github.com/en/enterprise-cloud@latest/desktop/installing-and-authenticating-to-github-desktop/installing-github-desktop)

Sign-in with your GitHub account, and clone the current repository.

### Driver for LoRa
You may run into an issue with not begin able to connect to the LoRa within Visual Studios Code. In that event:

1. Open up your device manager
2. Navigate to the device "CP2102 USB to UART Bridge"
3. Right click on the device, and click on **Properties**
4. Open up the driver window within this device, if there is no compatible driver installed in your system continue to [Drivers Download](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
5. If on Windows, install the Universal Windows Driver file. When completed extract the ZIP file
6. With the ZIP file extracted, return to the drivers menu of the "CP2102 USB to UART Bridge" device
7. Click on: Install Drivers > Browse my computer for drivers, and select the folder with your ZIP file > Next


## Visual Studios Code

### Extensions
platformio  
teleplot

### pip Installs
In Baja, there are a couple of python packages that need to be installed so that you can work on the various programs.
1. Open VS Code (or gany other command terminal), and check for the packages pyserial and gitpython using the "pip list" command.

2. If either of these packages are not installed, install them using the following:

    > pip install pyserial

    > pip install gitpython

    and by far the coolest pip install

    > pip install UofA-BAJA-2023-2024

3. Once you have done this, use the "pip list" command to check for these files again. They should appear in the displayed list along with their current versions.

*optional: if you would like to run the GUI, you must install the package "pyqt6"

### Useful commands

## [PlatformIO lib](https://registry.platformio.org/libraries/b-i-x/UofA-BAJA-2023-2024)
to update the pio libray run

    > pio pkg publish

to update the library in a project run

    > pio pkg update

## [PyPi lib](https://pypi.org/project/UofA-BAJA-2023-2024/)
to build pypi project windows

    > py -m build

to update pypi project

    > twine upload dist/* -u __token__ -p "api-key-u-ask-alex-for"

[the](https://packaging.python.org/en/latest/tutorials/packaging-projects/) link to the guide i followed for pypi stuff

### Example packets

Here are some example packets 

This is one meant for the rasberry pi, the test is the mesg, everything is headers, except for the rasbpi, thats the device

yes !fart! is start

and !bend! is end

    > <!fart!-nxtdev:rasbpi-mesg:test!bend!>


    