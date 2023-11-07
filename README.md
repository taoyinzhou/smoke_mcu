# Linux程序模拟测试思维烟感协议

1. 安装socat
`sudo apt install socat``
2. 创建虚拟串口对
打开第一个窗口运行
`socat -d -d PTY PTY`
会显示创建的串口号：
```
ubuntu@guoyuchao-vm:~/dev/smoke_test$ socat -d -d PTY PTY
2023/10/27 10:40:41 socat[44920] N PTY is /dev/pts/1
2023/10/27 10:40:41 socat[44920] N PTY is /dev/pts/2
2023/10/27 10:40:41 socat[44920] N starting data transfer loop with 
```
3. 运行程序
打开第二个窗口运行
```
./run.sh /dev/pts/1
```

4. echo测试
打开第三个串口运行
```
//第一个参数是object_id
//第二个参数是属性编号
//第三个参数是设置属性值
//举例：设置object_id为1的ExpressedState的值为1
set 1 1 1 > /dev/pts/9
```