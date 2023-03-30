#include "ModuleControl_v100.hpp"

#define SENSORS_I2C_ADDR 0x10
#define PDA50_I2C_ADDR 0x0C
#define TEMP_I2C_ADDR 0x4C
#define EEPROM_I2C_ADDR 0x50
#define REG_FB_STATE 0x53
#define REG_LINE_LENGTH 0x06
#define REG_TINT_LL 0x0B
#define REG_TINT_CK 0x0C
#define REG_ANA_GAIN 0x0D
#define REG_DIG_GAIN 0x0E

/**RGU UPDATE TOPAZ**/
// replace exit by return
// add temperature sensor read

/************************************************
 *Main fuction
 ************************************************/

ModuleCtrl::~ModuleCtrl()
{
#ifndef DEBUG_MODE
	ModuleControlClose(); // close IC2
#endif
}

/************************************************
 *Init I2C bus
 ************************************************/
void ModuleCtrl::ModuleControlInit()
{
	/* Open i2c bus */

	if ((bus = i2c_open(bus_name)) == -1)
	{

		fprintf(stderr, "Open i2c bus:%s error!\n", bus_name);
		return;
	}
	printf("Bus %s open\n", bus_name);

	/* Init sensor i2c device */

	memset(&device, 0, sizeof(device));
	i2c_init_device(&device);

	device.bus = bus;
	/*device address*/
	device.addr = SENSORS_I2C_ADDR;
	/*Unknown value*/
	device.page_bytes = 256;
	/*Address length in bytes*/
	device.iaddr_bytes = 1;

	/* Init i2c devicepda */

	memset(&devicepda, 0, sizeof(devicepda));
	i2c_init_device(&devicepda);

	devicepda.bus = bus;
	/*device address*/
	devicepda.addr = PDA50_I2C_ADDR;
	/*Unknown value*/
	devicepda.page_bytes = 8;
	/*Address length in bytes*/
	devicepda.iaddr_bytes = 1;

	// Enable PDA50 DAC
	printf("Enable PDA50 DAC\n");
	enable_VdacPda(devicepda, bus);

	/* Init i2c devicetemp */

	memset(&devicetemp, 0, sizeof(devicetemp));
	i2c_init_device(&devicetemp);

	devicetemp.bus = bus;
	/*device address*/
	devicetemp.addr = TEMP_I2C_ADDR;
	/*Unknown value*/
	devicetemp.page_bytes = 8;
	/*Address length in bytes*/
	devicetemp.iaddr_bytes = 1;
}

/************************************************
 *Close I2C bus
 ************************************************/
void ModuleCtrl::ModuleControlClose()
{
	// Disable PDA50 DAC
	printf("Disable PDA50 DAC\n");

	disable_VdacPda(devicepda, bus);

	/* Close de bus*/
	i2c_close(bus);

	printf("Bus %s closed\n", bus_name);
}

/************************************************
 *Read register
 ************************************************/
int ModuleCtrl::readReg(int regAddr, int *value)
{
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;

	device.page_bytes = 256;
	buffer[0] = 1;
	buffer[1] = 2;
	if ((i2c_ioctl_read(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "READ ERROR: device=0x%x register address=0x%x\n", device.addr, regAddr);
		// i2c_close(bus);
		error = -3;
	}
	else
	{
		*value = buffer[0] * 256 + buffer[1];
	}
	// printf("error=%d\n",error);
	return error;
}

/************************************************
 *Write register
 ************************************************/
int ModuleCtrl::writeReg(int regAddr, int value)
{
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;

	*buffer = ((value)&0xff00) >> 8;
	*(buffer + 1) = ((value)&0x00ff);

	device.page_bytes = 256;

	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "WRITE ERROR: device=0x%x register address=0x%x\n", device.addr, regAddr);
		// i2c_close(bus);
		error = -3;
	}
	// printf("error=%d\n",error);
	return error;
}
int ModuleCtrl::read_sensor_state(int *state)
{
	// device.page_bytes = 256;

	int ulAddress;
	int ulValue = 0;
	int error = 0;

	ulAddress = REG_FB_STATE;

	error = this->readReg(ulAddress, &ulValue);
	*state = ulValue;

	return error;
}

int ModuleCtrl::setTint(float b)
{
	int regAddr, line;
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	struct solution init;

	memset(buffer, 0, size);
	device.page_bytes = 256;

	b = b * 1000000; // convert to nanoseconds
	b = b / 20;

	// read the line length

	regAddr = REG_LINE_LENGTH;
	if ((i2c_ioctl_read(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", regAddr);
		i2c_close(bus);
		return 0;
	}

	line = (*buffer << 8) + *(buffer + 1);

	init.tintII = (int)b / line;
	init.tintclk = (int)(b - init.tintII * line);

	// write in reg_tint_ll
	regAddr = REG_TINT_LL;
	*buffer = ((init.tintII) & 0xff00) >> 8;
	*(buffer + 1) = (init.tintII & 0x00ff);

	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		i2c_close(bus);
		return 0;
	}

	// write in reg_tint_ck
	regAddr = REG_TINT_CK;
	*buffer = ((init.tintclk) & 0xff00) >> 8;
	*(buffer + 1) = (init.tintclk & 0x00ff);

	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		i2c_close(bus);
		return 0;
	}

	return 0;
}

/************************************************
 *Set the analog gain
 ************************************************/

int ModuleCtrl::setAnalogGain(float again)
{
	int regAddr, b;
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	regAddr = REG_ANA_GAIN;
	device.page_bytes = 256;

	b = (int)(again * 100);

	switch (b)
	{
	case 100:
		b = 0;
		break;
	case 120:
		b = 1;
		break;
	case 145:
		b = 2;
		break;
	case 150:
		b = 2;
		break;
	case 171:
		b = 3;
		break;
	case 200:
		b = 4;
		break;
	case 240:
		b = 5;
		break;
	case 300:
		b = 6;
		break;
	case 343:
		b = 7;
		break;
	case 400:
		b = 8;
		break;
	case 480:
		b = 9;
		break;
	case 500:
		b = 9;
		break;
	case 600:
		b = 10;
		break;
	case 686:
		b = 11;
		break;
	case 700:
		b = 11;
		break;
	case 800:
		b = 12;
		break;
	case 900:
		b = 13;
		break;
	case 960:
		b = 13;
		break;
	case 1000:
		b = 13;
		break;
	case 1200:
		b = 14;
		break;
	case 1600:
		b = 15;
		break;
	default:
		printf("Forbidden value, couldn't set analog gain\n");
		return 0;
	}

	*(buffer + 1) = (*(buffer + 1) & 0xf0) + b;

	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		i2c_close(bus);
		return 0;
	}
	return 0;
}

/************************************************
 *Set the digital gain
 ************************************************/
int ModuleCtrl::setDigitalGain(float dgain)
{
	int regAddr, b;
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	regAddr = REG_DIG_GAIN;
	device.page_bytes = 256;

	// from x0.004 to x16 (decimal ex: x1=256, x1.5=384...)
	b = (int)(dgain * 256);

	if (b > 4095)
		b = 4095;
	if (b < 1)
		b = 1;

	// write (b) in register
	*buffer = ((b)&0xff00) >> 8;
	*(buffer + 1) = ((b)&0x00ff);
	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		i2c_close(bus);
		return 0;
	}
	return 0;
}

/************************************************
 *TLENS PDA50 functions
 ************************************************/
int enable_VdacPda(I2CDevice device, int bus)
{
	int regAddr;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;

	// SET CFG_ENABLE = 1
	*buffer = 0x01;
	regAddr = 0x00;
	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		// fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		// i2c_close(bus);
		// exit(-3);
		error = -3;
	}
	return error;
}

int disable_VdacPda(I2CDevice device, int bus)
{
	int regAddr;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;

	// SET CFG_ENABLE = 0
	*buffer = 0x00;
	regAddr = 0x00;
	if ((i2c_ioctl_write(&device, regAddr, buffer, size)) != size)
	{
		// fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		// i2c_close(bus);
		// exit(-3);
		error = -3;
	}
	return error;
}

int ModuleCtrl::write_VdacPda(int PdaRegValue)
{
	int regAddr;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;

	// SET DAC VALUE
	// printf("PdaRegValue=%d\n", PdaRegValue);
	if (PdaRegValue > 879)
	{
		PdaRegValue = 879;
	}
	else if (PdaRegValue < -91)
	{
		PdaRegValue = -91;
	}

	unsigned char MSB, LSB;
	if (PdaRegValue >= 0)
	{
		MSB = 0x80 + ((PdaRegValue >> 8) & 0xFF);
		LSB = PdaRegValue & 0xFF;
	}
	else
	{
		PdaRegValue = -PdaRegValue;
		MSB = 0x0 + ((PdaRegValue >> 8) & 0xFF);
		LSB = PdaRegValue & 0xFF;
	}
	// printf("Val=%d, MSB=0x%x, LSB=0x%x\n", PdaRegValue, MSB, LSB);
	devicepda.page_bytes = 8;
	// WRITE MSB
	regAddr = 0x02;
	*buffer = MSB;
	if ((i2c_ioctl_write(&devicepda, regAddr, buffer, size)) != size)
	{
		// fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		// i2c_close(bus);
		// exit(-3);
		error = -3;
	}
	// dumpPda50Reg(devicepda, bus);
	//  WRITE LSB
	regAddr = 0x03;
	*buffer = LSB;
	if ((i2c_ioctl_write(&devicepda, regAddr, buffer, size)) != size)
	{
		// fprintf(stderr, "Can't write in %x reg!\n", regAddr);
		// i2c_close(bus);
		// exit(-3);
		error = -3;
	}
	// dumpPda50Reg(devicepda, bus);
	return error;
}

int ModuleCtrl::read_VdacPda(int *PdaRegValue, double *PdaVoltageValue)
{

	int regAddr, b;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;

	devicepda.page_bytes = 1;

	unsigned char MSB, LSB;
	// READ MSB
	regAddr = 0x02;
	if ((i2c_ioctl_read(&devicepda, regAddr, &MSB, size)) != size)
	{
		// fprintf(stderr, "Can't read %x reg!\n", regAddr);
		// i2c_close(bus);
		// exit(-3);
		error = -3;
	}

	// READ LSB
	regAddr = 0x03;
	*buffer = LSB;
	if ((i2c_ioctl_read(&devicepda, regAddr, &LSB, size)) != size)
	{
		// fprintf(stderr, "Can't read %x reg!\n", regAddr);
		// i2c_close(bus);
		// exit(-3);
		error = -3;
	}

	if (MSB >= 128)
	{
		b = ((MSB & 0x7F) << 8) + LSB;
		// printf("Val=%d, Val=0x%x, MSB=0x%x, LSB=0x%x\n", b, b, MSB, LSB);
	}
	else
	{
		b = -(((MSB & 0x7F) << 8) + LSB);
		// printf("Val=%d, Val=0x%x, MSB=0x%x, LSB=0x%x\n", b, -b, MSB, LSB);
	}
	*PdaRegValue = b;
	*PdaVoltageValue = (double)b * (double)(0.0546);
	return error;
}

int ModuleCtrl::read_Temp(double *LocalTempValue, double *RemoteTempValue, int TempMode)
{
	int regAddr, b;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;
	uint16_t value = 0;
	uint8_t extended = TempMode;
	unsigned char MSB, LSB;

	// READ REMOTE_MSB
	regAddr = 0x01;
	if ((i2c_ioctl_read(&devicetemp, regAddr, &MSB, size)) != size)
	{
		fprintf(stderr, "READ ERROR: device=0x%x register address=0x%x\n", devicetemp.addr, regAddr);
		error = -3;
	}

	// READ REMOTE_LSB
	regAddr = 0x10;
	*buffer = LSB;
	if ((i2c_ioctl_read(&devicetemp, regAddr, &LSB, size)) != size)
	{
		fprintf(stderr, "READ ERROR: device=0x%x register address=0x%x\n", devicetemp.addr, regAddr);
		error = -3;
	}

	if (error == 0)
	{
		value = ((MSB & 0x7F) << 8) + LSB;
		printf("REMOTE=0x%x (%d), MSB=0x%x, LSB=0x%x\n", value, value, MSB, LSB);
		*RemoteTempValue = (value >> 8) + (extended ? -64 : 0) + ((value & 0xF0) >> 4) * 0.0625;
		// fprintf(stderr, "RemoteTemp=%f°C\n",*RemoteTempValue);
	}

	// READ LOCAL
	regAddr = 0x00;
	if ((i2c_ioctl_read(&devicetemp, regAddr, &MSB, size)) != size)
	{
		fprintf(stderr, "READ ERROR: device=0x%x register address=0x%x\n", devicetemp.addr, regAddr);
		error = -3;
	}
	else
	{
		fprintf(stderr, "LOCAL=0x%x (%d)\n", MSB, MSB);
	}
	*LocalTempValue = (double)(MSB - 64 * TempMode);
	// fprintf(stderr, "LocalTemp=%f°C\n",*LocalTempValue);

	return error;
}

int ModuleCtrl::get_TempMode(int *tempMode)
{
	int regAddr, b;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;
	uint16_t value = 0;

	unsigned char MSB, LSB;

	// READ CONFIG REG
	regAddr = 0x03;
	if ((i2c_ioctl_read(&devicetemp, regAddr, &MSB, size)) != size)
	{
		fprintf(stderr, "READ ERROR: device=0x%x register address=0x%x\n", devicetemp.addr, regAddr);
		error = -3;
	}
	else
	{

		fprintf(stderr, "MSB=%x\n", MSB);
	}

	*tempMode = (MSB & 0x04) >> 2;

	return error;
}

int ModuleCtrl::set_TempMode(int tempMode)
{
	int regAddr, b;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	int error = 0;
	uint16_t value = 0;

	unsigned char MSB, LSB;
	//*buffer = ((value)&0xff00) >> 8;
	*buffer = 0x04 * tempMode;

	// devicepda.page_bytes = 8;
	//  WRITE CONFIG REG
	regAddr = 0x09;
	if ((i2c_ioctl_write(&devicetemp, regAddr, buffer, size)) != size)
	{
		fprintf(stderr, "WRITE ERROR: device=0x%x register address=0x%x\n", device.addr, regAddr);
		error = -3;
	}

	return error;
}
