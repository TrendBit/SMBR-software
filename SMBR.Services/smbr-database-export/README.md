# SMPBR-database-export
Configuration files for telegraf database exporter

Required to know device sid at startup to pass it to telegraf.

Add `export SMPBR_SID=$(core-module --sid)` to end of `/etc/profile` and reload it with `source /etc/profile`
