# raspberry2c

C-Libraries for I2C communication between an ATTiny2313 and a Raspberry PI.

![raspberry2c](https://raw.github.com/embedded-chaos/raspberry2c/master/assembly.jpg)

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
````

Write 0xff into I2C register 0x00 at address 0x1a

````bash
$ i2cset -y 0 0x1a 0x00 0xff
````
