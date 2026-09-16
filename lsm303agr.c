/**************************************************************************
 * Simple C library for the Adafruit LSM303AGR accelerometer.
 * ENGS 28
 * Accelerometer only, no magnetometer
 */
#include "i2c.h"
#include "lsm303agr.h"


/**************************************************************************
 * Write one byte to an accelerometer register (Datasheet, Table 20)
 */
void lsm303_AccelRegisterWrite(uint8_t RegisterAddress, uint8_t data) {
	i2c1_memWrite(LSM303_ADDRESS_ACCEL, RegisterAddress, 1, &data);


}

/**************************************************************************
 * Read one byte from an accelerometer register (Datasheet, Table 13)
 */
uint8_t lsm303_AccelRegisterRead(uint8_t RegisterAddress) {
	uint8_t data;
	i2c1_memRead(LSM303_ADDRESS_ACCEL, RegisterAddress, 1, &data);

	return data;
}

/**************************************************************************
 * Initialize the sensor
 * There are 8 control registers (Datasheet, Section 8.6), but only need the
 * first one for basic operation.  May generally accept defaults on the others,
 * e.g., full-scale range is set in Register 4.
 */
uint8_t lsm303_AccelInit() {
	// Check the WHOAMI register
  	if (lsm303_AccelRegisterRead(LSM303_WHO_AM_I_A) != 0x33) {
    	return 0;		// failure
  	}
  	else {
		/******* Control register 1 (Datasheet 8.6) *******/
		uint8_t ctrl_reg1 = 0b01110111     ; // normal power mode @ 400 Hz, all axes enabled
		lsm303_AccelRegisterWrite(LSM303_CTRL_REG1_A, ctrl_reg1);

		/******* Control register 4 (Datasheet 8.9) *******/
		uint8_t ctrl_reg4 =  0b00000000    ; // continuous update, 2g full scale, I2C
		lsm303_AccelRegisterWrite(LSM303_CTRL_REG4_A, ctrl_reg4);

		return 1;			// success
  	}
}

/**************************************************************************
 * Read acceleration data from the sensor
 * Pass a pointer to a structure to hold the three (int16_t) values.
 * The accelerometer is repeatedly sampling, I just have to grab the data.
 *
 * The output is a left-justified twos complement integer.
 * The three precision modes (high, normal, low) have 12, 10, and 8 nonzero bits,
 * respectively. Shift right by number of zero bits, then
 * multiply by resolution to get acceleration.
 * Multiply by 1000 to convert g to milli-g.
 *
 * When the chip is mounted horizontally, the Z component of gravitational
 * acceleration is positive upward.  Roll the circuit board over and the sign of the
 * acceleration flips to negative.
 */
void lsm303_AccelReadRaw(lsm303AccelData_s *result) {
 	uint8_t data[6];

 	/* If you want to read multiple registers at once you have to tell the chip:
 	 * An 8-bit sub-address (SUB) is transmitted: the 7 LSb represent the actual
 	 * register address while the MSB enables address auto increment.
 	 * If the MSb of the SUB field is ‘1’, the SUB (register address) is
 	 * automatically increased to allow multiple data read/writes.
 	 */

	i2c1_memRead(LSM303_ADDRESS_ACCEL, LSM303_OUT_X_L_A | (1<<7), 6, &data[0]);

	result->x = ((int16_t) data[1] << 8) | ((int16_t) data[0]);
	result->y = ((int16_t) data[3] << 8) | ((int16_t) data[2]);
	result->z = ((int16_t) data[5] << 8) | ((int16_t) data[4]);

	/* Alternatively, you could read each register in a single byte read and then
	 * manually combine low and high byte like this:
	 *
	 * int16_t accel_raw = ((int16_t) lsm303_AccelRegisterRead(LSM303_OUT_X_H_A) << 8)
 			              | (int16_t) lsm303_AccelRegisterRead(LSM303_OUT_X_L_A);
 	 */

	/* If you want to convert the raw data to an acceleration (in mg) you need to do
	 * some math. For a full-scale range of +/-2g, this means multiply by 4*1000 and
	 * divide by 2^16 (see class notes).
	 *
	 * int16_t accel_x = (4 * result->x * 1000) >> 16;
	 * int16_t accel_x = (ACC_FS * result->x * MILLI) >> ACC_REGISTERWIDTH;
	 */
}
