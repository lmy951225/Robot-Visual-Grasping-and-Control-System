#ifndef AMC_DYNAMIC_UR_H_
#define AMC_DYNAMIC_UR_H_

#include "model_solver.hpp"

namespace amc::dynamic
{
	/// @defgroup dynamic_model_group 动力学建模模块
	/// @{
	///
	class Ur5InverseKinematicSolver :public amc::dynamic::InverseKinematicSolver
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto virtual kinPos()->bool override;
		auto setWhichRoot(int root_of_0_to_7)->void;

		virtual ~Ur5InverseKinematicSolver() = default;
		explicit Ur5InverseKinematicSolver(const std::string &name = "ur5_inverse_solver", amc::Size max_iter_count = 100, double max_error = 1e-10) :InverseKinematicSolver(name, max_iter_count, max_error) {}
		AMC_REGISTER_TYPE(Ur5InverseKinematicSolver);
		AMC_DEFINE_BIG_FOUR(Ur5InverseKinematicSolver);

	private:
		int which_root_{ 0 };
	};
	///
	/// @}
}

#endif
