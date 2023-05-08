﻿#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <limits>
#include <sstream>
#include <regex>
#include <limits>
#include <type_traits>
#include <ios>

#include "../core/core.hpp"
#include "matrix.hpp"
#include "screw.hpp"
#include "model.hpp"

namespace amc::dynamic
{
	auto Element::attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name)const->amc::core::Matrix
	{
		std::string error = "failed to get Matrix attribute \"" + attribute_name + "\" in element \"" + xml_ele.Name() + "\", because ";

		amc::core::Matrix mat;
		try
		{
			mat = this->ancestor<Model>()->calculator().calculateExpression(xml_ele.Attribute(attribute_name.c_str()));
		}
		catch (std::exception &e)
		{
			throw std::runtime_error(error + "failed to evaluate matrix:" + e.what());
		}

		return mat;
	}
	auto Element::attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name, const amc::core::Matrix& default_value)const->amc::core::Matrix
	{
		return xml_ele.Attribute(attribute_name.c_str()) ? attributeMatrix(xml_ele, attribute_name) : default_value;
	}
	auto Element::attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name, Size m, Size n)const->amc::core::Matrix
	{
		std::string error = "failed to get Matrix attribute \"" + attribute_name + "\" in element \"" + xml_ele.Name() + "\", because ";

		amc::core::Matrix mat = attributeMatrix(xml_ele, attribute_name);

		if (mat.m() != m || mat.n() != n)
		{
			throw std::runtime_error(error + "matrix has wrong dimensions, it's dimentsion should be \"" + std::to_string(m) + "," + std::to_string(n)
				+ "\", while the real value is \"" + std::to_string(mat.m()) + "," + std::to_string(mat.n()) + "\"");
		}

		return mat;
	}
	auto Element::attributeMatrix(const amc::core::XmlElement &xml_ele, const std::string &attribute_name, Size m, Size n, const amc::core::Matrix& default_value)const->amc::core::Matrix
	{
		return xml_ele.Attribute(attribute_name.c_str()) ? attributeMatrix(xml_ele, attribute_name, m, n) : default_value;
	}

	auto DynEle::saveXml(amc::core::XmlElement &xml_ele) const->void
	{
		Element::saveXml(xml_ele);
		xml_ele.SetAttribute("active", active() ? "true" : "false");
	}
	auto DynEle::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		active_ = attributeBool(xml_ele, "active", true);
		Element::loadXml(xml_ele);
	}

	auto Environment::saveXml(amc::core::XmlElement &xml_ele) const->void
	{
		Object::saveXml(xml_ele);
		xml_ele.SetAttribute("gravity", core::Matrix(1, 6, gravity_).toString().c_str());
	}
	auto Environment::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		Object::loadXml(xml_ele);
		std::copy_n(attributeMatrix(xml_ele, "gravity", 1, 6).data(), 6, gravity_);
	}

	auto Variable::saveXml(amc::core::XmlElement &xml_ele) const->void
	{
		Element::saveXml(xml_ele);
		xml_ele.SetText(this->toString().c_str());
	}

	auto MatrixVariable::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		data() = ancestor<Model>()->calculator().calculateExpression(xml_ele.GetText());
		Variable::loadXml(xml_ele);
		ancestor<Model>()->calculator().addVariable(name(), data());
	}
	auto StringVariable::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		data() = std::string(xml_ele.GetText());
		ancestor<Model>()->calculator().addVariable(name(), data());
		Variable::loadXml(xml_ele);
	}
}
