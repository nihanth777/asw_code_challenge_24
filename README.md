# AAC-Clydespace Avionics Software Challenge
You are given the LIS3MDLTR 3-Axis Magnetometer sensor. The data sheet is
located at `doc/lism3mdl.pdf`

## Task Description
You must use the C API defined in `i2c.h` to operate on an I2C bus.

We provide example stub implementations of this API in `i2c.c`. You are free to
extend the provided stub functions for the API.

Please write a device driver that implements the following API:
- Get the full-scale configuration
- Get and set the output data rate
- Enable or disable the interrupt pin
- Read the output data of a specified axis

## Extra Thoughts
If you have time, answer the following questions:

What changes you would make to this interfaces for use in an RTOS environment?
How might the I2C API be improved

## Implementation
A new file is created 'LIS3MDLTR.c' as a device driver according to the requirements mentioned the above task 
and other files are left as they were

i2c.h contains i2c read and write functions which are used in the LIS3MDLTR.c for setting and gettings details 
from the registers according to the data sheet in the doc/lis3mdl.pdf . 

The code is well commented and modular so it can be integrated to further application and work on this sensor.

There is a comment section at the end of the file regarding the extra thoughts which are here below as well
"for use in RTOS environment, we can improve by adding asynchronous support and make it more clear in the documentation regarding the 
default values that needs no changing"


