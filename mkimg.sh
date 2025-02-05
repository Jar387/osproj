#!/usr/bin/sh
dd if=/dev/zero of=disk.img bs=64M count=1 > /dev/null
chmod 666 disk.img
(
echo o      # Create a new DOS partition table
echo n      # Create a new partition
echo p      # Primary partition
echo 1      # Partition number 1
echo        # Default start sector
echo        # Default end (use full disk)
echo t      # Change partition type
echo 81     # Minix type (0x81)
echo a      # Make partition bootable (active)
echo w      # Write changes
) | fdisk disk.img > /dev/null
losetup /dev/loop0 disk.img
losetup /dev/loop1 disk.img -o 1048576
mkfs.minix /dev/loop1
mount /dev/loop1 /mnt
grub-install --target=i386-pc --boot-directory=/mnt/boot --modules="normal multiboot part_msdos minix biosdisk" --no-floppy --force /dev/loop0
cp grub.cfg /mnt/boot/grub
mkdir /mnt/dev
umount /mnt
losetup --detach-all