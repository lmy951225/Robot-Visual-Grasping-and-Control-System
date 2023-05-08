﻿#ifndef AMC_DYNAMIC_MODEL_SIMULATION_H_
#define AMC_DYNAMIC_MODEL_SIMULATION_H_

#include "model_interaction.hpp"
#include "model_solver.hpp"


namespace amc::plan
{
	struct PlanTarget;
	class Plan;
}

namespace amc::dynamic
{
	/// @defgroup dynamic_model_group 动力学建模模块
	/// @{
	///

	class SimResult : public Element
	{
	public:
		class TimeResult : public Element
		{
		public:
			auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
			auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
			auto record()->void;
			auto restore(Size pos)->void;

			virtual ~TimeResult();
			explicit TimeResult(const std::string &name = "time_result");
			AMC_REGISTER_TYPE(TimeResult);
			AMC_DECLARE_BIG_FOUR(TimeResult);

		private:
			struct Imp;
			amc::core::ImpPtr<Imp> imp_;

			friend class SimResult;
		};
		class PartResult : public Element
		{
		public:
			auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
			auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
			auto part()->Part&;
			auto part()const->const Part& { return const_cast<PartResult*>(this)->part(); }
			auto record()->void;
			auto restore(Size pos)->void;

			virtual ~PartResult();
			explicit PartResult(const std::string &name = "part_result", Part *part = nullptr);
			AMC_REGISTER_TYPE(PartResult);
			AMC_DECLARE_BIG_FOUR(PartResult);

		private:
			struct Imp;
			amc::core::ImpPtr<Imp> imp_;

			friend class SimResult;
		};
		class ConstraintResult : public Element
		{
		public:
			auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
			auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
			auto constraint()->Constraint&;
			auto constraint()const->const Constraint& { return const_cast<ConstraintResult*>(this)->constraint(); }
			auto record()->void;
			auto restore(Size pos)->void;

			virtual ~ConstraintResult();
			explicit ConstraintResult(const std::string &name = "constraint_result", Constraint *constraint = nullptr);
			AMC_REGISTER_TYPE(ConstraintResult);
			AMC_DECLARE_BIG_FOUR(ConstraintResult);

		private:
			struct Imp;
			amc::core::ImpPtr<Imp> imp_;

			friend class SimResult;
		};

		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto timeResult()->TimeResult&;
		auto timeResult()const->const TimeResult& { return const_cast<SimResult*>(this)->timeResult(); }
		auto partResultPool()->amc::core::ObjectPool<PartResult, Element>&;
		auto partResultPool()const->const amc::core::ObjectPool<PartResult, Element>& { return const_cast<SimResult*>(this)->partResultPool(); };
		auto constraintResultPool()->amc::core::ObjectPool<ConstraintResult, Element>&;
		auto constraintResultPool()const->const amc::core::ObjectPool<ConstraintResult, Element>& { return const_cast<SimResult*>(this)->constraintResultPool(); };

		auto allocateMemory()->void;
		auto record()->void;
		auto restore(Size pos)->void;
		auto size()const->Size;
		auto clear()->void;

		virtual ~SimResult();
		explicit SimResult(const std::string &name = "sim_result");
		AMC_REGISTER_TYPE(SimResult);
		AMC_DECLARE_BIG_FOUR(SimResult);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class Simulator :public Element
	{
	public:
		auto virtual simulate(amc::plan::Plan &plan, SimResult &result)->void;

		virtual ~Simulator();
		explicit Simulator(const std::string &name = "simulator");
		AMC_REGISTER_TYPE(Simulator);
		AMC_DECLARE_BIG_FOUR(Simulator);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class SolverSimulator : public Simulator
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto virtual simulate(amc::plan::Plan &plan, SimResult &result)->void override;
		using Simulator::simulate;
		auto solver()->Solver&;
		auto solver()const ->const Solver& { return const_cast<SolverSimulator*>(this)->solver(); };

		virtual ~SolverSimulator();
		explicit SolverSimulator(const std::string &name = "solver_simulator", Solver *solver = nullptr);
		AMC_REGISTER_TYPE(SolverSimulator);
		AMC_DECLARE_BIG_FOUR(SolverSimulator);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class AdamsSimulator :public Simulator
	{
	public:
		auto saveAdams(const std::string &filename, SimResult &result, Size pos = -1)->void;
		auto saveAdams(std::ofstream &file, SimResult &result, Size pos = -1)->void;
		auto saveAdams(const std::string &filename)->void;
		auto saveAdams(std::ofstream &file)->void;
		auto adamsID(const Marker &mak)const->Size;
		auto adamsID(const Part &prt)const->Size;
		auto adamsID(const Element &ele)const->Size { return ele.id() + 1; };

		virtual ~AdamsSimulator();
		explicit AdamsSimulator(const std::string &name = "adams_solver");
		AMC_REGISTER_TYPE(AdamsSimulator);
		AMC_DECLARE_BIG_FOUR(AdamsSimulator);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};

	class Calibrator :public Element
	{
	public:
		auto virtual allocateMemory()->void;
		auto m()->Size;
		auto n()->Size { return g() + k(); }
		auto g()->Size;
		auto k()->Size;
		auto A()->double*;
		auto x()->double*;
		auto b()->double*;
		auto clb()->void;
		auto clbFile(const std::string &file_paths)->void;
		auto clbFiles(const std::vector<std::string> &file_paths)->void;
		auto verifyFiles(const std::vector<std::string> &file_paths)->void;
		auto updateInertiaParam(const double *inetia_param)->void;

		virtual ~Calibrator();
		explicit Calibrator(const std::string &name = "calibrator");
		AMC_REGISTER_TYPE(Calibrator);
		AMC_DECLARE_BIG_FOUR(Calibrator);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};

	/// @}
}

#endif
