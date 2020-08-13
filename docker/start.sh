#!/bin/bash

rm -f /run/rsyslogd.pid &&

/etc/init.d/rsyslog start &&

sleep 5 &&

if [ -f /dldata/dldata.ie2 ];
  then
    InfEngineManager.pl --dl-update /dldata/dldata.ie2 -v;
fi &&

rm -f /engine/tmp/process-server.pid &&

InfEngineControl.pl --start &&

tail -f /var/log/syslog
