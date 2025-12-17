#!/bin/bash

if [ $# -eq 0 ]; then
    exec "$APPDIR/usr/bin/libresplit"
else
    exec "$APPDIR/usr/bin/libresplit-ctl" "$@"
fi
