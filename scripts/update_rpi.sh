#!/bin/bash

#parameter is ip address of rpi

IP_ADDRESS=$1


#if ip address is empty print help
if [ -z "$IP_ADDRESS" ]; then
  echo "Usage: $0 <ip_address>"
  exit 1
fi

echo "IP_ADDRESS: $IP_ADDRESS"

#run build_rpi.sh
./scripts/build_rpi.sh

#stop services over ssh
ssh root@${IP_ADDRESS} systemctl stop avahi-daemon.service can0.service reactor-core-module.service reactor-api-server.service reactor-web-control.service

#rsync to rpi
rsync -av \build/rpi/install/filesystem/ root@$IP_ADDRESS:/

#start services over ssh
ssh root@$IP_ADDRESS systemctl start avahi-daemon.service can0.service reactor-core-module.service reactor-api-server.service reactor-web-control.service

