#ifndef AMC_ROBOT_UR_H_
#define AMC_ROBOT_UR_H_

#include <memory>

#include "../dynamic/dynamic.hpp"
#include "../control/control.hpp"
#include "../plan/plan.hpp"

/// \brief 机器人命名空间
/// \ingroup amc
/// 
///
///
namespace amc::robot
{
	auto createModelUr5(const double *robot_pm = nullptr)->std::unique_ptr<amc::dynamic::Model>;
	auto createControllerUr5()->std::unique_ptr<amc::control::Controller>;
	auto createPlanRootUr5()->std::unique_ptr<amc::plan::PlanRoot>;
}


#endif
