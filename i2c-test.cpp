#include <iostream>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>

struct Bmp180_Calibration
{
        uint16_t AC1;
        uint16_t AC2;
        uint16_t AC3;
        uint16_t AC4;
        uint16_t AC5;
        uint16_t AC6;
        uint16_t B1;
        uint16_t B2;
        uint16_t MB;
        uint16_t MC;
        uint16_t MD;
}bmp180_Calibration;

const int16_t BMP085_CAL_AC1 = 0xAA;
const int16_t BMP085_CAL_AC2 = 0xAC;
const int16_t BMP085_CAL_AC3 = 0xAE;
const uint16_t BMP085_CAL_AC4 = 0xB0;
const uint16_t BMP085_CAL_AC5 = 0xB2;
const uint16_t BMP085_CAL_AC6 = 0xB4;
const int16_t BMP085_CAL_B1 = 0xB6;
const int16_t BMP085_CAL_B2 = 0xB8;
const int16_t BMP085_CAL_MB = 0xBA;
const int16_t BMP085_CAL_MC = 0xBC;
const int16_t BMP085_CAL_MD = 0xBE;
const int16_t BMP085_CONTROL = 0xF4;
const int16_t BMP085_READTEMPCMD = 0x2E;
const int16_t BMP085_READPRESSURECMD = 0x34;
const int16_t BMP085_TEMPDATA = 0xF6;
const int16_t BMP085_PRESSUREDATA = 0xF6;
const int16_t BMP085_ULTRALOWPOWER = 0;
const int16_t BMP085_STANDARD = 1;
const int16_t BMP085_HIGHRES = 2;
const int16_t BMP085_ULTRAHIGHRES = 3;


int main() {
    using std::cout;
    using std::cerr;

    const int8_t addr = 0x77;

    // open the library
    void* handle = dlopen("./i2c-dotnet.so", RTLD_LAZY);
    
    if (!handle) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }
    
    // load the symbol
    typedef void (*reg_read_byte_t)(int , uint8_t , uint8_t , uint8_t* );
    typedef void (*reg_read_short_t)(int , uint8_t , uint8_t , uint16_t* );
    typedef void (*reg_read_24_t)(int , uint8_t , uint8_t , int32_t* );
    typedef void (*reg_write_byte_t)(int , uint8_t , uint8_t , uint8_t );
    typedef void (*reg_write_bytes_t)(int , uint8_t , int16_t*, int);

    // reg_read_byte
    dlerror();
    reg_read_byte_t reg_read_byte = (reg_read_byte_t) dlsym(handle, "reg_read_byte");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'reg_read_byte': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }

    // reg_read_short
    dlerror();
    reg_read_short_t reg_read_short = (reg_read_short_t) dlsym(handle, "reg_read_short");
    dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'reg_read_short': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }

    // reg_read_24_t
    dlerror();
    reg_read_24_t reg_read_24 = (reg_read_24_t) dlsym(handle, "reg_read_24");
    dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'reg_read_24': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }
    
    // reg_write_byte_t
    dlerror();
    reg_write_byte_t reg_write_byte = (reg_write_byte_t) dlsym(handle, "reg_write_byte");
    dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'reg_write_byte': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }

    // reg_write_bytes_t
    dlerror();
    reg_write_bytes_t reg_write_bytes = (reg_write_bytes_t) dlsym(handle, "reg_write_bytes");
    dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Cannot load symbol 'reg_write_bytes': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }

    const char* device = "/dev/i2c-1";
	int fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("Failed to open device");
		return -1;
    }

    // Calib
    reg_read_short(fd, addr, BMP085_CAL_AC1, &bmp180_Calibration.AC1);
    reg_read_short(fd, addr, BMP085_CAL_AC2, &bmp180_Calibration.AC2);
    reg_read_short(fd, addr, BMP085_CAL_AC3, &bmp180_Calibration.AC3);
    reg_read_short(fd, addr, BMP085_CAL_AC4, &bmp180_Calibration.AC4);
    reg_read_short(fd, addr, BMP085_CAL_AC5, &bmp180_Calibration.AC5);
    reg_read_short(fd, addr, BMP085_CAL_AC6, &bmp180_Calibration.AC6);
    reg_read_short(fd, addr, BMP085_CAL_B1, &bmp180_Calibration.B1);
    reg_read_short(fd, addr, BMP085_CAL_B2, &bmp180_Calibration.B2);
    reg_read_short(fd, addr, BMP085_CAL_MB, &bmp180_Calibration.MB);
    reg_read_short(fd, addr, BMP085_CAL_MC, &bmp180_Calibration.MC);
    reg_read_short(fd, addr, BMP085_CAL_MD, &bmp180_Calibration.MD);


    // ReadRawTemperature
    uint16_t raw = 0;
    reg_write_byte(fd, addr, BMP085_CONTROL, BMP085_READTEMPCMD);
    reg_read_short(fd, addr, BMP085_TEMPDATA, &raw);

    // Read Temp
    int16_t UT = (int16_t)raw;
    int X1 = (UT - bmp180_Calibration.AC6) * bmp180_Calibration.AC5 / (1 << 15);
    int X2 = (int16_t)bmp180_Calibration.MC * (1 << 11) / (X1 + bmp180_Calibration.MD);
    int B5 = (X1 + X2);
    int T = (B5 + 8) / (1 << 4);
        
    cout << "Temp:" << (float)T/(float)10 << " *c\n";

    // close the library
    close(fd);
    dlclose(handle);
}
