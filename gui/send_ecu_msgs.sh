#!/usr/bin/bash

# send a few `log` messages
echo "log,info,test info message one" >> temp_serial_port
echo "log,info,test info message two" >> temp_serial_port

# send a few `ptn` messages

echo "ptn,1,85,2048,49,0,1" >> temp_serial_port
echo "ptn,2,170,1802,85,0,1" >> temp_serial_port

# send a few `ecu` messages

echo "ecu,0,reset state entered" >> temp_serial_port
echo "ecu,1,autolocalization started" >> temp_serial_port
echo "ecu,1,autolocalization finished" >> temp_serial_port
echo "ecu,2,regular system operation begin" >> temp_serial_port
