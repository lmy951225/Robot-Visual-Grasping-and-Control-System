﻿#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <future>

#include "controller_motion.hpp"

namespace amc::control
{
	struct Motion::Imp
	{
		double max_pos_, min_pos_, max_vel_, min_vel_, max_acc_, min_acc_, max_pos_following_error_, max_vel_following_error_;
		//double pos_factor_, pos_offset_;
		double home_pos_;
		amc::Size mot_id_;
	};
	
	auto Motion::saveXml(amc::core::XmlElement &xml_ele) const->void
	{
		Slave::saveXml(xml_ele);
		xml_ele.SetAttribute("max_pos", maxPos());
		xml_ele.SetAttribute("min_pos", minPos());
		xml_ele.SetAttribute("max_vel", maxVel());
		xml_ele.SetAttribute("min_vel", minVel());
		xml_ele.SetAttribute("max_acc", maxAcc());
		xml_ele.SetAttribute("min_acc", minAcc());
		xml_ele.SetAttribute("max_pos_following_error", maxPosFollowingError());
		xml_ele.SetAttribute("max_vel_following_error", maxVelFollowingError());
		//xml_ele.SetAttribute("pos_factor", posFactor());
		//xml_ele.SetAttribute("pos_offset", posOffset());
		xml_ele.SetAttribute("home_pos", homePos());
	}
	
	auto Motion::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		imp_->max_pos_ = attributeDouble(xml_ele, "max_pos", 1.0);
		imp_->min_pos_ = attributeDouble(xml_ele, "min_pos", -1.0);
		imp_->max_vel_ = attributeDouble(xml_ele, "max_vel", 1.0);
		imp_->min_vel_ = attributeDouble(xml_ele, "min_vel", -imp_->max_vel_);
		imp_->max_acc_ = attributeDouble(xml_ele, "max_acc", 1.0);
		imp_->min_acc_ = attributeDouble(xml_ele, "min_acc", -imp_->max_acc_);
		imp_->max_pos_following_error_ = attributeDouble(xml_ele, "max_pos_following_error", 1.0);
		imp_->max_vel_following_error_ = attributeDouble(xml_ele, "max_vel_following_error", 1.0);
		//imp_->pos_factor_ = attributeDouble(xml_ele, "pos_factor", 1.0);
		//imp_->pos_offset_ = attributeDouble(xml_ele, "pos_offset", 0.0);
		imp_->home_pos_ = attributeDouble(xml_ele, "home_pos", 0.0);
		Slave::loadXml(xml_ele);
	}
	
	auto Motion::motId()const->amc::Size { return imp_->mot_id_; }
	auto Motion::maxPos()const->double { return imp_->max_pos_; }
	auto Motion::setMaxPos(double max_pos)->void { imp_->max_pos_ = max_pos; }
	auto Motion::minPos()const->double { return imp_->min_pos_; }
	auto Motion::setMinPos(double min_pos)->void { imp_->min_pos_ = min_pos; }
	auto Motion::maxVel()const->double { return imp_->max_vel_; }
	auto Motion::setMaxVel(double max_vel)->void { imp_->max_vel_ = max_vel; }
	auto Motion::minVel()const->double { return imp_->min_vel_; }
	auto Motion::setMinVel(double min_vel)->void { imp_->min_vel_ = min_vel; }
	auto Motion::maxAcc()const->double { return imp_->max_acc_; }
	auto Motion::setMaxAcc(double max_acc)->void { imp_->max_acc_ = max_acc; }
	auto Motion::minAcc()const->double { return imp_->min_acc_; }
	auto Motion::setMinAcc(double min_acc)->void { imp_->min_acc_ = min_acc; }
	auto Motion::maxPosFollowingError()const->double { return imp_->max_pos_following_error_; }
	auto Motion::setMaxPosFollowingError(double max_pos_following_error)->void { imp_->max_pos_following_error_ = max_pos_following_error; }
	auto Motion::maxVelFollowingError()const->double { return imp_->max_vel_following_error_; }
	auto Motion::setMaxVelFollowingError(double max_vel_following_error)->void { imp_->max_vel_following_error_ = max_vel_following_error; }
	//auto Motion::posOffset()const->double { return imp_->pos_offset_; }
	//auto Motion::setPosOffset(double pos_offset)->void { imp_->pos_offset_ = pos_offset; }
	//auto Motion::posFactor()const->double { return imp_->pos_factor_; }
	//auto Motion::setPosFactor(double pos_factor)->void { imp_->pos_factor_ = pos_factor; }
	auto Motion::homePos()const->double { return imp_->home_pos_; }
	auto Motion::setHomePos(double home_pos)->void { imp_->home_pos_ = home_pos; }
	Motion::~Motion() = default;
	
	Motion::Motion(const std::string &name, std::uint16_t phy_id
		, double max_pos, double min_pos, double max_vel, double min_vel, double max_acc, double min_acc
		, double max_pos_following_error, double max_vel_following_error, double home_pos) :Slave(name, phy_id)
	{
		imp_->max_pos_ = max_pos;
		imp_->min_pos_ = min_pos;
		imp_->max_vel_ = max_vel;
		imp_->min_vel_ = min_vel;
		imp_->max_acc_ = max_acc;
		imp_->min_acc_ = min_acc;
		imp_->max_pos_following_error_ = max_pos_following_error;
		imp_->max_vel_following_error_ = max_vel_following_error;
		//imp_->pos_factor_ = pos_factor;
		//imp_->pos_offset_ = pos_offset;
		imp_->home_pos_ = home_pos;
	}

	struct Controller::Imp { amc::core::RefPool<Motion> motion_pool_; };
	auto Controller::motionPool()->amc::core::RefPool<Motion>& { return imp_->motion_pool_; }
	auto Controller::init()->void
	{
		motionPool().clear();
		for (auto &s : slavePool())
		{
			if (dynamic_cast<Motion*>(&s))motionPool().push_back_ptr(dynamic_cast<Motion*>(&s));
			motionPool().back().imp_->mot_id_ = motionPool().size() - 1;
		}
	}
	auto Controller::motionAtAbs(amc::Size id)->Motion& { return imp_->motion_pool_.at(id); }
	auto Controller::motionAtPhy(amc::Size id)->Motion& { return dynamic_cast<Motion&>(slaveAtPhy(id)); }
	auto Controller::motionAtSla(amc::Size id)->Motion& { return dynamic_cast<Motion&>(slavePool().at(id)); }
	Controller::~Controller() = default;
	Controller::Controller(const std::string &name) :imp_(new Imp), Master(name) 
	{
		this->registerType<amc::core::ObjectPool<Slave, amc::core::Object> >();
	}
}
