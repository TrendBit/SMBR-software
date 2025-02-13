#!/bin/bash

#parameter is ip address of rpi

IP_ADDRESS=$1


#if ip address is empty print help
if [ -z "$IP_ADDRESS" ]; then
  echo "Usage: $0 <ip_address>"
  exit 1
fi

echo "IP_ADDRESS: $IP_ADDRESS"

#enable services over ssh
ssh root@${IP_ADDRESS} systemctl enable avahi-daemon.service can0.service smbr-core-module.service smbr-api-server.service


