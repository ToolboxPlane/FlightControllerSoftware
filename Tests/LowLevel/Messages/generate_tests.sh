#!/usr/bin/env bash
echo "motor: 17, pitch: -18, roll: 19" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_decode.bin

echo "motor: -2147483648, pitch: 0, roll: 0" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_min_power.bin

echo "motor: 2147483647, pitch: 0, roll: 0" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_max_power.bin

echo "motor: 0, pitch: -2147483648, roll: 0" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_min_pitch.bin

echo "motor: 0, pitch: 2147483647, roll: 0" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_max_pitch.bin

echo "motor: 0, pitch: 0, roll: -2147483648" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_min_roll.bin

echo "motor: 0, pitch: 0, roll: 2147483647" | \
  protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightControllerSetpoint.proto > fcs_max_roll.bin
