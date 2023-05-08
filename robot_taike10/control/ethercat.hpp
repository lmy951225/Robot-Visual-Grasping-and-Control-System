﻿#ifndef AMC_CONTROL_ETHERCAT_H_
#define AMC_CONTROL_ETHERCAT_H_

#include "controller_motion.hpp"

namespace amc::control
{
	class PdoEntry :public amc::core::Object
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		
		auto ecHandle()->std::any&;
		auto ecHandle()const->const std::any& { return const_cast<std::decay_t<decltype(*this)>*>(this)->ecHandle(); }
		auto index()const->std::uint16_t;
		auto subindex()const->std::uint8_t;
		auto bitSize()const->amc::Size;

		explicit PdoEntry(const std::string &name = "entry", std::uint16_t index = 0x0000, std::uint8_t subindex = 0x00, amc::Size bit_size = 8);
		AMC_REGISTER_TYPE(PdoEntry);
		AMC_DECLARE_BIG_FOUR(PdoEntry);

	public:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class Pdo :public amc::core::ObjectPool<PdoEntry>
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto ecHandle()->std::any&;
		auto ecHandle()const->const std::any& { return const_cast<std::decay_t<decltype(*this)>*>(this)->ecHandle(); }
		auto index()const->std::uint16_t;

		explicit Pdo(const std::string &name = "pdo", std::uint16_t index = 0x0000);
		AMC_REGISTER_TYPE(Pdo);
		AMC_DECLARE_BIG_FOUR(Pdo);

	public:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class SyncManager :public amc::core::ObjectPool<Pdo>
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto tx()const->bool;
		auto rx()const->bool;

		explicit SyncManager(const std::string &name = "sm", bool is_tx = true);
		AMC_REGISTER_TYPE(SyncManager);
		AMC_DECLARE_BIG_FOUR(SyncManager);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};

	class EthercatSlave : virtual public Slave
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto vendorID()const->std::uint32_t;
		auto productCode()const->std::uint32_t;
		auto revisionNum()const->std::uint32_t;
		auto dcAssignActivate()const->std::uint32_t;
		auto smPool()->amc::core::ObjectPool<SyncManager>&;
		auto smPool()const->const amc::core::ObjectPool<SyncManager>& { return const_cast<std::decay_t<decltype(*this)>*>(this)->smPool(); }
		auto ecHandle()->std::any&;
		auto ecHandle()const->const std::any& { return const_cast<std::decay_t<decltype(*this)>*>(this)->ecHandle(); }

		template<typename ValueType>
		auto readPdo(std::uint16_t index, std::uint8_t subindex, ValueType &value)->void { readPdo(index, subindex, &value, sizeof(ValueType) * 8); }
		auto readPdo(std::uint16_t index, std::uint8_t subindex, void *value, amc::Size bit_size)->void;
		template<typename ValueType>
		auto writePdo(std::uint16_t index, std::uint8_t subindex, const ValueType &value)->void { writePdo(index, subindex, &value, sizeof(ValueType) * 8); }
		auto writePdo(std::uint16_t index, std::uint8_t subindex, const void *value, amc::Size bit_size)->void;
		template<typename ValueType>
		auto readSdo(std::uint16_t index, std::uint8_t subindex, ValueType &value)->void { readSdo(index, subindex, &value, sizeof(ValueType)); }
		auto readSdo(std::uint16_t index, std::uint8_t subindex, void *value, amc::Size byte_size)->void;
		template<typename ValueType>
		auto writeSdo(std::uint16_t index, std::uint8_t subindex, const ValueType &value)->void { writeSdo(index, subindex, &value, sizeof(ValueType)); }
		auto writeSdo(std::uint16_t index, std::uint8_t subindex, const void *value, amc::Size byte_size)->void;

		virtual ~EthercatSlave();
		explicit EthercatSlave(const std::string &name = "ethercat_slave", std::uint16_t phy_id = 0, std::uint32_t vendor_id = 0x00000000, std::uint32_t product_code = 0x00000000, std::uint32_t revision_num = 0x00000000, std::uint32_t dc_assign_activate = 0x00000000);
		AMC_REGISTER_TYPE(EthercatSlave);
		AMC_DELETE_BIG_FOUR(EthercatSlave);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;

		friend class EthercatMaster;
	};
	class EthercatMaster : virtual public Master
	{
	public:
		auto ecHandle()->std::any&;
		auto ecHandle()const->const std::any& { return const_cast<std::decay_t<decltype(*this)> *>(this)->ecHandle(); }
		auto ecSlavePool()->amc::core::RefPool<EthercatSlave>&;
		auto ecSlavePool()const->const amc::core::RefPool<EthercatSlave>& { return const_cast<std::decay_t<decltype(*this)> *>(this)->ecSlavePool(); }
		auto scan()->void;

		virtual ~EthercatMaster();
		EthercatMaster(const std::string &name = "ethercat_master");
		EthercatMaster(const EthercatMaster &other) = delete;
		EthercatMaster(EthercatMaster &&other) = delete;
		EthercatMaster& operator=(const EthercatMaster &other) = delete;
		EthercatMaster& operator=(EthercatMaster &&other) = delete;
		AMC_REGISTER_TYPE(EthercatMaster);

	protected:
		auto virtual init()->void override;
		auto virtual send()->void override;
		auto virtual recv()->void override;
		auto virtual sync()->void override;
		auto virtual release()->void override;

	private:
		class Imp;
		amc::core::ImpPtr<Imp> imp_;

		friend class PdoEntry;
	};

	class EthercatMotion :public EthercatSlave, public Motion
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;

		auto virtual controlWord()const->std::uint16_t override;
		auto virtual modeOfOperation()const->std::uint8_t override;
		auto virtual targetPos()const->double override;
		auto virtual targetVel()const->double override;
		auto virtual targetCur()const->double override;
		auto virtual offsetVel()const->double override;
		auto virtual offsetCur()const->double override;
		// require pdo 0x6040 //
		auto virtual setControlWord(std::uint16_t control_word)->void override;
		// require pdo 0x6060 //
		auto virtual setModeOfOperation(std::uint8_t mode)->void override;
		// require pdo 0x607A //
		auto virtual setTargetPos(double pos)->void override;
		// require pdo 0x60FF //
		auto virtual setTargetVel(double vel)->void override;
		// require pdo 0x6071 //
		auto virtual setTargetCur(double cur)->void override;
		// require pdo 0x60B1 //
		auto virtual setOffsetVel(double vel)->void override;
		// require pdo 0x60B2 //
		auto virtual setOffsetCur(double cur)->void override;
		// require pdo 0x6041 //
		auto virtual statusWord()->std::uint16_t override;
		// require pdo 0x6061 //
		auto virtual modeOfDisplay()->std::uint8_t override;
		// require pdo 0x6064 //
		auto virtual actualPos()->double override;
		// require pdo 0x606C //
		auto virtual actualVel()->double override;
		// require pdo 0x6077 //
		auto virtual actualCur()->double override;


		// require pdo 0x6040 0x6041 // 
		auto virtual disable()->int override;
		// require pdo 0x6040 0x6041 //
		auto virtual enable()->int override;
		// require pdo 0x6040 0x6041 0x6060 0x6061 //
		auto virtual home()->int override;
		// require pdo 0x6060 0x6061 //
		auto virtual mode(std::uint8_t md)->int override;

		EthercatMotion(const std::string &name = "ethercat_motion", std::uint16_t phy_id = 0
			, std::uint32_t vendor_id = 0x00000000, std::uint32_t product_code = 0x00000000, std::uint32_t revision_num = 0x00000000, std::uint32_t dc_assign_activate = 0x00000000
			, double max_pos = 1.0, double min_pos = -1.0, double max_vel = 1.0, double min_vel = -1.0, double max_acc = 1.0, double min_acc = -1.0, double max_pos_following_error = 1.0, double max_vel_following_error = 1.0, double home_pos = 0.0);
		AMC_REGISTER_TYPE(EthercatMotion);
		AMC_DELETE_BIG_FOUR(EthercatMotion);


	private:
		class Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
	class EthercatController :public EthercatMaster, public Controller
	{
	public:
		virtual ~EthercatController() = default;
		EthercatController(const std::string &name = "ethercat_controller");
		EthercatController(const EthercatController &other) = delete;
		EthercatController(EthercatController &&other) = delete;
		EthercatController& operator=(const EthercatController &other) = delete;
		EthercatController& operator=(EthercatController &&other) = delete;
		AMC_REGISTER_TYPE(EthercatController);

	protected:
		auto virtual init()->void override { EthercatMaster::init(); Controller::init(); }
		auto virtual send()->void override { EthercatMaster::send(); }
		auto virtual recv()->void override { EthercatMaster::recv(); }
		auto virtual sync()->void override { EthercatMaster::sync(); }
		auto virtual release()->void override { EthercatMaster::release(); }
	};
}

#endif
