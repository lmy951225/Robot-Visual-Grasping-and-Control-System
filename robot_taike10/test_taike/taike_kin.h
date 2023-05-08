/*
 * @Author: Lmy
 * @Date: 2022-11-07 13:49:45
 * @LastEditors: lmy mingyue.li@grandhonor.com
 * @LastEditTime: 2022-11-21 15:02:41
 * @FilePath: /socket/data/limingyue/RobotControlSystem_example/amc-1.5/taike_kin.h
 * @Description: 基于DH法的泰科10kg机械臂运动学建模
 * 
 * Copyright (c) 2022 by GrandHonor, All Rights Reserved. 
 */

#ifndef TAIKE_KIN_H
#define TAIKE_KIN_H

namespace taike_kinematics 
{
  // q       6个关节角度
  // T       4x4末端位姿矩阵
  void forward(const double* q, double* T);

  // q       6个关节角度
  // Ti      4x4各个关节位姿矩阵
  void forward_all(const double* q, double* T1, double* T2, double* T3, 
                                    double* T4, double* T5, double* T6);

  // T       4x4末端位姿矩阵
  // q_sols  8x6的数组，角度值范围为 [0,2*PI)
  // q6_des  可选参数，制定q6的角度反解值，理论上有无限解
  // 返回值        最多8组逆解
  int inverse(const double* T, double* q_sols, double *q);
};

#endif
