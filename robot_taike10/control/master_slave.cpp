﻿#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <future>

#include "../control/rt_timer.hpp"
#include "../control/master_slave.hpp"

#include "../core/core.hpp"

namespace amc::control
{
	struct Slave::Imp { std::uint16_t phy_id_, sla_id_; };
	auto Slave::saveXml(amc::core::XmlElement &xml_ele) const->void
	{
		Object::saveXml(xml_ele);
		xml_ele.SetAttribute("phy_id", std::to_string(phyId()).c_str());
	}
	auto Slave::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		Object::loadXml(xml_ele);
		imp_->phy_id_ = attributeUint16(xml_ele, "phy_id");
	}
	auto Slave::phyId()const->std::uint16_t { return imp_->phy_id_; }
	Slave::~Slave() = default;
	Slave::Slave(const std::string &name, std::uint16_t phy_id) :Object(name), imp_(new Imp) { imp_->phy_id_ = phy_id; }
	AMC_DEFINE_BIG_FOUR_CPP(Slave);

	struct Master::Imp
	{
	public:
		enum { LOG_NEW_FILE = 1 };
		static auto rt_task_func(void *master)->void
		{
			auto &mst = *reinterpret_cast<Master*>(master);
			auto add_time_to_stastics = [](std::int64_t time, RtStasticsData *data) 
			{
				data->avg_time_consumed += (double(time) - data->avg_time_consumed) / (data->total_count + 1);
				data->max_time_occur_count = time < data->max_time_consumed ? data->max_time_occur_count : data->total_count;
				data->max_time_consumed = std::max(time, data->max_time_consumed);
				data->min_time_occur_count = time > data->min_time_consumed ? data->min_time_occur_count : data->total_count;
				data->min_time_consumed = std::min(time, data->min_time_consumed);
				data->total_count++;
				data->overrun_count += time > 900000 ? 1 : 0;
			};

			amc_rt_task_set_periodic(mst.imp_->sample_period_ns_);

			///////////////////////////////////////////////////////////////
			//std::int64_t time_series[10];
			///////////////////////////////////////////////////////////////

			while (mst.imp_->is_rt_thread_running_)
			{
				///////////////////////////////////////////////////////////////
				//time_series[1] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////

				// receive //
				mst.recv();

				///////////////////////////////////////////////////////////////
				//time_series[2] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////


				// tragectory generator //
				if (mst.imp_->strategy_)mst.imp_->strategy_();

				///////////////////////////////////////////////////////////////
				//time_series[3] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////


				// sync
				mst.sync();

				///////////////////////////////////////////////////////////////
				//time_series[4] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////



				// send
				mst.send();


				///////////////////////////////////////////////////////////////
				//time_series[5] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////


				// flush lout
				mst.lout() << std::flush;
				if (!mst.imp_->lout_msg_.empty())
				{
					mst.imp_->lout_pipe_->sendMsg(mst.imp_->lout_msg_);
					mst.lout().reset();
				}

				// flush mout
				mst.mout() << std::flush;
				if (!mst.imp_->mout_msg_.empty())
				{
					mst.imp_->mout_pipe_->sendMsg(mst.imp_->mout_msg_);
					mst.mout().reset();
				}

				///////////////////////////////////////////////////////////////
				//time_series[6] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////


				// record stastics //
				auto time = amc_rt_time_since_last_time();
				add_time_to_stastics(time, &mst.imp_->global_stastics_);
				if(mst.imp_->this_stastics_)add_time_to_stastics(time, mst.imp_->this_stastics_);
				if (mst.imp_->is_need_change_)
				{
					if (mst.imp_->is_this_ready_)mst.imp_->is_this_ready_->store(true);
					mst.imp_->this_stastics_ = mst.imp_->next_stastics_;
					mst.imp_->is_this_ready_ = mst.imp_->is_next_ready_;
				}

				///////////////////////////////////////////////////////////////
				/*time_series[7] = amc_rt_time_since_last_time();
				
				if (time_series[7] > 500000)
				{
					mst.mout() << "0: " << time_series[0]
						<< "\n1: " << time_series[1]
						<< "\n2: " << time_series[2]
						<< "\n3: " << time_series[3]
						<< "\n4: " << time_series[4]
						<< "\n5: " << time_series[5]
						<< "\n6: " << time_series[6]
						<< "\n7: " << time_series[7] << "\n-----------------------------------------------" << std::endl;
				}
				
				
				*/
				///////////////////////////////////////////////////////////////




				// rt timer //
				amc_rt_task_wait_period();

				///////////////////////////////////////////////////////////////
				//time_series[0] = amc_rt_time_since_last_time();
				///////////////////////////////////////////////////////////////
			}

			mst.imp_->is_mout_thread_running_ = false;
		}

		// slave //
		amc::core::ObjectPool<Slave> *slave_pool_;
		std::vector<Size> sla_vec_phy2abs_;

		// for mout and lout //
		amc::core::Pipe *mout_pipe_, *lout_pipe_;
		amc::core::MsgFix<MAX_MSG_SIZE> mout_msg_, lout_msg_;
		std::unique_ptr<amc::core::MsgStream> mout_msg_stream_, lout_msg_stream_;
		std::thread mout_thread_;

		// strategy //
		std::function<void()> strategy_{ nullptr };

		// running flag //
		std::mutex mu_running_;
		std::atomic_bool is_rt_thread_running_{ false };
		std::atomic_bool is_mout_thread_running_{ false };

		const int sample_period_ns_{ 1000000 };

		// rt stastics //
		Master::RtStasticsData global_stastics_{ 0,0,0,0x8fffffff,0,0,0 };
		Master::RtStasticsData* this_stastics_{ nullptr }, *next_stastics_{ nullptr };
		std::atomic_bool* is_this_ready_{ nullptr }, *is_next_ready_{ nullptr };
		bool is_need_change_{ false };

		std::any rt_task_handle_;

		Imp() { mout_msg_stream_.reset(new amc::core::MsgStream(mout_msg_)); lout_msg_stream_.reset(new amc::core::MsgStream(lout_msg_)); }

		friend class Slave;
		friend class Master;
	};
	auto Master::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		Object::loadXml(xml_ele);

		imp_->slave_pool_ = findByName("slave_pool") == children().end() ? &add<amc::core::ObjectPool<Slave, Object> >("slave_pool") : static_cast<amc::core::ObjectPool<Slave, Object> *>(&(*findByName("slave_pool")));
		imp_->mout_pipe_ = findOrInsert<amc::core::Pipe>("mout_pipe");
		imp_->lout_pipe_ = findOrInsert<amc::core::Pipe>("lout_pipe");
	}
	auto Master::start()->void
	{
		std::unique_lock<std::mutex> running_lck(imp_->mu_running_);
		if (imp_->is_rt_thread_running_)throw std::runtime_error("master already running, so cannot start");
		imp_->is_rt_thread_running_ = true;

		// make vec_phy2abs //
		imp_->sla_vec_phy2abs_.clear();
		for (auto &sla : slavePool())
		{
			imp_->sla_vec_phy2abs_.resize(std::max(static_cast<amc::Size>(sla.phyId() + 1), imp_->sla_vec_phy2abs_.size()), -1);
			if (imp_->sla_vec_phy2abs_.at(sla.phyId()) != -1) throw std::runtime_error("invalid Master::Slave phy id:\"" + std::to_string(sla.phyId()) + "\" of slave \"" + sla.name() + "\" already exists");
			imp_->sla_vec_phy2abs_.at(sla.phyId()) = sla.id();
		}

		// init child master //
		init();

		// lock memory // 
		amc_mlockall();

		// create mout & lout thread //
		imp_->is_mout_thread_running_ = true;
		imp_->mout_thread_ = std::thread([this]()
		{
			// prepair lout //
			auto file_name = amc::core::logDirPath() / ("rt_log--" + amc::core::logFileTimeFormat(std::chrono::system_clock::now()) + "--");
			std::fstream file;
			file.open(file_name.string() + "0.txt", std::ios::out | std::ios::trunc);

			// start read mout and lout //
			amc::core::Msg msg;
			while (imp_->is_mout_thread_running_)
			{
				if (imp_->lout_pipe_->recvMsg(msg))
				{
					if (msg.msgID() == Imp::LOG_NEW_FILE)
					{
						file.close();
						file.open(file_name.string() + msg.toString() + ".txt", std::ios::out | std::ios::trunc);
					}
					else if (!msg.empty())
					{
						file << msg.toString();
					}
				}
				else if (imp_->mout_pipe_->recvMsg(msg))
				{
					if (!msg.empty())std::cout << msg.toString() << std::flush;
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}

			// 结束前最后一次接收，此时实时线程已经结束 //
			while (imp_->mout_pipe_->recvMsg(msg)) if (!msg.empty())std::cout << msg.toString() << std::endl;
			while (imp_->lout_pipe_->recvMsg(msg))
			{
				if (msg.msgID() == Imp::LOG_NEW_FILE)
				{
					file.close();
					file.open(file_name.string() + msg.toString() + ".txt", std::ios::out | std::ios::trunc);
				}
				else if (!msg.empty())
				{
					file << msg.toString();
				}
			}
		});

		// create and start rt task //
		imp_->rt_task_handle_ = amc_rt_task_create();
		if (!imp_->rt_task_handle_.has_value()) throw std::runtime_error("rt_task_create failed");
		if (amc_rt_task_start(rtHandle(), &Imp::rt_task_func, this))throw std::runtime_error("rt_task_start failed");
	}
	auto Master::stop()->void
	{
		std::unique_lock<std::mutex> running_lck(imp_->mu_running_);
		if (!imp_->is_rt_thread_running_)throw std::runtime_error("master is not running, so can't stop");
		imp_->is_rt_thread_running_ = false;

		// join rt task //
		if (amc_rt_task_join(rtHandle()))throw std::runtime_error("amc_rt_task_join failed");

		// join mout task //
		imp_->mout_thread_.join();

		// release child resources //
		release();
	}
	auto Master::setControlStrategy(std::function<void()> strategy)->void
	{
		std::unique_lock<std::mutex> running_lck(imp_->mu_running_);
		if (imp_->is_rt_thread_running_)throw std::runtime_error("master already running, cannot set control strategy");
		imp_->strategy_ = strategy;
	}
	auto Master::rtHandle()->std::any& { return imp_->rt_task_handle_; }
	auto Master::logFile(const char *file_name)->void
	{
		// 将已有的log数据发送过去 //
		if (!imp_->lout_msg_.empty())
		{
			// 补充一个0作为结尾 //
			lout() << std::flush;
			imp_->lout_pipe_->sendMsg(imp_->lout_msg_);
			imp_->lout_msg_.resize(0);
		}

		// 发送切换文件的msg //
		imp_->lout_msg_.setMsgID(Imp::LOG_NEW_FILE);
		imp_->lout_msg_.copy(file_name);
		imp_->lout_pipe_->sendMsg(imp_->lout_msg_);

		// 将msg变更回去
		imp_->lout_msg_.setMsgID(0);
		imp_->lout_msg_.resize(0);
	}
	auto Master::lout()->amc::core::MsgStream & { return *imp_->lout_msg_stream_; }
	auto Master::mout()->amc::core::MsgStream & { return *imp_->mout_msg_stream_; }
	auto Master::slaveAtPhy(amc::Size id)->Slave& { return slavePool().at(imp_->sla_vec_phy2abs_.at(id)); }
	auto Master::slavePool()->amc::core::ObjectPool<Slave, amc::core::Object>& { return *imp_->slave_pool_; }
	auto Master::resetRtStasticData(RtStasticsData *stastics, bool is_new_data_include_this_count)->void
	{
		if (stastics)*stastics = RtStasticsData{ 0,0,0,0x8fffffff,0,0,0 };
		
		if (is_new_data_include_this_count)
		{
			// make new stastics //
			imp_->this_stastics_ = stastics;
			imp_->is_need_change_ = !is_new_data_include_this_count;
		}
		else
		{
			// mark need to change when this count finished //
			imp_->next_stastics_ = stastics;
			imp_->is_need_change_ = !is_new_data_include_this_count;
		}
	}
	Master::~Master() = default;
	Master::Master(const std::string &name) :imp_(new Imp), Object(name)
	{
		this->registerType<amc::core::ObjectPool<Slave, amc::core::Object> >();
		
		imp_->slave_pool_ = &add<amc::core::ObjectPool<Slave> >("slave_pool");
		imp_->mout_pipe_ = &add<amc::core::Pipe>("mout_pipe");
		imp_->lout_pipe_ = &add<amc::core::Pipe>("lout_pipe");
	}
}
