#ifndef AMC_SERVER_H_
#define AMC_SERVER_H_

#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <future>

#include <../core/core.hpp>
#include <../control/control.hpp>
#include <../sensor/sensor.hpp>
#include <../dynamic/dynamic.hpp>
#include <../plan/plan.hpp>

namespace amc::server
{
#ifdef UNIX
	auto inline exec(const char* cmd)->std::string {
		std::array<char, 128> buffer;
		std::string result;
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
		return result;
	}
#endif




	class InterfaceRoot : public amc::core::Object
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override
		{
			auto ins = doc_.RootElement()->DeepClone(xml_ele.GetDocument());
			xml_ele.Parent()->InsertAfterChild(&xml_ele, ins);
			xml_ele.Parent()->DeleteChild(&xml_ele);
		}
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override
		{
			doc_.Clear();
			auto root = xml_ele.DeepClone(&doc_);
			doc_.InsertEndChild(root);
		}

		AMC_REGISTER_TYPE(InterfaceRoot);

	private:
		amc::core::XmlDocument doc_;
	};

	class ControlServer : public amc::core::Object
	{
	public:
		static auto instance()->ControlServer &;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		template<typename T = amc::dynamic::Model, typename... Args>
		auto makeModel(Args&&... args)->void { this->resetModel(new T(std::forward<Args>(args)...)); }
		template<typename T = amc::control::Controller, typename... Args>
		auto makeController(Args&&... args)->void { this->resetController(new T(std::forward<Args>(args)...)); }
		template<typename T = amc::sensor::SensorRoot, typename... Args>
		auto makeSensorRoot(Args&&... args)->void { this->resetSensorRoot(new T(std::forward<Args>(args)...)); }
		template<typename T = amc::plan::PlanRoot, typename... Args>
		auto makePlanRoot(Args&&... args)->void { this->resetPlanRoot(new T(std::forward<Args>(args)...)); }
		auto resetModel(dynamic::Model *model)->void;
		auto resetController(control::Controller *controller)->void;
		auto resetSensorRoot(sensor::SensorRoot *sensor_root)->void;
		auto resetPlanRoot(plan::PlanRoot *sensor_root)->void;
		auto model()->dynamic::Model&;
		auto model()const->const dynamic::Model& { return const_cast<ControlServer *>(this)->model(); }
		auto controller()->control::Controller&;
		auto controller()const->const control::Controller& { return const_cast<ControlServer *>(this)->controller(); }
		auto sensorRoot()->sensor::SensorRoot&;
		auto sensorRoot()const->const sensor::SensorRoot& { return const_cast<ControlServer *>(this)->sensorRoot(); }
		auto planRoot()->plan::PlanRoot&;
		auto planRoot()const->const plan::PlanRoot& { return const_cast<ControlServer *>(this)->planRoot(); }
		auto interfaceRoot()->InterfaceRoot&;
		auto interfaceRoot()const->const InterfaceRoot& { return const_cast<ControlServer *>(this)->interfaceRoot(); }

		auto executeCmd(const amc::core::Msg &cmd_string)->std::shared_ptr<amc::plan::PlanTarget>;
		auto start()->void;
		auto stop()->void;
		auto running()->bool;
		auto waitForAllExecution()->void;
		auto waitForAllCollection()->void;

		auto globalCount()->std::int64_t;
		auto currentExecuteId()->std::int64_t;
		auto currentCollectId()->std::int64_t;
		auto getRtData(const std::function<void(ControlServer&, std::any&)>& get_func, std::any& data)->void;

		AMC_REGISTER_TYPE(ControlServer);

	private:
		~ControlServer();
		ControlServer();
		ControlServer(const ControlServer &) = delete;
		ControlServer &operator=(const ControlServer &) = delete;

	private:
		struct Imp;
		std::unique_ptr<Imp> imp_;
	};
}

#endif

