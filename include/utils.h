#ifndef _UTILS_H
#define _UTILS_H

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
extern int fd_uart;
//shell交互套接字
extern int fd_shell;

int uart_thread_create(char *serial_port);

int shell_thread_create(char *serial_port);

int test_thread_create();

#endif