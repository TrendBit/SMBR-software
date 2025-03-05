#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <hostname_or_ip>"
  exit 1
fi

#parameter is ip address of rpi
if [[ $1 =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    # Input is already an IP address
    HOST="root@$1"
else
    HOST="$1"
fi

echo "Host: $HOST"

#run build_rpi.sh
./scripts/build_rpi.sh

#stop services over ssh
ssh ${HOST} systemctl stop can0.service reactor-database-export.service reactor-core-module.service reactor-api-server.service reactor-web-control.service

#rsync to rpi
rsync -rv --no-perms \build/rpi/install/filesystem/ ${HOST}:/

#update systemd services
ssh ${HOST} systemctl daemon-reload

#start services over ssh
ssh ${HOST} systemctl start can0.service reactor-database-export.service reactor-core-module.service reactor-api-server.service reactor-web-control.service

#restart avahi separately at he end to avoid issue not non existing hostname
ssh ${HOST} systemctl restart avahi-daemon.socket avahi-daemon.service
