/*
 /home/kk/amc-1.5_taike/test/taike_model.cpp
 本例子展示基于Taike机器人的建模及运动学和动力学求解及adams仿真
 by LMY
 date:2022.10.17
*/

#include <iostream>
#include <array>
#include <algorithm>
#include <chrono>
#include <./core/core.hpp>
#include <./control/control.hpp>
#include <./dynamic/dynamic.hpp>
#include <./plan/plan.hpp>
#include <./sensor/sensor.hpp>
#include <./server/server.hpp>
#include <./robot/ur.hpp>

using namespace amc::dynamic;
using namespace amc::robot;

const double PI = 3.14159265358979;


// 机器人建模
auto build_Taike(const double *robot_pm = nullptr)->std::unique_ptr<amc::dynamic::Model>
{
    std::unique_ptr<amc::dynamic::Model> Taike = std::make_unique<amc::dynamic::Model>("Taike");
    // 设置重力 //
    const double gravity[6]{0.0, 0.0, -9.8, 0.0, 0.0, 0.0};
    Taike->environment().setGravity(gravity);

    // 添加杆件 //
    // iv : 惯性向量 : m cx cy cz Ixx Iyy Izz Ixy Ixz Iyz inertia_vector的定义为：[m, m*x, m*y, m*z, Ixx, Iyy, Izz, Ixy, Ixz, Iyz]，其中x,y,z为质心位置
    // pm : 4x4 位姿矩阵
    // pe : 6x1 位置和欧拉角

    double iv[10], pm[16];

    const double p1_pe[6]{0, 0.0, 0.148, 0.0, 0.0, 0.0};
    const double p1_iv[10]{8.8414, 8.8414*0.0068091, 8.8414*0.0002614, -8.8414*0.0235903, 0.0354404363, 0.0366632259, 0.0276889420, -0.0000284143, -0.0000943487, -0.0001164211};
    s_pe2pm(p1_pe, pm, "321");
    s_iv2iv(pm, p1_iv, iv);
    auto &p1 = Taike->partPool().add<Part>("L1", iv);

    const double p2_pe[6]{0.3315, 0.182, 0.148, 0.0, 0.0, 0.0};
    const double p2_iv[10]{17.3086, 17.3086*0.2519809, -17.3086*0.0000016, -17.3086*0.0175053, 0.0585662117, 2.5646270229, 2.5517830494, 0.0000016067, -0.0587790900, -0.0000002770};
    s_pe2pm(p2_pe, pm, "321");
    s_iv2iv(pm, p2_iv, iv);
    auto &p2 = Taike->partPool().add<Part>("L2", iv);

    const double p3_pe[6]{0.663 + 0.27375, 0.182 - 0.1345, 0, 0.0, 0.0, 0.0};
    const double p3_iv[10]{5.1536, 5.1536*0.3736876, 5.1536*0.0000320, -5.1536*0.1262755, 0.0915409715, 1.0356705857, 0.9515246752, 0.0000930281, -0.2415439808, -0.0000241865};
    s_pe2pm(p3_pe, pm, "321");
    s_iv2iv(pm, p3_iv, iv);
    auto &p3 = Taike->partPool().add<Part>("L3", iv);

    const double p4_pe[6]{0.663 + 0.5475 + 0.148, 0.182 - 0.1345, 0, 0.0, 0.0, 0.0};
    const double p4_iv[10]{2.6282, 2.6282*0.0122476, -2.6282*0.0000629, 2.6282*0.1192275, 0.0401377735, 0.0424407817, 0.0049297303, 0.0000019298, 0.0039090618, -0.0000225393};
    s_pe2pm(p4_pe, pm, "321");
    s_iv2iv(pm, p4_iv, iv);
    auto &p4 = Taike->partPool().add<Part>("L4", iv);

    const double p5_pe[6]{0.663 + 0.5475 + 0.148, 0.182 - 0.1345 + 0.1225, 0, 0.0, 0.0, 0.0};
    const double p5_iv[10]{2.6282, -2.6282*0.0122476, 2.6282*0.0000629, 2.6282*0.1192275, 0.0401377734, 0.0424407817, 0.0049297303, 0.0000019297, -0.0039090617, 0.0000225405};
    s_pe2pm(p5_pe, pm, "321");
    s_iv2iv(pm, p5_iv, iv);
    auto &p5 = Taike->partPool().add<Part>("L5", iv);

    const double p6_pe[6]{0.663 + 0.5475 + 0.148 + 0.1225, 0.182 - 0.1345 + 0.1225, 0, 0.0, 0.0, 0.0};
    const double p6_iv[10]{0.3379, -0.3379*0.0000010642, 0.3379*0.0000067, 0.3379*0.0864137, 0.0027994870, 0.0028098952, 0.0004508416, -0.0000000521, -0.0000353626, 0.0000001895};
    s_pe2pm(p6_pe, pm, "321");
    s_iv2iv(pm, p6_iv, iv);
    auto &p6 = Taike->partPool().add<Part>("L6", iv);

    // 添加仿真模型
    Taike->ground().geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/ground.xmt_txt");
	Taike->partPool().at(1).geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/joint1.xmt_txt");
	Taike->partPool().at(2).geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/joint2.xmt_txt");
	Taike->partPool().at(3).geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/joint3.xmt_txt");
	Taike->partPool().at(4).geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/joint4.xmt_txt");
    Taike->partPool().at(5).geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/joint5.xmt_txt");
	Taike->partPool().at(6).geometryPool().add<amc::dynamic::ParasolidGeometry>("parasolid_geometry", "/home/kk/robot_taike10/resource/joint6.xmt_txt");

    // 添加关节
    const double j1_pos[6]{ 0.148, 0.0, 0 },j1_axis[6]{0.0, 0.0, 1.0};
    const double j2_pos[6]{ 0.148, 0.182, 0},j2_axis[6]{0.0, 1.0, 0.0};
    const double j3_pos[6]{ 0.663 + 0.148, 0.182 - 0.1345, 0},j3_axis[6]{0.0, 1.0, 0.0};
    const double j4_pos[6]{ 0.663 + 0.5475 + 0.148, 0.182 - 0.1345, 0 },j4_axis[6]{0.0, 1.0, 0.0};
    const double j5_pos[6]{ 0.663 + 0.5475 + 0.148, 0.182 - 0.1345 + 0.1225, 0 },j5_axis[6]{0.0, 0.0, 1.0};
    const double j6_pos[6]{ 0.663 + 0.5475 + 0.148 + 0.1225, 0.182 - 0.1345 + 0.1225, 0},j6_axis[6]{0.0, 1.0, 0.0};

    auto &j1 = Taike->addRevoluteJoint(p1, Taike->ground(), j1_pos, j1_axis);
    auto &j2 = Taike->addRevoluteJoint(p2, p1, j2_pos, j2_axis);
    auto &j3 = Taike->addRevoluteJoint(p3, p2, j3_pos, j3_axis);
    auto &j4 = Taike->addRevoluteJoint(p4, p3, j4_pos, j4_axis);
    auto &j5 = Taike->addRevoluteJoint(p5, p4, j5_pos, j5_axis);
    auto &j6 = Taike->addRevoluteJoint(p6, p5, j6_pos, j6_axis);

    // 添加驱动
    auto &m1 = Taike->addMotion(j1);
    auto &m2 = Taike->addMotion(j2);
    auto &m3 = Taike->addMotion(j3);
    auto &m4 = Taike->addMotion(j4);
    auto &m5 = Taike->addMotion(j5);
    auto &m6 = Taike->addMotion(j6);

    // 添加末端执行器
    double ee_pe[6]{0.663 + 0.5475 + 0.148 + 0.1225, 0.182 - 0.1345 + 0.1225 + 0.1065, 0, PI, PI / 2, 0};
    Taike->addGeneralMotionByPe(p6, Taike->ground(), ee_pe, "321");

    // 添加求解器
    auto &forward_kinematic = Taike->solverPool().add<amc::dynamic::ForwardKinematicSolver>();
    auto &inverse_kinematic = Taike->solverPool().add<amc::dynamic::Taike10InverseKinematicSolver>();
    //auto &inverse_kinematic = Taike->solverPool().add<amc::dynamic::InverseKinematicSolver>();
    auto &inverse_dynamic = Taike->solverPool().add<amc::dynamic::InverseDynamicSolver>();

    return Taike;
}


class My_plan :public amc::plan::Plan
{
public:
    auto virtual executeRT(amc::plan::PlanTarget &target)->int override
	{
		const double PI = 3.141592653589793;
		
		// 设置时间，如果伺服周期为1000Hz，那么每个周期时间递加0.001s 
		target.model->setTime(target.model->time() + 0.001);

		// 计算末端的位置 
		double pe[6]{ 0 , 0 , 0 , 0 , 0 , 0 };
		pe[0] = 1 + 0.3*std::cos(PI * 2 * target.count / 1000);
		pe[1] = 1 + 0.5*std::sin(PI * 2 * target.count / 1000);
		pe[2] = -0.2 + 0.5*std::sin(PI * 2 * target.count / 1000);
		pe[3] = 0.5*std::sin(PI * 2 * target.count / 1000);
        pe[4] = 0.4*std::cos(PI * 2 * target.count / 1000);
        pe[5] = 0.3*std::sin(PI * 2 * target.count / 1000);

		// 设置末端位置 
		target.model->generalMotionPool().front().setMpe(pe, "321");

		// 求反解 
		target.model->solverPool().at(1).kinPos();

		// 轨迹的长度和返回值有关，返回0时轨迹结束，这里仿真1000个周期 
		return 1000 - target.count;
	}

};


void kinematics_Position_solve()
{
    auto r = build_Taike();
    r->saveXmlFile("Taike_Model.xml");
    // 添加求解器
    auto &forward_kinematic = r->solverPool().add<amc::dynamic::ForwardKinematicSolver>();
    //auto &inverse_kinematic = r->solverPool().add<amc::dynamic::InverseKinematicSolver>();
    auto &inverse_kinematic = r->solverPool().add<amc::dynamic::Taike10InverseKinematicSolver>();
    //auto &sim = r->simulatorPool().add<amc::dynamic::Simulator>("sim");
    //auto &result = r->simResultPool().add<amc::dynamic::SimResult>("result");

    auto &ee = r->generalMotionPool().at(0);

    // 为求解器分配内存。注意，求解器一但分配内存后，请不要再添加或删除杆件、关节、驱动、末端等所有元素
    // result.allocateMemory();
    inverse_kinematic.allocateMemory();
    forward_kinematic.allocateMemory();

    
    // 运动学逆解，首先设置末端的位置与321欧拉角
    ee.setMpe(std::array<double, 6>{0.7, 0.3, 0.2, 0, PI + 0.2 , -PI / 2.0 + 0.1}.data(), "321");
    inverse_kinematic.kinPos();

    // 显示位姿矩阵
    std::cout << "添加末端位置后位姿:" << std::endl;
    std::cout << std::setprecision(16);
    dsp(4, 4, *ee.mpm());

    // 显示关节位置
    std::cout << "各个关节电机位置(rad)为:" << std::endl;
    for (auto &m : r->motionPool())
    {
        m.updMp();
        std::cout << m.mp() << std::endl;
    }

    // 运动学正解 首先设置六个关节位置 rad
    double pin[6]{0,0,0,0,0,0};

    // 显示结果
    for (amc::Size i = 0; i < 6; ++i)
    {
        r->motionPool().at(i).setMp(pin[i]);
    }
    forward_kinematic.kinPos();
    ee.updMpm();
    std::cout << "正解所得末端姿态矩阵为:" << std::endl;
    dsp(4, 4, *ee.mpm());
    double pe[6];
    amc::dynamic::s_pm2pe(*ee.mpm(),pe,"321");
    r->generalMotionPool().at(0).setMpe(pe);
	std::cout << "零位欧拉角为:" << std::endl;
	amc::dynamic::dsp(1, 6, pe);
    /*
    //用Adams验证
	// 添加一个 Adams 仿真器插件, 之后保存到amc的安装目录下，用户此时就可以在 Adams 查看结果了
	auto &adams = r->simulatorPool().add<amc::dynamic::AdamsSimulator>();
	//adams.saveAdams("/home/kk/amc-1.5_taike/build/amc_taike.cmd");


	//仿真整段轨迹 
	// 添加一个轨迹 
	My_plan my_plan;

	// 添加一个仿真结果 
	auto &result = r->simResultPool().add<amc::dynamic::SimResult>("result");
    result.allocateMemory();
	
	// 仿真，结果保存到上述变量中 
	adams.simulate(my_plan, result);

	// 要想查看结果，可以使用restore函数，这里将model复位到第500个周期处
	result.restore(200);
	std::cout << "第200个周期: " << std::endl;
	std::cout << "输入位置为: " << std::endl;
    for (auto &p : r->motionPool())
    {
        p.updMp();
        std::cout << p.mp() << std::endl;
    }

	// 储存成adams文件，注意这里需要把result参数加上，这个模型文件为整个轨迹的仿真 
	adams.saveAdams("/home/kk/amc-1.5_taike/build/amc_taike_simulate.cmd", result);
	*/
}

    
void kinematics_Velocity_solve()
{
    auto r = build_Taike();

    // 添加求解器
    auto &inverse_kinematic_solver = r->solverPool().add<amc::dynamic::InverseKinematicSolver>();
    auto &ee = r->generalMotionPool().at(0);

    // 为求解器分配内存
    inverse_kinematic_solver.allocateMemory();

    // 求速度反解，首先设置末端的线速度和角速度
    double end_effector_point_and_angular_velocity[6]{ 0.5, -0.6, 0.4, 0, 0, 0.3};
    ee.setMva(end_effector_point_and_angular_velocity);

    // 求解
    inverse_kinematic_solver.kinVel();
    
    
    // 打印结果
    std::cout << "电机输入速度为:" << std::endl;
    for (amc::Size i = 0; i < 6; ++i)
    {
        std::cout << r->motionPool().at(i).mv() << std::endl;
    }
}


void Dynamics_inverse_solve()
{
    auto r = build_Taike();

    // 添加求解器
    auto &inverse_dynamic_solver = r->solverPool().add<amc::dynamic::InverseDynamicSolver>();
    auto &ee = r->generalMotionPool().at(0);

    // 为求解器分配内存
    inverse_dynamic_solver.allocateMemory();

    // 设置设置电机的加速度
    double motion_acceleration[6]{9, 8, 7, 6, 5, 4};
    for (amc::Size i = 0; i < 6; ++i)
    {
        r->motionPool().at(i).setMa(motion_acceleration[i]);
    }

    // 求动力学反解
    inverse_dynamic_solver.dynAccAndFce();

    // 打印结果
    std::cout << "电机输入力为:" << std::endl;
    for (auto &m : r->motionPool())
    {
        //m.frcCoe();
        std::cout << m.mf() << std::endl;
    }

    // 查看末端线加速度与角加速度
    double ee_result[6];
    ee.getMaa(ee_result);
    std::cout << "末端线速度与角加速度为:" << std::endl;
    for (amc::Size i = 0; i < 6; ++i)
    {
        std::cout << ee_result[i] << std::endl;
    }
}

void Dynamics_Trajectory_cal()
{
    auto r = build_Taike();

    // 添加求解器
	auto &forward_kinematic = r->solverPool().add<amc::dynamic::ForwardKinematicSolver>();
    auto &inverse_kinematic = r->solverPool().add<amc::dynamic::InverseKinematicSolver>();
	auto &inverse_dynamic = r->solverPool().add<amc::dynamic::InverseDynamicSolver>();

    // 为求解器分配内存
	forward_kinematic.allocateMemory();
    inverse_kinematic.allocateMemory();
	inverse_dynamic.allocateMemory();

    // 定时器 //
    auto begin_time = std::chrono::high_resolution_clock::now();

    // 动力学计算轨迹 //
    double actuation_force[51][6];
    for (int i = 0; i <= 50; ++i)
    {
        double t = i*0.1;
        
        double q[6]{ 0.1 + 0.1*sin(t),0.2 + 0.2*sin(t * 2),0.3 + 0.3*sin(t * 3),0.4 + 0.4*sin(t * 4),0.5 + 0.5*sin(t * 5),0.6 + 0.6*sin(t * 6) };
        double dq[6]{ 0.1*cos(t), 0.2 * 2 * cos(t * 2), 0.3 * 3 * cos(t * 3), 0.4 * 4 * cos(t * 4), 0.5 * 5 * cos(t * 5), 0.6 * 6 * cos(t * 6) };
        double ddq[6]{ -0.1*sin(t), -0.2 * 2 * 2 * sin(t * 2), -0.3 * 3 * 3 * sin(t * 3), -0.4 * 4 * 4 * sin(t * 4), -0.5 * 5 * 5 * sin(t * 5), -0.6 * 6 * 6 * sin(t * 6) };

        // 分别设置电机的位置、速度、加速度
        for (amc::Size i = 0; i < 6; ++i)
        {
            r->motionPool().at(i).setMp(q[i]);
            r->motionPool().at(i).setMv(dq[i]);
            r->motionPool().at(i).setMa(ddq[i]);
        }

        // 用正解求解器求解, 0号求解器是运动学正解通用求解器,1号为逆解通用求解器,2号为逆动力学求解器
        r->solverPool().at(0).kinPos();
        r->solverPool().at(0).kinVel();
        r->solverPool().at(2).dynAccAndFce();

        for (amc::Size j = 0; j < 6; ++j)
        {
            actuation_force[i][j] = r->motionPool().at(j).mf();
        }
    }

    auto time_elapsed = std::chrono::high_resolution_clock::now() - begin_time;

    // 打印结果
    std::cout << "各个关节的正向动力学为:" << std::endl;
    for (int i = 0; i <= 50; ++i)
    {
        for (amc::Size j = 0; j < 6; ++j)
        {
            std::cout << std::setfill(' ') << std::setw(15) << actuation_force[i][j] << std::setw(15);
        }
        std::cout << std::endl;
    }
    // 计算循环次数，时间，单次循环时间
	std::cout << "time elapsed " << time_elapsed.count() <<"(ns) for 51 cycles, each cycle consumes "<< time_elapsed.count()/51.0<<"(ns)" << std::endl;

    // 以下展示使用反解求解器求解的过程
    // 根据位置欧拉角来计算反解
    double ee_pe[6]{ 0.6,0.4,0.2,0.1,0.2,0.3 };
    r->generalMotionPool().at(0).setMpe(ee_pe, "321");


    r->solverPool().at(1).kinPos();

    std::cout << "逆解位置为:" << std::endl;
    for (amc::Size j = 0; j < 6; ++j)
    {
        std::cout << std::setfill(' ') << std::setw(15) << r->motionPool().at(j).mp() << std::setw(15);
    }
    std::cout << std::endl;

}


int main()
{
	//model
	//plan(moveL a b)
    //write pdo
   

    //运动学位置求解
    kinematics_Position_solve();

    //运动学速度反解
    //kinematics_Velocity_solve();

    //动力学反解
    //Dynamics_inverse_solve();

    //动力学轨迹计算
    //Dynamics_Trajectory_cal();

    
    return 0;
}
