#!/usr/bin/bash

serial_port=/dev/pts/12

# send a few `log` messages
echo "log,info,test info message one" >> "$serial_port"
echo "log,info,test info message two" >> "$serial_port"

# send the reset message
echo "reset,reset,reset" >> "$serial_port"

# ptn,ptn_id,sensor_id,pressure,temp,status_code,paired

echo "ptn,1,85,2048,49,0,1" >> "$serial_port"
sleep 5
echo "ptn,2,170,1802,85,0,1" >> "$serial_port"
sleep 5
echo "ptn,2,170,1090,48,1,1" >> "$serial_port"
sleep 5
echo "ptn,2,170,1090,48,2,1" >> "$serial_port"

# send a few `ecu` messages

echo "ecu,0,reset state entered" >> "$serial_port"
sleep 5
echo "ecu,1,autolocalization started" >> "$serial_port"
sleep 5
echo "ecu,1,autolocalization finished" >> "$serial_port"
sleep 5
echo "ecu,2,regular system operation begin" >> "$serial_port"
