# Magnetar BBS

Magnetar BBS is a high speed bulletin board software for the Commodore family of computers. It is designed to work with an RR-NET cartridge, SD2IEC drive and JiffyDOS for fastest performance. Multiple encodings are supported, including PETSCII and ASCII, as well as variable column widths. The BBS itself can be accessed through a standard telnet connection from any modern or retro computer. It aims for full compatibility.

Magnetar BBS is a fork of Contiki BBS v2.7 and has diverged greatly in both function and appearence.


Magnetar BBS 1.x quick startup guide
=====================================


Preamble:
---------
Magnetar BBS is a tiny, little and text based bulletin board system (BBS) for the Commodore 64 homecomputer and a compatible ethernet cartridge (e.g. TFE, RRnet, 64NIC+). It can be accessed through a standard telnet connection and currently provides basic BBS functionality (select from multiple boards, post/read messages, page sysop).

This guide is intended to be used as a quick startup guide. It will cover the compilation and installation of Magnetar BBS from source on a Linux system. This guide (c) 2015-2018 by Niels Haedecke and Kevin Casteels 

If you just want to run the software without compiling it (e.g. you have downloaded the .d64 disk image), skip to section 10.) of this document.


Prerequisites:
--------------
The following tools are required for a successful compile and installation of Magnetar BBS:

* GNU Make

* The cc65 (6502) C compiler

* Git revision control tool

* VICE Emulator installed (needed due to the included c1541 commandline program)

* A second machine (physical or VM) on the same network running a VICE Emulator with ethernet emulation enabled through libnet & libpcap (UNIX/Linux) or WinPCAP (Windows).

NOTE: On Linux you may have to use libnet0-1.0.2a from its original source code archives in order to get ethernet emulation working with VICE, since the libnet packages provided by recent Linux distros (e.g. libnet1) won't work (VICE will not be able to use the 'write_link_layer' function).

Alternatively, a Commodore 64 with a Commodore 1541/1570/1571/1581 disk drive and any compatible ethernet cartridge (e.g. TFE, RRnet, 64NIC+) will work as well. You just have to transfer the D64 image to a 5.25" / 3.5"   disk. Search the web for 'opencbm' for information on how to accomplish this.


How to build Magnetar BBS from scratch
------------------------------------
This guide assumes your are using Linux as your development platform. Shell commands needed to be entered will be displayed with the well known UNIX commandline prompt at the beginning. E.g. the following example will show the 'ls' command being issued:

  $ ls


1.) Create a folder where you are going to keep your Magnetar and Magnetar BBS sources.

  $ mkdir src

2.) Go to your new source folder

  $ cd src

3.) Get sources from Magnetar OS git repo:

  $ git clone https://github.com/magnetar-os/magnetar.git

4.) Get Magnetar BBS sources:

  $ git clone https://github.com/lodger-c64/Magnetar-BBS.git

5.) Go to the directory containing the Magnetar BBS sources

  $ cd magnetar-bbs/platform/c64/

6.) Build the binaries

  $ make

7.) Seperately build the Magnetar BBS setup program

  $ cl65 -t c64 -o bbs-setup bbs-setup.c bbs-setupfuncs.c bbs-file.c

Voila! You have now built your own Magnetar BBS binaries. Now, how do we get these on a D64 disk image?

8.) Still in the source directory, create a D64 disk image (make sure you have set your PATH to point to the c1541 commandline program)

  $ c1541 -format "magnetarbbs,27" magbbs.d64

9.) Write the neccessary files to your new D64 disk image. Depending on what ethernet hardware you are using, you may skip writing *.eth drivers to the image that you don't need in order to save disk space. TFE and RRnet devices require the cs8900a.eth driver to be present.

  $ c1541 ctkbbs.d64 -write magnetar-bbs.c64 magnetar-bbs 
  $ c1541 ctkbbs.d64 -write bbs-setup
  $ c1541 ctkbbs.d64 -write cs8900a.eth  
  $ c1541 ctkbbs.d64 -write lan91c96.eth
  $ c1541 ctkbbs.d64 -write w5100.eth
  $ c1541 ctkbbs.d64 -write login.txt
  $ c1541 ctkbbs.d64 -write logout.txt
  $ c1541 ctkbbs.d64 -write menu.txt

Congratulations, you're done! Now you have a working D64 disk image containing Magnetar BBS, ready to run in an emulator or to be written back to a real 1541 Disk in order to run it on a real Commodore 64. 

Okay, so this is how to build and prepare Magnetar BBS for use on a Commodore 64. Now let's go through the setup process of Magnetar BBS itself. So startup your VICE emulator and attach your ctkbbs.d64 disk image to drive 8. You are now at the Commodore 64 BASIC prompt.

10.) Load the Magnetar BBS setup program

     LOAD "BBS-SETUP",8,1

11.) Start it ...

     RUN
   

Magnetar BBS configuration
------------------------

The main menu will appear
   
     *** Magnetar BBS setup ***

     1...TCP/IP setup
     q...Quit

1...TCP/IP setup

This configuration step will configure your Commodore 64 ethernet hardware, including IP addresses, driver and memory location of the cartridge. The example below shows a typical LAN setup using a RRnet or TFE cartridge at memory address $de08. Both cartridges use the cs8900a.eth driver. ETH64 network devices may use the lan91c96.eth driver along with its appropriate memory address. 

NOTE: do not prefix your memory address with the '$' sign. E.g. if your ethernet cartridge resides at $de08 just enter 'de08' when being prompted for the memory address.

    * BBS network setup

    Host IP             : 192.168.200.64
    Netmask             : 255.255.255.0
    Gateway IP          : 192.168.200.1
    DNS IP              : 192.168.200.1
    Mem addr. ($de08)   : de08
    Driver (cs8900a.eth): cs8900a.eth
    Write to drive # (8): 8
    
    Network data correct (y/n)? y

q...Quit

You have now set up your Magnetar BBS system and are ready to run. Quit the BBS setup program from the main menu by entering 'q'.


Running Magnetar BBS
------------------
Once you have set up your Magnetar BBS, reset your machine or emulator and load the Magnetar BBS server from the Commodore 64 BASIC prompt.

1.) Load and run Magnetar BBS
    LOAD "MAGNETAR-BBS",8,1

2.) Start it...
    RUN

3.) Now use a second machine to connect to the IP address of your Commodore 64 (the Host IP address you entered in Step 3 of the BBS setup program) via telnet.

    $ telnet 192.168.200.64

+--------------------------------------+
i                                      i
i         w e l c o m e   t o :        i
i                                      i
i           >> magnetar bbs <<         i
i                                      i
+--------------------------------------+

login: _

You are now connected to your Magnetar BBS system and can log in using the username and password you entered in Step 4 of the BBS setup program. Well done, have fun!
