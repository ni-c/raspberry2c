# raspberry2c

Software implementation for I2C communication between AVR microcontrollers and a Raspberry PI.

![raspberry2c](https://raw.github.com/ni-c/raspberry2c/master/assembly.jpg)

## Supported MCUs

The I2C software implementation has definitions for the following MCUs:

- ATTiny2313
- ATTiny24
- ATTiny24A
- ATTiny25
- ATTiny26
- ATTiny261
- ATTiny44
- ATTiny44A
- ATTiny45
- ATTiny461
- ATTiny84
- ATTiny861
- ATMega165
- ATMega169
- ATMega325
- ATMega3250
- ATMega329
- ATMega3290
- ATMega645
- ATMega6450

## Prepare the Raspberry PI for I2C communication

### Activate I2C kernel module

Edit the file `/etc/modprobe.d/raspi-blacklist.conf`

````bash
$ sudo vi /etc/modprobe.d/raspi-blacklist.conf
````

Search for the row

````
blacklist i2c-bcm2708
````

and comment it
````
#blacklist i2c-bcm2708
````

Leave the editor (with `:wq!`)

### Load I2C kernel modules

Load the required kernel modules

````bash
$ modprobe i2c-bcm2708
$ modprobe i2c_dev
````

Use `lsmod` to check, if the modules are loaded.

````bash
$ lsmod
Module                  Size  Used by
nfsd                  236425  11
i2c_dev                 5587  20
i2c_bcm2708             3542  0
snd_bcm2835            19889  0
snd_pcm                74834  1 snd_bcm2835
snd_seq                52536  0
snd_timer              19698  2 snd_seq,snd_pcm
snd_seq_device          6300  1 snd_seq
snd                    52489  5 snd_seq_device,snd_timer,snd_seq,snd_pcm,snd_bcm2835
snd_page_alloc          4951  1 snd_pcm
````

To load the modules automatically on boot, you can add them to the `/etc/modules` file.

````bash
$ sudo vi /etc/modules

snd_bcm2835
i2c-bcm2708
i2c-dev
````

#### Install and use I2C tools

Install the i2c-tools

````bash
$ sudo apt-get install i2c-tools
````

Add your user to the i2c usergroup so you don't need to use sudo to interact with the i2c device.

````bash
$ sudo usermod -aG i2c pi
````

Scan I2C-bus 0 for devices

````bash
$ i2cdetect -y 0

    0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- 1a -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
````

Read I2C register from address 0x1a

````bash
$ i2cdump -y 0 0x1a

No size specified (using byte-data access)
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f    0123456789abcdef
00: ff c7 16 58 0e 0f 10 11 12 13 00 00 00 00 00 00    .??X??????......
10: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
20: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
30: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
40: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
50: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
60: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
70: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
80: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
90: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
a0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
b0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
c0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
d0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
e0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
f0: ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
````

Write 0xff into I2C register 0x00 at address 0x1a

````bash
$ i2cset -y 0 0x1a 0x00 0xff
````

## Schematics

![schematics](https://raw.github.com/ni-c/raspberry2c/master/schematics.png)

### Raspberry GPIO Pins

see http://elinux.org/RPi_Low-level_peripherals#General_Purpose_Input.2FOutput_.28GPIO.29
