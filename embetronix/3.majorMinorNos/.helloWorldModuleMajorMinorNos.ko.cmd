cmd_/home/pi/Desktop/LDD/majorMinorNos/helloWorldModuleMajorMinorNos.ko := ld -r  -EL -T ./scripts/module-common.lds -T ./arch/arm/kernel/module.lds  --build-id  -o /home/pi/Desktop/LDD/majorMinorNos/helloWorldModuleMajorMinorNos.ko /home/pi/Desktop/LDD/majorMinorNos/helloWorldModuleMajorMinorNos.o /home/pi/Desktop/LDD/majorMinorNos/helloWorldModuleMajorMinorNos.mod.o ;  true
