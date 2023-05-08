/*
 * @Author: Lmy
 * @Date: 2022-11-09 15:48:12
 * @LastEditors: Lmy
 * @LastEditTime: 2022-11-09 15:55:54
 * @FilePath: /robot_taike10/robot/rokae.hpp
 * @Description: 
 * 
 * Copyright (c) 2022 by GrandHonor, All Rights Reserved. 
 */
#ifndef AMC_ROBOT_ROKAE_H_
#define AMC_ROBOT_ROKAE_H_

#include <memory>

#include <../control/control.hpp>
#include <../dynamic/dynamic.hpp>
#include <../plan/plan.hpp>

/// \brief 机器人命名空间
/// \ingroup amc
/// 
///
///
namespace amc::robot
{
	auto createModelRokaeXB4(const double *robot_pm = nullptr)->std::unique_ptr<amc::dynamic::Model>;
	auto createControllerRokaeXB4()->std::unique_ptr<amc::control::Controller>;
	auto createPlanRootRokaeXB4()->std::unique_ptr<amc::plan::PlanRoot>;
	auto createRokaeXB4Interface()->std::string;
}


#endif