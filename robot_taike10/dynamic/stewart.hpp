#ifndef AMC_DYNAMIC_STEWART_H_
#define AMC_DYNAMIC_STEWART_H_

#include "model_solver.hpp"

namespace amc::dynamic
{
	/// @defgroup dynamic_model_group 动力学建模模块
	/// @{
	///

	auto createModelStewart()->std::unique_ptr<amc::dynamic::Model>;

	class StewartInverseKinematicSolver :public amc::dynamic::InverseKinematicSolver
	{
	public:
		auto virtual allocateMemory()->void override;
		auto virtual kinPos()->bool override;

		virtual ~StewartInverseKinematicSolver() = default;
		explicit StewartInverseKinematicSolver(const std::string &name = "stewart_inverse_solver");
		AMC_REGISTER_TYPE(StewartInverseKinematicSolver);
		AMC_DECLARE_BIG_FOUR(StewartInverseKinematicSolver);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	///
	/// @}
}

#endif
