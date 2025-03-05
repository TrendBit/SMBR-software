#!/bin/bash

#parameter is ip address of rpi
if [[ $1 =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    # Input is already an IP address
    IP_ADDRESS=$1
else
    # Input is a hostname, try to resolve it
    RESOLVED_IP=$(getent hosts "$1" | awk '{ print $1 }')

    if [ -z "$RESOLVED_IP" ]; then
        # Try specific mDNS resolution if getent doesn't work
        RESOLVED_IP=$(avahi-resolve -n -4 "$1" 2>/dev/null | awk '{ print $2 }')
    fi

    if [ -z "$RESOLVED_IP" ]; then
        echo "Error: Could not resolve hostname $1"
        exit 1
    fi

    echo "Resolved $1 to $RESOLVED_IP"
    IP_ADDRESS=$RESOLVED_IP
fi

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

