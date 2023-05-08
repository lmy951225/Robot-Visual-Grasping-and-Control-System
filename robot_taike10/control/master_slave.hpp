﻿#ifndef AMC_CONTROL_MASTER_SLAVE_H_
#define AMC_CONTROL_MASTER_SLAVE_H_

#include <any>

#include "../core/object.hpp"
#include "../core/basic_type.hpp"
#include "../core/msg.hpp"

namespace amc::control
{
	class Slave : public amc::core::Object
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto virtual send()->void {}
		auto virtual recv()->void {}
		auto phyId()const->std::uint16_t;
		auto slaId()const->std::uint16_t { return static_cast<std::uint16_t>(id()); }

		virtual ~Slave();
		explicit Slave(const std::string &name = "slave", std::uint16_t phy_id = 0);
		AMC_REGISTER_TYPE(Slave);
		AMC_DECLARE_BIG_FOUR(Slave);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class Master : public amc::core::Object
	{
	public:
		struct RtStasticsData
		{
			double avg_time_consumed;
			std::int64_t max_time_consumed;
			std::int64_t max_time_occur_count;
			std::int64_t min_time_consumed;
			std::int64_t min_time_occur_count;
			std::int64_t total_count;
			std::int64_t overrun_count;
		};
		enum { MAX_MSG_SIZE = 8192 };
		static auto Type()->const std::string & { static const std::string type("Master"); return std::ref(type); }
		auto virtual type() const->const std::string& override { return Type(); }
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		// used only in non-rt thread //
		auto start()->void;
		auto stop()->void;
		auto setControlStrategy(std::function<void()> strategy)->void;

		// used in rt thread //
		auto logFile(const char *file_name)->void;
		auto lout()->amc::core::MsgStream &;
		auto mout()->amc::core::MsgStream &;
		auto slaveAtAbs(amc::Size id)->Slave& { return slavePool().at(id); }
		auto slaveAtAbs(amc::Size id)const->const Slave& { return const_cast<std::decay_t<decltype(*this)> *>(this)->slaveAtAbs(id); }
		auto slaveAtPhy(amc::Size id)->Slave&;
		auto slaveAtPhy(amc::Size id)const->const Slave& { return const_cast<std::decay_t<decltype(*this)> *>(this)->slaveAtPhy(id); }
		auto slavePool()->amc::core::ObjectPool<Slave>&;
		auto slavePool()const->const amc::core::ObjectPool<Slave>& { return const_cast<std::decay_t<decltype(*this)> *>(this)->slavePool(); }
		auto rtHandle()->std::any&;
		auto rtHandle()const->const std::any& { return const_cast<std::decay_t<decltype(*this)> *>(this)->rtHandle(); }
		auto resetRtStasticData(RtStasticsData *stastics, bool is_new_data_include_this_count = false)->void;

		virtual ~Master();
		explicit Master(const std::string &name = "master");
		Master(const Master &other) = delete;
		Master(Master &&other) = delete;
		Master& operator=(const Master &other) = delete;
		Master& operator=(Master &&other) = delete;

	protected:
		auto virtual init()->void {}
		auto virtual send()->void { for (auto &s : slavePool())s.send(); }
		auto virtual recv()->void { for (auto &s : slavePool())s.recv(); }
		auto virtual sync()->void {}
		auto virtual release()->void {}

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;

		friend class Slave;
		friend class PdoEntry;
	};
}

#endif
