# custom-drivers
A bunch of custom Linux drivers
## loading instruction
If you have a SecureBoot on, you need to:
1. generate private and public key by openssl
2. add public key to MOK (Machine Owner Key), instruction in the following link --> https://docs.fedoraproject.org/en-US/fedora/latest/system-administrators-guide/kernel-module-driver-configuration/Working_with_Kernel_Modules/
3. sign module by:  /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv public_key.der my_module.ko
4. load module by: sudo insmod my_module.ko
