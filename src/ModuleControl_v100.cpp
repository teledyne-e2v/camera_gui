#include "ModuleControl_v100.hpp"

#include <unistd.h>

#define SDTBY 0x100

/**RGU TESTS**/

/************************************************
 *Main fuction
 ************************************************/

ModuleCtrl::~ModuleCtrl()
{
#ifndef DEBUG_MODE
	ModuleControlClose(); // close IC2
#endif
}

void ModuleCtrl::ModuleControlInit()
{

	/* Open i2c bus */

	if ((bus = i2c_open(bus_name)) == -1)
	{

		fprintf(stderr, "Open i2c bus:%s error!\n", bus_name);
		exit(-3);
	}
	printf("Bus %s open\n", bus_name);

	/* Init i2c device */

	memset(&device, 0, sizeof(device));
	i2c_init_device(&device);

	device.bus = bus;
	/*device address*/
	device.addr = 0x3D;
	/*Unknown value*/
	device.page_bytes = 256;
	/*Address length in bytes*/
	device.iaddr_bytes = 1;

	/* Init i2c devicepda */

	memset(&devicepda, 0, sizeof(devicepda));
	i2c_init_device(&devicepda);

	devicepda.bus = bus;
	/*device address*/
	devicepda.addr = 0x0C;
	/*Unknown value*/
	devicepda.page_bytes = 8;
	/*Address length in bytes*/
	devicepda.iaddr_bytes = 1;

	// Load driver
	// system("echo jet | sudo -S insmod ~/Driver/snappy.ko");
	#ifdef LOAD_DRIVER
	int attempt = 5;
	bool inStandBy = false;

	while (!inStandBy && attempt > 0)
	{
		
		system("sudo -S insmod ~/Driver/snappy.ko");

		// Load pixel timing
		writeTP();
		printf("\n");
		sleep(1);

		inStandBy = (read_sensor_state() == SDTBY);

		if (!inStandBy)
		{
			printf("The sensor is not in standby. Reloading driver...\n\n");
			system("sudo -S rmmod snappy");
			usleep(500000);
			attempt--;
		}

	}

	if (attempt == 0 && !inStandBy)
	{
		printf("Error: unable to properly initialize the sensor\nThe sensor is not in standby\n\tAborting\n");
		exit(EXIT_FAILURE);
	}

	printf("Snappy Module driver loaded\n");
	#endif
	// Enable PDA50 DAC
	printf("Enable PDA50 DAC");
	enable_VdacPda(devicepda, bus);
}

void ModuleCtrl::ModuleControlClose()

{
	// Desable PDA50 DAC
	printf("Desable PDA50 DAC\n");
	desable_VdacPda(devicepda, bus);

	// Please adjust the password (jet) and the product name
	#ifdef LOAD_DRIVER
	system("echo jet | sudo -S rmmod snappy");
	printf("Snappy Module driver unloaded\n");
	#endif
	/* Close de bus*/
	i2c_close(bus);

	printf("Bus %s closed\n", bus_name);
}

int ModuleCtrl::read_sensor_state()
{
	device.page_bytes = 256;

	// PARAMETERS
	unsigned long ulAddress;
	unsigned long ulValue;
	size_t iSize;
	int nError;
	unsigned char buffer[2];

	ulAddress = 0x0074;
	iSize = 2;
	ulValue = 0;
	nError = i2c_ioctl_read(&device,ulAddress,buffer,iSize);
	ulValue = (*buffer<<8) + *(buffer+1);

	return ulValue;
}

int ModuleCtrl::setTint(float b)
{
	int registre, line;
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	struct solution init;

	memset(buffer, 0, size);
	device.page_bytes = 256;

	b = b * 1000000; // convert to nanoseconds
	b = b / 20;

	// read the line length

	registre = 0x06;
	if ((i2c_ioctl_read(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}

	line = (*buffer << 8) + *(buffer + 1);

	init.tintII = (int)b / line;
	init.tintclk = (int)(b - init.tintII * line);

	// write in @0x16

	registre = 0x16;
	*buffer = ((init.tintII) & 0xff00) >> 8;
	*(buffer + 1) = (init.tintII & 0x00ff);

	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}

	// write in @0x17

	registre = 0x17;
	*buffer = ((init.tintclk) & 0xff00) >> 8;
	*(buffer + 1) = (init.tintclk & 0x00ff);

	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}

	return 0;
}

/************************************************
 *Set the analog gain
 ************************************************/

int ModuleCtrl::setAnalogGain(float b)
{
	int registre, b1;
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	registre = 0x18;
	device.page_bytes = 256;

	b1 = (int)(b * 10);

	switch (b1)
	{
	case 10:
		b1 = 2;
		break;
	case 15:
		b1 = 3;
		break;
	case 20:
		b1 = 4;
		break;
	case 30:
		b1 = 6;
		break;
	case 40:
		b1 = 8;
		break;
	case 60:
		b1 = 12;
		break;
	case 80:
		b1 = 14;
		break;
	default:
		printf("Forbidden value, couldn't set analog gain\n");
		return 0;
	}

	if ((i2c_ioctl_read(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", registre);
		i2c_close(bus);
		return -3;
	}

	*(buffer + 1) = (*(buffer + 1) & 0xf0) + b1;

	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		return -3;
	}
	return 0;
}

/************************************************
 *Read register
 ************************************************/

int ModuleCtrl::readReg(int registre, int *value)
{
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	device.page_bytes = 256;
	buffer[0] = 1;
	buffer[1] = 2;
	if ((i2c_ioctl_read(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}
	// printf("@0x%x -> %02x%02x\n", registre, *buffer, *(buffer+1));
	*value = buffer[0] * 256 + buffer[1];

	return 0;
}

int ModuleCtrl::writeReg(int registre, int value)
{
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	*buffer = ((value)&0xff00) >> 8;
	*(buffer + 1) = ((value)&0x00ff);

	device.page_bytes = 256;

	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}
	return 0;
}

/************************************************
 *Set the digital gain
 ************************************************/

int ModuleCtrl::setDigitalGain(float b1)
{
	int registre, b;
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	registre = 0x18;
	device.page_bytes = 3;

	// printf("Set digital gain (decimal ex: x1=256, x1.5=384...)\nValue: ");
	// scanf("%f",&b1);
	b = (int)(b1 * 256);

	if (b > 4095)
		b = 4095;
	if (b < 1)
		b = 1;

	// printf("effective gain = %f\n", (float) b /256);

	if ((i2c_ioctl_read(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}

	*buffer = ((b << 4) & 0xff00) >> 8;
	*(buffer + 1) = ((b << 4) & 0x00ff) + (*(buffer + 1) & 0x0f);

	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		return -3;
	}
	return 0;
}

int enable_VdacPda(I2CDevice device, int bus)
{
	int registre;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	// SET CFG_ENABLE = 1
	*buffer = 0x01;
	registre = 0x00;
	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}
	return 0;
}
int desable_VdacPda(I2CDevice device, int bus)
{
	int registre;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	// SET CFG_ENABLE = 0
	*buffer = 0x00;
	registre = 0x00;
	if ((i2c_ioctl_write(&device, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}
	return 0;
}

int ModuleCtrl::write_VdacPda(int PdaRegValue)
{
	int registre;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

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
	registre = 0x02;
	*buffer = MSB;
	if ((i2c_ioctl_write(&devicepda, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}
	// dumpPda50Reg(devicepda, bus);
	//  WRITE LSB
	registre = 0x03;
	*buffer = LSB;
	if ((i2c_ioctl_write(&devicepda, registre, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}
	// dumpPda50Reg(devicepda, bus);
	return 0;
}

int ModuleCtrl::read_VdacPda(int *PdaRegValue, double *PdaVoltageValue)
{

	int registre, b;
	unsigned char buffer[1];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);

	devicepda.page_bytes = 1;

	unsigned char MSB, LSB;
	// READ MSB
	registre = 0x02;
	if ((i2c_ioctl_read(&devicepda, registre, &MSB, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
	}

	// READ LSB
	registre = 0x03;
	*buffer = LSB;
	if ((i2c_ioctl_read(&devicepda, registre, &LSB, size)) != size)
	{
		fprintf(stderr, "Can't read %x reg!\n", registre);
		i2c_close(bus);
		exit(-3);
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
	return 0;
}
