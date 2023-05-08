#include <algorithm>
#include <future>
#include <array>
#include <iostream>
#include <iterator>
#include"function.hpp"
#include"root.hpp"
#include <../control/control.hpp>
#include <../dynamic/dynamic.hpp>
#include <../server/server.hpp>
#include <chrono>
using namespace std;
// uint8_t buff1[10];
// uint8_t buff2[8];
// uint8_t buff[12];
// const char *dev  = "/dev/ttyUSB0";
namespace amc::plan
{
	
	struct Plan::Imp {};
	auto Plan::command()->amc::core::Command & { return dynamic_cast<amc::core::Command&>(children().front()); }
	Plan::~Plan() = default;
	Plan::Plan(const std::string &name) :Object(name), imp_(new Imp)
	{
		add<amc::core::Command>(name);
	}
	AMC_DEFINE_BIG_FOUR_CPP(Plan);

	struct PlanRoot::Imp { Imp() {} };
	auto PlanRoot::planPool()->amc::core::ObjectPool<Plan> & { return dynamic_cast<amc::core::ObjectPool<Plan> &>(children().front()); }
	auto PlanRoot::planParser()->amc::core::CommandParser
	{
		amc::core::CommandParser parser;
		for (auto &plan : planPool()) parser.commandPool().add<amc::core::Command>(plan.command());
		return parser;
	}
	PlanRoot::~PlanRoot() = default;
	PlanRoot::PlanRoot(const std::string &name) :Object(name)
	{	
		amc::core::Object::registerTypeGlobal<amc::core::ObjectPool<Plan> >();
		add<amc::core::ObjectPool<Plan> >("plan_pool_object");
	}
	AMC_DEFINE_BIG_FOUR_CPP(PlanRoot);

#define CHECK_PARAM_STRING \
		"		<UniqueParam default=\"check_none\">" \
		"			<Param name=\"check_all\"/>" \
		"			<Param name=\"check_none\"/>" \
		"			<GroupParam>"\
		"				<UniqueParam default=\"check_pos\">"\
		"					<Param name=\"check_pos\"/>"\
		"					<Param name=\"not_check_pos\"/>"\
		"					<GroupParam>"\
		"						<UniqueParam default=\"check_pos_max\">"\
		"							<Param name=\"check_pos_max\"/>"\
		"							<Param name=\"not_check_pos_max\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_pos_min\">"\
		"							<Param name=\"check_pos_min\"/>"\
		"							<Param name=\"not_check_pos_min\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_pos_continuous\">"\
		"							<Param name=\"check_pos_continuous\"/>"\
		"							<Param name=\"not_check_pos_continuous\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_pos_continuous_at_start\">"\
		"							<Param name=\"check_pos_continuous_at_start\"/>"\
		"							<Param name=\"not_check_pos_continuous_at_start\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_pos_continuous_second_order\">"\
		"							<Param name=\"check_pos_continuous_second_order\"/>"\
		"							<Param name=\"not_check_pos_continuous_second_order\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_pos_continuous_second_order_at_start\">"\
		"							<Param name=\"check_pos_continuous_second_order_at_start\"/>"\
		"							<Param name=\"not_check_pos_continuous_second_order_at_start\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_pos_following_error\">"\
		"							<Param name=\"check_pos_following_error\"/>"\
		"							<Param name=\"not_check_pos_following_error\"/>"\
		"						</UniqueParam>"\
		"					</GroupParam>"\
		"				</UniqueParam>"\
		"				<UniqueParam default=\"check_vel\">"\
		"					<Param name=\"check_vel\"/>"\
		"					<Param name=\"not_check_vel\"/>"\
		"					<GroupParam>"\
		"						<UniqueParam default=\"check_vel_max\">"\
		"							<Param name=\"check_vel_max\"/>"\
		"							<Param name=\"not_check_vel_max\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_vel_min\">"\
		"							<Param name=\"check_vel_min\"/>"\
		"							<Param name=\"not_check_vel_min\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_vel_continuous\">"\
		"							<Param name=\"check_vel_continuous\"/>"\
		"							<Param name=\"not_check_vel_continuous\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_vel_continuous_at_start\">"\
		"							<Param name=\"check_vel_continuous_at_start\"/>"\
		"							<Param name=\"not_check_vel_continuous_at_start\"/>"\
		"						</UniqueParam>"\
		"						<UniqueParam default=\"check_vel_following_error\">"\
		"							<Param name=\"check_vel_following_error\"/>"\
		"							<Param name=\"not_check_vel_following_error\"/>"\
		"						</UniqueParam>"\
		"					</GroupParam>"\
		"				</UniqueParam>"\
		"			</GroupParam>"\
		"		</UniqueParam>"
	auto set_check_option(const std::map<std::string, std::string> &cmd_params, PlanTarget &target)->void
	{
		for (auto cmd_param : cmd_params)
		{
			if (cmd_param.first == "check_all")
			{
				target.option &= ~(
					Plan::NOT_CHECK_POS_MIN |
					Plan::NOT_CHECK_POS_MAX |
					Plan::NOT_CHECK_POS_CONTINUOUS |
					Plan::NOT_CHECK_POS_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START |
					Plan::NOT_CHECK_POS_FOLLOWING_ERROR |
					Plan::NOT_CHECK_VEL_MIN |
					Plan::NOT_CHECK_VEL_MAX |
					Plan::NOT_CHECK_VEL_CONTINUOUS |
					Plan::NOT_CHECK_VEL_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_VEL_FOLLOWING_ERROR);
			}
			else if (cmd_param.first == "check_none")
			{
				target.option |=
					Plan::NOT_CHECK_POS_MIN |
					Plan::NOT_CHECK_POS_MAX |
					Plan::NOT_CHECK_POS_CONTINUOUS |
					Plan::NOT_CHECK_POS_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START |
					Plan::NOT_CHECK_POS_FOLLOWING_ERROR |
					Plan::NOT_CHECK_VEL_MIN |
					Plan::NOT_CHECK_VEL_MAX |
					Plan::NOT_CHECK_VEL_CONTINUOUS |
					Plan::NOT_CHECK_VEL_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_VEL_FOLLOWING_ERROR;
			}
			else if (cmd_param.first == "check_pos")
			{
				target.option &= ~(
					Plan::NOT_CHECK_POS_MIN |
					Plan::NOT_CHECK_POS_MAX |
					Plan::NOT_CHECK_POS_CONTINUOUS |
					Plan::NOT_CHECK_POS_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START |
					Plan::NOT_CHECK_POS_FOLLOWING_ERROR);
			}
			else if (cmd_param.first == "not_check_pos")
			{
				target.option |=
					Plan::NOT_CHECK_POS_MIN |
					Plan::NOT_CHECK_POS_MAX |
					Plan::NOT_CHECK_POS_CONTINUOUS |
					Plan::NOT_CHECK_POS_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER |
					Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START |
					Plan::NOT_CHECK_POS_FOLLOWING_ERROR;
			}
			else if (cmd_param.first == "check_vel")
			{
				target.option &= ~(
					Plan::NOT_CHECK_VEL_MIN |
					Plan::NOT_CHECK_VEL_MAX |
					Plan::NOT_CHECK_VEL_CONTINUOUS |
					Plan::NOT_CHECK_VEL_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_VEL_FOLLOWING_ERROR);
			}
			else if (cmd_param.first == "not_check_vel")
			{
				target.option |=
					Plan::NOT_CHECK_VEL_MIN |
					Plan::NOT_CHECK_VEL_MAX |
					Plan::NOT_CHECK_VEL_CONTINUOUS |
					Plan::NOT_CHECK_VEL_CONTINUOUS_AT_START |
					Plan::NOT_CHECK_VEL_FOLLOWING_ERROR;
			}
			else if (cmd_param.first == "check_pos_min")
			{
				target.option &= ~Plan::NOT_CHECK_POS_MIN;
			}
			else if (cmd_param.first == "not_check_pos_min")
			{
				target.option |= Plan::NOT_CHECK_POS_MIN;
			}
			else if (cmd_param.first == "check_pos_max")
			{
				target.option &= ~Plan::NOT_CHECK_POS_MAX;
			}
			else if (cmd_param.first == "not_check_pos_max")
			{
				target.option |= Plan::NOT_CHECK_POS_MAX;
			}
			else if (cmd_param.first == "check_pos_continuous")
			{
				target.option &= ~Plan::NOT_CHECK_POS_CONTINUOUS;
			}
			else if (cmd_param.first == "not_check_pos_continuous")
			{
				target.option |= Plan::NOT_CHECK_POS_CONTINUOUS;
			}
			else if (cmd_param.first == "check_pos_continuous_at_start")
			{
				target.option &= ~Plan::NOT_CHECK_POS_CONTINUOUS_AT_START;
			}
			else if (cmd_param.first == "not_check_pos_continuous_at_start")
			{
				target.option |= Plan::NOT_CHECK_POS_CONTINUOUS_AT_START;
			}
			else if (cmd_param.first == "check_pos_continuous_second_order")
			{
				target.option &= ~Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER;
			}
			else if (cmd_param.first == "not_check_pos_continuous_second_order")
			{
				target.option |= Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER;
			}
			else if (cmd_param.first == "check_pos_continuous_second_order_at_start")
			{
				target.option &= ~Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START;
			}
			else if (cmd_param.first == "not_check_pos_continuous_second_order_at_start")
			{
				target.option |= Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START;
			}
			else if (cmd_param.first == "check_pos_following_error")
			{
				target.option &= ~Plan::NOT_CHECK_POS_FOLLOWING_ERROR;
			}
			else if (cmd_param.first == "not_check_pos_following_error")
			{
				target.option |= Plan::NOT_CHECK_POS_FOLLOWING_ERROR;
			}
			else if (cmd_param.first == "check_vel_min")
			{
				target.option &= ~Plan::NOT_CHECK_VEL_MIN;
			}
			else if (cmd_param.first == "not_check_vel_min")
			{
				target.option |= Plan::NOT_CHECK_VEL_MIN;
			}
			else if (cmd_param.first == "check_vel_max")
			{
				target.option &= ~Plan::NOT_CHECK_VEL_MAX;
			}
			else if (cmd_param.first == "not_check_vel_max")
			{
				target.option |= Plan::NOT_CHECK_VEL_MAX;
			}
			else if (cmd_param.first == "check_vel_continuous")
			{
				target.option &= ~Plan::NOT_CHECK_VEL_CONTINUOUS;
			}
			else if (cmd_param.first == "not_check_vel_continuous")
			{
				target.option |= Plan::NOT_CHECK_VEL_CONTINUOUS;
			}
			else if (cmd_param.first == "check_vel_continuous_at_start")
			{
				target.option &= ~Plan::NOT_CHECK_VEL_CONTINUOUS_AT_START;
			}
			else if (cmd_param.first == "not_check_vel_continuous_at_start")
			{
				target.option |= Plan::NOT_CHECK_VEL_CONTINUOUS_AT_START;
			}
			else if (cmd_param.first == "check_vel_following_error")
			{
				target.option &= ~Plan::NOT_CHECK_VEL_FOLLOWING_ERROR;
			}
			else if (cmd_param.first == "not_check_vel_following_error")
			{
				target.option |= Plan::NOT_CHECK_VEL_FOLLOWING_ERROR;
			}
		}
	}

	struct SetActiveMotor { std::vector<int> active_motor; };
#define SELECT_MOTOR_STRING \
		"		<UniqueParam default=\"all\">"\
		"			<Param name=\"all\" abbreviation=\"a\"/>"\
		"			<Param name=\"motion_id\" abbreviation=\"m\" default=\"0\"/>"\
		"			<Param name=\"physical_id\" abbreviation=\"p\" default=\"0\"/>"\
		"			<Param name=\"slave_id\" abbreviation=\"s\" default=\"0\"/>"\
		"		</UniqueParam>"
	auto set_active_motor(const std::map<std::string, std::string> &cmd_params, PlanTarget &target, SetActiveMotor &param)->void
	{
		param.active_motor.clear();
		param.active_motor.resize(target.controller->motionPool().size(), 0);
		
		for (auto cmd_param : cmd_params)
		{
			if (cmd_param.first == "all")
			{
				std::fill(param.active_motor.begin(), param.active_motor.end(), 1);
			}
			else if (cmd_param.first == "motion_id")
			{
				param.active_motor.at(std::stoi(cmd_param.second)) = 1;
			}
			else if (cmd_param.first == "physical_id")
			{
				param.active_motor.at(target.controller->motionAtPhy(std::stoi(cmd_param.second)).motId()) = 1;
			}
			else if (cmd_param.first == "slave_id")
			{
				param.active_motor.at(target.controller->motionAtSla(std::stoi(cmd_param.second)).motId()) = 1;
			}
		}
	}

	struct SetInputMovement 
	{
		std::vector<double> axis_begin_pos_vec;
		std::vector<double> axis_pos_vec;
		std::vector<double> axis_vel_vec;
		std::vector<double> axis_acc_vec;
		std::vector<double> axis_dec_vec;
	};
#define SET_INPUT_MOVEMENT_STRING \
		"		<Param name=\"pos\" default=\"0.5\"/>"\
		"		<Param name=\"acc\" default=\"0.1\"/>"\
		"		<Param name=\"vel\" default=\"0.1\"/>"\
		"		<Param name=\"dec\" default=\"0.1\"/>"
	auto set_input_movement(const std::map<std::string, std::string> &cmd_params, PlanTarget &target, SetInputMovement &param)->void
	{
		param.axis_begin_pos_vec.resize(target.controller->motionPool().size(), 0.0);
		for (auto cmd_param : cmd_params)
		{
			if (cmd_param.first == "pos")
			{
				auto p = target.model->calculator().calculateExpression(cmd_param.second);
				if (p.size() == 1)
				{
					param.axis_pos_vec.resize(target.controller->motionPool().size(), p.toDouble());
				}
				else if (p.size() == target.controller->motionPool().size())
				{
					param.axis_pos_vec.assign(p.begin(), p.end());
				}
				else
				{
					THROW_FILE_AND_LINE("");
				}
			}
			else if (cmd_param.first == "acc")
			{
				auto a = target.model->calculator().calculateExpression(cmd_param.second);

				if (a.size() == 1)
				{
					param.axis_acc_vec.resize(target.controller->motionPool().size(), a.toDouble());
				}
				else if (a.size() == target.controller->motionPool().size())
				{
					param.axis_acc_vec.assign(a.begin(), a.end());
				}
				else
				{
					THROW_FILE_AND_LINE("");
				}

			}
			else if (cmd_param.first == "vel")
			{
				auto v = target.model->calculator().calculateExpression(cmd_param.second);

				if (v.size() == 1)
				{
					param.axis_vel_vec.resize(target.controller->motionPool().size(), v.toDouble());
				}
				else if (v.size() == target.controller->motionPool().size())
				{
					param.axis_vel_vec.assign(v.begin(), v.end());
				}
				else
				{
					THROW_FILE_AND_LINE("");
				}
			}
			else if (cmd_param.first == "dec")
			{
				auto d = target.model->calculator().calculateExpression(cmd_param.second);

				if (d.size() == 1)
				{
					param.axis_dec_vec.resize(target.controller->motionPool().size(), d.toDouble());
				}
				else if (d.size() == target.controller->motionPool().size())
				{
					param.axis_dec_vec.assign(d.begin(), d.end());
				}
				else
				{
					THROW_FILE_AND_LINE("");
				}
			}
		}
	}
	auto check_input_movement(const std::map<std::string, std::string> &cmd_params, PlanTarget &target, SetInputMovement &param)->void
	{
		auto c = target.controller;
		for (Size i = 0; i < c->motionPool().size(); ++i)
		{
			if (param.axis_pos_vec[i] > c->motionPool()[i].maxPos() || param.axis_pos_vec[i] < c->motionPool()[i].minPos())
				THROW_FILE_AND_LINE("");

			if (param.axis_vel_vec[i] > c->motionPool()[i].maxVel() || param.axis_vel_vec[i] <= 0.0)
				THROW_FILE_AND_LINE("");

			if (param.axis_acc_vec[i] > c->motionPool()[i].maxAcc() || param.axis_acc_vec[i] <= 0.0)
				THROW_FILE_AND_LINE("");

			if (param.axis_dec_vec[i] > c->motionPool()[i].maxAcc() || param.axis_dec_vec[i] <= 0.0)
				THROW_FILE_AND_LINE("");
		}
	}

	struct EnableParam :public SetActiveMotor { std::int32_t limit_time; };
	struct Enable::Imp { Imp() {} };
	auto Enable::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		EnableParam param;
		//cout << "prepairNrt获取电机参数" << endl;
		set_check_option(params, target);
		set_active_motor(params, target, param);
		param.limit_time = std::stoi(params.at("limit_time"));

		target.param = param;
		target.option |= amc::plan::Plan::NOT_CHECK_OPERATION_ENABLE;
		//cout << "prepairNrt完成" << endl;
	}
	
	auto Enable::executeRT(PlanTarget &target)->int
	{
		//cout << "executeRT开始" << endl;
		auto &param = std::any_cast<EnableParam &>(target.param);
		bool is_all_finished = true;
		for (std::size_t i = 0; i < target.controller->motionPool().size(); ++i)
		{	
			if (param.active_motor[i])
			{
				auto &cm = target.controller->motionPool().at(i);
				auto ret = cm.enable();
				if (ret)
				{
					is_all_finished = false;

					if (target.count % 1000 == 0)
					{
						target.controller->mout() << "Unenabled motor, slave id: " << cm.id() 
							<< ", absolute id: " << i << ", ret: " << ret << std::endl;
					}
				}
			}
		}
		
		//cout << "executeRT完成" << endl;
		return (is_all_finished || target.count >= param.limit_time) ? 0 : 1;
	}
	
	Enable::~Enable() = default;
	Enable::Enable(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"en\">"
			"	<GroupParam>"
			"		<Param name=\"limit_time\" default=\"5000\"/>"
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Enable);

	struct DisableParam :public SetActiveMotor { std::int32_t limit_time; };
	struct Disable::Imp { Imp() {} };
	auto Disable::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		DisableParam param;

		set_check_option(params, target);
		set_active_motor(params, target, param);
		param.limit_time = std::stoi(params.at("limit_time"));

		target.param = param;
		target.option |= amc::plan::Plan::NOT_CHECK_OPERATION_ENABLE;
	}
	auto Disable::executeRT(PlanTarget &target)->int
	{
		auto &param = std::any_cast<DisableParam &>(target.param);

		bool is_all_finished = true;
		for (std::size_t i = 0; i < target.controller->motionPool().size(); ++i)
		{
			if (param.active_motor[i])
			{
				auto &cm = target.controller->motionPool().at(i);
				auto ret = cm.disable();
				if (ret)
				{
					is_all_finished = false;

					if (target.count % 1000 == 0)
					{
						target.controller->mout() << "Undisabled motor, slave id: " << cm.id() 
							<< ", absolute id: " << i << ", ret: " << ret << std::endl;
					}
				}
			}
		}

		return (is_all_finished || target.count >= param.limit_time) ? 0 : 1;
	}
	Disable::~Disable() = default;
	Disable::Disable(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"ds\">"
			"	<GroupParam>"
			"		<Param name=\"limit_time\" default=\"5000\"/>"
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Disable);

	struct HomeParam :public SetActiveMotor 
	{ 
		std::int32_t limit_time;
		double offset;
	};
	struct Home::Imp { Imp() {} };
	auto Home::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		HomeParam param;

		set_active_motor(params, target, param);
		param.limit_time = std::stoi(params.at("limit_time"));

		for (amc::Size i = 0; i<param.active_motor.size(); ++i)
		{
			if (param.active_motor[i])
			{
				std::int8_t method = std::stoi(params.at(std::string("method")));
				if (method < 1 || method > 35) throw std::runtime_error("invalid home method");

				param.offset = std::stod(params.at(std::string("offset")));
				std::int32_t offset = std::stoi(params.at(std::string("offset")));
				std::uint32_t high_speed = std::stoi(params.at(std::string("high_speed")));
				std::uint32_t low_speed = std::stoi(params.at(std::string("low_speed")));
				std::uint32_t acc = std::stoi(params.at(std::string("acceleration")));

				auto &cm = dynamic_cast<amc::control::EthercatMotion &>(target.controller->motionPool()[i]);
			
				cm.writeSdo(0x6098, 0x00, method);
				std::int8_t method_read;
				cm.readSdo(0x6098, 0x00, method_read);
				if (method_read != method)throw std::runtime_error("home sdo write failed method");
				cm.writeSdo(0x607C, 0x00, offset);
				std::int32_t offset_read;
				cm.readSdo(0x607C, 0x00, offset_read);
				if (offset_read != offset)throw std::runtime_error("home sdo write failed offset");
				cm.writeSdo(0x6099, 0x01, high_speed);
				std::int32_t high_speed_read;
				cm.readSdo(0x6099, 0x01, high_speed_read);
				if (high_speed_read != high_speed)throw std::runtime_error("home sdo write failed high_speed");
				cm.writeSdo(0x6099, 0x02, low_speed);
				std::int32_t low_speed_read;
				cm.readSdo(0x6099, 0x02, low_speed_read);
				if (low_speed_read != low_speed)throw std::runtime_error("home sdo write failed low_speed");
				cm.writeSdo(0x609A, 0x00, acc);
				std::int32_t acc_read;
				cm.readSdo(0x609A, 0x00, acc_read);
				if (acc_read != acc)throw std::runtime_error("home sdo write failed acc");
				
			}
		}


		target.param = param;
	}
	auto Home::executeRT(PlanTarget &target)->int
	{
		auto &param = std::any_cast<HomeParam &>(target.param);
		
		bool is_all_finished = true;
		for (std::size_t i = 0; i < target.controller->motionPool().size(); ++i)
		{
			if (param.active_motor[i])
			{
				auto &cm = target.controller->motionPool().at(i);
				auto ret = cm.home();
				if (ret)
				{
					is_all_finished = false;

					if (target.count % 1000 == 0)
					{
						target.controller->mout() << "Unhomed motor, slave id: " << cm.id()
							<< ", absolute id: " << i << ", ret: " << ret << std::endl;
					}
				}
				else
				{
					param.active_motor[i] = false;
				}
			}
		}

		return is_all_finished ? 0 : 1;
	}
	Home::~Home() = default;
	Home::Home(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"hm\">"
			"	<GroupParam>"
			"		<Param name=\"method\" default=\"35\"/>"
			"		<Param name=\"offset\" default=\"0\"/>"
			"		<Param name=\"high_speed\" default=\"20000\"/>"
			"		<Param name=\"low_speed\" default=\"20\"/>"
			"		<Param name=\"acceleration\" default=\"100000\"/>"
			"		<Param name=\"limit_time\" default=\"5000\"/>"
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Home);

	struct ModeParam :public SetActiveMotor { std::int32_t limit_time, mode; };
	struct Mode::Imp { Imp() {} };
	auto Mode::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		ModeParam param;

		set_check_option(params, target);
		set_active_motor(params, target, param);
		param.limit_time = std::stoi(params.at("limit_time"));
		param.mode = std::stoi(params.at("mode"));

		if (param.mode > 10 && param.mode < 8)throw std::runtime_error("invalid mode, amc now only support mode 8,9,10");

		target.param = param;
		target.option |= amc::plan::Plan::NOT_CHECK_OPERATION_ENABLE;
	}
	auto Mode::executeRT(PlanTarget &target)->int
	{
		auto &param = std::any_cast<ModeParam &>(target.param);

		//bool is_all_finished = true;
		for (std::size_t i = 0; i < target.controller->motionPool().size(); ++i)
		{
			if (param.active_motor[i])
			{
				auto &cm = target.controller->motionPool().at(i);
				auto ret = cm.mode(param.mode);
				if (target.count == 1) 
				{
					switch (param.mode)
					{
					case 8:
						cm.setTargetPos(cm.actualPos());
						break;
					case 9:
						cm.setTargetVel(0.0);
						break;
					case 10:
						cm.setTargetCur(0.0); //30mA关节一可以逆时针拖动,-30mA则只能顺时针拖动
						break;
					default:
						break;
					}
				}

				if (ret)
				{
					//is_all_finished = false;

					if (target.count % 1000 == 0)
					{
						target.controller->mout() << "Unmoded motor, slave id: " << cm.id() 
							<< ", absolute id: " << i << ", ret: " << ret << std::endl;
					}
				}
			}
		}

		///////////////////////////////////////////////log/////////////////////////////////////////////////
		target.controller->lout() << target.count << " " << 0 << " " << 1 << " " << 2 << " " << 3 << " " << 4 << " " << 5 << "  ";
		for (auto &cm : target.controller->motionPool())
		{
			target.controller->lout() << "  " << cm.targetCur() << "  " << cm.actualPos() << "  " << cm.actualVel() << "  " << cm.actualCur() << "  ";
		}
		target.controller->lout() << "\n";
		//////////////////////////////////////////////////////////////////////////////////////////////////

		return (target.count >= param.limit_time) ? 0 : 1;
	}
	Mode::~Mode() = default;
	Mode::Mode(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"md\">"
			"	<GroupParam>"
			"		<Param name=\"limit_time\" default=\"5000\"/>"
			"       <Param name=\"mode\" abbreviation=\"d\" default=\"8\"/>"
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Mode);

	struct DragParam :public SetActiveMotor
	{
		double fc1,fv1,fc2,fv2,g21,g22,g23,fc3,fv3,g31,g32,fc4,fv4,g4,fc5,fv5;
		std::int32_t limit_time,mode;
	};
	struct Drag::Imp {Imp() {} };
	auto Drag::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		DragParam param;
		set_check_option(params, target);
		set_active_motor(params, target, param);
		param.limit_time = std::stoi(params.at("limit_time"));
		param.mode = std::stoi(params.at("mode"));
		param.fc1 = std::stoi(params.at("fc1"));
		param.fv1 = std::stoi(params.at("fv1"));
		param.fc2 = std::stoi(params.at("fc2"));
		param.fv2 = std::stoi(params.at("fv2"));
		param.g21 = std::stoi(params.at("g21"));
		param.g22 = std::stoi(params.at("g22"));
		param.g23 = std::stoi(params.at("g23"));
		param.fc3 = std::stoi(params.at("fc3"));
		param.fv3 = std::stoi(params.at("fv3"));
		param.g31 = std::stoi(params.at("g31"));
		param.g32 = std::stoi(params.at("g32"));
		param.fc4 = std::stoi(params.at("fc4"));
		param.fv4 = std::stoi(params.at("fv4"));
		param.g4 = std::stoi(params.at("g4"));
		param.fc5 = std::stoi(params.at("fc5"));
		param.fv5 = std::stoi(params.at("fv5"));
		

		if (param.mode > 10 && param.mode < 8)throw std::runtime_error("invalid mode, amc now only support mode 8,9,10");

		target.param = param;
		target.option |= amc::plan::Plan::NOT_CHECK_OPERATION_ENABLE;
		
	}
	auto Drag::executeRT(PlanTarget &target)->int
	{
		auto &param = std::any_cast<DragParam &>(target.param);
		auto controller = target.controller;
		
		auto &cm0 = controller->motionPool().at(0);
		auto &cm1 = controller->motionPool().at(1);
		auto &cm2 = controller->motionPool().at(2);
		auto &cm3 = controller->motionPool().at(3);
		auto &cm4 = controller->motionPool().at(4);
		auto &cm5 = controller->motionPool().at(5);
		
		double F1,F2,F3,F4,F5;

		for (std::size_t i = 0; i < controller->motionPool().size(); ++i)
		{
			if (param.active_motor[0])
			{
				auto ret0 = cm0.mode(param.mode);
				if (target.count <= param.limit_time) 
				{
					switch (param.mode)
					{
					case 8:
						cm0.setTargetPos(cm0.actualPos());
						break;
					case 9:
						cm0.setTargetVel(0.0);
						break;
					case 10:  // 关节1补偿算法
						{
							
							F1 = sign(cm0.actualVel()) * (param.fc1)  + (param.fv1) * (cm0.actualVel() * 2 * PI / 60 / 100);
							// if (cm.actualVel() >= 0)
							// 	F = param.fc1  + (param.fv1) * (cm.actualVel() * 2 * PI / 60 / 100);
							// else
							// 	F = -1.0 * param.fc1 + (param.fv1) * (cm.actualVel() * 2 * PI / 60 / 100);
							// std::cout << "各项参数:" << endl;
							// std::cout << "fc1:" << param.fc1 << endl;
							// std::cout << "fv1:" << param.fv1 << endl;
							// std::cout << "vel:" << cm.actualVel() << endl;
							// std::cout << "back1:" << cm.actualVel() * 2 * PI / 60 / 100 << endl;
							// std::cout << "back2:" << (param.fv1) * (cm.actualVel() * 2 * PI / 60 / 100) << endl;
							// std::cout << "all:" << -1.0 * param.fc1 + (param.fv1) * (cm.actualVel() * 2 * PI / 60 / 100) << endl;
							// std::cout << "F:" << F << endl;
							if (F1 > 100)
							{
								F1 = 100;
							}
							else if (F1 < -100)
							{
								F1 = -100;
							}
							cm0.setTargetCur(F1);
							break;
						}
					default:
						break;
					}
				}
			}

			if (param.active_motor[1])
			{
				auto ret1 = cm1.mode(param.mode);
				if (target.count <= param.limit_time) 
				{
					switch (param.mode)
					{
					case 8:
						cm1.setTargetPos(cm1.actualPos());
						break;
					case 9:
						cm1.setTargetVel(0.0);
						break;
					case 10:  // 关节2补偿算法
						{
							
							F2 = (param.g21) * sin(cm1.actualPos() * PI / 180) + (param.g22) * sin((cm1.actualPos() - cm2.actualPos()) * PI / 180) + (param.g23) * sin((cm1.actualPos() - cm2.actualPos() + cm3.actualPos()) * PI / 180) + sign(cm1.actualVel()) * (param.fc2)  + (param.fv2) * (cm1.actualVel() * 2 * PI / 60 / 100);
							
							if (F2 > 600)
							{
								F2 = 600;
							}
							else if (F2 < -600)
							{
								F2 = -600;
							}
							cm1.setTargetCur(F2);
							break;
						}
					default:
						break;
					}
				}
			}

			if (param.active_motor[2])
			{
				auto ret2 = cm2.mode(param.mode);
				if (target.count <= param.limit_time) 
				{
					switch (param.mode)
					{
					case 8:
						cm2.setTargetPos(cm2.actualPos());
						break;
					case 9:
						cm2.setTargetVel(0.0);
						break;
					case 10:  // 关节3补偿算法
						{
							
							F3 = (param.g31) * sin((cm1.actualPos() - cm2.actualPos()) * PI / 180) + (param.g32) * sin((cm1.actualPos() - cm2.actualPos() + cm3.actualPos()) * PI / 180) + sign(cm2.actualVel()) * (param.fc3)  + (param.fv3) * (cm2.actualVel() * 2 * PI / 60 / 100);
							
							if (F3 > 400)
							{
								F3 = 400;
							}
							else if (F3 < -400)
							{
								F3 = -400;
							}
							cm2.setTargetCur(F3);
							break;
						}
					default:
						break;
					}
				}
			}

			if (param.active_motor[3])
			{
				auto ret3 = cm3.mode(param.mode);
				if (target.count <= param.limit_time) 
				{
					switch (param.mode)
					{
					case 8:
						cm3.setTargetPos(cm3.actualPos());
						break;
					case 9:
						cm3.setTargetVel(0.0);
						break;
					case 10:  // 关节4补偿算法
						{
							
							F4 = (param.g4) * sin((cm1.actualPos() - cm2.actualPos() + cm3.actualPos()) * PI / 180) + sign(cm3.actualVel()) * (param.fc4)  + (param.fv4) * (cm3.actualVel() * 2 * PI / 60 / 100);
							
							if (F4 > 200)
							{
								F4 = 200;
							}
							else if (F4 < -200)
							{
								F4 = -200;
							}
							cm3.setTargetCur(F4);
							break;
						}
					default:
						break;
					}
				}
			}

			if (param.active_motor[4])
			{
				auto ret4 = cm4.mode(param.mode);
				if (target.count <= param.limit_time) 
				{
					switch (param.mode)
					{
					case 8:
						cm4.setTargetPos(cm4.actualPos());
						break;
					case 9:
						cm4.setTargetVel(0.0);
						break;
					case 10:  // 关节5补偿算法
						{
							
							F5 = sign(cm4.actualVel()) * (param.fc5)  + (param.fv5) * (cm4.actualVel() * 2 * PI / 60 / 100);
							
							if (F5 > 100)
							{
								F5 = 100;
							}
							else if (F5 < -100)
							{
								F5 = -100;
							}
							cm4.setTargetCur(F5);
							break;
						}
					default:
						break;
					}
				}
			}

			if (param.active_motor[5])
			{
				auto ret5 = cm5.mode(param.mode);
				if (target.count <= param.limit_time) 
				{
					switch (param.mode)
					{
					case 8:
						cm5.setTargetPos(cm5.actualPos());
						break;
					case 9:
						cm5.setTargetVel(0.0);
						break;
					case 10:  // 关节6补偿算法(关节6因无负载暂时不用补偿)
						cm5.setTargetCur(30.0);
						break;
					default:
						break;
					}
				}
			}

		}

		///////////////////////////////////////////////log/////////////////////////////////////////////////
		target.controller->lout() << target.count << " " << 0 << " " << 1 << " " << 2 << " " << 3 << " " << 4 << " " << 5 << "  ";
		for (auto &cm : target.controller->motionPool())
		{
			target.controller->lout() << "  " << cm.targetCur() << "  " << cm.actualPos() << "  " << cm.actualVel() << "  " << cm.actualCur() << "  ";
		}
		target.controller->lout() << "\n";
		//////////////////////////////////////////////////////////////////////////////////////////////////

		return (target.count >= param.limit_time) ? 0 : 1;
	}
	Drag::~Drag() = default;
	Drag::Drag(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"dg\">"
			"	<GroupParam>"
			"		<Param name=\"limit_time\" default=\"5000\"/>"
			"       <Param name=\"mode\" abbreviation=\"d\" default=\"8\"/>"
			"		<Param name=\"fc1\" default=\"0\"/>"
			"		<Param name=\"fv1\" default=\"0\"/>"
			"		<Param name=\"fc2\" default=\"0\"/>"
			"		<Param name=\"fv2\" default=\"0\"/>"
			"		<Param name=\"g21\" default=\"0\"/>"
			"		<Param name=\"g22\" default=\"0\"/>"
			"		<Param name=\"g23\" default=\"0\"/>"
			"		<Param name=\"fc3\" default=\"0\"/>"
			"		<Param name=\"fv3\" default=\"0\"/>"
			"		<Param name=\"g31\" default=\"0\"/>"
			"		<Param name=\"g32\" default=\"0\"/>"
			"		<Param name=\"fc4\" default=\"0\"/>"
			"		<Param name=\"fv4\" default=\"0\"/>"
			"		<Param name=\"g4\" default=\"0\"/>"
			"		<Param name=\"fc5\" default=\"0\"/>"
			"		<Param name=\"fv5\" default=\"0\"/>"
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Drag);

	struct ResetParam :public SetActiveMotor, SetInputMovement { std::vector<Size> total_count_vec; };
	struct Reset::Imp { Imp() {} };
	auto Reset::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		ResetParam param;
		set_check_option(params, target);
		set_active_motor(params, target, param);
		set_input_movement(params, target, param);

		for (Size i = 0; i < target.controller->motionPool().size(); ++i)
		{
			auto &cm = target.controller->motionPool()[i];
			param.axis_pos_vec[i] = param.axis_pos_vec[i] * (cm.maxPos() - cm.minPos()) + cm.minPos();
			param.axis_acc_vec[i] = param.axis_acc_vec[i] * cm.maxAcc();
			param.axis_vel_vec[i] = param.axis_vel_vec[i] * cm.maxVel();
			param.axis_dec_vec[i] = param.axis_dec_vec[i] * cm.maxAcc();
		}
		check_input_movement(params, target, param);

		param.total_count_vec.resize(target.controller->motionPool().size(), 1);

		target.option |=
			amc::plan::Plan::EXECUTE_WHEN_ALL_PLAN_COLLECTED |
			amc::plan::Plan::NOT_CHECK_POS_CONTINUOUS_AT_START |
			amc::plan::Plan::NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START;

		target.param = param;
	}
	auto Reset::executeRT(PlanTarget &target)->int
	{
		auto &param = std::any_cast<ResetParam&>(target.param);

		// 取得起始位置 //
		if (target.count == 1)
		{
			for (Size i = 0; i < target.controller->motionPool().size(); ++i)
			{
				if (param.active_motor[i])
				{
					param.axis_begin_pos_vec[i] = target.controller->motionPool().at(i).actualPos();
				}
			}
		}

		// 设置驱动器的位置 //
		for (Size i = 0; i < target.controller->motionPool().size(); ++i)
		{
			if (param.active_motor[i])
			{
				double p, v, a;
				amc::plan::moveAbsolute(target.count, param.axis_begin_pos_vec[i], param.axis_pos_vec[i], param.axis_vel_vec[i] / 1000
					, param.axis_acc_vec[i] / 1000 / 1000, param.axis_dec_vec[i] / 1000 / 1000, p, v, a, param.total_count_vec[i]);
				target.controller->motionAtAbs(i).setTargetPos(p);
			}
		}

		return (static_cast<int>(*std::max_element(param.total_count_vec.begin(), param.total_count_vec.end())) > target.count) ? 1 : 0;
	}
	Reset::~Reset() = default;
	Reset::Reset(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"rs\">"
			"	<GroupParam>"
					SET_INPUT_MOVEMENT_STRING
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Reset);

	struct RecoverParam
	{
		std::atomic_bool is_kinematic_ready_;
		std::atomic_bool is_rt_waiting_ready_;
		std::future<void> fut;
		int kin_ret;
	};
	auto Recover::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		auto p = std::make_shared<RecoverParam>();

		p->is_kinematic_ready_ = false;
		p->is_rt_waiting_ready_ = false;
		p->fut = std::async(std::launch::async, [](std::shared_ptr<RecoverParam> p, PlanTarget &target)
		{
			// 等待正解求解的需求 //
			while (!p->is_rt_waiting_ready_.load())std::this_thread::sleep_for(std::chrono::milliseconds(1));

			// 求正解 //
			p->kin_ret = target.model->solverPool()[1].kinPos() ? 0 : -1;

			// 通知实时线程 //
			p->is_kinematic_ready_.store(true);
		}, p, std::ref(target));

		target.param = p;
		target.option |= 
			NOT_CHECK_POS_CONTINUOUS_AT_START | 
			NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START |
			NOT_CHECK_OPERATION_ENABLE;
	}
	auto Recover::executeRT(PlanTarget &target)->int
	{
		auto param = std::any_cast<std::shared_ptr<RecoverParam> &>(target.param);

		if (target.count == 1)
		{
			for (Size i = 0; i < std::min(target.controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				target.controller->motionPool()[i].setTargetPos(target.controller->motionPool().at(i).actualPos());
				target.model->motionPool()[i].setMp(target.controller->motionPool().at(i).actualPos());
			}

			param->is_rt_waiting_ready_.store(true);

			return 1;
		}

		return param->is_kinematic_ready_.load() ? param->kin_ret : 1;
	}
	auto Recover::collectNrt(PlanTarget &target)->void
	{
		if (target.count)
		{
			std::any_cast<std::shared_ptr<RecoverParam>&>(target.param)->fut.get();
		}
		else
		{
			// 此时前面指令出错，系统清理了该命令，这时设置一下 //
			std::any_cast<std::shared_ptr<RecoverParam>&>(target.param)->is_rt_waiting_ready_.store(true);
			std::any_cast<std::shared_ptr<RecoverParam>&>(target.param)->fut.get();
		}
	}
	Recover::~Recover() = default;
	Recover::Recover(const std::string &name) :Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"rc\">"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Recover);

	struct SleepParam { int count; };
	struct Sleep::Imp {};
	auto Sleep::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		target.param = SleepParam{ std::stoi(params.at("count")) };
		target.option |= NOT_CHECK_OPERATION_ENABLE;
	}
	auto Sleep::executeRT(PlanTarget &target)->int { return std::any_cast<SleepParam&>(target.param).count - target.count; }
	Sleep::~Sleep() = default;
	Sleep::Sleep(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"sl\">"
			"	<Param name=\"count\" default=\"1000\" abbreviation=\"c\"/>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Sleep);

	auto Show::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		target.option |=
			NOT_CHECK_OPERATION_ENABLE |
			NOT_CHECK_POS_MIN |
			NOT_CHECK_POS_MAX |
			NOT_CHECK_POS_CONTINUOUS |
			NOT_CHECK_POS_CONTINUOUS_AT_START |
			NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER |
			NOT_CHECK_POS_CONTINUOUS_SECOND_ORDER_AT_START |
			NOT_CHECK_POS_FOLLOWING_ERROR |
			NOT_CHECK_VEL_MIN |
			NOT_CHECK_VEL_MAX |
			NOT_CHECK_VEL_CONTINUOUS |
			NOT_CHECK_VEL_CONTINUOUS_AT_START |
			NOT_CHECK_VEL_FOLLOWING_ERROR;
	}
	auto Show::executeRT(PlanTarget &target)->int
	{
		target.controller->mout() << "pos: ";
		for (auto &m : target.controller->motionPool())
		{
			target.controller->mout() << std::setprecision(15) << m.actualPos() << "   ";
		}
		target.controller->mout() << std::endl;

		return 0;
	}
	Show::Show(const std::string &name) : Plan(name)
	{
		command().loadXmlStr("<Command name=\"sh\"/>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(Show);

	struct MoveAbsJParam :public SetActiveMotor, SetInputMovement{};
	auto MoveAbsJ::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		MoveAbsJParam param;
		//cout << "prepairNrt获取参数" << endl;
		set_check_option(params, target);
		set_active_motor(params, target, param);
		set_input_movement(params, target, param);
		check_input_movement(params, target, param);

		param.axis_begin_pos_vec.resize(target.controller->motionPool().size());
		target.param = param;
		//cout << "prepairNrt任务完成" << endl;
	}
	auto MoveAbsJ::executeRT(PlanTarget &target)->int
	{
		auto param = std::any_cast<MoveAbsJParam>(&target.param);
		auto controller = target.controller;
		//cout << "executeRT获取参数" << endl;
		if (target.count == 1)
		{
			for (Size i = 0; i < param->active_motor.size(); ++i)
			{
				if (param->active_motor[i])
				{
					param->axis_begin_pos_vec[i] = target.controller->motionPool()[i].targetPos();
				}
			}
		}
		cout << "executeRT每周期发送六个关节位置如下:" << endl;
		amc::Size total_count{ 1 };
		for (Size i = 0; i < param->active_motor.size(); ++i)
		{
			if (param->active_motor[i])
			{
				double p, v, a;
				amc::Size t_count;
				amc::plan::moveAbsolute(target.count,
					param->axis_begin_pos_vec[i], param->axis_pos_vec[i],
					param->axis_vel_vec[i] / 1000, param->axis_acc_vec[i] / 1000 / 1000, param->axis_dec_vec[i] / 1000 / 1000,
					p, v, a, t_count);
				target.controller->motionPool()[i].setTargetPos(p);
				total_count = std::max(total_count, t_count);
			}
		}
		//抓取操作,有点问题
		// if (target.count == total_count)
		// {
		// 	serialPort myserial;
		// 	myserial.OpenPort(dev);
		// 	myserial.setup(115200,0,8,1,'N');
		// 	//抓取
		// 	close(buff1,myserial);
		// 	sleep(2);//休眠2秒
		// 	//read_state(buff,myserial);
		// 	//松开
		// 	open(buff2,myserial);
		// 	sleep(2);
		// 	read_state(buff,myserial);
		// }
		
		///////////////////////////////////////////////log/////////////////////////////////////////////////
		target.controller->lout() << target.count << " " << param->axis_begin_pos_vec[0] << " " << param->axis_begin_pos_vec[1] << " " << param->axis_begin_pos_vec[2] << " " << param->axis_begin_pos_vec[3] << " " << param->axis_begin_pos_vec[4] << " " << param->axis_begin_pos_vec[5] << "  ";
		for (auto &cm : controller->motionPool())
		{
			target.controller->lout() << "  " << cm.targetPos() << "  " << cm.actualPos() << "  " << cm.actualVel() << "  " << cm.actualCur() << "  ";
		}
		target.controller->lout() << "\n";
		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		//cout << "executeRT任务完成" << endl;
		return total_count - target.count;
	}
	struct MoveAbsJ::Imp {};
	MoveAbsJ::~MoveAbsJ() = default;
	MoveAbsJ::MoveAbsJ(const std::string &name) : Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"mvaj\">"
			"	<GroupParam>"
			"		<Param name=\"pos\" default=\"0.0\"/>"
			"		<Param name=\"vel\" default=\"1.0\"/>"
			"		<Param name=\"acc\" default=\"1.0\"/>"
			"		<Param name=\"dec\" default=\"1.0\"/>"
					SELECT_MOTOR_STRING
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(MoveAbsJ);

	auto check_eul_validity(const std::string &eul_type)->bool
	{
		if (eul_type.size()<3)return false;

		for (int i = 0; i < 3; ++i)if (eul_type[i] > '3' || eul_type[i] < '1')return false;

		if (eul_type[0] == eul_type[1] || eul_type[1] == eul_type[2]) return false;

		return true;
	}
	auto find_pq(const std::map<std::string, std::string> &params, PlanTarget &target, double *pq_out)->bool
	{
		double pos_unit;
		auto pos_unit_found = params.find("pos_unit");
		if (pos_unit_found == params.end()) pos_unit = 1.0;
		else if (pos_unit_found->second == "m")pos_unit = 1.0;
		else if (pos_unit_found->second == "mm")pos_unit = 0.001;
		else if (pos_unit_found->second == "cm")pos_unit = 0.01;
		else THROW_FILE_AND_LINE("");

		for (auto cmd_param : params)
		{
			if (cmd_param.first == "pq")
			{
				auto pq_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pq_mat.size() != 7)THROW_FILE_AND_LINE("");
				amc::dynamic::s_vc(7, pq_mat.data(), pq_out);
				amc::dynamic::s_nv(3, pos_unit, pq_out);
				return true;
			}
			else if (cmd_param.first == "pm")
			{
				auto pm_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pm_mat.size() != 16)THROW_FILE_AND_LINE("");
				amc::dynamic::s_pm2pq(pm_mat.data(), pq_out);
				amc::dynamic::s_nv(3, pos_unit, pq_out);
				return true;
			}
			else if (cmd_param.first == "pe")
			{
				double ori_unit;
				auto ori_unit_found = params.find("ori_unit");
				if (ori_unit_found == params.end()) ori_unit = 1.0;
				else if (ori_unit_found->second == "rad")ori_unit = 1.0;
				else if (ori_unit_found->second == "degree")ori_unit = PI / 180.0;
				else THROW_FILE_AND_LINE("");

				std::string eul_type;
				auto eul_type_found = params.find("eul_type");
				if (eul_type_found == params.end()) eul_type = "321";
				else if (check_eul_validity(eul_type_found->second.data()))	eul_type = eul_type_found->second;
				else THROW_FILE_AND_LINE("");

				auto pe_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe_mat.size() != 6)THROW_FILE_AND_LINE("");
				amc::dynamic::s_nv(3, ori_unit, pe_mat.data() + 3);
				amc::dynamic::s_pe2pq(pe_mat.data(), pq_out, eul_type.data());
				amc::dynamic::s_nv(3, pos_unit, pq_out);
				return true;
			}
			else if (cmd_param.first == "pe1")
			{
				double ori_unit;
				auto ori_unit_found = params.find("ori_unit");
				if (ori_unit_found == params.end()) ori_unit = 1.0;
				else if (ori_unit_found->second == "rad")ori_unit = 1.0;
				else if (ori_unit_found->second == "degree")ori_unit = PI / 180.0;
				else THROW_FILE_AND_LINE("");

				std::string eul_type;
				auto eul_type_found = params.find("eul_type");
				if (eul_type_found == params.end()) eul_type = "321";
				else if (check_eul_validity(eul_type_found->second.data()))	eul_type = eul_type_found->second;
				else THROW_FILE_AND_LINE("");

				auto pe_mat_1 = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe_mat_1.size() != 6)THROW_FILE_AND_LINE("");
				amc::dynamic::s_nv(3, ori_unit, pe_mat_1.data() + 3);
				amc::dynamic::s_pe2pq(pe_mat_1.data(), pq_out, eul_type.data());
				amc::dynamic::s_nv(3, pos_unit, pq_out);
				return true;
			}
			else if (cmd_param.first == "pe2")
			{
				double ori_unit;
				auto ori_unit_found = params.find("ori_unit");
				if (ori_unit_found == params.end()) ori_unit = 1.0;
				else if (ori_unit_found->second == "rad")ori_unit = 1.0;
				else if (ori_unit_found->second == "degree")ori_unit = PI / 180.0;
				else THROW_FILE_AND_LINE("");

				std::string eul_type;
				auto eul_type_found = params.find("eul_type");
				if (eul_type_found == params.end()) eul_type = "321";
				else if (check_eul_validity(eul_type_found->second.data()))	eul_type = eul_type_found->second;
				else THROW_FILE_AND_LINE("");

				auto pe_mat_2 = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe_mat_2.size() != 6)THROW_FILE_AND_LINE("");
				amc::dynamic::s_nv(3, ori_unit, pe_mat_2.data() + 3);
				amc::dynamic::s_pe2pq(pe_mat_2.data(), pq_out, eul_type.data());
				amc::dynamic::s_nv(3, pos_unit, pq_out);
				return true;
			}
			else if (cmd_param.first == "pe3")
			{
				double ori_unit;
				auto ori_unit_found = params.find("ori_unit"); 
				if (ori_unit_found == params.end()) ori_unit = 1.0;
				else if (ori_unit_found->second == "rad")ori_unit = 1.0;
				else if (ori_unit_found->second == "degree")ori_unit = PI / 180.0;
				else THROW_FILE_AND_LINE("");

				std::string eul_type;
				auto eul_type_found = params.find("eul_type");
				if (eul_type_found == params.end()) eul_type = "321";
				else if (check_eul_validity(eul_type_found->second.data()))	eul_type = eul_type_found->second;
				else THROW_FILE_AND_LINE("");

				auto pe_mat_3 = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe_mat_3.size() != 6)THROW_FILE_AND_LINE("");
				amc::dynamic::s_nv(3, ori_unit, pe_mat_3.data() + 3);
				amc::dynamic::s_pe2pq(pe_mat_3.data(), pq_out, eul_type.data());
				amc::dynamic::s_nv(3, pos_unit, pq_out);
				return true;
			}
			
		}

		THROW_FILE_AND_LINE("No pose input");
	}
	struct MoveJParam
	{
		std::vector<double> joint_vel, joint_acc, joint_dec, ee_pq, joint_pos_begin, joint_pos_end;
		std::vector<Size> total_count;
	};
	struct MoveJ::Imp {};
	auto MoveJ::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{	
		MoveJParam mvj_param;
		set_check_option(params, target);
		
		//cout << "prepairNrt获取参数" << endl;
		// find ee pq //
		mvj_param.ee_pq.resize(7);
		find_pq(params, target, mvj_param.ee_pq.data());

		mvj_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);
		mvj_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);
		mvj_param.total_count.resize(target.model->motionPool().size(), 0);

		// find joint acc/vel/dec/
		for (auto cmd_param : params)
		{
			auto c = target.controller;
			if (cmd_param.first == "joint_acc")
			{
				mvj_param.joint_acc.clear();
				mvj_param.joint_acc.resize(target.model->motionPool().size(), 0.0);

				auto acc_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (acc_mat.size() == 1)std::fill(mvj_param.joint_acc.begin(), mvj_param.joint_acc.end(), acc_mat.toDouble());
				else if (acc_mat.size() == target.model->motionPool().size()) std::copy(acc_mat.begin(), acc_mat.end(), mvj_param.joint_acc.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (mvj_param.joint_acc[i] <= 0 || mvj_param.joint_acc[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_vel")
			{
				mvj_param.joint_vel.clear();
				mvj_param.joint_vel.resize(target.model->motionPool().size(), 0.0);

				auto vel_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (vel_mat.size() == 1)std::fill(mvj_param.joint_vel.begin(), mvj_param.joint_vel.end(), vel_mat.toDouble());
				else if (vel_mat.size() == target.model->motionPool().size()) std::copy(vel_mat.begin(), vel_mat.end(), mvj_param.joint_vel.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (mvj_param.joint_vel[i] <= 0 || mvj_param.joint_vel[i] > c->motionPool()[i].maxVel())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_dec")
			{
				mvj_param.joint_dec.clear();
				mvj_param.joint_dec.resize(target.model->motionPool().size(), 0.0);

				auto dec_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (dec_mat.size() == 1)std::fill(mvj_param.joint_dec.begin(), mvj_param.joint_dec.end(), dec_mat.toDouble());
				else if (dec_mat.size() == target.model->motionPool().size()) std::copy(dec_mat.begin(), dec_mat.end(), mvj_param.joint_dec.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (mvj_param.joint_dec[i] <= 0 || mvj_param.joint_dec[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
		}

		target.param = mvj_param;
		
		//cout << "prepairNrt完成" << endl;
	}
	
	auto MoveJ::executeRT(PlanTarget &target)->int
	{
		auto mvj_param = std::any_cast<MoveJParam>(&target.param);
		
		auto controller = target.controller;

		auto &cm0 = controller->motionPool().at(0);
		auto &cm1 = controller->motionPool().at(1);
		auto &cm2 = controller->motionPool().at(2);
		auto &cm3 = controller->motionPool().at(3);
		auto &cm4 = controller->motionPool().at(4);
		auto &cm5 = controller->motionPool().at(5);
		
		double F1,F2,F3,F4,F5;
		double y0,y1,y2,y3,y4,y5;
		//cout << "executeRT获取参数" << endl;
		// 取得起始位置 //
		double p, v, a;
		static Size max_total_count;
		if (target.count == 1)
		{
			// inverse kinematic //
			double end_pm[16];
			amc::dynamic::s_pq2pm(mvj_param->ee_pq.data(), end_pm);
			end_pm[7] = -end_pm[7];
			target.model->generalMotionPool().at(0).setMpm(end_pm);
			cout << "终点位姿为:" << endl;
			amc::dynamic::dsp(4, 4, end_pm);

			if (!target.model->solverPool().at(1).kinPos())return -1;

			// init joint_pos //
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				mvj_param->joint_pos_begin[i] = controller->motionPool()[i].targetPos();
				mvj_param->joint_pos_end[i] = target.model->motionPool()[i].mp() * (180.0) / PI;
				amc::plan::moveAbsolute(target.count, mvj_param->joint_pos_begin[i], mvj_param->joint_pos_end[i]
					, mvj_param->joint_vel[i] / 1000, mvj_param->joint_acc[i] / 1000 / 1000, mvj_param->joint_dec[i] / 1000 / 1000
					, p, v, a, mvj_param->total_count[i]);
				
				
			}

			max_total_count = *std::max_element(mvj_param->total_count.begin(), mvj_param->total_count.end());
		}
		std::cout << "executeRT每周期发送六个关节位置如下:" << endl;
		//cout << "executeRT每周期发送六个关节速度如下:" << endl;
		//cout << "executeRT每周期输入六个关节力矩如下:" << endl;
		for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
		{
			amc::plan::moveAbsolute(static_cast<double>(target.count) * mvj_param->total_count[i] / max_total_count,
				mvj_param->joint_pos_begin[i], mvj_param->joint_pos_end[i],
				mvj_param->joint_vel[i] / 1000, mvj_param->joint_acc[i] / 1000 / 1000, mvj_param->joint_dec[i] / 1000 / 1000,
				p, v, a, mvj_param->total_count[i]);
			
			
			controller->motionPool()[i].setTargetPos(p);
			// controller->motionPool()[i].setTargetVel(v);
			//target.model->motionPool().at(i).setMa(a);
			//target.model->solverPool().at(2).dynAccAndFce();// 采用逆动力学求解电机输入力矩
			/*		
			//碰撞检测算法(只针对指令mvj --pe={0.6,0.6,0.6,0,0,0}有效)
			if (target.count >= 1)
			{
				y0 = (-7.593e-6) * (target.count) + 30.11;
				y1 = (0.001658) * (target.count) - 11.56;
				y2 = (-0.001497) * (target.count) + 5.779;
				y3 = (9.178e-5) * (target.count) + 48.15;
				y4 = (-5.413e-6) * (target.count) - 47.31;
				y5 = (2.142e-5) * (target.count) + 47.28;
				if ((target.count >= 2000) && (Abs(cm0.actualCur() - y0) >= 20 || Abs(cm1.actualCur() - y1) >= 80 || Abs(cm2.actualCur() - y2) >= 100 
					|| Abs(cm3.actualCur() - y3) >= 60 || Abs(cm4.actualCur() - y4) >= 40 || Abs(cm5.actualCur() - y5) >= 50))
				{
					// 停止运动
					// auto ret0 = cm0.disable();
					// auto ret1 = cm1.disable();
					// auto ret2 = cm2.disable();
					// auto ret3 = cm3.disable();
					// auto ret4 = cm4.disable();
					// auto ret5 = cm5.disable();
					// target.server->stop();
					// target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION;
				
					// 切换电流模式并加入补偿
					// auto ret0 = cm0.mode(10);
					// auto ret1 = cm1.mode(10);
					// auto ret2 = cm2.mode(10);
					// auto ret3 = cm3.mode(10);
					// auto ret4 = cm4.mode(10);
					// auto ret5 = cm5.mode(10);

					// F1 = sign(cm0.actualVel()) * (30)  + (50) * (cm0.actualVel() * 2 * PI / 60 / 100);
					// F2 = (-300) * sin(cm1.actualPos() * PI / 180) + (-100) * sin((cm1.actualPos() - cm2.actualPos()) * PI / 180) + (-20) * sin((cm1.actualPos() - cm2.actualPos() + cm3.actualPos()) * PI / 180) + sign(cm1.actualVel()) * (20)  + (40) * (cm1.actualVel() * 2 * PI / 60 / 100);
					// F3 = (200) * sin((cm1.actualPos() - cm2.actualPos()) * PI / 180) + (70) * sin((cm1.actualPos() - cm2.actualPos() + cm3.actualPos()) * PI / 180) + sign(cm2.actualVel()) * (10)  + (40) * (cm2.actualVel() * 2 * PI / 60 / 100);
					// F4 = (-40) * sin((cm1.actualPos() - cm2.actualPos() + cm3.actualPos()) * PI / 180) + sign(cm3.actualVel()) * (10)  + (20) * (cm3.actualVel() * 2 * PI / 60 / 100);
					// F5 = sign(cm4.actualVel()) * (20)  + (40) * (cm4.actualVel() * 2 * PI / 60 / 100);
							
					// if (F1 > 100)
					// {
					// 	F1 = 100;
					// }
					// else if (F1 < -100)
					// {
					// 	F1 = -100;
					// }
					// if (F2 > 600)
					// {
					// 	F2 = 600;
					// }
					// else if (F2 < -600)
					// {
					// 	F2 = -600;
					// }
					// if (F3 > 400)
					// {
					// 	F3 = 400;
					// }
					// else if (F3 < -400)
					// {
					// 	F3 = -400;
					// }
					// if (F4 > 200)
					// {
					// 	F4 = 200;
					// }
					// else if (F4 < -200)
					// {
					// 	F4 = -200;
					// }
					// if (F5 > 100)
					// {
					// 	F5 = 100;
					// }
					// else if (F5 < -100)
					// {
					// 	F5 = -100;
					// }

					// cm0.setTargetCur(F1);
					// cm1.setTargetCur(F2);
					// cm2.setTargetCur(F3);
					// cm3.setTargetCur(F4);
					// cm4.setTargetCur(F5);
					// cm5.setTargetCur(30.0);
				
				
				}	
			}
			*/
		}
		//cout << "executeRT任务成功" << endl;

		///////////////////////////////////////////////log/////////////////////////////////////////////////
		controller->lout() << target.count << " " << mvj_param->joint_pos_begin[0] << " " << mvj_param->joint_pos_begin[1] << " " << mvj_param->joint_pos_begin[2] << " " << mvj_param->joint_pos_begin[3] << " " << mvj_param->joint_pos_begin[4] << " " << mvj_param->joint_pos_begin[5] << "  ";
		for (auto &cm : controller->motionPool())
		{
			target.controller->lout() << "  " << cm.targetPos() << "  " << cm.actualPos() << "  " << cm.actualVel() << "  " << cm.actualCur() << "  ";
		}
		// target.controller->lout() << target.model->motionPool()[0].mf() << "  " 
		// << target.model->motionPool()[1].mf() << " " << target.model->motionPool()[2].mf() << " " << target.model->motionPool()[3].mf() << " " << target.model->motionPool()[4].mf() << " " << target.model->motionPool()[5].mf() << " ";
		target.controller->lout() << "\n";
		//////////////////////////////////////////////////////////////////////////////////////////////////
		return max_total_count == 0 ? 0 : max_total_count - target.count;
	}
	
	MoveJ::~MoveJ() = default;
	MoveJ::MoveJ(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"mvj\">"
			"	<GroupParam>"
			"		<Param name=\"pos_unit\" default=\"m\"/>"
			"		<UniqueParam default=\"pq\">"
			"			<Param name=\"pq\" default=\"{0,0,0,0,0,0,1}\"/>"
			"			<Param name=\"pm\" default=\"{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}\"/>"
			"			<GroupParam>"
			"				<Param name=\"pe\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"ori_unit\" default=\"rad\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
			"		<Param name=\"joint_acc\" default=\"0.1\"/>"
			"		<Param name=\"joint_vel\" default=\"0.5\"/>"
			"		<Param name=\"joint_dec\" default=\"0.1\"/>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(MoveJ);

	struct MoveLParam
	{
		std::vector<double> joint_vel, joint_acc, joint_dec, ee_pq, joint_pos_begin, joint_pos_end;
		Size total_count[6];

		double acc, vel, dec;
		double angular_acc, angular_vel, angular_dec;
	};
	struct MoveL::Imp {};
	auto MoveL::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		set_check_option(params, target);
		//cout << "prepairNrt获取参数" << endl;
		MoveLParam mvl_param;
		mvl_param.ee_pq.resize(7);
		if (!find_pq(params, target, mvl_param.ee_pq.data()))THROW_FILE_AND_LINE("");

		for (auto cmd_param : params)
		{
			if (cmd_param.first == "acc")
			{
				mvl_param.acc = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "vel")
			{
				mvl_param.vel = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "dec")
			{
				mvl_param.dec = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_acc")
			{
				mvl_param.angular_acc = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_vel")
			{
				mvl_param.angular_vel = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_dec")
			{
				mvl_param.angular_dec = std::stod(cmd_param.second);
			}
		}

		target.option |= USE_TARGET_POS;
		target.param = mvl_param;
		//cout << "prepairNrt完成" << endl;
	}
	auto MoveL::executeRT(PlanTarget &target)->int
	{
		auto mvl_param = std::any_cast<MoveLParam>(&target.param);
		auto controller = target.controller;

		// 取得起始位置 //
		static double begin_pm[16], relative_pm[16], relative_pa[6], pos_ratio, ori_ratio, norm_pos, norm_ori;
		double p, v, a;
		amc::Size pos_total_count, ori_total_count;
		//cout << "executeRT获取参数" << endl;
		if (target.count == 1)
		{
			double end_pm[16];
			amc::dynamic::s_pq2pm(mvl_param->ee_pq.data(), end_pm);
			target.model->generalMotionPool().at(0).updMpm();
			target.model->generalMotionPool().at(0).getMpm(begin_pm);
			amc::dynamic::s_inv_pm_dot_pm(begin_pm, end_pm, relative_pm);

			// relative_pa //
			amc::dynamic::s_pm2pa(relative_pm, relative_pa);

			norm_pos = amc::dynamic::s_norm(3, relative_pa);
			norm_ori = amc::dynamic::s_norm(3, relative_pa + 3);

			amc::plan::moveAbsolute(target.count, 0.0, norm_pos, mvl_param->vel / 1000, mvl_param->acc / 1000 / 1000, mvl_param->dec / 1000 / 1000, p, v, a, pos_total_count);
			amc::plan::moveAbsolute(target.count, 0.0, norm_ori, mvl_param->angular_vel / 1000, mvl_param->angular_acc / 1000 / 1000, mvl_param->angular_dec / 1000 / 1000, p, v, a, ori_total_count);

			pos_ratio = pos_total_count < ori_total_count ? double(pos_total_count) / ori_total_count : 1.0;
			ori_ratio = ori_total_count < pos_total_count ? double(ori_total_count) / pos_total_count : 1.0;

			amc::plan::moveAbsolute(target.count, 0.0, norm_pos, mvl_param->vel / 1000 * pos_ratio, mvl_param->acc / 1000 / 1000 * pos_ratio* pos_ratio, mvl_param->dec / 1000 / 1000 * pos_ratio* pos_ratio, p, v, a, pos_total_count);
			amc::plan::moveAbsolute(target.count, 0.0, norm_ori, mvl_param->angular_vel / 1000 * ori_ratio, mvl_param->angular_acc / 1000 / 1000 * ori_ratio * ori_ratio, mvl_param->angular_dec / 1000 / 1000 * ori_ratio * ori_ratio, p, v, a, ori_total_count);
		}
		//cout << "executeRT参数设置" << endl;
		double pa[6]{ 0,0,0,0,0,0 }, pm[16], pm2[16];

		amc::plan::moveAbsolute(target.count, 0.0, norm_pos, mvl_param->vel / 1000 * pos_ratio, mvl_param->acc / 1000 / 1000 * pos_ratio* pos_ratio, mvl_param->dec / 1000 / 1000 * pos_ratio* pos_ratio, p, v, a, pos_total_count);
		if (norm_pos > 1e-10)amc::dynamic::s_vc(3, p / norm_pos, relative_pa, pa);

		amc::plan::moveAbsolute(target.count, 0.0, norm_ori, mvl_param->angular_vel / 1000 * ori_ratio, mvl_param->angular_acc / 1000 / 1000 * ori_ratio * ori_ratio, mvl_param->angular_dec / 1000 / 1000 * ori_ratio * ori_ratio, p, v, a, ori_total_count);
		if (norm_ori > 1e-10)amc::dynamic::s_vc(3, p / norm_ori, relative_pa + 3, pa + 3);

		amc::dynamic::s_pa2pm(pa, pm);
		amc::dynamic::s_pm_dot_pm(begin_pm, pm, pm2);
		cout << "executeRT每周期发送六个关节位置如下:" << endl;
		// 反解计算电机位置 //
		target.model->generalMotionPool().at(0).setMpm(pm2);
		cout << "末端初始位姿为:" << endl;
		amc::dynamic::dsp(4, 4, pm2);
		if (!target.model->solverPool().at(1).kinPos())return -1;

		////////////////////////////////////// log ///////////////////////////////////////
		double pq[7];
		amc::dynamic::s_pm2pq(*target.model->generalMotionPool().at(0).mpm(), pq);
		target.controller->lout() << target.count << " " << pq[0] << " " << pq[1] << " " << pq[2] << " " << pq[3] << " " << pq[4] << " " << pq[5] << " " << pq[6] << "  ";

		for (auto &cm : controller->motionPool())
		{
			target.controller->lout() << "  " << cm.targetPos() << "  " << cm.actualPos() << "  " << cm.actualVel() << "  " << cm.actualCur() << "  ";
		}
		target.controller->lout() << "\n";
		//////////////////////////////////////////////////////////////////////////////////


		//cout << "executeRT完成任务" << endl;
		return std::max(pos_total_count, ori_total_count) > target.count ? 1 : 0;
	}
	MoveL::~MoveL() = default;
	MoveL::MoveL(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"mvl\">"
			"	<GroupParam>"
			"		<Param name=\"pos_unit\" default=\"m\"/>"
			"		<UniqueParam default=\"pq\">"
			"			<Param name=\"pq\" default=\"{0,0,0,0,0,0,1}\"/>"
			"			<Param name=\"pm\" default=\"{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}\"/>"
			"			<GroupParam>"
			"				<Param name=\"pe\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"ori_unit\" default=\"rad\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
			"		<Param name=\"acc\" default=\"0.1\"/>"
			"		<Param name=\"vel\" default=\"0.5\"/>"
			"		<Param name=\"dec\" default=\"0.1\"/>"
			"		<Param name=\"angular_acc\" default=\"0.1\"/>"
			"		<Param name=\"angular_vel\" default=\"0.5\"/>"
			"		<Param name=\"angular_dec\" default=\"0.1\"/>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(MoveL);
	// 关节空间轨迹规划
	struct PtpJParam
	{
		std::vector<double> joint_vel, joint_acc, joint_dec,joint_pos_begin, joint_pos_end, joint_pos_begin1, joint_pos_begin2, joint_pos_end1, joint_pos_end2;
		std::vector<Size> total_count1, total_count2;
	};
	struct PtpJ::Imp {};
	auto PtpJ::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		set_check_option(params, target);

		PtpJParam ptpj_param;
		//cout << "prepairNrt获取参数" << endl;
		// find pos_bigin  pos_end 
		ptpj_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpj_param.joint_pos_begin.data());
		ptpj_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpj_param.joint_pos_end.data());
		ptpj_param.total_count1.resize(target.model->motionPool().size(), 0);
		ptpj_param.total_count2.resize(target.model->motionPool().size(), 0);
		ptpj_param.joint_pos_begin1.resize(target.model->motionPool().size(), 0.0);
		ptpj_param.joint_pos_begin2.resize(target.model->motionPool().size(), 0.0);
		ptpj_param.joint_pos_end1.resize(target.model->motionPool().size(), 0.0);
		ptpj_param.joint_pos_end2.resize(target.model->motionPool().size(), 0.0);

		// find joint acc/vel/begin_pos/end_pos
		for (auto cmd_param : params)
		{
			auto c = target.controller;
			if (cmd_param.first == "joint_acc")
			{
				ptpj_param.joint_acc.clear();
				ptpj_param.joint_acc.resize(target.model->motionPool().size(), 0.0);

				auto acc_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (acc_mat.size() == 1)std::fill(ptpj_param.joint_acc.begin(), ptpj_param.joint_acc.end(), acc_mat.toDouble());
				else if (acc_mat.size() == target.model->motionPool().size()) std::copy(acc_mat.begin(), acc_mat.end(), ptpj_param.joint_acc.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpj_param.joint_acc[i] <= 0 || ptpj_param.joint_acc[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_vel")
			{
				ptpj_param.joint_vel.clear();
				ptpj_param.joint_vel.resize(target.model->motionPool().size(), 0.0);

				auto vel_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (vel_mat.size() == 1)std::fill(ptpj_param.joint_vel.begin(), ptpj_param.joint_vel.end(), vel_mat.toDouble());
				else if (vel_mat.size() == target.model->motionPool().size()) std::copy(vel_mat.begin(), vel_mat.end(), ptpj_param.joint_vel.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpj_param.joint_vel[i] <= 0 || ptpj_param.joint_vel[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_dec")
			{
				ptpj_param.joint_dec.clear();
				ptpj_param.joint_dec.resize(target.model->motionPool().size(), 0.0);

				auto dec_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (dec_mat.size() == 1)std::fill(ptpj_param.joint_dec.begin(), ptpj_param.joint_dec.end(), dec_mat.toDouble());
				else if (dec_mat.size() == target.model->motionPool().size()) std::copy(dec_mat.begin(), dec_mat.end(), ptpj_param.joint_dec.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpj_param.joint_dec[i] <= 0 || ptpj_param.joint_dec[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe1")
			{
				ptpj_param.joint_pos_begin.clear();
				ptpj_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);

				auto pe1_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe1_mat.size() == 1)std::fill(ptpj_param.joint_pos_begin.begin(), ptpj_param.joint_pos_begin.end(), pe1_mat.toDouble());
				else if (pe1_mat.size() == target.model->motionPool().size()) std::copy(pe1_mat.begin(), pe1_mat.end(), ptpj_param.joint_pos_begin.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpj_param.joint_pos_begin[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe2")
			{
				ptpj_param.joint_pos_end.clear();
				ptpj_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);

				auto pe2_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe2_mat.size() == 1)std::fill(ptpj_param.joint_pos_end.begin(), ptpj_param.joint_pos_end.end(), pe2_mat.toDouble());
				else if (pe2_mat.size() == target.model->motionPool().size()) std::copy(pe2_mat.begin(), pe2_mat.end(), ptpj_param.joint_pos_end.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpj_param.joint_pos_end[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
		}

		target.param = ptpj_param;
		//cout << "prepairNrt完成" << endl;
	}
	
	auto PtpJ::executeRT(PlanTarget &target)->int
	{
		auto ptpj_param = std::any_cast<PtpJParam>(&target.param);
		auto controller = target.controller;
		static Size max_total_count1, max_total_count2, max_total_count;
		// 初始化变量
		double p, v, a;
		
		if (target.count == 1)
		{
			// inverse kinematic 

			double pos_begin[16]; // 初始位姿
			amc::dynamic::s_pq2pm(ptpj_param->joint_pos_begin.data(), pos_begin);
			target.model->generalMotionPool().at(0).setMpm(pos_begin);
			cout << "末端规划起始点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_begin);
			cout << "起始点末端欧拉角坐标为:" << endl;
			copy(ptpj_param->joint_pos_begin.begin(),ptpj_param->joint_pos_begin.end(),ostream_iterator<double>(cout, " "));
			cout << endl;
			if (!target.model->solverPool().at(1).kinPos())return -1;

			// 先运动到初始位姿
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpj_param->joint_pos_begin1[i] = controller->motionPool()[i].targetPos();
				ptpj_param->joint_pos_end1[i] = target.model->motionPool()[i].mp() * 180 / PI;
				amc::plan::moveAbsolute(target.count, ptpj_param->joint_pos_begin1[i], ptpj_param->joint_pos_end1[i]
					, 2 * (ptpj_param->joint_vel[i]) / 1000, ptpj_param->joint_acc[i] / 1000 / 1000, ptpj_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpj_param->total_count1[i]);
				
				
			}
			max_total_count1 = *std::max_element(ptpj_param->total_count1.begin(), ptpj_param->total_count1.end());
				
			double pos_end[16]; // 终止位姿
			amc::dynamic::s_pq2pm(ptpj_param->joint_pos_end.data(), pos_end);
			target.model->generalMotionPool().at(0).setMpm(pos_end);
			cout << "末端规划终止点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_end);
			cout << "终止点末端欧拉角坐标为:" << endl;
			copy(ptpj_param->joint_pos_end.begin(),ptpj_param->joint_pos_end.end(),ostream_iterator<double>(cout, " "));
			cout << endl;
			if (!target.model->solverPool().at(1).kinPos())return -1;

			// 再运动到终止位姿
			
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpj_param->joint_pos_begin2[i] = ptpj_param->joint_pos_end1[i];
				ptpj_param->joint_pos_end2[i] = target.model->motionPool()[i].mp() * 180 / PI;
				amc::plan::moveAbsolute(target.count, ptpj_param->joint_pos_begin2[i], ptpj_param->joint_pos_end2[i]
					, ptpj_param->joint_vel[i] / 1000, ptpj_param->joint_acc[i] / 1000 / 1000, ptpj_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpj_param->total_count2[i]);
			}
			max_total_count2 = *std::max_element(ptpj_param->total_count2.begin(), ptpj_param->total_count2.end());

			max_total_count = max_total_count1 + max_total_count2;
		}
		cout << "executeRT每周期发送六个关节位置如下:" << endl;
		if (target.count <= max_total_count1) // 第一段轨迹，由当前位置移动到规划轨迹初始点
		{
			cout << "先移动到起始点" << endl;
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpj_param->total_count1[i] / max_total_count1 
					, ptpj_param->joint_pos_begin1[i], ptpj_param->joint_pos_end1[i]
					, 2 * (ptpj_param->joint_vel[i]) / 1000, ptpj_param->joint_acc[i] / 1000 / 1000, ptpj_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpj_param->total_count1[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
		}
		else
		{	
			cout << "开始轨迹规划"<< endl;
			target.count = target.count - max_total_count1;// 重置target.count为1 开始第二段轨迹，由初始点移动到终止点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpj_param->total_count2[i] / max_total_count2
					, ptpj_param->joint_pos_begin2[i], ptpj_param->joint_pos_end2[i]
					, ptpj_param->joint_vel[i] / 1000, ptpj_param->joint_acc[i] / 1000 / 1000, ptpj_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpj_param->total_count2[i]);

				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1; // 为了让target.count保持连续性
		}
		
		return max_total_count == 0 ? 0 : max_total_count - target.count;
	}
	
	PtpJ::~PtpJ() = default;
	PtpJ::PtpJ(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"ptpj\">"
			"	<GroupParam>"
			"		<Param name=\"pos_unit\" default=\"m\"/>"
			"		<UniqueParam default=\"pq\">"
			"			<Param name=\"pq\" default=\"{0,0,0,0,0,0,1}\"/>"
			"			<Param name=\"pm\" default=\"{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}\"/>"
			"			<GroupParam>"
			"				<Param name=\"pe1\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"pe2\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"ori_unit\" default=\"rad\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
			"		<Param name=\"joint_acc\" default=\"0.1\"/>"
			"		<Param name=\"joint_vel\" default=\"0.5\"/>"
			"		<Param name=\"joint_dec\" default=\"0.1\"/>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(PtpJ);

	//笛卡尔空间直线轨迹规划
	struct PtpLParam
	{
		std::vector<double> joint_vel, joint_acc, joint_dec, joint_pos_begin, joint_pos_end, pos1_begin, pos1_end,
							pos2_begin, pos2_end, pos3_begin, pos3_end, pos4_begin, pos4_end, pos5_begin, pos5_end;
		std::vector<Size> total_count1, total_count2, total_count3, total_count4, total_count5;

		double acc, vel, dec;
		double angular_acc, angular_vel, angular_dec;
	};
	struct PtpL::Imp {};
	auto PtpL::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		set_check_option(params, target);
		//cout << "prepairNrt获取参数" << endl;
		// find pe1 pe2
		PtpLParam ptpl_param;
		ptpl_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpl_param.joint_pos_begin.data());
		ptpl_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpl_param.joint_pos_end.data());
		ptpl_param.total_count1.resize(target.model->motionPool().size(), 0);
		ptpl_param.total_count2.resize(target.model->motionPool().size(), 0);
		ptpl_param.total_count3.resize(target.model->motionPool().size(), 0);
		ptpl_param.total_count4.resize(target.model->motionPool().size(), 0);
		ptpl_param.total_count5.resize(target.model->motionPool().size(), 0);
		ptpl_param.pos1_begin.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos1_end.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos2_begin.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos2_end.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos3_begin.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos3_end.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos4_begin.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos4_end.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos5_begin.resize(target.model->motionPool().size(), 0.0);
		ptpl_param.pos5_end.resize(target.model->motionPool().size(), 0.0);
		// 查看关节角度数组的大小和形式
		std::cout << "关节角度默认形式为:" << std::endl;
		std::cout << target.model->motionPool().size() << std::endl;
		copy(ptpl_param.pos1_begin.begin(),ptpl_param.pos1_begin.end(),ostream_iterator<double>(cout, " "));
		cout << endl;

		for (auto cmd_param : params)
		{
			auto c = target.controller;
			if (cmd_param.first == "acc")
			{
				ptpl_param.acc = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "vel")
			{
				ptpl_param.vel = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "dec")
			{
				ptpl_param.dec = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_acc")
			{
				ptpl_param.angular_acc = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_vel")
			{
				ptpl_param.angular_vel = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_dec")
			{
				ptpl_param.angular_dec = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "joint_acc")
			{
				ptpl_param.joint_acc.clear();
				ptpl_param.joint_acc.resize(target.model->motionPool().size(), 0.0);

				auto acc_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (acc_mat.size() == 1)std::fill(ptpl_param.joint_acc.begin(), ptpl_param.joint_acc.end(), acc_mat.toDouble());
				else if (acc_mat.size() == target.model->motionPool().size()) std::copy(acc_mat.begin(), acc_mat.end(), ptpl_param.joint_acc.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpl_param.joint_acc[i] <= 0 || ptpl_param.joint_acc[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_vel")
			{
				ptpl_param.joint_vel.clear();
				ptpl_param.joint_vel.resize(target.model->motionPool().size(), 0.0);

				auto vel_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (vel_mat.size() == 1)std::fill(ptpl_param.joint_vel.begin(), ptpl_param.joint_vel.end(), vel_mat.toDouble());
				else if (vel_mat.size() == target.model->motionPool().size()) std::copy(vel_mat.begin(), vel_mat.end(), ptpl_param.joint_vel.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpl_param.joint_vel[i] <= 0 || ptpl_param.joint_vel[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_dec")
			{
				ptpl_param.joint_dec.clear();
				ptpl_param.joint_dec.resize(target.model->motionPool().size(), 0.0);

				auto dec_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (dec_mat.size() == 1)std::fill(ptpl_param.joint_dec.begin(), ptpl_param.joint_dec.end(), dec_mat.toDouble());
				else if (dec_mat.size() == target.model->motionPool().size()) std::copy(dec_mat.begin(), dec_mat.end(), ptpl_param.joint_dec.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpl_param.joint_dec[i] <= 0 || ptpl_param.joint_dec[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe1")
			{
				ptpl_param.joint_pos_begin.clear();
				ptpl_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);

				auto pe1_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe1_mat.size() == 1)std::fill(ptpl_param.joint_pos_begin.begin(), ptpl_param.joint_pos_begin.end(), pe1_mat.toDouble());
				else if (pe1_mat.size() == target.model->motionPool().size()) std::copy(pe1_mat.begin(), pe1_mat.end(), ptpl_param.joint_pos_begin.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpl_param.joint_pos_begin[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe2")
			{
				ptpl_param.joint_pos_end.clear();
				ptpl_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);

				auto pe2_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe2_mat.size() == 1)std::fill(ptpl_param.joint_pos_end.begin(), ptpl_param.joint_pos_end.end(), pe2_mat.toDouble());
				else if (pe2_mat.size() == target.model->motionPool().size()) std::copy(pe2_mat.begin(), pe2_mat.end(), ptpl_param.joint_pos_end.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpl_param.joint_pos_end[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
		}

		target.param = ptpl_param;
		//cout << "prepairNrt完成" << endl;
	}
	auto PtpL::executeRT(PlanTarget &target)->int
	{
		auto ptpl_param = std::any_cast<PtpLParam>(&target.param);
		auto controller = target.controller;
		static Size max_total_count1, max_total_count2, max_total_count3, max_total_count4, max_total_count5, max_total_count;
		// 初始化变量
		double p, v, a;
		
		if (target.count == 1)
		{
			double pos_begin[16]; // 初始位姿
			amc::dynamic::s_pq2pm(ptpl_param->joint_pos_begin.data(), pos_begin);
			target.model->generalMotionPool().at(0).setMpm(pos_begin);
			cout << "末端规划起始点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_begin);
			cout << "起始点末端欧拉角坐标为:" << endl;
			copy(ptpl_param->joint_pos_begin.begin(),ptpl_param->joint_pos_begin.end(),ostream_iterator<double>(cout, " "));
			cout << endl;

			double pos_end[16]; // 终止位姿
			amc::dynamic::s_pq2pm(ptpl_param->joint_pos_end.data(), pos_end);
			target.model->generalMotionPool().at(0).setMpm(pos_end);
			cout << "末端规划终止点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_end);
			cout << "终止点末端欧拉角坐标为:" << endl;
			copy(ptpl_param->joint_pos_end.begin(),ptpl_param->joint_pos_end.end(),ostream_iterator<double>(cout, " "));
			cout << endl;
			

			// 获取五个插值点(包括首尾点)
			double q[5][6];
			amc::plan::SpaceLineJT(pos_begin,pos_end,q);
			
			// 先从当前运动到起始点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpl_param->pos1_begin[i] = controller->motionPool()[i].targetPos();
				ptpl_param->pos1_end[i+6] = q[0][i];
				amc::plan::moveAbsolute(target.count, ptpl_param->pos1_begin[i+6], ptpl_param->pos1_end[i+6]
					, 2 * (ptpl_param->joint_vel[i]) / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count1[i]);
			}
			max_total_count1 = *std::max_element(ptpl_param->total_count1.begin(), ptpl_param->total_count1.end());
			
			// 再开始插补的轨迹规划
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpl_param->pos2_begin[i] = q[0][i];
				ptpl_param->pos2_end[i] = q[1][i];
				amc::plan::moveAbsolute(target.count, ptpl_param->pos2_begin[i], ptpl_param->pos2_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count2[i]);
			}
			max_total_count2 = *std::max_element(ptpl_param->total_count2.begin(), ptpl_param->total_count2.end());

			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpl_param->pos3_begin[i] = q[1][i];
				ptpl_param->pos3_end[i] = q[2][i];
				amc::plan::moveAbsolute(target.count, ptpl_param->pos3_begin[i], ptpl_param->pos3_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count3[i]);
			}
			max_total_count3 = *std::max_element(ptpl_param->total_count3.begin(), ptpl_param->total_count3.end());

			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpl_param->pos4_begin[i] = q[2][i];
				ptpl_param->pos4_end[i] = q[3][i];
				amc::plan::moveAbsolute(target.count, ptpl_param->pos4_begin[i], ptpl_param->pos4_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count4[i]);
			}
			max_total_count4 = *std::max_element(ptpl_param->total_count4.begin(), ptpl_param->total_count4.end());

			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpl_param->pos5_begin[i] = q[3][i];
				ptpl_param->pos5_end[i] = q[4][i];
				amc::plan::moveAbsolute(target.count, ptpl_param->pos5_begin[i], ptpl_param->pos5_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count5[i]);
			}
			max_total_count5 = *std::max_element(ptpl_param->total_count5.begin(), ptpl_param->total_count5.end());		

			max_total_count = max_total_count1 + max_total_count2 + max_total_count3 + max_total_count4 + max_total_count5;		
			
		}

		cout << "executeRT每周期发送六个关节位置如下:" << endl;
	
		if (target.count <= max_total_count1) // 由当前位置移动到规划轨迹初始点
		{
			cout << "先移动到起始点" << endl;
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpl_param->total_count1[i] / max_total_count1 
					, ptpl_param->pos1_begin[i+6], ptpl_param->pos1_end[i+6]
					, 2 * (ptpl_param->joint_vel[i]) / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count1[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
		}
		else if(target.count > max_total_count1 && target.count <= (max_total_count1 + max_total_count2))
		{	
			cout << "开始轨迹规划第一段"<< endl;
			target.count = target.count - max_total_count1;// 重置target.count为1 开始第一段轨迹，由初始点移动到第一个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpl_param->total_count2[i] / max_total_count2
					, ptpl_param->pos2_begin[i], ptpl_param->pos2_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count2[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1; // 为了让target.count保持连续性
		}
		else if (target.count > (max_total_count1 + max_total_count2) && target.count <= (max_total_count1 + max_total_count2 + max_total_count3))
		{
			cout << "开始轨迹规划第二段"<< endl;
			target.count = target.count - max_total_count1 - max_total_count2;// 重置target.count为1 开始第二段轨迹，由初始点移动到第二个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpl_param->total_count3[i] / max_total_count3 
					, ptpl_param->pos3_begin[i], ptpl_param->pos3_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count3[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1 + max_total_count2; // 为了让target.count保持连续性
		}
		else if (target.count > (max_total_count1 + max_total_count2 + max_total_count3) && target.count <= (max_total_count1 + max_total_count2 + max_total_count3 + max_total_count4))
		{
			cout << "开始轨迹规划第三段"<< endl;
			target.count = target.count - max_total_count1 - max_total_count2 - max_total_count3;// 重置target.count为1 开始第三段轨迹，由初始点移动到第三个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpl_param->total_count4[i] / max_total_count4 
					, ptpl_param->pos4_begin[i], ptpl_param->pos4_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count4[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1 + max_total_count2 + max_total_count3; // 为了让target.count保持连续性
		}
		else
		{
			cout << "开始轨迹规划第四段"<< endl;
			target.count = target.count - max_total_count1 - max_total_count2 - max_total_count3 - max_total_count4;// 重置target.count为1 开始第四段轨迹，由初始点移动到第四个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpl_param->total_count5[i] / max_total_count5 
					, ptpl_param->pos5_begin[i], ptpl_param->pos5_end[i]
					, ptpl_param->joint_vel[i] / 1000, ptpl_param->joint_acc[i] / 1000 / 1000, ptpl_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpl_param->total_count5[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1 + max_total_count2 + max_total_count3 + max_total_count4; // 为了让target.count保持连续性
		}
		
		return max_total_count == 0 ? 0 : max_total_count - target.count;
	}
	PtpL::~PtpL() = default;
	PtpL::PtpL(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"ptpl\">"
			"	<GroupParam>"
			"		<Param name=\"pos_unit\" default=\"m\"/>"
			"		<UniqueParam default=\"pq\">"
			"			<Param name=\"pq\" default=\"{0,0,0,0,0,0,1}\"/>"
			"			<Param name=\"pm\" default=\"{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}\"/>"
			"			<GroupParam>"
			"				<Param name=\"pe1\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"pe2\" default=\"{0,0,0,0,0,0}\"/>"			
			"				<Param name=\"ori_unit\" default=\"rad\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
			"		<Param name=\"acc\" default=\"0.1\"/>"
			"		<Param name=\"vel\" default=\"0.5\"/>"
			"		<Param name=\"dec\" default=\"0.1\"/>"
			"		<Param name=\"angular_acc\" default=\"0.1\"/>"
			"		<Param name=\"angular_vel\" default=\"0.5\"/>"
			"		<Param name=\"angular_dec\" default=\"0.1\"/>"
			"		<Param name=\"joint_acc\" default=\"0.1\"/>"
			"		<Param name=\"joint_vel\" default=\"0.5\"/>"
			"		<Param name=\"joint_dec\" default=\"0.1\"/>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(PtpL);

	//笛卡尔空间圆弧轨迹规划
	struct PtpRParam
	{
		std::vector<double> joint_vel, joint_acc, joint_dec, joint_pos_begin, joint_pos_middle, joint_pos_end, pos1_begin, pos1_end,
							pos2_begin, pos2_end, pos3_begin, pos3_end, pos4_begin, pos4_end, pos5_begin, pos5_end;
		std::vector<Size> total_count1, total_count2, total_count3, total_count4, total_count5;

		double acc, vel, dec;
		double angular_acc, angular_vel, angular_dec;
	};
	struct PtpR::Imp {};
	auto PtpR::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		set_check_option(params, target);
		//cout << "prepairNrt获取参数" << endl;
		// find pe1 pe2 pe3
		PtpRParam ptpr_param;
		ptpr_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpr_param.joint_pos_begin.data());
		ptpr_param.joint_pos_middle.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpr_param.joint_pos_begin.data());
		ptpr_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);
		find_pq(params, target, ptpr_param.joint_pos_end.data());
		ptpr_param.total_count1.resize(target.model->motionPool().size(), 0);
		ptpr_param.total_count2.resize(target.model->motionPool().size(), 0);
		ptpr_param.total_count3.resize(target.model->motionPool().size(), 0);
		ptpr_param.total_count4.resize(target.model->motionPool().size(), 0);
		ptpr_param.total_count5.resize(target.model->motionPool().size(), 0);
		ptpr_param.pos1_begin.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos1_end.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos2_begin.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos2_end.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos3_begin.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos3_end.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos4_begin.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos4_end.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos5_begin.resize(target.model->motionPool().size(), 0.0);
		ptpr_param.pos5_end.resize(target.model->motionPool().size(), 0.0);
		
		for (auto cmd_param : params)
		{
			auto c = target.controller;
			if (cmd_param.first == "acc")
			{
				ptpr_param.acc = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "vel")
			{
				ptpr_param.vel = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "dec")
			{
				ptpr_param.dec = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_acc")
			{
				ptpr_param.angular_acc = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_vel")
			{
				ptpr_param.angular_vel = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "angular_dec")
			{
				ptpr_param.angular_dec = std::stod(cmd_param.second);
			}
			else if (cmd_param.first == "joint_acc")
			{
				ptpr_param.joint_acc.clear();
				ptpr_param.joint_acc.resize(target.model->motionPool().size(), 0.0);

				auto acc_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (acc_mat.size() == 1)std::fill(ptpr_param.joint_acc.begin(), ptpr_param.joint_acc.end(), acc_mat.toDouble());
				else if (acc_mat.size() == target.model->motionPool().size()) std::copy(acc_mat.begin(), acc_mat.end(), ptpr_param.joint_acc.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpr_param.joint_acc[i] <= 0 || ptpr_param.joint_acc[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_vel")
			{
				ptpr_param.joint_vel.clear();
				ptpr_param.joint_vel.resize(target.model->motionPool().size(), 0.0);

				auto vel_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (vel_mat.size() == 1)std::fill(ptpr_param.joint_vel.begin(), ptpr_param.joint_vel.end(), vel_mat.toDouble());
				else if (vel_mat.size() == target.model->motionPool().size()) std::copy(vel_mat.begin(), vel_mat.end(), ptpr_param.joint_vel.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpr_param.joint_vel[i] <= 0 || ptpr_param.joint_vel[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "joint_dec")
			{
				ptpr_param.joint_dec.clear();
				ptpr_param.joint_dec.resize(target.model->motionPool().size(), 0.0);

				auto dec_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (dec_mat.size() == 1)std::fill(ptpr_param.joint_dec.begin(), ptpr_param.joint_dec.end(), dec_mat.toDouble());
				else if (dec_mat.size() == target.model->motionPool().size()) std::copy(dec_mat.begin(), dec_mat.end(), ptpr_param.joint_dec.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpr_param.joint_dec[i] <= 0 || ptpr_param.joint_dec[i] > c->motionPool()[i].maxAcc())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe1")
			{
				ptpr_param.joint_pos_begin.clear();
				ptpr_param.joint_pos_begin.resize(target.model->motionPool().size(), 0.0);

				auto pe1_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe1_mat.size() == 1)std::fill(ptpr_param.joint_pos_begin.begin(), ptpr_param.joint_pos_begin.end(), pe1_mat.toDouble());
				else if (pe1_mat.size() == target.model->motionPool().size()) std::copy(pe1_mat.begin(), pe1_mat.end(), ptpr_param.joint_pos_begin.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpr_param.joint_pos_begin[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe2")
			{
				ptpr_param.joint_pos_middle.clear();
				ptpr_param.joint_pos_middle.resize(target.model->motionPool().size(), 0.0);

				auto pe2_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe2_mat.size() == 1)std::fill(ptpr_param.joint_pos_middle.begin(), ptpr_param.joint_pos_middle.end(), pe2_mat.toDouble());
				else if (pe2_mat.size() == target.model->motionPool().size()) std::copy(pe2_mat.begin(), pe2_mat.end(), ptpr_param.joint_pos_middle.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpr_param.joint_pos_middle[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
			else if (cmd_param.first == "pe3")
			{
				ptpr_param.joint_pos_end.clear();
				ptpr_param.joint_pos_end.resize(target.model->motionPool().size(), 0.0);

				auto pe3_mat = target.model->calculator().calculateExpression(cmd_param.second);
				if (pe3_mat.size() == 1)std::fill(ptpr_param.joint_pos_end.begin(), ptpr_param.joint_pos_end.end(), pe3_mat.toDouble());
				else if (pe3_mat.size() == target.model->motionPool().size()) std::copy(pe3_mat.begin(), pe3_mat.end(), ptpr_param.joint_pos_end.begin());
				else THROW_FILE_AND_LINE("");

				// check value validity //
				for (Size i = 0; i< std::min(target.model->motionPool().size(), c->motionPool().size()); ++i)
					if (ptpr_param.joint_pos_end[i] > c->motionPool()[i].maxPos())
						THROW_FILE_AND_LINE("");
			}
		}

		target.param = ptpr_param;
		//cout << "prepairNrt完成" << endl;
	}
	auto PtpR::executeRT(PlanTarget &target)->int
	{
		auto ptpr_param = std::any_cast<PtpRParam>(&target.param);
		auto controller = target.controller;
		static Size max_total_count1, max_total_count2, max_total_count3, max_total_count4, max_total_count5, max_total_count;
		// 初始化变量
		double p, v, a;
		
		if (target.count == 1)
		{
			double pos_begin[16]; // 初始位姿
			amc::dynamic::s_pq2pm(ptpr_param->joint_pos_begin.data(), pos_begin);
			target.model->generalMotionPool().at(0).setMpm(pos_begin);
			cout << "末端规划起始点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_begin);
			cout << "起始点末端欧拉角坐标为:" << endl;
			copy(ptpr_param->joint_pos_begin.begin(),ptpr_param->joint_pos_begin.end(),ostream_iterator<double>(cout, " "));
			cout << endl;

			double pos_middle[16]; // 中间位姿
			amc::dynamic::s_pq2pm(ptpr_param->joint_pos_middle.data(), pos_middle);
			target.model->generalMotionPool().at(0).setMpm(pos_middle);
			cout << "末端规划中间点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_middle);
			cout << "中间点末端欧拉角坐标为:" << endl;
			copy(ptpr_param->joint_pos_middle.begin(),ptpr_param->joint_pos_middle.end(),ostream_iterator<double>(cout, " "));
			cout << endl;

			double pos_end[16]; // 终止位姿
			amc::dynamic::s_pq2pm(ptpr_param->joint_pos_end.data(), pos_end);
			target.model->generalMotionPool().at(0).setMpm(pos_end);
			cout << "末端规划终止点末端位姿为:" << endl;
			amc::dynamic::dsp(4, 4, pos_end);
			cout << "终止点末端欧拉角坐标为:" << endl;
			copy(ptpr_param->joint_pos_end.begin(),ptpr_param->joint_pos_end.end(),ostream_iterator<double>(cout, " "));
			cout << endl;
			// double q0[6] = {0.0};

			// 获取五个插值点(包括首尾点)
			double q[6][6];
			amc::plan::SpaceCircleJT(pos_begin,pos_middle,pos_end,q);
			
			// 先从零位运动到起始点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpr_param->pos1_begin[i] = controller->motionPool()[i].targetPos();
				ptpr_param->pos1_end[i+6] = q[0][i];
				amc::plan::moveAbsolute(target.count, ptpr_param->pos1_begin[i+6], ptpr_param->pos1_end[i+6]
					, 2 * (ptpr_param->joint_vel[i]) / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count1[i]);
			}
			max_total_count1 = *std::max_element(ptpr_param->total_count1.begin(), ptpr_param->total_count1.end());
			
			// 再开始插补的轨迹规划
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpr_param->pos2_begin[i] = q[0][i];
				ptpr_param->pos2_end[i] = q[1][i];
				amc::plan::moveAbsolute(target.count, ptpr_param->pos2_begin[i], ptpr_param->pos2_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count2[i]);
			}
			max_total_count2 = *std::max_element(ptpr_param->total_count2.begin(), ptpr_param->total_count2.end());

			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpr_param->pos3_begin[i] = q[1][i];
				ptpr_param->pos3_end[i] = q[2][i];
				amc::plan::moveAbsolute(target.count, ptpr_param->pos3_begin[i], ptpr_param->pos3_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count3[i]);
			}
			max_total_count3 = *std::max_element(ptpr_param->total_count3.begin(), ptpr_param->total_count3.end());

			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpr_param->pos4_begin[i] = q[2][i];
				ptpr_param->pos4_end[i] = q[3][i];
				amc::plan::moveAbsolute(target.count, ptpr_param->pos4_begin[i], ptpr_param->pos4_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count4[i]);
			}
			max_total_count4 = *std::max_element(ptpr_param->total_count4.begin(), ptpr_param->total_count4.end());

			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				ptpr_param->pos5_begin[i] = q[3][i];
				ptpr_param->pos5_end[i] = q[4][i];
				amc::plan::moveAbsolute(target.count, ptpr_param->pos5_begin[i], ptpr_param->pos5_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count5[i]);
			}
			max_total_count5 = *std::max_element(ptpr_param->total_count5.begin(), ptpr_param->total_count5.end());		

			max_total_count = max_total_count1 + max_total_count2 + max_total_count3 + max_total_count4 + max_total_count5;		
			
		}

		cout << "executeRT每周期发送六个关节位置如下:" << endl;
	
		if (target.count <= max_total_count1) // 由当前位置移动到规划轨迹初始点
		{
			cout << "先移动到起始点" << endl;
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpr_param->total_count1[i] / max_total_count1 
					, ptpr_param->pos1_begin[i+6], ptpr_param->pos1_end[i+6]
					, 2 * (ptpr_param->joint_vel[i]) / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count1[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
		}
		else if(target.count > max_total_count1 && target.count <= (max_total_count1 + max_total_count2))
		{	
			cout << "开始轨迹规划第一段"<< endl;
			target.count = target.count - max_total_count1;// 重置target.count为1 开始第一段轨迹，由初始点移动到第一个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpr_param->total_count2[i] / max_total_count2
					, ptpr_param->pos2_begin[i], ptpr_param->pos2_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count2[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1; // 为了让target.count保持连续性
		}
		else if (target.count > (max_total_count1 + max_total_count2) && target.count <= (max_total_count1 + max_total_count2 + max_total_count3))
		{
			cout << "开始轨迹规划第二段"<< endl;
			target.count = target.count - max_total_count1 - max_total_count2;// 重置target.count为1 开始第二段轨迹，由初始点移动到第二个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpr_param->total_count3[i] / max_total_count3 
					, ptpr_param->pos3_begin[i], ptpr_param->pos3_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count3[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1 + max_total_count2; // 为了让target.count保持连续性
		}
		else if (target.count > (max_total_count1 + max_total_count2 + max_total_count3) && target.count <= (max_total_count1 + max_total_count2 + max_total_count3 + max_total_count4))
		{
			cout << "开始轨迹规划第三段"<< endl;
			target.count = target.count - max_total_count1 - max_total_count2 - max_total_count3;// 重置target.count为1 开始第三段轨迹，由初始点移动到第三个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpr_param->total_count4[i] / max_total_count4 
					, ptpr_param->pos4_begin[i], ptpr_param->pos4_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count4[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1 + max_total_count2 + max_total_count3; // 为了让target.count保持连续性
		}
		else
		{
			cout << "开始轨迹规划第四段"<< endl;
			target.count = target.count - max_total_count1 - max_total_count2 - max_total_count3 - max_total_count4;// 重置target.count为1 开始第四段轨迹，由初始点移动到第四个插补点
			for (Size i = 0; i < std::min(controller->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				amc::plan::moveAbsolute(static_cast<double>(target.count) * ptpr_param->total_count5[i] / max_total_count5 
					, ptpr_param->pos5_begin[i], ptpr_param->pos5_end[i]
					, ptpr_param->joint_vel[i] / 1000, ptpr_param->joint_acc[i] / 1000 / 1000, ptpr_param->joint_dec[i] / 1000 / 1000
					, p, v, a, ptpr_param->total_count5[i]);
				
				controller->motionPool()[i].setTargetPos(p);
			}
			target.count = target.count + max_total_count1 + max_total_count2 + max_total_count3 + max_total_count4; // 为了让target.count保持连续性
		}
		
		return max_total_count == 0 ? 0 : max_total_count - target.count;
	}
	PtpR::~PtpR() = default;
	PtpR::PtpR(const std::string &name) :Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"ptpr\">"
			"	<GroupParam>"
			"		<Param name=\"pos_unit\" default=\"m\"/>"
			"		<UniqueParam default=\"pq\">"
			"			<Param name=\"pq\" default=\"{0,0,0,0,0,0,1}\"/>"
			"			<Param name=\"pm\" default=\"{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}\"/>"
			"			<GroupParam>"
			"				<Param name=\"pe1\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"pe2\" default=\"{0,0,0,0,0,0}\"/>"	
			"				<Param name=\"pe3\" default=\"{0,0,0,0,0,0}\"/>"		
			"				<Param name=\"ori_unit\" default=\"rad\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
			"		<Param name=\"acc\" default=\"0.1\"/>"
			"		<Param name=\"vel\" default=\"0.5\"/>"
			"		<Param name=\"dec\" default=\"0.1\"/>"
			"		<Param name=\"angular_acc\" default=\"0.1\"/>"
			"		<Param name=\"angular_vel\" default=\"0.5\"/>"
			"		<Param name=\"angular_dec\" default=\"0.1\"/>"
			"		<Param name=\"joint_acc\" default=\"0.1\"/>"
			"		<Param name=\"joint_vel\" default=\"0.5\"/>"
			"		<Param name=\"joint_dec\" default=\"0.1\"/>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(PtpR);

	struct AutoMoveParam {};
	struct AutoMove::Imp
	{
		static std::atomic_bool is_running_;
		static std::atomic<std::array<double, 24>> pvade_;

		double max_pe_[6], min_pe_[6];
		std::string eul_type;
	};
	std::atomic_bool AutoMove::Imp::is_running_ = false;
	std::atomic<std::array<double, 24>> AutoMove::Imp::pvade_ = std::array<double, 24>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	auto AutoMove::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		set_check_option(params, target);

		target.option = 0;

		AutoMoveParam param;
		for (auto cmd_param : params)
		{
			if (cmd_param.first == "start")
			{
				if (Imp::is_running_.load())throw std::runtime_error("auto mode already started");

				imp_->eul_type = params.at("eul_type");
				if (!check_eul_validity(imp_->eul_type))THROW_FILE_AND_LINE("");

				auto mat = target.model->calculator().calculateExpression(params.at("max_pe"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), imp_->max_pe_);

				mat = target.model->calculator().calculateExpression(params.at("min_pe"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), imp_->min_pe_);

				std::array<double, 24> pvade;
				mat = target.model->calculator().calculateExpression(params.at("init_pe"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 0);

				mat = target.model->calculator().calculateExpression(params.at("init_ve"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 6);

				mat = target.model->calculator().calculateExpression(params.at("init_ae"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 12);

				mat = target.model->calculator().calculateExpression(params.at("init_de"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 18);

				Imp::pvade_.store(pvade);
				Imp::is_running_.store(true);
				target.option |= EXECUTE_WHEN_ALL_PLAN_COLLECTED | NOT_PRINT_EXECUTE_COUNT | USE_TARGET_POS;
			}
			else if (cmd_param.first == "stop")
			{
				if (!Imp::is_running_.load())throw std::runtime_error("auto mode not started, when stop");

				Imp::is_running_.store(false);
				target.option |= amc::plan::Plan::WAIT_FOR_COLLECTION;
			}
			else if (cmd_param.first == "pe")
			{
				if (!Imp::is_running_.load())throw std::runtime_error("auto mode not started, when pe");

				std::array<double, 24> pvade;
				auto mat = target.model->calculator().calculateExpression(params.at("pe"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 0);

				mat = target.model->calculator().calculateExpression(params.at("ve"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 6);

				mat = target.model->calculator().calculateExpression(params.at("ae"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 12);

				mat = target.model->calculator().calculateExpression(params.at("de"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), pvade.begin() + 18);

				for (int i = 0; i < 6; ++i)
				{
					pvade[i] = std::max(imp_->min_pe_[i], pvade[i]);
					pvade[i] = std::min(imp_->max_pe_[i], pvade[i]);
				}

				AutoMove::Imp::pvade_.store(pvade);
				target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION | NOT_PRINT_CMD_INFO | NOT_LOG_CMD_INFO;
			}
		}

		target.option |= NOT_CHECK_POS_FOLLOWING_ERROR;
		target.param = param;
	}
	auto AutoMove::executeRT(PlanTarget &target)->int
	{
		auto param = std::any_cast<AutoMoveParam>(&target.param);

		if (target.count == 1)
		{
			target.model->generalMotionPool()[0].setMve(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data(), "123");
		}

		// get current pe //
		double pe_now[6], ve_now[6], ae_now[6];
		target.model->generalMotionPool()[0].getMpe(pe_now, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].getMve(ve_now, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].getMae(ae_now, imp_->eul_type.c_str());
		for (int i = 3; i < 6; ++i)if (pe_now[i] > amc::PI) pe_now[i] -= 2 * PI;

		// get target pe //
		auto pvade = AutoMove::Imp::pvade_.load();
		auto pe = pvade.data() + 0;
		auto ve = pvade.data() + 6;
		auto ae = pvade.data() + 12;
		auto de = pvade.data() + 18;

		// now plan //
		double pe_next[6], ve_next[6], ae_next[6];
		for (int i = 0; i < 6; ++i)
		{
			amc::Size t;
			amc::plan::moveAbsolute2(pe_now[i], ve_now[i], ae_now[i]
				, pe[i], 0.0, 0.0
				, ve[i], ae[i], de[i]
				, 1e-3, 1e-10, pe_next[i], ve_next[i], ae_next[i], t);
		}

		target.model->generalMotionPool()[0].setMpe(pe_next, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].setMve(ve_next, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].setMae(ae_next, imp_->eul_type.c_str());

		static int i = 0;
		if (++i % 1000 == 0)
		{
			//target.controller->mout() << "pe_now :"
			//	<< pe_now[0] << "  " << pe_now[1] << "  " << pe_now[2] << "  "
			//	<< pe_now[3] << "  " << pe_now[4] << "  " << pe_now[5] << std::endl;

			//target.controller->mout() << "pe_target :"
			//	<< pvade[0] << "  " << pvade[1] << "  " << pvade[2] << "  "
			//	<< pvade[3] << "  " << pvade[4] << "  " << pvade[5] << std::endl;

			//target.controller->mout() << "pe_next:"
			//	<< pe_next[0] << "  " << pe_next[1] << "  " << pe_next[2] << "  "
			//	<< pe_next[3] << "  " << pe_next[4] << "  " << pe_next[5] << std::endl;
		}

		target.model->solverPool()[0].kinPos();
		target.model->solverPool()[0].kinVel();

		return imp_->is_running_.load() ? 1 : 0;
	}
	auto AutoMove::collectNrt(PlanTarget &target)->void { if (~(target.option | USE_TARGET_POS))Imp::is_running_.store(false); }
	AutoMove::~AutoMove() = default;
	AutoMove::AutoMove(const std::string &name) : Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"am\">"
			"	<GroupParam>"
			"		<UniqueParam default=\"start_group\">"
			"			<GroupParam name=\"start_group\">"
			"				<Param name=\"start\"/>"
			"				<Param name=\"init_pe\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"init_ve\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"init_ae\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"init_de\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"max_pe\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"min_pe\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"			<Param name=\"stop\"/>"
			"			<GroupParam>"
			"				<Param name=\"pe\" default=\"{0,0,0,0,0,0}\"/>"
			"				<Param name=\"ve\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"ae\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"de\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(AutoMove);

	struct ManualMoveParam {};
	struct ManualMove::Imp
	{
		static std::atomic_bool is_running_;
		static std::atomic<std::array<int, 6>> is_increase_;
		double ve_[6], ae_[6], de_[6];
		std::string eul_type;
		int increase_count;
	};
	std::atomic_bool ManualMove::Imp::is_running_ = false;
	std::atomic<std::array<int, 6>> ManualMove::Imp::is_increase_ = std::array<int, 6>{0, 0, 0, 0, 0, 0};
	auto ManualMove::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		set_check_option(params, target);

		target.option = 0;

		ManualMoveParam param;
		for (auto cmd_param : params)
		{
			if (cmd_param.first == "start")
			{
				if (Imp::is_running_.load())throw std::runtime_error("auto mode already started");

				imp_->eul_type = params.at("eul_type");
				if (!check_eul_validity(imp_->eul_type))THROW_FILE_AND_LINE("");

				imp_->increase_count = std::stoi(params.at("increase_count"));
				if (imp_->increase_count < 0 || imp_->increase_count>1e5)THROW_FILE_AND_LINE("");

				auto mat = target.model->calculator().calculateExpression(params.at("ve"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), imp_->ve_);

				mat = target.model->calculator().calculateExpression(params.at("ae"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), imp_->ae_);

				mat = target.model->calculator().calculateExpression(params.at("de"));
				if (mat.size() != 6)THROW_FILE_AND_LINE("");
				std::copy(mat.begin(), mat.end(), imp_->de_);

				Imp::is_increase_.store(std::array<int, 6>{0, 0, 0, 0, 0, 0});
				Imp::is_running_.store(true);
				target.option |= EXECUTE_WHEN_ALL_PLAN_COLLECTED | NOT_PRINT_EXECUTE_COUNT | USE_TARGET_POS;
			}
			else if (cmd_param.first == "stop")
			{
				if (!Imp::is_running_.load())throw std::runtime_error("manual mode not started, when stop");

				Imp::is_running_.store(false);
				target.option |= WAIT_FOR_COLLECTION;
			}
			else if (cmd_param.first == "x")
			{
				if (!Imp::is_running_.load())throw std::runtime_error("manual mode not started, when pe");

				std::array<int, 6> is_increase;
				is_increase[0] = std::stoi(params.at("x"));
				is_increase[1] = std::stoi(params.at("y"));
				is_increase[2] = std::stoi(params.at("z"));
				is_increase[3] = std::stoi(params.at("a"));
				is_increase[4] = std::stoi(params.at("b"));
				is_increase[5] = std::stoi(params.at("c"));

				for (auto &value : is_increase)value = std::max(std::min(1, value), -1) * imp_->increase_count;

				Imp::is_increase_.store(is_increase);
				target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION | NOT_PRINT_CMD_INFO | NOT_LOG_CMD_INFO;
			}
		}

		target.option |= NOT_CHECK_POS_FOLLOWING_ERROR;
		target.param = param;
	}
	auto ManualMove::executeRT(PlanTarget &target)->int
	{
		auto param = std::any_cast<ManualMoveParam>(&target.param);

		// get current pe //
		double pe_now[6], ve_now[6], ae_now[6];
		target.model->generalMotionPool()[0].getMpe(pe_now, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].getMve(ve_now, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].getMae(ae_now, imp_->eul_type.c_str());
		for (int i = 3; i < 6; ++i) if (pe_now[i] > amc::PI) pe_now[i] -= 2 * PI;

		// init status //
		static std::array<int, 6> increase_status;
		static std::array<double, 6> target_pe{ 0,0,0,0,0,0 };
		if (target.count == 1)
		{
			std::fill_n(increase_status.data(), 6, 0);
			std::copy_n(pe_now, 6, target_pe.data());
			std::fill_n(ve_now, 6, 0.0);
			std::fill_n(ae_now, 6, 0.0);
		}

		// get is_increase //
		auto is_increase = Imp::is_increase_.exchange(std::array<int, 6>{0, 0, 0, 0, 0, 0});
		for (int i = 0; i < 6; ++i) if (is_increase[i] != 0) increase_status[i] = is_increase[i];

		// calculate target pe //
		std::array<double, 6> target_pe_new;
		for (int i = 0; i < 6; ++i)
		{
			target_pe_new[i] = target_pe[i] + amc::dynamic::s_sgn(increase_status[i])*imp_->ve_[i] * 1e-3;
			increase_status[i] -= amc::dynamic::s_sgn(increase_status[i]);
		}

		// check if target_pe is valid //
		target.model->generalMotionPool()[0].setMpe(target_pe_new.data(), imp_->eul_type.c_str());
		auto check_motion_limit = [&]()->bool
		{
			auto c = target.controller;
			for (std::size_t i = 0; i < std::min(c->motionPool().size(), target.model->motionPool().size()); ++i)
			{
				auto &cm = c->motionPool().at(i);
				auto &mm = target.model->motionPool().at(i);

				if (mm.mp() < cm.minPos() + 0.005 || mm.mp() > cm.maxPos() - 0.005) return false;
			}
			return true;
		};
		if (target.model->solverPool()[0].kinPos() && check_motion_limit()) std::swap(target_pe, target_pe_new);

		// calculate real value //
		double pe_next[6], ve_next[6], ae_next[6];
		for (int i = 0; i < 6; ++i)
		{
			amc::Size t;
			amc::plan::moveAbsolute2(pe_now[i], ve_now[i], ae_now[i]
				, target_pe[i], 0.0, 0.0
				, imp_->ve_[i], imp_->ae_[i], imp_->de_[i]
				, 1e-3, 1e-10, pe_next[i], ve_next[i], ae_next[i], t);
		}

		// set everything //
		target.model->generalMotionPool()[0].setMpe(pe_next, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].setMve(ve_next, imp_->eul_type.c_str());
		target.model->generalMotionPool()[0].setMae(ae_next, imp_->eul_type.c_str());
		target.model->solverPool()[0].kinPos();
		target.model->solverPool()[0].kinVel();

		return imp_->is_running_.load() ? 1 : 0;
	}
	auto ManualMove::collectNrt(PlanTarget &target)-> void
	{ 
		if (~(target.option | USE_TARGET_POS))Imp::is_running_.store(false); 
	}
	ManualMove::~ManualMove() = default;
	ManualMove::ManualMove(const std::string &name) : Plan(name), imp_(new Imp)
	{
		command().loadXmlStr(
			"<Command name=\"mm\">"
			"	<GroupParam>"
			"		<UniqueParam default=\"start_group\">"
			"			<GroupParam name=\"start_group\">"
			"				<Param name=\"start\"/>"
			"				<Param name=\"ve\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"ae\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"de\" default=\"{0.1,0.1,0.1,0.1,0.1,0.1}\"/>"
			"				<Param name=\"increase_count\" default=\"50\"/>"
			"				<Param name=\"eul_type\" default=\"321\"/>"
			"			</GroupParam>"
			"			<Param name=\"stop\"/>"
			"			<GroupParam>"
			"				<Param name=\"x\" default=\"0\"/>"
			"				<Param name=\"y\" default=\"0\"/>"
			"				<Param name=\"z\" default=\"0\"/>"
			"				<Param name=\"a\" default=\"0\"/>"
			"				<Param name=\"b\" default=\"0\"/>"
			"				<Param name=\"c\" default=\"0\"/>"
			"			</GroupParam>"
			"		</UniqueParam>"
					CHECK_PARAM_STRING
			"	</GroupParam>"
			"</Command>");
	}
	AMC_DEFINE_BIG_FOUR_CPP(ManualMove);

	auto GetPartPq::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		auto pm = std::make_any<std::vector<double> >(target.model->partPool().size() * 16);
		target.server->getRtData([](amc::server::ControlServer& cs, std::any& data)
		{
			for (amc::Size i(-1); ++i < cs.model().partPool().size();)
				cs.model().partPool().at(i).getPm(std::any_cast<std::vector<double>& >(data).data() + i * 16);
		}, pm);

		auto pq = std::vector<double>(target.model->partPool().size() * 7);

		for (amc::Size i(-1); ++i < target.server->model().partPool().size();)
			amc::dynamic::s_pm2pq(std::any_cast<std::vector<double>& >(pm).data() + i * 16, pq.data() + i * 7);
		
		//   return text
		// target.param = amc::core::Matrix(pq.size(), 1, pq.data()).toString();
		//

		//  return binary
		std::string ret(reinterpret_cast<char*>(pq.data()), pq.size() * sizeof(double));
		target.ret = ret;

		target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_PRINT_CMD_INFO | NOT_PRINT_CMD_INFO;
	}
	GetPartPq::~GetPartPq() = default;
	GetPartPq::GetPartPq(const std::string &name) : Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"get_part_pq\">"
			"</Command>");
	}

	auto GetXml::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		target.server->waitForAllCollection();
		target.ret = target.server->xmlString();
		target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION;
	}
	GetXml::~GetXml() = default;
	GetXml::GetXml(const std::string &name) : Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"get_xml\">"
			"</Command>");
	}

	auto SetXml::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{		
		// remove all symbols "{" "}"
		if (target.server->running())THROW_FILE_AND_LINE("server is not running,can't save xml");
		auto xml_str = params.at("xml").substr(1, params.at("xml").size() - 2);
		// 这一句要小心，此时 this 已被销毁，后面不能再调用this了 //
		target.server->loadXmlStr(xml_str);
		target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION;
	}
	SetXml::~SetXml() = default;
	SetXml::SetXml(const std::string &name) : Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"set_xml\">"
			"	<Param name=\"xml\"/>"
			"</Command>");
	}

	auto Start::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		target.server->start();
		target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION;
	}
	Start::~Start() = default;
	Start::Start(const std::string &name) : Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"start\">"
			"</Command>");
	}

	auto Stop::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		target.server->stop();
		target.option |= NOT_RUN_EXECUTE_FUNCTION | NOT_RUN_COLLECT_FUNCTION;
	}
	Stop::~Stop() = default;
	Stop::Stop(const std::string &name) : Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"stop\">"
			"</Command>");
	}

	auto RemoveFile::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void
	{
		std::uintmax_t  memo = std::stoull(params.at("memo"));
		auto file_path = params.at("filePath");

		// 获得所有文件
		std::vector<std::filesystem::path> files;
		for (auto &p : std::filesystem::directory_iterator(file_path))
		{
			if (p.is_regular_file())files.push_back(p.path());
		}
		// 按照修改时间排序
		std::sort(files.begin(), files.end(), [](const std::filesystem::path &p1, const std::filesystem::path &p2)->bool   //lambda函数，匿名
		{
			return std::filesystem::last_write_time(p1) < std::filesystem::last_write_time(p2);
		});

		std::cout << "log file num:" << files.size() << std::endl;

		std::filesystem::space_info devi = std::filesystem::space(file_path);
		// 根据内存地址删除;
		while (devi.available < std::uintmax_t(1048576) * memo && !files.empty())
		{
			std::filesystem::remove(files.back());
			files.pop_back();
			devi = std::filesystem::space(file_path);
		}

		std::cout << "left space  :" << devi.available / std::uintmax_t(1048576) << "MB" << std::endl;

		target.option =	NOT_RUN_EXECUTE_FUNCTION;
	}
	RemoveFile::~RemoveFile() = default;
	RemoveFile::RemoveFile(const std::string &name) :Plan(name)
	{
		command().loadXmlStr(
			"<Command name=\"rmFi\">"
			"	<GroupParam>"
			"	    <Param name=\"filePath\" default=\"C:/Users/qianch_kaanh_cn/Desktop/build_qianch/log/\" abbreviation=\"f\" />"
			"	    <Param name=\"memo\" default=\"40\" abbreviation=\"m\" />"
			"	</GroupParam>"
			"</Command>");
	}

	struct UniversalPlan::Imp
	{
		PrepairFunc prepair_nrt;
		ExecuteFunc execute_rt;
		CollectFunc collect_nrt;
	};
	auto UniversalPlan::prepairNrt(const std::map<std::string, std::string> &params, PlanTarget &target)->void { if (imp_->prepair_nrt)imp_->prepair_nrt(params, target); }
	auto UniversalPlan::executeRT(PlanTarget &param)->int { return imp_->execute_rt ? imp_->execute_rt(param) : 0; }
	auto UniversalPlan::collectNrt(PlanTarget &param)->void
	{ 
		if (imp_->collect_nrt)imp_->collect_nrt(param);
	}
	auto UniversalPlan::setPrepairFunc(PrepairFunc func)->void { imp_->prepair_nrt = func; }
	auto UniversalPlan::setExecuteFunc(ExecuteFunc func)->void { imp_->execute_rt = func; }
	auto UniversalPlan::setCollectFunc(CollectFunc func)->void { imp_->collect_nrt = func; }
	UniversalPlan::~UniversalPlan() = default;
	UniversalPlan::UniversalPlan(const std::string &name, PrepairFunc prepair_func, ExecuteFunc execute_func, CollectFunc collect_func, const std::string & cmd_xml_str) :Plan(name), imp_(new Imp)
	{
		imp_->prepair_nrt = prepair_func;
		imp_->execute_rt = execute_func;
		imp_->collect_nrt = collect_func;
		command().loadXmlStr(cmd_xml_str);
	}
	AMC_DEFINE_BIG_FOUR_CPP(UniversalPlan);
}
