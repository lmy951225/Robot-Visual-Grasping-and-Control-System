/*
 * @Author: Lmy
 * @Date: 2022-11-07 14:15:19
 * @LastEditors: lmy mingyue.li@grandhonor.com
 * @LastEditTime: 2022-11-21 15:30:54
 * @FilePath: /socket/data/limingyue/RobotControlSystem_example/amc-1.5/test_taike/test_taike_dh.cpp
 * @Description: 对DH法建模的taike机械臂进行正逆运动学验证
 * 
 * Copyright (c) 2022 by GrandHonor, All Rights Reserved. 
 */
#include <taike_kin.h>
#include <math.h>
#include <stdio.h> 

using namespace std;
using namespace taike_kinematics;
const double PI = M_PI;

int main(int argc, char* argv[])
{
    
  double q[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double* T = new double[16];
  forward(q, T);
  printf("关节角为[%1.2f %1.2f %1.2f %1.2f %1.2f %1.2f]的末端位姿矩阵为:\n", q[0], q[1], q[2], q[3], q[4], q[5]);
  for(int i=0;i<4;i++)
   {
      for(int j=i*4;j<(i+1)*4;j++)
      {
          printf("%1.3f ", T[j]);
      }
      printf("\n");  
  }

  double q_sols[8*6];
  int num_sols;
  num_sols = inverse(T, q_sols,q);
  printf("上述末端位姿矩阵可反解求得的关节角度为:\n");
  for(int i=0;i<num_sols;i++) 
  {
    printf("第%d组解:\n", i+1);
    printf("%1.6f %1.6f %1.6f %1.6f %1.6f %1.6f\n", 
    q_sols[i*6+0], q_sols[i*6+1], q_sols[i*6+2], q_sols[i*6+3], q_sols[i*6+4], q_sols[i*6+5]);
  }
  printf("取最优解为:\n");
  printf("%1.6f %1.6f %1.6f %1.6f %1.6f %1.6f\n", 
    q[0], q[1], q[2], q[3], q[4], q[5]);
  
  return 0;
}
