/*
 * @Author: lmy mingyue.li@grandhonor.com
 * @Date: 2023-04-10 16:23:41
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-04-26 09:31:04
 * @FilePath: /amc-1.5/usb_read/serialTest.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "serialPort.hpp"
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
uint8_t buff1[10];
uint8_t buff2[8];
uint8_t buff[12];
uint16_t str1;
uint16_t str2;
char buf[128];
const char *dev  = "/dev/ttyUSB0";

void read_state(uint8_t buff[],serialPort myserial)
{
    int nread,nwrite;
    //cout<<"serialPort state_Test"<<endl;
    buff[0] = 0xEB;
    buff[1] = 0x90;
    buff[2] = 0x01;
    buff[3] = 0x01;
    buff[4] = 0x41;
    buff[5] = 0x43;
    //读取当前运行状态
    nwrite = myserial.writeBuffer(buff, 6);
    nread = myserial.readBuffer(buff, 13);
    for (int i = 0; i < 13; i++)
    {
        //printf("%#x",buff[i]);
        //printf(" ");
        switch (i)
        {
            case 5:
                if (buff[5] == 0x01)
                {
                    printf("当前工作状态: 夹爪张开到最大且空闲\n");
                }
                else if (buff[5] == 0x02)
                {
                    printf("当前工作状态: 夹爪闭合到最小且空闲\n");
                }
                else if (buff[5] == 0x03)
                {
                    printf("当前工作状态: 夹爪停止且空闲\n");
                }
                else if (buff[5] == 0x04)
                {
                    printf("当前工作状态: 夹爪正在闭合\n");
                }
                else if (buff[5] == 0x05)
                {
                    printf("当前工作状态: 夹爪正在张开\n");
                }
                else if (buff[5] == 0x06)
                {
                    printf("当前工作状态: 夹爪在闭合过程中遇到力控停止\n");
                }
                break;
            case 7:
                printf("当前工作温度为: %d度\n",buff[i]);
                break;
            case 9:
                str1 = buff[9];
                str1 = (str1 << 8);
                str1 |= buff[8];
                printf("当前夹爪开口度: %d\n",str1);
                break;
            case 11:
                str2 = buff[11];
                str2 = (str2 << 8);
                str2 |= buff[10];
                printf("当前夹爪夹持力: %d",str2);
                break;
            default:
                break;
        }
        
    }
    printf("\n"); 

}

void close(uint8_t buff[],serialPort myserial)
{
    int nread,nwrite;
    cout<<"serialPort close_Test"<<endl;
    buff[0] = 0xEB;
    buff[1] = 0x90;
    buff[2] = 0x01;
    buff[3] = 0x05;
    buff[4] = 0x10;
    buff[5] = 0xF4;
    buff[6] = 0x01;
    buff[7] = 0xF4;
    buff[8] = 0x01;
    buff[9] = 0x00;

    //抓取,速度500,力矩阈值500
    // 定时器 //
    auto begin_time = std::chrono::high_resolution_clock::now();
    nwrite = myserial.writeBuffer(buff, 10);
	auto time_elapsed1 = std::chrono::high_resolution_clock::now() - begin_time;
    nread = myserial.readBuffer(buff, 10);
    auto time_elapsed2 = std::chrono::high_resolution_clock::now() - begin_time;
    for (int i = 0; i < 7; i++)
    {
        printf("%#x",buff[i]);
        printf(" ");
    }
    printf("\n"); 
    read_state(buff,myserial);
    cout << "write time:" << time_elapsed1.count() / 1000.0 <<"(us)"<< endl;
    cout << "read and write time:" << time_elapsed2.count() / 1000.0 <<"(us)"<< endl;
    printf("\n"); 
}

void open(uint8_t buff[],serialPort myserial)
{
    int nread,nwrite;
    cout<<"serialPort open_Test"<<endl;
    buff[0] = 0xEB;
    buff[1] = 0x90;
    buff[2] = 0x01;
    buff[3] = 0x03;
    buff[4] = 0x11;
    buff[5] = 0xF4;
    buff[6] = 0x01;
    buff[7] = 0x0A;

    //松开
    // 定时器 //
    auto begin_time = std::chrono::high_resolution_clock::now();
    nwrite = myserial.writeBuffer(buff, 8);
    auto time_elapsed1 = std::chrono::high_resolution_clock::now() - begin_time;
    nread = myserial.readBuffer(buff, 8);
    auto time_elapsed2 = std::chrono::high_resolution_clock::now() - begin_time;
    for (int i = 0; i < 7; i++)
    {
        printf("%#x",buff[i]);
        printf(" ");
    }
    printf("\n"); 
    read_state(buff,myserial);
    cout << "write time:" << time_elapsed1.count() / 1000.0 <<"(us)"<< endl;
    cout << "read and write time:" << time_elapsed2.count() / 1000.0 <<"(us)"<< endl;
    printf("\n"); 
}

int main()
{ 
    serialPort myserial;
    myserial.OpenPort(dev);
    myserial.setup(115200,0,8,1,'N');

    //创建管道,实现机器人与夹抓的通信
	//判断有名管道文件是否存在
	int ret1 = access("fifo",F_OK);
	if (ret1 == -1)
	{
		printf("管道文件不存在，创建对应的有名管道\n");
		ret1 = mkfifo("fifo",0664);
		if (ret1 == -1)
		{
			perror("mkfifo");
			exit(0);
		}
		
	}

	// 以只读的方式打开管道fifo
	int fdr = open("fifo",O_RDONLY);
	if (fdr == -1)
	{
		perror("open");
		exit(0);
	}
	printf("打开管道fifo成功,等待读取机器人运行指令...\n");
    
    
    while (1)
    {
        //读取指令
        memset(buf,0,128);
        fgets(buf,128,stdin);
        ret1 = read(fdr,buf,128);
        if (ret1 <= 0)
        {
            perror("read");
            break;
        }
        printf("接收到指令，开始执行对应操作...\n");
        //cout << strlen(buf) << endl;
        if (strlen(buf) == 6)
        {
            close(buff1,myserial);
        }
        else if (strlen(buf) == 5)
        {
            open(buff2,myserial);
        }
        else if (strlen(buf) == 4)
        {
            read_state(buff,myserial);
        }
        
    }
    close(fdr);

/*
    //抓取
    close(buff1,myserial);
    sleep(2);//休眠2秒
    //read_state(buff,myserial);
    //松开
    open(buff2,myserial);
    sleep(2);
    read_state(buff,myserial);
*/
    return 0;
}
