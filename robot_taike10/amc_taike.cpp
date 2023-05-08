/*
 /home/kk/amc-1.5_taike/amc_taike.cpp
 本例子展示基于xml的Taike机器人交互命令控制，并与实体机器人联合调试，测试movel.xml文件位于:/home/kk/amc-1.5_taike/Taike_Server.xml
 by LMY
 date:2022.10.21
*/

#include <iostream>
#include <./core/core.hpp>
#include <./control/control.hpp>
#include <./dynamic/dynamic.hpp>
#include <./plan/plan.hpp>
#include <./sensor/sensor.hpp>
#include <./server/server.hpp>
#include <./robot/ur.hpp>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace amc::dynamic;
using namespace amc::robot;
/*
auto inline outputCsByPq(const amc::server::ControlServer &cs, std::string file_path) -> void
{
	amc::core::XmlDocument doc;

	cs.saveXmlDoc(doc);

	auto part_pool = doc.FirstChildElement()->FirstChildElement("model")->FirstChildElement("part_pool");

	for (auto prt = part_pool->FirstChildElement(); prt; prt = prt->NextSiblingElement())
	{
		amc::core::Calculator c;
		auto mat = c.calculateExpression(prt->Attribute("pe"));
		prt->DeleteAttribute("pe");

		double pq[7];
		s_pe2pq(mat.data(), pq);
		prt->SetAttribute("pq", amc::core::Matrix(1, 7, pq).toString().c_str());

		for (auto geo = prt->FirstChildElement("geometry_pool")->FirstChildElement(); geo; geo = geo->NextSiblingElement())
		{
			auto mat = c.calculateExpression(geo->Attribute("pe"));
			geo->DeleteAttribute("pe");

			double pq[7];
			s_pe2pq(mat.data(), pq);
			geo->SetAttribute("pq", amc::core::Matrix(1, 7, pq).toString().c_str());
		}
	}

	doc.SaveFile(file_path.c_str());
}
*/
// 示例轨迹规划 
class MyPlan :public amc::plan::Plan
{
public:
	auto virtual prepairNrt(const std::map<std::string, std::string> &params, amc::plan::PlanTarget &target)->void override
	{
	}
	auto virtual executeRT(amc::plan::PlanTarget &target)->int override
	{
		return 0;
	}
	auto virtual collectNrt(amc::plan::PlanTarget &target)->void override 
	{
	}

	explicit MyPlan(const std::string &name = "my_plan")
	{
		command().loadXmlStr(
			"<Command name=\"my_plan\">"
			"</Command>"
		);
	}
	AMC_REGISTER_TYPE(MyPlan);
};

int main()
{
	// 从xml中读取机器人，仅包含基本的轨迹规划 
	auto&cs = amc::server::ControlServer::instance();
	cs.loadXmlFile(std::string("/home/kk/robot_taike10/Taike_Server.xml"));

	// 添加自己的轨迹规划 
	cs.planRoot().planPool().add<MyPlan>();
    //cs.planRoot().planPool().add<amc::plan::RemoveFile>("remove_file"); // 清理日志空间

	// 启动 server 
	cs.start();

    // try
	// {
	// 	cs.executeCmd(amc::core::Msg("rmFi --filePath=/home/kk/var/log/ --memo=20000"));
	// }
	// catch (std::exception &e)
	// {
	// 	std::cout << e.what() << std::endl;
	// }

	
    //----------------------以下内容为CS架构，需要修改，暂时不用---------------------
    // 交互
	std::list<std::tuple<amc::core::Msg, std::shared_ptr<amc::plan::PlanTarget>>> result_list;
	std::mutex result_mutex;

	amc::core::Socket socket("server", "", "5866", amc::core::Socket::WEB);
	socket.setOnReceivedMsg([&](amc::core::Socket *socket, amc::core::Msg &msg) -> int
							{
		std::string msg_data = msg.toString();

		static int cout_count = 0;
		if(++cout_count%10 == 0)
			std::cout << "recv:" << msg_data << std::endl;

		LOG_INFO << "the request is cmd:"
			<< msg.header().msg_size_ << "&"
			<< msg.header().msg_id_ << "&"
			<< msg.header().msg_type_ << "&"
			<< msg.header().reserved1_ << "&"
			<< msg.header().reserved2_ << "&"
			<< msg.header().reserved3_ << ":"
			<< msg_data << std::endl;

		try
		{
			std::stringstream ss(msg_data);
			for (std::string cmd; std::getline(ss, cmd);)
			{
				auto result = cs.executeCmd(amc::core::Msg(cmd));

				std::unique_lock<std::mutex> l(result_mutex);
				result_list.push_back(std::make_tuple(msg, result));
			}
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
			LOG_ERROR << e.what() << std::endl;

			try
			{
				amc::core::Msg m;
				m.setMsgID(msg.header().msg_id_);
				m.setType(msg.header().msg_type_);
				m.header().reserved1_ = msg.header().reserved1_;
				m.header().reserved2_ = msg.header().reserved2_;
				m.header().reserved3_ = msg.header().reserved3_;
				socket->sendMsg(m);
			}
			catch (std::exception &e)
			{
				std::cout << e.what() << std::endl;
				LOG_ERROR << e.what() << std::endl;
			}
		}

		return 0; });
	socket.setOnReceivedConnection([](amc::core::Socket *sock, const char *ip, int port) -> int
								   {
		std::cout << "socket receive connection" << std::endl;
		LOG_INFO << "socket receive connection:\n"
			<< std::setw(amc::core::LOG_SPACE_WIDTH) << "|" << "  ip:" << ip << "\n"
			<< std::setw(amc::core::LOG_SPACE_WIDTH) << "|" << "port:" << port << std::endl;
		return 0; });
	socket.setOnLoseConnection([](amc::core::Socket *socket)
							   {
		std::cout << "socket lose connection" << std::endl;
		LOG_INFO << "socket lose connection" << std::endl;
		for (;;)
		{
			try
			{
				socket->startServer("5866");
				break;
			}
			catch (std::runtime_error &e)
			{
				std::cout << e.what() << std::endl << "will try to restart server socket in 1s" << std::endl;
				LOG_ERROR << e.what() << std::endl << "will try to restart server socket in 1s" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		std::cout << "socket restart successful" << std::endl;
		LOG_INFO << "socket restart successful" << std::endl;

		return 0; });
    socket.startServer();
	//socket.startServer(std::to_string(port));

	std::thread result_thread([&]()
							  {
		while (true)
		{
			std::unique_lock<std::mutex> lck(result_mutex);
			for (auto result = result_list.begin(); result != result_list.end();)
			{
				auto cmd_ret = std::get<1>(*result);
				if (cmd_ret->finished.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
				{
					auto ret = cmd_ret->ret;
					auto &msg = std::get<0>(*result);

					if (auto str = std::any_cast<std::string>(&ret))
					{
						try
						{
							amc::core::Msg ret_msg(*str);
							
							ret_msg.setMsgID(msg.header().msg_id_);
							ret_msg.setType(msg.header().msg_type_);
							ret_msg.header().reserved1_ = msg.header().reserved1_;
							ret_msg.header().reserved2_ = msg.header().reserved2_;
							ret_msg.header().reserved3_ = msg.header().reserved3_;
							socket.sendMsg(ret_msg);
						}
						catch (std::exception &e)
						{
							std::cout << e.what() << std::endl;
							LOG_ERROR << e.what() << std::endl;
						}
					}
					else
					{
						try
						{
							amc::core::Msg ret_msg;
							ret_msg.setMsgID(msg.header().msg_id_);
							ret_msg.setType(msg.header().msg_type_);
							ret_msg.header().reserved1_ = msg.header().reserved1_;
							ret_msg.header().reserved2_ = msg.header().reserved2_;
							ret_msg.header().reserved3_ = msg.header().reserved3_;
							socket.sendMsg(ret_msg);
						}
						catch (std::exception &e)
						{
							std::cout << e.what() << std::endl;
							LOG_ERROR << e.what() << std::endl;
						}
					}
				}

				result_list.erase(result++);
			}
			lck.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		} });
    /*
	amc::core::Socket udp_socket("server", "", "5867", amc::core::Socket::UDP_RAW);
	udp_socket.setOnReceivedRawData([&](amc::core::Socket *socket, const char *data, int size) -> int
									{
		try
		{
			std::string msg_data(data, size);

			amc::core::Calculator c;
			auto mat = c.calculateExpression(msg_data);

			double value[6]{ 2147483647, 2147483647, 2147483647, 2147483647, 2147483647, 2147483647 };
			s_vs(6, value, mat.data());
			s_nv(6, 1.0 / 2147483647.0, mat.data());

			// xy 客户和simtool不一样 //
			std::swap(mat.data()[0], mat.data()[1]);
			mat.data()[0] = -mat.data()[0];

			std::swap(mat.data()[3], mat.data()[4]);
			mat.data()[3] = -mat.data()[3];

			mat.data()[0] *= 0.035;
			mat.data()[1] *= 0.035;
			mat.data()[2] *= 0.037;
			mat.data()[3] *= 0.08;
			mat.data()[4] *= 0.08;
			mat.data()[5] *= 0.04;

			// 向上的轴加1.0，为默认位置 //
			mat.data()[2] += 0.513;
			mat.data()[1] -= 0.0103;

			auto cmd = "am --pe=" + mat.toString();
			
			static int i = 0;
			if (++i % 100 == 0)
			{
				std::cout << cmd << std::endl;
			}


			cs.executeCmd(amc::core::Msg(cmd));
		}
		catch (std::runtime_error &e)
		{
			std::cout << e.what() << std::endl;
		}

		return 0; });
    udp_socket.setOnReceivedConnection([](amc::core::Socket *sock, const char *ip, int port) -> int
									   {
		std::cout << "socket receive connection" << std::endl;
		LOG_INFO << "socket receive connection:\n"
			<< std::setw(amc::core::LOG_SPACE_WIDTH) << "|" << "  ip:" << ip << "\n"
			<< std::setw(amc::core::LOG_SPACE_WIDTH) << "|" << "port:" << port << std::endl;
		return 0; });
	udp_socket.setOnLoseConnection([](amc::core::Socket *socket)
								   {
		std::cout << "socket lose connection" << std::endl;
		LOG_INFO << "socket lose connection" << std::endl;
		for (;;)
		{
			try
			{
				socket->startServer("5866");
				break;
			}
			catch (std::runtime_error &e)
			{
				std::cout << e.what() << std::endl << "will try to restart server socket in 1s" << std::endl;
				LOG_ERROR << e.what() << std::endl << "will try to restart server socket in 1s" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
		std::cout << "socket restart successful" << std::endl;
		LOG_INFO << "socket restart successful" << std::endl;

		return 0; });
	udp_socket.startServer();
    */

   	//创建管道,实现机器人与夹抓的通信
	//判断有名管道文件是否存在
	int ret1 = access("fifo",F_OK);
	if (ret1 == -1)
	{
		printf("管道文件不存在，创建对应的有名管道\n");
		ret1 = mkfifo("fifo",0664);
		if (ret1 == -1)
		{
			perror("mkfifo");
			exit(0);
		}
		
	}

	int ret2 = access("fifo2",F_OK);
	if (ret2 == -1)
	{
		printf("管道文件不存在，创建对应的有名管道\n");
		ret2 = mkfifo("fifo2",0664);
		if (ret1 == -1)
		{
			perror("mkfifo");
			exit(0);
		}
		
	}

	//以只读的方式打开管道fifo2
	int fdr = open("fifo2",O_RDONLY);
	if (fdr == -1)
	{
		perror("open");
		exit(0);
	}
	printf("打开管道fifo2成功,等待读取相机指令...\n");
	char buf1[128];

	// 以只写的方式打开管道fifo
	int fdw = open("fifo",O_WRONLY);
	if (fdw == -1)
	{
		perror("open");
		exit(0);
	}
	printf("打开管道fifo成功,等待写入机器人运行指令...\n");
	char buf[128];
	while (1)
	{
		memset(buf,0,128);
		memset(buf1,0,128);
		// 接收命令 //
		for (std::string command_in; std::getline(std::cin, command_in);)
		{
			if (command_in == "grab")
			{
				printf("输入夹爪指令...\n");
				fgets(buf,128,stdin);
				//写数据
				ret1 = write(fdw,buf,strlen(buf));
				if (ret1 == -1)
				{
					perror("write");
					exit(0);
				}

			}
			else if (command_in == "camera")
			{
				memset(buf1,0,128);
				ret2 = read(fdr,buf1,128);
				if (ret2 <= 0)
				{
					perror("read");
					break;
				}
				printf("机器人的坐标点为:%s\n",buf1);
			}
			else
			{
				try
				{
					auto target = cs.executeCmd(amc::core::Msg(command_in));
					target->finished.get();
					if (auto str = std::any_cast<std::string>(&target->ret))
					{
						std::cout << *str << std::endl;
					}
				}
				catch (std::exception &e)
				{
					std::cout << e.what() << std::endl;
					LOG_ERROR << e.what() << std::endl;
				}
			}
				
		}
	}
	
	
	close(fdr);
	close(fdw);
	return 0;


}
