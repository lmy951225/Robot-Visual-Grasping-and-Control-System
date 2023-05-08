﻿#ifndef AMC_SENSOR_BASE_H_
#define AMC_SENSOR_BASE_H_

#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <memory>

#include <../core/core.hpp>

namespace amc::sensor
{
	class Sensor;

	struct SensorData {};
	class SensorDataProtector
	{
	public:
		auto get() const->const SensorData * { return data_; }
		auto data() const->const SensorData & { return *data_; }
		auto operator->()const -> const SensorData * { return data_; }
		auto operator*()const -> const SensorData & { return std::ref(*data_); }
		auto operator=(SensorDataProtector && other)->SensorDataProtector & { std::swap(*this, other); return *this; }

		~SensorDataProtector() = default;
		SensorDataProtector() : sensor_(nullptr), data_(nullptr) {}
		SensorDataProtector(SensorDataProtector && other) = default;

	private:
		explicit SensorDataProtector(Sensor *sensor);
		SensorDataProtector(const SensorDataProtector&) = delete;
		SensorDataProtector & operator=(const SensorDataProtector&) = delete;

		Sensor *sensor_;
		const SensorData *data_;
		std::unique_lock<std::recursive_mutex> lock_;

		friend class Sensor;
	};
	class Sensor :public amc::core::Object
	{
	public:
		static auto Type()->const std::string & { static const std::string type("Sensor"); return std::ref(type); }
		auto virtual type() const->const std::string& override { return Type(); }
		auto start()->void;
		auto stop()->void;
		auto dataProtector()->SensorDataProtector;

		virtual ~Sensor();
		Sensor(const std::string &name, std::function<SensorData*()> new_func);

	protected:
		auto virtual init()->void {}
		auto virtual release()->void {}
		auto virtual updateData(SensorData & data)->void {}

	private:
		auto operator=(const Sensor &)->Sensor& = default;
		auto operator=(Sensor &&)->Sensor& = default;
		Sensor(const Sensor &) = default;
		Sensor(Sensor &&) = default;

		struct Imp;
		amc::core::ImpPtr<Imp> imp_;

		friend class SensorDataProtector;
		template<class DataType> friend class SensorTemplate;
	};
	template<class DataType> class SensorTemplate :public Sensor
	{
	public:
		SensorTemplate(const std::string &name) :Sensor(name, []()->SensorData* {return new DataType; }) {}
		SensorTemplate(Object &father, const amc::core::XmlElement &xml_ele) :Sensor(father, xml_ele, []()->SensorData* {return new DataType; }) {}
	};

	class SensorRoot :public amc::core::Object
	{
	public:
		using Object::loadXml;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto sensorPool()->amc::core::ObjectPool<Sensor> &;
		auto sensorPool()const->const amc::core::ObjectPool<Sensor> &;
		auto start()->void { for (auto &sensor : sensorPool())sensor.start(); }
		auto stop()->void { for (auto &sensor : sensorPool())sensor.stop(); }

		virtual ~SensorRoot();
		explicit SensorRoot(const std::string &name = "sensor_root");
		AMC_REGISTER_TYPE(SensorRoot);
		AMC_DECLARE_BIG_FOUR(SensorRoot);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
}


#endif
