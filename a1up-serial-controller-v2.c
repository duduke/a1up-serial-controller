#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <bitset>

using namespace std;

int main()
{
    // Open the serial port
    int fd = open("/dev/ttyS0", O_RDWR);
    if (fd == -1) {
        cerr << "Failed to open serial port" << endl;
        return 1;
    }

    // Configure the serial port
    termios tty;
    tcgetattr(fd, &tty);
    cfsetspeed(&tty, B115200);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &tty);

    while (true) {
        // Write command to serial port
        char cmd[] = {0xA6, 0x01, 0x00};
        write(fd, cmd, sizeof(cmd));

        // Wait for reply
        usleep(1563);

        // Read reply from serial port
        char reply[18];
        read(fd, reply, sizeof(reply));

        // Check reply format
        if (reply[0] != 0xA7 || reply[1] != 0x10) {
            cerr << "Invalid reply format" << endl;
            continue;
        }

        // Extract volume state
        int volume_state = reply[2];

        // Check 4th byte
        if (reply[3] != 0x01) {
            cerr << "Invalid 4th byte" << endl;
            continue;
        }

        // Extract bit arrays
        bitset<8> byte5(reply[4]);
        bitset<8> byte6(reply[5]);

        bitset<4> p1_start_button_state(byte5.to_string().substr(0, 4));
        bitset<4> p1_joystick_hat(byte5.to_string().substr(4, 4));

        bitset<5> p1_b1_b6(byte6.to_string().substr(3, 5));
        bitset<5> p1_b5_b2_b3_b4(byte6.to_string().substr(0, 5));

        // Print results
        cout << "Volume state: " << volume_state << endl;
        cout << "P1 start button state: " << p1_start_button_state.to_ulong() << endl;
        cout << "P1 joystick hat: " << p1_joystick_hat.to_ulong() << endl;
        cout << "P1 b1-b6: " << p1_b1_b6.to_ulong() << endl;
        cout << "P1 b5-b2-b3-b4: " << p1_b5_b2_b3_b4.to_ulong() << endl;

        // Wait for next cycle
        usleep(20000);
    }

    // Close the serial port
    close(fd);

    return 0;
}

