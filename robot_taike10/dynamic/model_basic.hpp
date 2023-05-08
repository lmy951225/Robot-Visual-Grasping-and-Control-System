#ifndef AMC_DYNAMIC_MODEL_BASIC_H_
#define AMC_DYNAMIC_MODEL_BASIC_H_

#include "../core/object.hpp"
#include "../core/expression_calculator.hpp"
#include "screw.hpp"

namespace amc::dynamic
{
	using double6x6 = double[6][6];
	using double4x4 = double[4][4];
	using double3 = double[3];
	using double6 = double[6];
	using double7 = double[7];
	using double10 = double[10];

	/// @defgroup dynamic_model_group 动力学建模模块
	/// @{
	///

	class Model;

	class Element :public amc::core::Object
	{
	public:
		auto model()noexcept->Model& { return *ancestor<Model>(); }
		auto model()const noexcept->const Model& { return const_cast<std::decay_t<decltype(*this)> *>(this)->model(); }
		auto attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name)const->amc::core::Matrix;
		auto attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name, const amc::core::Matrix& default_value)const->amc::core::Matrix;
		auto attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name, Size m, Size n)const->amc::core::Matrix;
		auto attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name, Size m, Size n, const amc::core::Matrix& default_value)const->amc::core::Matrix;

		~Element() = default;
		explicit Element(const std::string &name = "element") :Object(name) {}
		AMC_REGISTER_TYPE(Element);
		AMC_DEFINE_BIG_FOUR(Element);
	};
	class DynEle : public Element
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto active() const noexcept->bool { return active_; }
		auto activate(bool active = true)noexcept->void { active_ = active; }

		virtual ~DynEle() = default;
		explicit DynEle(const std::string &name, bool active = true) : Element(name), active_(active) {};
		AMC_REGISTER_TYPE(DynEle);
		AMC_DEFINE_BIG_FOUR(DynEle);

	private:
		bool active_;
	};

	class Environment final :public Element
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto gravity()const noexcept->const double6& { return gravity_; }
		auto setGravity(const double *gravity)noexcept->void { s_vc(6, gravity, gravity_); }

		virtual ~Environment() = default;
		explicit Environment(const std::string &name = "dyn_ele") :Element(name) {}
		AMC_REGISTER_TYPE(Environment);
		AMC_DEFINE_BIG_FOUR(Environment);

	private:
		double gravity_[6]{ 0, -9.8, 0, 0, 0, 0 };
	};

	class Variable :public Element
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual toString() const->std::string { return ""; }

		virtual ~Variable() = default;
		explicit Variable(const std::string &name = "variable") : Element(name) {}
		AMC_REGISTER_TYPE(Variable);
		AMC_DEFINE_BIG_FOUR(Variable);
	};
	template<typename VariableType> class VariableTemplate : public Variable
	{
	public:
		auto data()->VariableType& { return data_; }
		auto data()const->const VariableType& { return data_; }

		virtual ~VariableTemplate() = default;
		explicit VariableTemplate(const std::string &name = "variable_template", const VariableType &data = VariableType(), bool active = true) : Variable(name), data_(data) {}
		VariableTemplate(const VariableTemplate &other) = default;
		VariableTemplate(VariableTemplate &&other) = default;
		VariableTemplate& operator=(const VariableTemplate &other) = default;
		VariableTemplate& operator=(VariableTemplate &&other) = default;

	private:
		VariableType data_;
	};
	class MatrixVariable final : public VariableTemplate<amc::core::Matrix>
	{
	public:
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto virtual toString() const->std::string override { return data().toString(); }

		virtual ~MatrixVariable() = default;
		explicit MatrixVariable(const std::string &name = "matrix_variable", const amc::core::Matrix &data = amc::core::Matrix()) : VariableTemplate(name, data) {}
		AMC_REGISTER_TYPE(MatrixVariable);
		AMC_DEFINE_BIG_FOUR(MatrixVariable);
	};
	class StringVariable final : public VariableTemplate<std::string>
	{
	public:
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto virtual toString() const->std::string override { return data(); }

		virtual ~StringVariable() = default;
		explicit StringVariable(const std::string &name = "string_variable", const std::string &data = "") : VariableTemplate(name, data) {}
		AMC_REGISTER_TYPE(StringVariable);
		AMC_DEFINE_BIG_FOUR(StringVariable);
	};

	/// @}
}

#endif
