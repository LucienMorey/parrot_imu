# parrot_imu

## Purpose

This design will host an IMU board for the Parody tripedal robotic platform. It will expose connect an IMU to the teensy 4.1 microcontroller with the ability to deliver this data over CAN bus and over an ethernet connection.

## TODOs

### Power Subsystem

Take a 24 V input and reduce it to supplies for powering the associated subsystems.

- [x] 5v supply - tentatitvely [VR10S05](https://www.digikey.com.au/en/products/detail/xp-power/VR10S05/13147713)
- [x] 3.3v supply - tentatitvely [TR05S3V3](https://www.digikey.com.au/en/products/detail/xp-power/TR05S3V3/4488636)
- [x] add terminal to supply power input. probably one of thosegreen terminals that can fit a ferule or a wago terminal [panel mount xt30](https://www.ebay.com.au/itm/393171351901)
- [x] Is it worth bothering with reverse polarity protection? Just a shottkey diode? Nope. just added a gendered connector to prevent this.
- [x] debugging LEDS to indicate presense of power
  - [x] at input line
  - [x] after 5v supply
  - [x] after 3.3V supply
- [ ] calculate resitors for power leds

Q. what is an LDO and will it suffice or is a proper dc/dc converter required?

A. They both do the same thing with regard to switching direct current voltages. DC/DC converters are more efficent but more suspetable to noise because they use fets and things that generate switching noise. LDOS are less efficient and will have some current drop depending on the load drawn from the supply. An LDO will work in this scenario and they are cheaper so I will probably go for that. Tony recommended dcdc converter so switching

### Can Transeiver

One potential mechanism for delivering IMU data to the robot comms network. This is the back up option because it cannot interface directly to the ros network. It does not require FD functionality for higher bit rates but this option will be made available if a component can be sourced.

- [x] Pick a can trasniever that can interface with the teensy - picked the [TJA1441AT/0Z](https://www.digikey.com.au/en/products/detail/nxp-usa-inc/TJA1441AT-0Z/13691175). This shouldnt be an issue because it provides both 5v VCC and a seperate VIO input that I can specify the 3.3v logic level required for the teensy
- [x] create footprint and symbol
- [x] add optional termination points
- [x] Add a proper screw terminal or similar mechanism to make adding to the can bus easy [scrw terminal here](https://www.digikey.com.au/en/products/detail/cui-devices/TB005-762-02BE/10064113)
- [ ] Add proper Silk screen markers to indicate CAN-H/L so things are easy

### Ethernet Jack

Intended to be the primary mechanism for delivering imu data to the robot comms network. This is known to beable to interface directly with the ros network through microros on udp transports.

- [x] Find an approppriate RJ45 magjack connector - Currently the [SI-60062-F](https://www.digikey.com.au/en/products/detail/bel-fuse-inc/SI-60062-F/1033372)
- [x] Wire the connector to the teensy. note to follow the schematic outlined on thier magjack page found [here](https://www.pjrc.com/store/ethernet_kit.html). This should be as simple as just adding a capacitor and joining the pins but double check the smc design to be sure.

### Teensy 4.1

Primary microcontroller for interfacing with the IMU.

- [x] obtain premade footprint
- [ ] remember different pin styles for ethernet and general gpio
- [x] spi connection avoiding pin 13 so that ethernet can be used

### IMU

- [x] create symbol
- [x] create footprint
- [x] connect to SPI2 or 3 to avoid collision with ethernet
- [ ] Place footprint of breakout board