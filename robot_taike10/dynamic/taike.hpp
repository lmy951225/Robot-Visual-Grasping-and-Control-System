/*
 * @Author: Lmy
 * @Date: 2022-11-07 17:07:30
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2022-11-21 16:31:07
 * @FilePath: /robot_taike10/dynamic/taike.hpp
 * @Description: 泰科机械臂运动学逆解求解器代码
 * 
 * Copyright (c) 2022 by GrandHonor, All Rights Reserved. 
 */
#ifndef AMC_DYNAMIC_TAIKE_H_
#define AMC_DYNAMIC_TAIKE_H_

#include "model_solver.hpp"

namespace amc::dynamic
{
	
	/// @defgroup dynamic_model_group 动力学建模模块
	/// @{
	///
	int inverse(const double* T, double* q_sols, double *q);
	class Taike10InverseKinematicSolver :public amc::dynamic::InverseKinematicSolver
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto virtual kinPos()->bool override;
		auto setWhichRoot(int root_of_0_to_7)->void;
		

		virtual ~Taike10InverseKinematicSolver() = default;
		explicit Taike10InverseKinematicSolver(const std::string &name = "taike10_inverse_solver", amc::Size max_iter_count = 100, double max_error = 1e-10) :InverseKinematicSolver(name, max_iter_count, max_error) {}
		AMC_REGISTER_TYPE(Taike10InverseKinematicSolver);
		AMC_DEFINE_BIG_FOUR(Taike10InverseKinematicSolver);
		
	private:
		int which_root_{ 0 };
	};
	///
	/// @}
}

#endif
