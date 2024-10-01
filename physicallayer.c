#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encDec.h"


// (working)Function to convert an ASCII value to an 8-bit binary string
char* ascii_to_binary(char c) {
    char* binary = malloc(9);
    for (int i = 0; i < 8; i++) {
        binary[7 - i] = ((c >> i) & 1) + '0'; // Convert each bit to '0' or '1'
    }
    binary[8] = '\0'; // Add null terminator
    return binary;
}

//integer to binary
char* int_to_binary(int num) {
    char* binary = malloc(9);
    for (int i = 0; i < 8; i++) {
        binary[7 - i] = ((num >> i) & 1) + '0';
    }
    binary[8] = '\0';
    return binary;
}

//(working) Function to convert a binary string to an ASCII value
char binary_to_ascii(char* binary) {
    char c = 0;
    for (int i = 0; i < 8; i++) {
        c |= (binary[i] - '0') << (7 - i); // Add bit to correct position in ASCII char
    }
    return c;
}

//(working)Function to add odd parity to a binary string
char* add_odd_parity(char* binary) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        if (binary[i] == '1') {
            count++;
        }
    }
    if (count % 2 == 0) { // Add odd parity bit
        binary[0] = '1';
    } else {
        binary[0] = '0';
    }

    return binary;
}

// (working)Function to remove odd parity from a binary string and return the updated string
char* remove_odd_parity(char* binary) {
    if (binary[0] == '1') { // Even number of 1's with odd parity
        binary[0] = '0';}
    return binary;
}