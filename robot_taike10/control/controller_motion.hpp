﻿#ifndef AMC_CONTROL_CONTROLLER_MOTION_H_
#define AMC_CONTROL_CONTROLLER_MOTION_H_

#include "master_slave.hpp"

namespace amc::control
{
	class Motion : public virtual Slave
	{
	public:
		auto virtual saveXml(amc::core::XmlElement &xml_ele) const->void override;
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto motId()const->amc::Size;
		auto maxPos()const->double;
		auto setMaxPos(double max_pos)->void;
		auto minPos()const->double;
		auto setMinPos(double min_pos)->void;
		auto maxVel()const->double;
		auto setMaxVel(double max_vel)->void;
		auto minVel()const->double;
		auto setMinVel(double min_vel)->void;
		auto maxAcc()const->double;
		auto setMaxAcc(double max_acc)->void;
		auto minAcc()const->double;
		auto setMinAcc(double min_acc)->void;
		auto maxPosFollowingError()const->double;
		auto setMaxPosFollowingError(double max_pos_following_error)->void;
		auto maxVelFollowingError()const->double;
		auto setMaxVelFollowingError(double max_vel_following_error)->void;
		//auto posOffset()const->double;
		//auto setPosOffset(double pos_offset)->void;
		//auto posFactor()const->double;
		//auto setPosFactor(double pos_factor)->void;
		auto homePos()const->double;
		auto setHomePos(double home_pos)->void;

		auto virtual controlWord()const->std::uint16_t = 0;
		auto virtual modeOfOperation()const->std::uint8_t = 0;
		auto virtual targetPos()const->double = 0;
		auto virtual targetVel()const->double = 0;
		auto virtual targetCur()const->double = 0;
		auto virtual offsetVel()const->double = 0;
		auto virtual offsetCur()const->double = 0;

		auto virtual setControlWord(std::uint16_t control_word)->void = 0;
		auto virtual setModeOfOperation(std::uint8_t mode)->void = 0;
		auto virtual setTargetPos(double pos)->void = 0;
		auto virtual setTargetVel(double vel)->void = 0;
		auto virtual setTargetCur(double cur)->void = 0;
		auto virtual setOffsetVel(double vel)->void = 0;
		auto virtual setOffsetCur(double cur)->void = 0;

		auto virtual statusWord()->std::uint16_t = 0;
		auto virtual modeOfDisplay()->std::uint8_t = 0;
		auto virtual actualPos()->double = 0;
		auto virtual actualVel()->double = 0;
		auto virtual actualCur()->double = 0;

		auto virtual disable()->int = 0;
		auto virtual enable()->int = 0;
		auto virtual home()->int = 0;
		auto virtual mode(std::uint8_t md)->int = 0;

		virtual ~Motion();
		explicit Motion(const std::string &name = "motion", std::uint16_t phy_id = 0
			, double max_pos = 1.0, double min_pos = -1.0, double max_vel = 1.0, double min_vel = -1.0, double max_acc = 1.0, double min_acc = -1.0
			, double max_pos_following_error = 1.0, double max_vel_following_error = 1.0, double home_pos = 0.0);
		AMC_DELETE_BIG_FOUR(Motion);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;

		friend class Controller;
	};
	class Controller : public virtual Master
	{
	public:
		auto motionPool()->amc::core::RefPool<Motion>&;
		auto motionPool()const->const amc::core::RefPool<Motion>& { return const_cast<std::decay_t<decltype(*this)> *>(this)->motionPool(); }
		auto motionAtSla(amc::Size id)->Motion&;
		auto motionAtSla(amc::Size id)const->const Motion& { return const_cast<std::decay_t<decltype(*this)> *>(this)->motionAtSla(id); }
		auto motionAtPhy(amc::Size id)->Motion&;
		auto motionAtPhy(amc::Size id)const->const Motion& { return const_cast<std::decay_t<decltype(*this)> *>(this)->motionAtPhy(id); }
		auto motionAtAbs(amc::Size id)->Motion&;
		auto motionAtAbs(amc::Size id)const->const Motion& { return const_cast<std::decay_t<decltype(*this)> *>(this)->motionAtAbs(id); }

		AMC_REGISTER_TYPE(Controller);
		virtual ~Controller();
		Controller(const std::string &name = "controller");
		Controller(const Controller &other) = delete;
		Controller(Controller &&other) = delete;
		Controller& operator=(const Controller &other) = delete;
		Controller& operator=(Controller &&other) = delete;

	protected:
		auto virtual init()->void override;

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
}

#endif
