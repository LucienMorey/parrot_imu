# parrot_imu

## Purpose

This design will host an IMU board for the Parody tripedal robotic platform. It will expose connect an IMU to the teensy 4.1 microcontroller with the ability to deliver this data over CAN bus and over an ethernet connection.

## TODOs

### Power Subsystem

Take a 24 V input and reduce it to supplies for powering the associated subsystems.

- [ ] 5v supply
- [ ] 3.3v supply
- [ ] debugging LEDS to indicate presense of power
  - [ ] at input line
  - [ ] after 5v supply
  - [ ] after 3.3V supply

- what is an LDO and will it suffice or is a proper dc/dc converter required

### Can Transeiver

One potential mechanism for delivering IMU data to the robot comms network. This is the back up option because it cannot interface directly to the ros network. It does not require FD functionality for higher bit rates but this option will be made available if a component can be sourced.

- [ ] Pick a can trasniever that can interface with the teensy
- [ ] add optional termination points


### Teensy 4.1

Primary microcontroller for interfacing with the IMU.

- [ ] obtain premade footprint
- [ ] remember different pin styles for ethernet and general gpio
- [ ] spi connection avoiding pin 13 so that ethernet can be used

### IMU

- [ ] Place footprint of breakout board

