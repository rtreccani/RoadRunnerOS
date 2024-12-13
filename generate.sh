#!/bin/bash

set -e
cmake -Bbuild
make -Cbuild

if [[ "$1" == "-bin" ]]; then

    echo "running qemu directly on the bin file"
    qemu-system-i386 -kernel build/rros.bin -display gtk,zoom-to-fit=on

elif [[ "$1" == "-grub" ]]; then

    echo "running via grub MB option"
    mkdir -p build/isodir/boot/grub
    cp build/rros.bin build/isodir/boot/rros.bin
    cp extras/grub.cfg build/isodir/boot/grub/grub.cfg
    grub-mkrescue -o build/rros.iso build/isodir
    qemu-system-i386 -cdrom build/rros.iso -display gtk,zoom-to-fit=on

elif [[ "$1" == "-bochs" ]]; then
    echo "running via grub MB option"
    mkdir -p build/isodir/boot/grub
    cp build/rros.bin build/isodir/boot/rros.bin
    cp extras/grub.cfg build/isodir/boot/grub/grub.cfg
    grub-mkrescue -o build/rros.iso build/isodir
    bochs -q

elif [[ "$1" == "-gdb" ]]; then
    echo "running qemu directly on the bin file"
    qemu-system-i386 -kernel build/rros.bin -display gtk,zoom-to-fit=on -s -S

else
    echo "please use either -grub or -bin options"

fi
