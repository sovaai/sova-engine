#!/bin/bash

# Save environment variables.
mkdir -p ~/.ssh &&
chmod 700 ~/.ssh &&
env | grep '^IFS_' > /root/.ssh/environment &&

# Start sshd.
if [[ -n "$SSHD_ENABLE" ]]; then
    if [[ -n "$SSHD_AUTHORIZED_KEYS" ]]; then
        echo "$SSHD_AUTHORIZED_KEYS" >> ~/.ssh/authorized_keys &&
        chmod 600 ~/.ssh/authorized_keys
    fi &&
    /etc/init.d/ssh start
fi &&

# Start rsyslog.
rm -f /run/rsyslogd.pid &&
/etc/init.d/rsyslog start &&
while ! /etc/init.d/rsyslog status; do sleep 1; done;

# Update Knowledge Base.
if [ -f /dldata/dldata.ie2 ]; then
    InfEngineManager.pl --dl-update /dldata/dldata.ie2 -v;
fi &&

# Start engine.
rm -f /engine/tmp/process-server.pid &&
InfEngineControl.pl --start &&

# Infinite cycle.
while true; do sleep 1; done;