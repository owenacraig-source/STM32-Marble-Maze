#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>

void i2c1_init(void);

void i2c1_byteWrite(uint8_t deviceAddr, uint8_t data);
void i2c1_byteRead(uint8_t deviceAddr, uint8_t *data);
void i2c1_memWrite(uint8_t deviceAddr, uint8_t registerAddr, uint8_t n, uint8_t *data);
void i2c1_memRead(uint8_t deviceAddr, uint8_t registerAddr, uint8_t n, uint8_t *data);

#endif /* I2C_H_ */
