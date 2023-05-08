/*
 /home/kk/amc-1.5_taike/test/test_workspace.cpp
 本例子展示基于Taike机器人的工作空间余量求解,xml文件位于:/home/kk/amc-1.5_taike/Taike_Server.xml
 by LMY
 date:2022.10.21
*/

#include <iostream>
#include <array>
#include <./core/core.hpp>
#include <./control/control.hpp>
#include <./dynamic/dynamic.hpp>
#include <./plan/plan.hpp>
#include <./sensor/sensor.hpp>
#include <./server/server.hpp>
#include <./robot/ur.hpp>

using namespace amc::dynamic;

int main()
{
	std::cout << std::endl << "-----------------test model taike---------------------" << std::endl;
	auto&cs = amc::server::ControlServer::instance();

	try
	{
		cs.loadXmlFile(std::string("/home/kk/amc-1.5_taike/Taike_Server.xml"));
	}
	catch (std::exception &e)
	{
		std::cout << "exception:" << e.what() << std::endl;
		return 0;
	}
	

	auto &m = cs.model();
	auto &inv = m.solverPool()[0];
	auto &fwd = m.solverPool()[1];
	auto &ee = m.generalMotionPool()[0];

	// old without yaw//
	//double mid_pe[6]{ 0, -0.012, 0.515, 0, 0, 0 };
	//double x_max{ 0.04 }, y_max{ 0.04 }, z_max{ 0.04 }, a_max{ 0.085 }, b_max{ 0.085 }, c_max{ 0.0 };

	// new with yaw //
	double mid_pe[6]{ 0, 0.13825, 0.7405, 0, 0, 0 };
	double x_max{ 1 }, y_max{ 1 }, z_max{ 1.4810 }, a_max{ 2.5 }, b_max{ 2.5 }, c_max{ 3.14 };
	double vx_max{ 0.1 }, vy_max{ 0.1 }, vz_max{ 0.11 }, va_max{ 0.12 }, vb_max{ 0.12 }, vc_max{ 0.1 };

	auto cpt_margin = [&]() ->double
	{
		std::vector < std::array<double, 6> > points;
		for (int i = 0; i < std::pow(2, 6); ++i)
		{
			points.push_back(std::array<double, 6>{x_max, y_max, z_max, a_max, b_max, c_max });

			for (int j = 0; j < 6; ++j)
			{
				auto v = (i >> j) % 2 == 0 ? 1 : -1;

				points.back()[j] *= v;
			}
		}
		
		
		double max_max_margin(1);
		
		for (auto pnt : points)
		{
			s_va(6, mid_pe, pnt.data());
			ee.setMpe(pnt.data(), "123");

			bool ret = inv.kinPos();

			double max_margin{ 1 };
			for (auto &mot : m.motionPool())
			{
				max_margin = std::min(0.8 - mot.mp(), max_margin);
				max_margin = std::min(mot.mp() - 0.6, max_margin);
			}


			max_max_margin = std::min(max_max_margin, max_margin);
			if (max_max_margin < 0) return max_max_margin;
		}

		return max_max_margin;
	};
	auto cpt_exact = [&](double &p_ret, double &v_ret) ->void
	{
		const int num = 2;
		const int t_num = 2 * num + 1;
		std::vector < std::array<double, 6> > points;
		for (int i = 0; i < std::pow(t_num, 6); ++i)
		{
			points.push_back(std::array<double, 6>{x_max, y_max, z_max, a_max, b_max, c_max });

			for (int j = 0; j < 6; ++j)
			{
				auto v = (i / (int)std::pow(t_num, j)) % t_num;
				double r = (v - num) / double(num);
				points.back()[j] *= r;
			}
		}

		std::vector < std::array<double, 6> > vels;
		for (int i = 0; i < std::pow(2, 6); ++i)
		{
			vels.push_back(std::array<double, 6>{vx_max, vy_max, vz_max, va_max, vb_max, vc_max });

			for (int j = 0; j < 6; ++j)
			{
				auto v = (i >> j) % 2 == 0 ? 1 : -1;

				vels.back()[j] *= v;
			}
		}

		double max_max_margin(1), max_max_v_margin(1);



		double k = 0;
		int aaa = 0;
		for (auto pnt : points)
		{
			s_va(6, mid_pe, pnt.data());
			ee.setMpe(pnt.data(), "123");

			bool ret = inv.kinPos();
			if (ret == false) throw std::runtime_error("failed");
			double max_margin{ 1 };
			for (auto &mot : m.motionPool())
			{
				max_margin = std::min(0.8 - mot.mp(), max_margin);
				max_margin = std::min(mot.mp() - 0.6, max_margin);
			}

			max_max_margin = std::min(max_max_margin, max_margin);
			//if (max_max_margin < 0) return max_max_margin;


			for (auto &vel : vels)
			{
				ee.setMve(vel.data(), "123");
				inv.kinVel();
				double max_v_margin{ 1 };
				for (auto &mot : m.motionPool())
				{
					max_v_margin = std::min(0.3 - mot.mv(), max_v_margin);
					max_v_margin = std::min(mot.mv() + 0.3, max_v_margin);
				}
				max_max_v_margin = std::min(max_max_v_margin, max_v_margin);
			}
			
			//if (max_max_margin < 0) return max_max_margin;


			if (double(aaa) / points.size() > k + 0.01)
			{
				std::cout << k + 0.01 << std::endl;
				k += 0.01;
			}
				
			aaa++;
		}


		p_ret = max_max_margin;
		v_ret = max_max_v_margin;
	};




	std::cout << "work space margin result:" << cpt_margin() << std::endl;

	double p_margin, v_margin;
	cpt_exact(p_margin, v_margin);
	std::cout << "work space margin result:" << p_margin << "  " << v_margin << std::endl;


	ee.setMpe(std::array<double, 6>{0.0, 0.2765, 1.4810, 0.0, 0.0, 0.0}.data(), "123");
	bool ret = inv.kinPos();

	for (auto &m : cs.model().motionPool())
	{
		std::cout << std::setprecision(16) << (m.mp() - 0.535) / (0.765 - 0.535) << std::endl;
	}



	


	
	
	

	

	// std::cout << "input:";

	
	// std::cout <<"margin: "<< max_margin << std::endl;
	// ret = fwd.kinPos();

	

	ee.updMpm();
	dsp(4, 4, *ee.mpm());




	







	std::cout << "demo_taike finished, press any key to continue" << std::endl;
	std::cin.get();
	return 0; 
}

