cmd_/home/pi/Desktop/LDD/8.procfs/procfsExample.ko := ld -r  -EL -T ./scripts/module-common.lds -T ./arch/arm/kernel/module.lds  --build-id  -o /home/pi/Desktop/LDD/8.procfs/procfsExample.ko /home/pi/Desktop/LDD/8.procfs/procfsExample.o /home/pi/Desktop/LDD/8.procfs/procfsExample.mod.o ;  true