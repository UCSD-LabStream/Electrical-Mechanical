# LabStream - Electrical & Mechanial
A key aspect of _LabStream_ is the **automation of lab components** to enable precise remote control.

## Mechanical Components
### [VA100/M](https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=1465&pn=VA100/M) and [RSP1D/M](https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=246&pn=RSP1D/M) (Slit Mount)
<p align="center">
  <img src="images/VA100.jpg" alt="VA100" width="200"> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
  <img src="images/RSP1D.jpg" alt="RSP1D" width="200">
</p>

- Utilizes a micrometer to open/close the slit
- Mounted in front of [RSP1D/M](https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=246&pn=RSP1D/M)

### [XYF1/M](https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=5343&pn=XYF1/M) (Pattern Mount)
<p align="center">
  <img src="images/XYF1.jpg" alt="XYF1" width="200">
</p>

- Mounted with pattern filters made from micro-fabrication lab

# Integrated Systems
## Gear-Driven VA100/M with Elevation Support System
### Gear-driven Slit Mount
<p align="center">
  <img src="images/GDSM.png" alt="GDSM" width="200">
</p>

- A gear system that made rotation of the slit mount feasible without complexity
- The smaller gear is driven by a 5V DC stepper motor

### Elevation Support System (ESS)
<p align="center">
  <img src="images/ESS1.png" alt="ESS1" width="200"> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
  <img src="images/ESS2.png" alt="ESS2" width="200">
</p>

- **Physical limitation:** Micrometer elevates **up and down** when rotated, in order to “squeeze” the ball and thus closing the slit
- A sub-system implemented to handle **multi-directional motions**
- The system supports the movement of micrometer without needing the top motor to move, remaining **fixed in position**

## Dual-motion Pattern Mount
<p align="center">
  <img src="images/DM1.png" alt="DualMotion1" width="200"> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
  <img src="images/DM2.png" alt="DualMotion2" width="200">
</p>

- An automation system for dual-knob mechanism used in controlling movement of pattern filters
- Stepper motors were mounted directly onto the knobs and attached with screw heads
- Minimized the weight and mechanical complexity

## Substitute of Stationary Motor Mount
<p align="center">
  <img src="images/SMM1.png" alt="Stationary1" width="200"> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
  <img src="images/SMM2.png" alt="Stationary2" width="200">
</p>

- To provide more stability, a new design was created using a stable stand
- The mount fits the stand perfectly and thus minimizes the wiggling when operating

## Steps of using MicroPython in Thonny IDE
Assuming Python (any version) is installed and set up already.
### 1. Download esptool
`pip install esptool`
`pip3 install esptool`
### 2. Erase ESP32
`esptool.exe --chip esp32 --port <port_name> erase_flash`
### 3. Download/install MicroPython firmware
Download the firmware: https://micropython.org/download/

`esptool.exe --chip esp32 --port <port_name> write_flash -z 0x1000 “address/path of .bin firmware file”`
### Start coding!
