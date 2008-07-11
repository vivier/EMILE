# EMILE configuration file
#

PACKAGE       = emile
VERSION       = 0.12CVS

PREFIX=/

# kernel boot arguments

#FLOPPY=/dev/floppy/0
FLOPPY=/dev/sda
#CONSOLE=console=ttyS0,9600n8 console=tty0
#GESTALTID=43

NETBOOT_ARGS="root=/dev/nfs ip=dhcp rw $(CONSOLE)"
RESCUE_ARGS="root=/dev/ramdisk ramdisk_size=2048 $(CONSOLE)"
CDBOOT_ARGS="root=/dev/ramdisk ramdisk_size=13000 $(CONSOLE)"
CDBOOT_ARGS26="root=/dev/ram ramdisk_size=13000 $(CONSOLE)"
INSTALLER_ARGS="prompt_ramdisk=1 load_ramdisk=1 ramdisk_start=0 root=/dev/fd0 ramdisk_size=4096 $(CONSOLE)"
FLOPPY_CONF="floppy.conf"
BOOT_ARGS="root=/dev/sda4 $(CONSOLE)"

LINUXPATH=vmlinux
LINUXRAMDISK=ramdisk.gz
