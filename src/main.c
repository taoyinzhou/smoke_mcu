#include "utils.h"

//需要用户自己实现发送函数
void uart_puts(char *send_data)
{
    // 向串口发送数据
    int send_len = strlen(send_data);
    int ret = write(fd_uart, send_data, send_len);
    if (ret == -1) {
        perror("write error");
    }
}

int main(int argc, char **argv) 
{
    printf("main start\n");

    if(argc != 3)
    {
        printf("param error\n");
    }

    // 创建线程，用于接收烟感AT指令
    uart_thread_create(argv[1]);

    // 创建线程，用于接收shell指令
    shell_thread_create(argv[2]);

    //创建线程，用于测试
    test_thread_create();

    while(1)
    {
        attribute_change_detect_handler(1);
        sleep(1);
    }

    return 0;
}
