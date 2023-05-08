#ifndef AMC_CONTROL_RT_TIMER_H_
#define AMC_CONTROL_RT_TIMER_H_

#include <cstdint>
#include <any>

namespace amc::control
{
	auto amc_mlockall()->void;

	auto amc_rt_task_create()->std::any;
	auto amc_rt_task_start(std::any& handle, void(*task_func)(void*), void*param)->int;
	auto amc_rt_task_join(std::any& handle)->int;
	auto amc_rt_task_set_periodic(int nanoseconds)->int;
	auto amc_rt_task_wait_period()->int;
	auto amc_rt_timer_read()->std::int64_t;

	// in nano seconds
	auto amc_rt_time_since_last_time()->std::int64_t;
}

#endif
