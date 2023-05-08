#include <iostream>
#include <thread>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <atomic>


#include <./control/control.hpp>

using namespace amc::control;

void test_control_master_slave()
{
	amc::control::Master m;
	
	std::vector<amc::control::Master::RtStasticsData> stastics(50);
	std::vector<std::atomic_bool> flags(50);
	for (auto &flag : flags)flag.store(false);

	m.setControlStrategy([&]() 
	{
		static int count{0};

		if (count % 1000 == 0)
		{
			std::cout << "count:" << count << std::endl;
			stastics.push_back(amc::control::Master::RtStasticsData());

			m.resetRtStasticData(&stastics.at(count / 1000), true);
		}

		if (count > 1000) flags[count / 1000 - 1].store(true);

		++count;
	});
	m.start();

	for (int i = 0; i < 25; ++i)
	{
		while (!flags[i].load())std::this_thread::sleep_for(std::chrono::milliseconds(1));
		
		std::cout << "---------------------------------------------------------" << std::endl;
		std::cout << "total count:" << stastics[i].total_count << std::endl;
		std::cout << "avg cost   :" << stastics[i].avg_time_consumed << std::endl;
		std::cout << "max cost   :" << stastics[i].max_time_consumed << std::endl;
		std::cout << "max count  :" << stastics[i].max_time_occur_count << std::endl;
		std::cout << "min cost   :" << stastics[i].min_time_consumed << std::endl;
		std::cout << "min count  :" << stastics[i].min_time_occur_count << std::endl;
		std::cout << "overruns   :" << stastics[i].overrun_count << std::endl;
	}

	m.stop();
}


int main(int argc, char *argv[])
{

    test_control_master_slave();

    std::cout << "test_control finished, press any key to continue" << std::endl;
    std::cin.get();

    return 0;
}

