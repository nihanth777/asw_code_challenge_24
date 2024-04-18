#include "i2c.h"
#include <stdint.h>
#include <stdio.h>

//Address declarations according to the data sheet
#define DEVICE_ADDRESS (0x0F)
#define CTRL_REG2 (0x21)
#define CTRL_REG1 (0x20)
#define INT_CFG (0x30)
#define OUT_X_L (0x28)
#define OUT_X_H (0x29)
#define OUT_Y_L (0x2A)
#define OUT_Y_H (0x2B)
#define OUT_Z_L (0x2C)
#define OUT_Z_H (0x2D)

uint8_t *podrAddress = (uint8_t) CTRL_REG1;
uint8_t output_data_rate = 0;
uint8_t *pintcfgAddress = (uint8_t) INT_CFG;
uint8_t do0 ;
uint8_t do1 ;
uint8_t do2 ;


// Define the lookup table with entries from the output rate table from data sheet
typedef struct {
    uint8_t do0;
    uint8_t do1;
    uint8_t do2;
    float odr;
} ODR_Table_Entry;


ODR_Table_Entry odr_table[] = {
    {0, 0, 0, 0.625},
    {0, 0, 1, 1.25},
    {0, 1, 0, 2.5},
    {0, 1, 1, 5},
    {1, 0, 0, 10},
    {1, 0, 1, 20},
    {1, 1, 0, 40},
    {1, 1, 1, 80}
};

//1.Get the full-scale configuration
uint8_t get_full_configuration(){
    uint8_t *buffer;
    if (i2c_read(DEVICE_ADDRESS, CTRL_REG2, sizeof(uint8_t), *buffer) == STATUS_OK) {
        return *buffer;
    } else {
        // Handle error
        return 0;
    }
}
//--------------------------------------------------------

//2.Get and set the output data rate
//get output data rate
uint8_t get_output_data_rate(){
    uint8_t *buffer;
    uint8_t output_datarate;
    if (i2c_read(DEVICE_ADDRESS, CTRL_REG1, sizeof(uint8_t), buffer) == STATUS_OK) {
        
        do0 = get_nth_bit_value(buffer, 2);
        do1 = get_nth_bit_value(buffer, 3);
        do2 = get_nth_bit_value(buffer, 4);

        for (int i = 0; i < sizeof(odr_table) / sizeof(odr_table[0]); ++i) {
        // Check if the input conditions match the entry in the table
        if (do0 == odr_table[i].do0 && do1 == odr_table[i].do1 && do2 == odr_table[i].do2) {
            // Return the corresponding ODR value
            output_data_rate = odr_table[i].odr;
        }
        return output_data_rate;
    }
    // Return a default value if no match is found (you can handle this case as needed)
    return -1.0; // Default value indicating no match found
    } else {
        // Handle error
        return 0;
    }
}

//set output data rate
uint8_t set_output_data_rate(uint8_t ST, uint8_t FAST_ODR, uint8_t DO0, uint8_t DO1, uint8_t DO2, uint8_t OM0, uint8_t OM1, uint8_t TEMP_EN){  // All parameters are given as input as final usage wasnt clear so given maximum control here

    update_address(&ST, &FAST_ODR, &DO0, &DO1, &DO2, &OM0, &OM1, &TEMP_EN);

    uint8_t podrAddress = (ST << 7) | (FAST_ODR << 6) | (DO0 << 5) | (DO1 << 4) | (DO2 << 3) | (OM0 << 2) | (OM1 << 1) | TEMP_EN;
    
    if (i2c_write(DEVICE_ADDRESS, CTRL_REG1, sizeof(uint8_t), podrAddress) != STATUS_OK) {
       return "Output datarate set Successfully";
    }
}
//-----------------------------------------------------------------------------------

//3.Enable or disable the interrupt pin
uint8_t ctrl_interrupt_pin(uint8_t pin){ //pin = 0||1, 0-disable, 1-enable
    *pintcfgAddress |= pin<<0;  // 0 - IEN bit to enable or disable interrupt pin
    if (i2c_write(DEVICE_ADDRESS, INT_CFG, sizeof(uint8_t), *pintcfgAddress) != STATUS_OK) {
        return "Interrupt pin updated";
    }
}
//-----------------------------------------------------------------------------------

//4.Read the output data of a specified axis
uint8_t axis_output_data(uint8_t axis){
    uint8_t reg_L, reg_H;
    switch(axis) {
        case 'X':
            reg_L = OUT_X_L;
            reg_H = OUT_X_H;
            break;
        case 'Y':
            reg_L = OUT_X_L;
            reg_H = OUT_X_H;
            break;
        case 'Z':
            reg_L = OUT_X_L;
            reg_H = OUT_X_H;
            break;
        default:
            // Handle invalid axis
            return 0;
    }
    uint8_t buffer[2];
    if (i2c_read(DEVICE_ADDRESS, reg_L, sizeof(uint16_t), buffer) == STATUS_OK) {
        return (buffer[0] << 8) | buffer[1];
    } else {
        // Handle error
        return 0;
    }
    if (i2c_read(DEVICE_ADDRESS, reg_H, sizeof(uint16_t), buffer) == STATUS_OK) {
        return (buffer[0] << 8) | buffer[1];
    } else {
        // Handle error
        return 0;
    }
}
//------------------------------------------------------------------------------------

//Function to update variables a 8 bit address- can be used for other purposes as well as we have many 8 bit address
void update_address(uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d, uint8_t *e, uint8_t *f, uint8_t *g, uint8_t *h) {
    // Combine individual bits into an 8-bit address
    uint8_t address = (*h << 7) | (*g << 6) | (*f << 5) | (*e << 4) | (*d << 3) | (*c << 2) | (*b << 1) | *a;
    
    // Update the variables
    *a = (address >> 0) & 0x01;
    *b = (address >> 1) & 0x01;
    *c = (address >> 2) & 0x01;
    *d = (address >> 3) & 0x01;
    *e = (address >> 4) & 0x01;
    *f = (address >> 5) & 0x01;
    *g = (address >> 6) & 0x01;
    *h = (address >> 7) & 0x01;

    //return address; //return updated address
}

//Function to convert twos comliment to decimal
int8_t twosCompToDec(uint8_t two_compliment_val)
{
    uint8_t sign_mask = 0x80;

    // if positive
    if ( (two_compliment_val & sign_mask) == 0 ) {
        return two_compliment_val;
    //  if negative
    } else {
        // invert all bits, add one, and make negative
        return -(~two_compliment_val + 1);
    }
}

// Function to get the value of the nth bit in an 8-bit address
uint8_t get_nth_bit_value(uint8_t address, uint8_t n) {
    // Shift 1 to the left by n positions to create a mask for the nth bit
    uint8_t mask = 1 << n;
    // Use bitwise AND to extract the value of the nth bit
    uint8_t bit_value = (address & mask) ? 1 : 0;
    return bit_value;
}


/*
for use in RTOS environment, we can improve by adding asynchronous support and make it more clear in the documentation regarding the 
default values that needs no changing
*/