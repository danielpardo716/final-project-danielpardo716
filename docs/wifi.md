# WiFi Config
In base_external/rootfs_overlay/etc, create the file wpa_supplicant.conf following [configs/wpa_supplicant_example.conf](../configs/wpa_supplicant_example.conf).

For obvious reasons, I have included this file in the .gitignore so that it is not pushed to github.

Also, ensure that base_external/rootfs_overlay/etc/init.d/S40network has executable permissions on the build host, as the buildroot image will inherit those permissions.