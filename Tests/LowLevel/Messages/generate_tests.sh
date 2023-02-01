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

echo "imu: {
        imu_ok: true,
        roll_mul_16: 17,
        pitch_mul_16: -18
        yaw_mul_16: 19,
        dRoll_mul_16: -20,
        dPitch_mul_16: 21,
        dYaw_mul_16: -22
        accX_mul_100: 23,
        accY_mul_100: -24,
        accZ_mul_100: 25
      },
      remote: {
        remote_ok: false;
        throttleMixed: -26;
        elevonLeftMixed: 27;
        elevonRightMixed: -28;
        isArmed: true;
        overrideActive: false;
      },
      motor: 29,
      servoLeft: 30,
      servoRight: 31" | \
  protoc --encode=ToolboxPlaneMessages.FlightController \
    --proto_path "$1" \
    Src/Messages/Definitions/FlightController.proto > fc_encode.bin
