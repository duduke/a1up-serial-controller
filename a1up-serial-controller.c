#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main() {
  int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
  if (fd == -1) {
    std::cerr << "Failed to open serial port" << std::endl;
    return 1;
  }

  // Configure serial port settings
  termios tty;
  memset(&tty, 0, sizeof(tty));
  cfsetospeed(&tty, B115200);
  cfsetispeed(&tty, B115200);
  tty.c_cflag |= (CLOCAL | CREAD);
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_cc[VMIN] = 18;
  tty.c_cc[VTIME] = 0;
  tcsetattr(fd, TCSANOW, &tty);

  // Main loop
  while (true) {
    // Send command
    unsigned char cmd[] = {0xA6, 0x01, 0x00};
    write(fd, cmd, sizeof(cmd));

    // Wait for response
    usleep(1563);
    unsigned char resp[18];
    read(fd, resp, sizeof(resp));

    // Check response format
    if (resp[0] != 0xA7 || resp[1] != 0x10 || resp[3] != 0x01) {
      std::cerr << "Invalid response format" << std::endl;
      continue;
    }

    // Parse response fields
    unsigned char volume_state = resp[2];
    unsigned char p1_start_button_state = (resp[4] >> 4) & 0x0F;
    unsigned char p1_joystick_hat = resp[4] & 0x0F;

    // Print parsed values
    std::cout << "volume_state=" << (int)volume_state << std::endl;
    std::cout << "p1_start_button_state=" << (int)p1_start_button_state << std::endl;
    std::cout << "p1_joystick_hat=" << (int)p1_joystick_hat << std::endl;

    // Wait for next iteration
    usleep(20000);
  }

  close(fd);
  return 0;
}

