#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include "command.h"

//AT串口交付的套接字
int fd_uart;
//shell交互套接字
int fd_shell;

//若是通过USB与模组直接通信，则创建与ttyUSB0的串口通信
int create_ttyUSB_socket(char *serial_port_device)
{
    struct termios options;
    // 打开串口设备
    int fd = open(serial_port_device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 配置串口属性
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    //每次串口能接收的字节数
    options.c_cc[VMIN] = 64;
    options.c_cc[VTIME] = 10;
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

//若是通过USB与模组直接通信，则创建与ttyUSB0的串口通信
int create_pts_socket(char *serial_port_device)
{
   int serial_port = open(serial_port_device, O_RDWR);

    if (serial_port < 0)
    {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return -1;
    }

    // Create new termios struct, we call it 'tty' for convention
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    // Read in existing settings, and handle any error
    if (tcgetattr(serial_port, &tty) != 0)
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity
    tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication
    tty.c_cflag &= ~CSIZE;         //  Clear all bits that set the data size
    tty.c_cflag |= CS8;            // 8 bits per byte
    tty.c_cflag &= ~020000000000;  // Disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore control lines

    tty.c_lflag &= ~ICANON; // Disable canonical mode (i.e., raw mode on)
    tty.c_lflag &= ~ECHO;   // Disable echo
    tty.c_lflag &= ~ECHOE;  // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP

    // Set in/out baud rate to be 9600 (you may need to change this according to your specific device)
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }
    return serial_port;
}

void* uart_handle(void* arg) 
{
    char read_buf[64];

    while (1)
    {
        memset(&read_buf, '\0', sizeof(read_buf));

        // Read available input bytes:
        int num_bytes = read(fd_uart, &read_buf, sizeof(read_buf));
        if (num_bytes < 0)
        {
            printf("Error reading: %s", strerror(errno));
        }
        else if (num_bytes > 0)
        {
            printf("Received cmd: %s\n", read_buf);
        }
        command_parse(read_buf);
    }
}

int uart_thread_create(char *serial_port)
{
    pthread_t thread_id;
    int result;

    fd_uart = create_ttyUSB_socket(serial_port);

    result = pthread_create(&thread_id, NULL, uart_handle, NULL);
    if (result != 0) {
        perror("Thread creation failed");
        return -1;
    }
}

void* shell_handle(void* arg) 
{
    char read_buf[64];

    while (1)
    {
        memset(&read_buf, '\0', sizeof(read_buf));

        // Read available input bytes:
        int num_bytes = read(fd_shell, &read_buf, sizeof(read_buf));
        if (num_bytes < 0)
        {
            printf("Error reading: %s", strerror(errno));
        }
        else if (num_bytes > 0)
        {
            printf("Received cmd: %s\n", read_buf);
        }
        command_parse(read_buf);
    }
}

int shell_thread_create(char *serial_port)
{
    pthread_t thread_id;
    int result;

    fd_shell = create_pts_socket(serial_port);
    result = pthread_create(&thread_id, NULL, shell_handle, NULL);
    if (result != 0) {
        perror("Thread creation failed");
        return -1;
    }
}

void* test_handle(void* arg) 
{
    test_main();
}

int test_thread_create()
{
    pthread_t thread_id;
    int result;

    result = pthread_create(&thread_id, NULL, test_handle, NULL);
    if (result != 0) {
        perror("Thread creation failed");
        return -1;
    }
}