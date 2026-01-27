#!/bin/bash
# Utility script to connect to the target after flashing a new image.
# The target creates new SSH keys each time the image is flashed, so we need to remove
# the previous keys from our host's known hosts.
HOSTNAME=$1

ssh-keygen -f '/home/$USER/.ssh/known_hosts' -R '$HOSTNAME'
ssh root@$HOSTNAME