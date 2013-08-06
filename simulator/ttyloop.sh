#!/bin/sh

#sudo socat PTY,link=/dev/ttyS10,group=dialout PTY,link=/dev/ttyS11,group=dialout
sudo socat PTY,link=/dev/ttyS10,setgid-early=dialout PTY,link=/dev/ttyS11,setgid-early=dialout
