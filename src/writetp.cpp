#include "writetp.hpp"

int loadTimingPixel(__u16 pixelData[])
{
	FILE *fic;
	int i = 0;

	// Open the timing pixel file
	char nomFichier[] = SENSOR_TIMING_PIXEL_FILENAME;
	fic = fopen(nomFichier, "rb");
	if (fic == NULL)
	{
		printf("ERROR: Pixel timing binary file not found\n");
		return -1;
	}

	printf("Pixel timing binary file opened\n");

	// skip the header
	while (i < SENSOR_TIMING_PIXEL_HEADER_SIZE / 2)
	{
		fread(&pixelData[0], 2, 1, fic);
		i++;
	}

	// read the data
	i = 0;
	while ((i < SENSOR_TIMING_PIXEL_DATA_SIZE / 2) && (!feof(fic)))
	{
		fread(&pixelData[i], 2, 1, fic);
		i++;
	}
	fclose(fic);
	printf("Pixel timing successfully loaded\n");
	return 0;
}

void writeTP()
{
	unsigned char buffer[2];
	ssize_t size = sizeof(buffer);
	memset(buffer, 0, size);
	__u16 pucTimingPixelData[SENSOR_TIMING_PIXEL_DATA_SIZE / WORD_SIZE];

	int reg_addr1 = 0x4f; //<-put the address of the device register you want to test
	int reg_addr2 = 0x50; //<-put the address of the device register you want to test
	// int reg_addr3 = 0x51; //<-put the address of the device register you want to test
	int reg;

	/* Open i2c bus */
	int bus;
	char bus_name[32] = "/dev/i2c-6"; //<--bus 0
	if ((bus = i2c_open(bus_name)) == -1)
	{

		fprintf(stderr, "Open i2c bus:%s error!\n", bus_name);
		exit(-3);
	}
	printf("Bus %s open\n", bus_name);

	/* Init i2c device */
	I2CDevice device;
	memset(&device, 0, sizeof(device));
	i2c_init_device(&device);

	device.bus = bus;
	device.addr = 0x3D;		 //<-put the address of the device (found with i2cdetect command)
	device.page_bytes = 256; //<-i don't what is this
	device.iaddr_bytes = 1;	 //<-address length is the device (bytes)
	int i = 0;

	// write 0 in 0x01 to do a soft reset
	reg = 0x01;
	*buffer = 0x00;
	*(buffer + 1) = 0x00;
	// device.page_bytes = 4;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x0015 in 0x05
	reg = 0x05;
	*buffer = 0x00;
	*(buffer + 1) = 0x15;
	// device.page_bytes = 4;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x02ad in 0x16
	reg = 0x16;
	*buffer = 0x02;
	*(buffer + 1) = 0xad;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x1002 in 0x18
	reg = 0x18;
	*buffer = 0x10;
	*(buffer + 1) = 0x02;
	// device.page_bytes = 3;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x02da in 0x06
	reg = 0x06;
	*buffer = 0x02;
	*(buffer + 1) = 0xda;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x01f4 in 0x17
	reg = 0x17;
	*buffer = 0x01;
	*(buffer + 1) = 0xf4;
	// device.page_bytes = 5;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x0C08 in 0xD7
	reg = 0xD7;
	*buffer = 0x0c;
	*(buffer + 1) = 0x08;
	// device.page_bytes = 5;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x0008 in 0x1C
	reg = 0x1c;
	*buffer = 0x00;
	*(buffer + 1) = 0x08;
	// device.page_bytes = 3;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x9000 in 0x34
	reg = 0x34;
	*buffer = 0x90;
	*(buffer + 1) = 0x00;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x092c in 0x86
	reg = 0x86;
	*buffer = 0x09;
	*(buffer + 1) = 0x2c;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x0011 in 0x88
	reg = 0x88;
	*buffer = 0x00;
	*(buffer + 1) = 0x11;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0xEBF2 in 0x89
	reg = 0x89;
	*buffer = 0xEB;
	*(buffer + 1) = 0xF2;
	// device.page_bytes = 5;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x10aa in 0x8a
	reg = 0x8a;
	*buffer = 0x10;
	*(buffer + 1) = 0xaa;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x2660 in 0x8B
	reg = 0x8b;
	*buffer = 0x26;
	*(buffer + 1) = 0x60;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x62fc in 0x8c
	reg = 0x8c;
	*buffer = 0x62;
	*(buffer + 1) = 0xfc;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x03E8 in 0xBA
	reg = 0xBA;
	*buffer = 0x03;
	*(buffer + 1) = 0xE8;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x0161 in 0xBB
	reg = 0xbb;
	*buffer = 0x01;
	*(buffer + 1) = 0x61;
	// device.page_bytes = 3;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x3820 in 0xBd
	reg = 0xbd;
	*buffer = 0x38;
	*(buffer + 1) = 0x20;
	// device.page_bytes = 3;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x1304 in 0xc7
	reg = 0xc7;
	*buffer = 0x13;
	*(buffer + 1) = 0x04;
	// device.page_bytes = 3;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}
	// write 0x000a in 0xd8
	reg = 0xd8;
	*buffer = 0x00;
	*(buffer + 1) = 0x0a;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x0006 in 0xd9
	reg = 0xd9;
	*buffer = 0x00;
	*(buffer + 1) = 0x06;
	// device.page_bytes = 4;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// write 0x001c in 0xda
	reg = 0xda;
	*buffer = 0x00;
	*(buffer + 1) = 0x1c;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}
	// write 0x0041 in 0xdB
	reg = 0xdb;
	*buffer = 0x00;
	*(buffer + 1) = 0x41;
	// device.page_bytes = 3;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}
	// write 0x0140 in 0xdc
	reg = 0xdc;
	*buffer = 0x01;
	*(buffer + 1) = 0x40;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}
	// write 0x0161 in 0xdd
	reg = 0xdd;
	*buffer = 0x03;
	*(buffer + 1) = 0x98;
	// device.page_bytes = 4;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}
	// write 0x03ff in 0xde
	reg = 0xde;
	*buffer = 0x03;
	*(buffer + 1) = 0xff;
	// device.page_bytes = 2;
	if ((i2c_ioctl_write(&device, reg, buffer, size)) != size)
	{
		fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
		i2c_close(bus);
		exit(-3);
	}

	// load timing pixel from file
	if (loadTimingPixel(pucTimingPixelData) != 0)
	{
		return;
	}

	// writing timing pixel
	while (i <= 0x97)
	{
		// printf("%x -> %x\n", i, pucTimingPixelData[i]);
		*buffer = 0x00;
		*(buffer + 1) = i;
		// device.page_bytes = 3;

		// write i in 0x4F
		if ((i2c_ioctl_write(&device, reg_addr1, buffer, size)) != size)
		{
			fprintf(stderr, "Can't write in %x reg!\n", reg_addr1);
			i2c_close(bus);
			exit(-3);
		}

		*buffer = (pucTimingPixelData[i] & 0xFF00) >> 8;
		*(buffer + 1) = (pucTimingPixelData[i] & 0x00FF);
		// printf("%x : 0x%04x\n", i, pucTimingPixelData[i]);

		// write in 0x50
		// device.page_bytes = 4;/*_________________________________________________*/

		if ((i2c_ioctl_write(&device, reg_addr2, buffer, size)) != size)
		{
			fprintf(stderr, "Can't write in %x reg!\n", reg_addr2);
			i2c_close(bus);
			exit(-3);
		}
		i++;
	}

	/* Close de bus*/
	i2c_close(bus);

	printf("Bus %s closed\n", bus_name);
}
