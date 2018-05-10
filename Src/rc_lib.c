#include "rc_lib.h"

uint8_t rc_lib_global_package_uid = 0;
uint8_t rc_lib_transmitter_id = 0;
uint8_t rc_lib_error_count = 0;

uint8_t rc_lib_encode(rc_lib_package_t* package) {
    package->buffer[0] = RC_LIB_START;
    package->buffer[1] = ++rc_lib_global_package_uid;
    package->buffer[2] = rc_lib_transmitter_id;
    package->mesh = (uint8_t)(package->mesh?1:0);
    package->buffer[3] = (uint8_t)(_rc_lib_resolution_steps_2_key(package->resolution) |
                                     _rc_lib_channel_count_2_key(package->channel_count) << 3 |
                                     (rc_lib_error_count>0?1:0) << 6 |
                                     package->mesh << 7);
    if(package->mesh){
        package->buffer[4] = package->routing_length;
    }

    uint8_t resBits = _rc_lib_resolution_steps_2_bit_count(package->resolution);

    uint16_t dataSize = resBits * package->channel_count;
    if(dataSize % 8 == 0){
        dataSize /= 8;
    } else {
        dataSize = (uint8_t)(dataSize/8 + 1);
    }

    for(int c=0; c<dataSize; c++){
        package->buffer[4+c+package->mesh] = 0;
        for(int b=0; b<8 && (c*8+b)<(resBits*package->channel_count); b++){
            uint8_t bit = (uint8_t)(package->channel_data[(c * 8 + b) / resBits] & (0b1 << ((c * 8 + b) % resBits)) ? 1:0);
            package->buffer[4+c+package->mesh] |= bit << b;
        }
    }

    package->buf_count = (uint8_t)(4+dataSize+2+package->mesh);
    package->buffer[4+dataSize+package->mesh] = rc_lib_calculate_checksum(package);
    package->buffer[4+dataSize+package->mesh+1] = RC_LIB_END;

    return package->buf_count;
}

uint8_t rc_lib_decode(rc_lib_package_t* package, uint8_t data) {
    static uint8_t receiveStateMachineState = 0;
    static uint8_t dataByteCount = 0;

    switch(receiveStateMachineState){
        case 0: // Initial state
            package->buf_count = 0;
            if(data == RC_LIB_START) {
                receiveStateMachineState = 1;
            }
            break;
        case 1: // Start word received
            package->uid = data;
            rc_lib_global_package_uid = package->uid;

            receiveStateMachineState = 2;
            break;
        case 2: // Transmitter Id
            package->tid = data;
            receiveStateMachineState = 3;
            break;
        case 3: // Configuration
            package->resolution = _rc_lib_key_2_resolution_steps(data&0b111);
            package->channel_count = _rc_lib_key_2_channel_count((data&0b111000) >> 3);

            for(int c=0; c<package->channel_count; c++){
                package->channel_data[c] = 0;
            }
            dataByteCount = 0;

            // Following
            if(data & (0b1 << 7)){
                receiveStateMachineState = 4;
            } else {
                receiveStateMachineState = 5;
            }
            break;
        case 4: // Mesh
            package->routing_length = data & 0b1111;
            package->mesh = package->routing_length > 0;
            receiveStateMachineState = 5;
            break;
        case 5: // Data
        {
            uint8_t resBits = _rc_lib_resolution_steps_2_bit_count(package->resolution);

            uint16_t dataSize = resBits * package->channel_count;
            if (dataSize % 8 == 0) {
                dataSize /= 8;
            } else {
                dataSize = dataSize / 8 + 1;
            }

            for (int c = 0; c < 8; c++) {
                uint8_t bit = (dataByteCount * 8 + c) % resBits;
                package->channel_data[(dataByteCount * 8 + c) / resBits] |= ((data & (0b1 << c))?1:0) << bit;
            }
            if (++dataByteCount >= dataSize) {
                receiveStateMachineState = 6;
            }
        }
            break;
        case 6: // Checksum
            package->checksum = data;
            receiveStateMachineState = 7;

            if(rc_lib_calculate_checksum(package) == package->checksum){
                rc_lib_error_count += 4;
            }
            break;
        case 7: // End byte
            if(data == RC_LIB_END) {
                receiveStateMachineState = 0;
                package->buf_count++;
                return (uint8_t)true;
            } else {
                rc_lib_error_count += 4;
                receiveStateMachineState = 0;
                return (uint8_t)false;
            }
        default:
            receiveStateMachineState = 0;
            break;
    }
    package->buffer[package->buf_count++] = data;

    return (uint8_t)false;
}

uint8_t rc_lib_calculate_checksum(rc_lib_package_t* package) {
    uint8_t checksum = 0;

    for(uint16_t c=0; c<package->buf_count-3; c++){
        checksum ^= package->buffer[c+1];
    }

    return checksum;
}

uint8_t is_discover_message(rc_lib_package_t *package) {
    return (uint8_t) (package->discover_state == 1);
}

uint8_t is_discover_response(rc_lib_package_t *package) {
    return (uint8_t) (package->discover_state == 2);
}

void set_discover_message(rc_lib_package_t *package) {
    package->discover_state = 1;
    package->channel_count = 0;
    package->mesh = 1;
}

void make_discover_response(rc_lib_package_t *new_package, rc_lib_package_t *responses, uint8_t len) {
    new_package->resolution = 256;
    new_package->channel_count = 1;
    new_package->mesh = 1;
    new_package->discover_state = 2;

    uint16_t tidCount = 0;
    for(uint8_t r=0; r<len; r++) {
        for(uint16_t c=0; c<responses[r].channel_count; c++) {
            if(responses[r].channel_data[c] != 0) {
                if(tidCount+1 > new_package->channel_count) {
                    new_package->channel_count *= 2;
                }

                new_package->channel_data[tidCount++] = responses[r].channel_data[c];
            }
        }
    }

    if(tidCount+1 > new_package->channel_count) {
        new_package->channel_count *= 2;
    }
    new_package->channel_data[tidCount++] = rc_lib_transmitter_id;
    for(uint16_t c=tidCount; c<new_package->channel_count; c++) {
        new_package->channel_data[c] = 0;
    }
}

uint8_t _rc_lib_resolution_steps_2_key(uint16_t steps) {
    switch(steps){
        case 32:
            return 0b000;
        case 64:
            return 0b001;
        case 128:
            return 0b010;
        case 256:
            return 0b011;
        case 512:
            return 0b100;
        case 1024:
            return 0b101;
        case 2048:
            return 0b110;
        case 4096:
            return 0b111;
        default:
            return 0b111;
    }
}

uint8_t _rc_lib_channel_count_2_key(uint16_t channel_count) {
    switch(channel_count){
        case 1:
            return 0b000;
        case 2:
            return 0b001;
        case 4:
            return 0b010;
        case 8:
            return 0b011;
        case 16:
            return 0b100;
        case 32:
            return 0b101;
        case 64:
            return 0b110;
        case 256:
            return 0b111;
        default:
            return 0b111;
    }
}

uint8_t _rc_lib_resolution_steps_2_bit_count(uint16_t steps) {
    switch(steps){
        case 32:
            return 5;
        case 64:
            return 6;
        case 128:
            return 7;
        case 256:
            return 8;
        case 512:
            return 9;
        case 1024:
            return 10;
        case 2048:
            return 11;
        case 4096:
            return 12;
        default:
            return 12;
    }
}

uint16_t _rc_lib_key_2_resolution_steps(uint8_t key) {
    switch(key){
        case 0b000:
            return 32;
        case 0b001:
            return 64;
        case 0b010:
            return 128;
        case 0b011:
            return 256;
        case 0b100:
            return 512;
        case 0b101:
            return 1024;
        case 0b110:
            return 2048;
        case 0b111:
            return 4096;
        default:
            return 4096;
    }
}

uint16_t _rc_lib_key_2_channel_count(uint8_t key) {
    switch(key){
        case 0b000:
            return 1;
        case 0b001:
            return 2;
        case 0b010:
            return 4;
        case 0b011:
            return 8;
        case 0b100:
            return 16;
        case 0b101:
            return 32;
        case 0b110:
            return 64;
        case 0b111:
            return 256;
        default:
            return 256;
    }
}
