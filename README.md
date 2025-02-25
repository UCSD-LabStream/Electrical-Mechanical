# Electrical-Mechanical
A key aspect of LabStream is the automation of lab components to enable precise remote control.

## Mechanical Components
<ul>
  <li>VA100/M (Slit Mount)</li>
  <img href="images/VA100" alt="VA100">
  <li>XYF1/M (Pattern Mount)</li>
  <img href="images/XYF1" alt="XYF1">
</ul>

## Steps of using MicroPython in Thonny IDE
Assuming Python (any version) is installed and set up already.
### 1. Download esptool
`pip install esptool`
`pip3 install esptool`
### 2. Erase ESP32
`esptool.exe --chip esp32 --port <port_name> erase_flash`
### 3. Download MicroPython firmware
`esptool.exe --chip esp32 --port <port_name> write_flash -z 0x1000 “address/path of .bin firmware file”`
