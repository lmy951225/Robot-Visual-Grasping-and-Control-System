#include <algorithm>

#include"function.hpp"
#include "../control/control.hpp"
#include "../dynamic/dynamic.hpp"
#include <cmath>
uint16_t str1;
uint16_t str2;
namespace amc::plan
{
	auto moveAbsolute(double i, double begin_pos, double end_pos, double vel, double acc, double dec, double &current_pos, double &current_vel, double &current_acc, Size& total_count)->void
	{
		auto v = std::abs(vel);
		auto a = std::abs(acc);
		auto d = std::abs(dec);
		auto s = std::abs(end_pos - begin_pos);

		Size n1 = static_cast<Size>(std::ceil(v / a));
		Size n3 = static_cast<Size>(std::ceil(v / d));

		a = v / n1;
		d = v / n3;

		double s1 = a * n1 * n1 / 2.0;
		double s3 = d * n3 * n3 / 2.0;
		double s2 = s - s1 - s3;
		
		// 判断是否有匀速段
		if (s2 > 0)
		{
			Size n2 = static_cast<Size>(std::ceil(s2 / v));
			double coe = s / (s1 + v * n2 + s3);

			a *= coe;
			d *= coe;
			v *= coe;
			s1 *= coe;
			s2 *= coe;
			s3 *= coe;

			total_count = n1 + n2 + n3;
			if (i < n1)
			{
				current_pos = a * i * i / 2.0;
				current_vel = a * i;
				current_acc = a;
			}
			else if (i < n1 + n2)
			{
				current_pos = s1 + v * (i - n1);
				current_vel = v;
				current_acc = 0.0;
			}
			else if (i < n1 + n2 + n3)
			{
				current_pos = s - d * (n1 + n2 + n3 - i) * (n1 + n2 + n3 - i) / 2.0;
				current_vel = d * (n1 + n2 + n3 - i);
				current_acc = -d;
			}
			else
			{
				current_pos = s;
				current_vel = 0;
				current_acc = 0;
			}
		}
		else
		{
			v = std::sqrt(2 * s * a * d / (a + d));
			n1 = static_cast<Size>(std::ceil(v / a));
			n3 = static_cast<Size>(std::ceil(v / d));

			double coe = s / (a*n1*n1 / 2.0 + d * n3*n3 / 2.0);
			a *= coe;
			d *= coe;

			total_count = n1 + n3;
			if (i < n1)
			{
				current_pos = a * i * i / 2.0;
				current_vel = a * i;
				current_acc = a;
			}
			else if (i < n1 + n3)
			{
				current_pos = s - d * (n1 + n3 - i) * (n1 + n3 - i) / 2.0;
				current_vel = d * (n1 + n3 - i);
				current_acc = -d;
			}
			else
			{
				current_pos = s;
				current_vel = 0;
				current_acc = 0;
			}

		}

		// 修正位置、速度、加速度方向
		if (end_pos < begin_pos)
		{
			current_pos = begin_pos - current_pos;
			current_vel = -current_vel;
			current_acc = -current_acc;
		}
		else
		{
			current_pos = begin_pos + current_pos;
			current_vel = current_vel;
			current_acc = current_acc;
		}
	}
	
	auto SpaceLineJT(double pm1[16], double pm2[16], double q[5][6])->void
	{	
		
		for (int i = 0; i <= 4; i++)
		{
			double T_temp[16]; double* T_Temp = &T_temp[0];
			double q_temp[6];
			double s = i / 4;
			
			// 姿态部分
			double quat1[4], quat2[4], quat_temp[4];
			double* Quat1 = &quat1[0], * Quat2 = &quat2[0], * Quat_temp = &quat_temp[0];
			amc::dynamic::s_pm2rq(pm1,Quat1);
			amc::dynamic::s_pm2rq(pm2,Quat2);
			quat_temp[0] = quat1[0] + s * (quat2[0] - quat1[0]);
			quat_temp[1] = quat1[1] + s * (quat2[1] - quat1[1]);
			quat_temp[2] = quat1[2] + s * (quat2[2] - quat1[2]);
			quat_temp[3] = quat1[3] + s * (quat2[3] - quat1[3]);
			amc::dynamic::s_rq2rm(quat_temp,T_Temp,4);

			// 位置部分
			double p1[3], p2[3], p_temp[3];
			p1[0] = pm1[3]; p1[1] = pm1[7]; p1[0] = pm1[11];
			p2[0] = pm2[3]; p2[1] = pm2[7]; p2[3] = pm2[11];
			p_temp[0] = p1[0] + s * (p2[0] - p1[0]);
			p_temp[1] = p1[1] + s * (p2[1] - p1[1]);
			p_temp[2] = p1[2] + s * (p2[2] - p1[2]);
			T_temp[3] = p_temp[0]; T_temp[7] = p_temp[1]; T_temp[11] = p_temp[2];
			T_temp[12] = 0; T_temp[13] = 0; T_temp[14] = 0; T_temp[15] = 1;

			// 逆解得到关节角度
			double q_sols[48];
			int num_sols;
			num_sols = amc::dynamic::inverse(T_Temp,q_sols,q_temp);
			for (int j = 0; j < 6; j++)
			{
				q[i][j] = q_temp[j] * 180 / PI; // 转化为角度
			}
			
		}

		std::cout << "逆解得到的五组关节角度为：" << std::endl;
		for (int k = 0; k < 5; k++)
		{
			for (int m = 0; m < 6; m++)
			{
				if (q[k][m] > 180)
				{
					q[k][m] = q[k][m] - 360; // 让角度在[-180,180]
				}
				else if (q[k][m] < -180)
				{
					q[k][m] = q[k][m] + 360;
				}
				
				std::cout << q[k][m] << " ";
			}
			std::cout << std::endl;
		}
		
	}

	// 求两1*3向量内积函数
	void cross(double x[3], double y[3], double z[3])
	{
		z[0] = x[1] * y[2] - x[2] * y[1];
		z[1] = x[2] * y[0] - x[0] * y[2];
		z[2] = x[0] * y[1] - x[1] * y[0];
	}

	// 求两1*3向量点积函数
	void dot(double x[3], double y[3],double z)
	{
		z = x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
	}

	// 求三阶矩阵的逆矩阵
	void s_inv(double A[9],double Result[9])
	{
		double a1 = A[0]; double b1 = A[1]; double c1 = A[2];
		double a2 = A[3]; double b2 = A[4]; double c2 = A[5];
		double a3 = A[6]; double b3 = A[7]; double c3 = A[8];
		double k = 1 / (a1*(b2*c3 - c2 * b3) - a2 * (b1*c3 - c1 * b3) + a3 * (b1*c2 - b2 * c1));
		Result[0] = k * (b2*c3 - c2 * b3);
		Result[1] = k * (c1*b3 - b1 * c3);
		Result[2] = k * (b1*c2 - c1 * b2);

		Result[3] = k * (c2*a3 - a2 * c3);
		Result[4] = k * (a1*c3 - c1 * a3);
		Result[5] = k * (a2*c1 - a1 * c2);

		Result[6] = k * (a2*b3 - b2 * a3);
		Result[7] = k * (b1*a3 - a1 * b3);
		Result[8] = k * (a1*b2 - a2 * b1);
	}

	// 求向量旋转过程的旋转矩阵
	void rotation_matrix(double axis[3],double theta,double r_matrix[16])
	{
		double ax = sqrt(pow(axis[0],2) + pow(axis[1],2) + pow(axis[2],2));
		double axis_1[3] = {axis[0] / ax,axis[1] / ax,axis[2] / ax};
		double a = cos(theta / 2.0);
		double bcd[3] = {-axis_1[0] * sin(theta / 2.0),-axis_1[1] * sin(theta / 2.0),-axis_1[2] * sin(theta / 2.0)};
		double b = bcd[0]; double c = bcd[1]; double d = bcd[2];
		double aa = a * a; double bb = b * b; double cc = c * c; double dd = d * d;
		double bc = b * c; double ad = a * d; double ac = a * c; double ab = a * b; double bd = b * d; double cd = c * d;
		r_matrix[0] = aa+bb-cc-dd; r_matrix[1] = 2*(bc+ad); r_matrix[2] = 2*(bd-ac); r_matrix[3] = 0;
		r_matrix[4] = 2*(bc-ad); r_matrix[5] = aa+cc-bb-dd; r_matrix[6] = 2*(cd+ab); r_matrix[7] = 0;
		r_matrix[8] = 2*(bd+ac); r_matrix[9] = 2*(cd-ab); r_matrix[10] = aa+dd-bb-cc; r_matrix[11] = 0;
		r_matrix[12] = 0; r_matrix[13] = 0; r_matrix[14] = 0; r_matrix[15] = 1;
	}

	auto SpaceCircleJT(double pm1[16], double pm2[16], double pm3[16], double q[6][6])->void
	{
		// 求空间圆弧半径
		double po[3] = {pm1[3],pm1[7],pm1[11]};
		double pm[3] = {pm2[3],pm2[7],pm2[11]};
		double pf[3] = {pm3[3],pm3[7],pm3[11]};
		double a = sqrt(pow((po[0] - pm[0]),2) + pow((po[1] - pm[1]),2) + pow((po[2] - pm[2]),2));
		double b = sqrt(pow((pm[0] - pf[0]),2) + pow((pm[1] - pf[1]),2) + pow((pm[2] - pf[2]),2));
		double c = sqrt(pow((pf[0] - po[0]),2) + pow((pf[1] - po[1]),2) + pow((pf[2] - po[2]),2));
		double l = (a + b + c) / 2.0;
		double r = (a * b * c) / (4 * sqrt(l * (l - a) * (l - b) * (l - c)));

		// 求空间圆弧圆心
		double o[3]{ 0 };
		double T_o[9] = {po[0],po[1],po[2],pm[0],pm[1],pm[2],pf[0],pf[1],pf[2]};
		double T1[3]{ 0 };
		double T_m[9]{ 0 };
		double T2[3] = {1,1,1};
		s_inv(T_o,T_m);
		double T_n[16] = {T_m[0],T_m[1],T_m[2],0,T_m[3],T_m[4],T_m[5],0,T_m[6],T_m[7],T_m[8],0,0,0,0,1};
		amc::dynamic::s_pm_dot_v3(T_n,T2,T1);

		double T3[9] = {2*(pm[0] - po[0]),2*(pm[1] - po[1]),2*(pm[2] - po[2]),
						2*(pf[0] - po[0]),2*(pf[1] - po[1]),2*(pf[2] - po[2]),
						T1[0],T1[1],T1[2]};
		double T4[9]{ 0 };
		double T5[3] = {-pow(po[0],2)-pow(po[1],2)-pow(po[2],2)+pow(pm[0],2)+pow(pm[1],2)+pow(pm[2],2),
						-pow(po[0],2)-pow(po[1],2)-pow(po[2],2)+pow(pf[0],2)+pow(pf[1],2)+pow(pf[2],2),1};
		s_inv(T3,T4);
		double T6[16] = {T4[0],T4[1],T4[2],0,T4[3],T4[4],T4[5],0,T4[6],T4[7],T4[8],0,0,0,0,1};
		amc::dynamic::s_pm_dot_v3(T6,T5,o);

		// 求末端在圆弧上运动的旋转轴，过圆心且垂直圆所在平面
		double v_s_b[3] = {po[0] - o[0],po[1] - o[1],po[2] - o[2]};
		double oo = sqrt(pow(v_s_b[0],2) + pow(v_s_b[1],2) + pow(v_s_b[1],2));
		double of = sqrt(pow((pf[0] - o[00]),2) + pow((pf[1] - o[1]),2) + pow((pf[2] - o[2]),2));
		double v_s[3] = {v_s_b[0] / oo,v_s_b[1] / oo,v_s_b[2] / oo};
		double v_f[3] = {(pf[0] - o[0]) / of,(pf[1] - o[1]) / of,(pf[2] - o[2]) / of};
		double rotation_aixs[3]{ 0 };
		cross(v_s,v_f,rotation_aixs);

		// 求圆弧角度和每个轨迹点的角度
		double v_sf;
		dot(v_s,v_f,v_sf);
		double theta = acos(v_sf);
		double theta_per = theta / 5;

		// 插补生成轨迹点，并求其逆解的关节空间角度
		double theta_current{ 0 };
		double p_current[3]{ 0 };
		for (int i = 0; i <= 4; i++)
		{	
			// 求轨迹点笛卡尔坐标
			double matrix_current[16]{ 0 };
			double vector_current[3]{ 0 };
			rotation_matrix(rotation_aixs,theta_current,matrix_current);
			amc::dynamic::s_pm_dot_v3(matrix_current,v_s_b,vector_current);
			p_current[0] = vector_current[0] + o[0];
			p_current[1] = vector_current[1] + o[1];
			p_current[2] = vector_current[2] + o[2];
		
			// 更新轨迹点位姿矩阵
			matrix_current[3] = p_current[0];
			matrix_current[7] = p_current[1];
			matrix_current[11] = p_current[2];

			// 逆解得到关节角度
			double q_sols[48];
			double q_temp[6];
			int num_sols;
			num_sols = amc::dynamic::inverse(matrix_current,q_sols,q_temp);
			for (int j = 0; j < 6; j++)
			{
				q[i][j] = q_temp[j] * 180 / PI; // 转化为角度
			}

			theta_current = (i+1) * theta_per;// 移动向量
		}

		std::cout << "逆解得到的五组关节角度为：" << std::endl;
		for (int k = 0; k < 5; k++)
		{
			for (int m = 0; m < 6; m++)
			{
				if (q[k][m] > 180)
				{
					q[k][m] = q[k][m] - 360; // 让角度在[-180,180]
				}
				else if (q[k][m] < -180)
				{
					q[k][m] = q[k][m] + 360;
				}
				
				std::cout << q[k][m] << " ";
			}
			std::cout << std::endl;
		}

	}

	auto sign(double x)->double
	{
		if(x < 0)
		{
			return -1.0;
		}
		else if(x > 0)
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
		
	}

	auto Abs(double x)->double
	{
		if(x < 0)
		{
			double y;
			y = (-1.0) * x;
			return y;
		}
		else 
		{
			return x;
		}
		
	}

	auto read_state(uint8_t buff[],serialPort myserial)->void
	{
		int nread,nwrite;
		//cout<<"serialPort state_Test"<<endl;
		buff[0] = 0xEB;
		buff[1] = 0x90;
		buff[2] = 0x01;
		buff[3] = 0x01;
		buff[4] = 0x41;
		buff[5] = 0x43;
		//读取当前运行状态
		nwrite = myserial.writeBuffer(buff, 6);
		nread = myserial.readBuffer(buff, 13);
		for (int i = 0; i < 13; i++)
		{
			//printf("%#x",buff[i]);
			//printf(" ");
			switch (i)
			{
				case 5:
					if (buff[5] == 0x01)
					{
						printf("当前工作状态: 夹爪张开到最大且空闲\n");
					}
					else if (buff[5] == 0x02)
					{
						printf("当前工作状态: 夹爪闭合到最小且空闲\n");
					}
					else if (buff[5] == 0x03)
					{
						printf("当前工作状态: 夹爪停止且空闲\n");
					}
					else if (buff[5] == 0x04)
					{
						printf("当前工作状态: 夹爪正在闭合\n");
					}
					else if (buff[5] == 0x05)
					{
						printf("当前工作状态: 夹爪正在张开\n");
					}
					else if (buff[5] == 0x06)
					{
						printf("当前工作状态: 夹爪在闭合过程中遇到力控停止\n");
					}
					break;
				case 7:
					printf("当前工作温度为: %d度\n",buff[i]);
					break;
				case 9:
					str1 = buff[9];
					str1 = (str1 << 8);
					str1 |= buff[8];
					printf("当前夹爪开口度: %d\n",str1);
					break;
				case 11:
					str2 = buff[11];
					str2 = (str2 << 8);
					str2 |= buff[10];
					printf("当前夹爪夹持力: %d",str2);
					break;
				default:
					break;
			}
			
		}
		printf("\n"); 
	}

	auto close(uint8_t buff[],serialPort myserial)->void
	{
		int nread,nwrite;
		cout<<"serialPort close_Test"<<endl;
		buff[0] = 0xEB;
		buff[1] = 0x90;
		buff[2] = 0x01;
		buff[3] = 0x05;
		buff[4] = 0x10;
		buff[5] = 0xF4;
		buff[6] = 0x01;
		buff[7] = 0xF4;
		buff[8] = 0x01;
		buff[9] = 0x00;

		//抓取,速度500,力矩阈值500
		// 定时器 //
		auto begin_time = std::chrono::high_resolution_clock::now();
		nwrite = myserial.writeBuffer(buff, 10);
		auto time_elapsed1 = std::chrono::high_resolution_clock::now() - begin_time;
		nread = myserial.readBuffer(buff, 10);
		auto time_elapsed2 = std::chrono::high_resolution_clock::now() - begin_time;
		for (int i = 0; i < 7; i++)
		{
			printf("%#x",buff[i]);
			printf(" ");
		}
		printf("\n"); 
		read_state(buff,myserial);
		cout << "write time:" << time_elapsed1.count() / 1000.0 <<"(us)"<< endl;
		cout << "read and write time:" << time_elapsed2.count() / 1000.0 <<"(us)"<< endl;
		printf("\n"); 
	}

	auto open(uint8_t buff[],serialPort myserial)->void
	{
		int nread,nwrite;
		cout<<"serialPort open_Test"<<endl;
		buff[0] = 0xEB;
		buff[1] = 0x90;
		buff[2] = 0x01;
		buff[3] = 0x03;
		buff[4] = 0x11;
		buff[5] = 0xF4;
		buff[6] = 0x01;
		buff[7] = 0x0A;

		//松开
		// 定时器 //
		auto begin_time = std::chrono::high_resolution_clock::now();
		nwrite = myserial.writeBuffer(buff, 8);
		auto time_elapsed1 = std::chrono::high_resolution_clock::now() - begin_time;
		nread = myserial.readBuffer(buff, 8);
		auto time_elapsed2 = std::chrono::high_resolution_clock::now() - begin_time;
		for (int i = 0; i < 7; i++)
		{
			printf("%#x",buff[i]);
			printf(" ");
		}
		printf("\n"); 
		read_state(buff,myserial);
		cout << "write time:" << time_elapsed1.count() / 1000.0 <<"(us)"<< endl;
		cout << "read and write time:" << time_elapsed2.count() / 1000.0 <<"(us)"<< endl;
		printf("\n"); 
	}

/*
	auto SpaceLineRT(double i,double begin_point[6], double end_point[6], double vel, double acc, double dec, double current_point[6], double &current_vel, double &current_acc, Size& total_count)->void
	{
		auto v = std::abs(vel);
		auto a = std::abs(acc);
		auto d = std::abs(dec);
		auto s = sqrt(pow(end_point[0]-begin_point[0],2) + pow(end_point[1]-begin_point[1],2) + pow(end_point[2]-begin_point[2],2));

		Size n1 = static_cast<Size>(std::ceil(v / a));
		Size n3 = static_cast<Size>(std::ceil(v / d));

		a = v / n1;
		d = v / n3;

		double s1 = a * n1 * n1 / 2.0;
		double s3 = d * n3 * n3 / 2.0;
		double s2 = s - s1 - s3;
		double lamda;

		// 判断是否有匀速段
		if (s2 > 0)
		{
			Size n2 = static_cast<Size>(std::ceil(s2 / v));
			double coe = s / (s1 + v * n2 + s3);

			a *= coe;
			d *= coe;
			v *= coe;
			s1 *= coe;
			s2 *= coe;
			s3 *= coe;

			total_count = n1 + n2 + n3;
			if (i < n1)
			{
				lamda = a * i * i / 2.0;
				current_vel = a * i;
				current_acc = a;
			}
			else if (i < n1 + n2)
			{
				lamda = s1 + v * (i - n1);
				current_vel = v;
				current_acc = 0.0;
			}
			else if (i < n1 + n2 + n3)
			{
				lamda = s - d * (n1 + n2 + n3 - i) * (n1 + n2 + n3 - i) / 2.0;
				current_vel = d * (n1 + n2 + n3 - i);
				current_acc = -d;
			}
			else
			{
				lamda = s;
				current_vel = 0;
				current_acc = 0;
			}
		}
		else
		{
			v = std::sqrt(2 * s * a * d / (a + d));
			n1 = static_cast<Size>(std::ceil(v / a));
			n3 = static_cast<Size>(std::ceil(v / d));

			double coe = s / (a*n1*n1 / 2.0 + d * n3*n3 / 2.0);
			a *= coe;
			d *= coe;

			total_count = n1 + n3;
			if (i < n1)
			{
				lamda = a * i * i / 2.0;
				current_vel = a * i;
				current_acc = a;
			}
			else if (i < n1 + n3)
			{
				lamda = s - d * (n1 + n3 - i) * (n1 + n3 - i) / 2.0;
				current_vel = d * (n1 + n3 - i);
				current_acc = -d;
			}
			else
			{
				lamda = s;
				current_vel = 0;
				current_acc = 0;
			}

		}

		// 求插补出来的末端点的欧拉角坐标
		// cur = begin + delta*lamda
		for (int j = 0; j < 6; j++)
		{
			current_point[j] = begin_point[j] + (end_point[j] - begin_point[j]) * lamda;
		}
		

		// 修正速度、加速度方向
		if (end_point < begin_point)
		{
			
			current_vel = -current_vel;
			current_acc = -current_acc;
		}
		else
		{
			
			current_vel = current_vel;
			current_acc = current_acc;
		}
		
	}
*/
/*
	auto Fivetraj(double i, double t0, double t1, double begin_pos, double end_pos, double begin_vel, double end_vel, double begin_acc, double end_acc, double &current_pos, double &current_vel, double &current_acc, Size& total_count)->void
	{
		
		double T = t1 - t0;
		double T2 = T * T;
		double h = end_pos - begin_pos;
		double a0 = begin_pos;
		double a1 = begin_vel;
		double a2 = begin_acc / 2.0;
		double a3 = (20.0 * h - (8.0 * end_vel) * T - (3.0 * begin_acc - end_acc) * T2) / (2.0 * T * T2);
		double a4 = (-30.0 * h + (14.0 * end_vel + 16.0 * begin_vel) * T + (3.0 * begin_acc - 2.0 * end_acc) * T2) / (2.0 * T2 * T2);
		double a5 = (12.0 * h - 6 * (end_vel + begin_vel) * T + (end_acc - begin_acc) * T2) / (2.0 * T2 * T2 * T);
		
		
		
		current_pos = a0 + a1*(i - t0) + a2*(i - t0)*(i - t0) + a3*(i - t0)*(i - t0)*(i - t0) + a4*(i - t0)*(i - t0)*(i - t0)*(i - t0) + a5*(i - t0)*(i - t0)*(i - t0)*(i - t0)*(i - t0);
		current_vel= a1 + 2*a2*(i - t0) + 3*a3*(i - t0)*(i - t0) + 4*a4*(i - t0)*(i - t0)*(i - t0) + 5*a5*(i - t0)*(i - t0)*(i - t0)*(i - t0);
		current_acc = 2*a2 + 6*a3*(i - t0) + 12*a4*(i - t0)*(i - t0) + 20*a5*(i - t0)*(i - t0)*(i - t0);
		
		total_count = t1 - t0;
		
	}	
*/
	auto moveAbsolute2(double pa, double va, double aa, double pt, double vt, double at, double vm, double am, double dm, double dt, double zero_check, double &pc, double &vc, double &ac, Size& total_count)->int
	{
		vt = 0.0;
		at = 0.0;

		// 当前速度超过速度上限 //
		if (std::abs(va) > vm + dm * dt)
		{
			ac = -amc::dynamic::s_sgn2(va) * dm;
			goto return_flag;
		}
		
		// 查看当前速度是否过快 //
		{
			// 计算完全减速所产生的位移，vdec和sdec有方向 //
			const double vdec = va;
			const double ndec = std::abs(std::ceil(std::abs(vdec) / dm / dt - zero_check));
			const double tdec = ndec * dt;
			const double sdec = vdec * tdec / 2;

			// 当前速度需要完全减速到零，之后可能还需要反转 //
			if ((va > zero_check && (pt - pa) < sdec + zero_check) || (va < -zero_check && (pt - pa) > sdec - zero_check))
			{
				ac = -va / ndec / dt;
				goto return_flag;
			}
		}
		
		// 二分法算当前可以最快的加速度，并沿着该加速度加速 //
		{
			double lower_bound = pt - pa < 0.0 ? std::min(dm, -va / dt) : std::max(-dm, -va / dt);
			double upper_bound = pt - pa < 0.0 ? std::max(-am, (-vm - va) / dt) : std::min(am, (vm - va) / dt);

			while (std::abs(lower_bound - upper_bound) > zero_check)
			{
				const double a1 = (lower_bound + upper_bound) / 2.0;
				const double v1 = va + a1 * dt;
				const double p1 = pa + 0.5*(va + v1)*dt;
				const double ndec1 = std::abs(std::ceil(std::abs(v1) / dm / dt));
				const double tdec1 = ndec1 * dt;
				const double sdec1 = v1 * tdec1 / 2.0;

				if ((pt - pa >= 0 && (pt - p1) >= sdec1) || (pt - pa < 0 && (pt - p1) <= sdec1))
				{
					lower_bound = a1;
				}
				else
				{
					upper_bound = a1;
				}
			}
			ac = lower_bound;
		}

	return_flag:
		vc = va + ac * dt;
		pc = pa + 0.5 * (va + vc) * dt;
		total_count = 1;
		return std::abs(pt - pc)<zero_check && std::abs(vt - vc)<zero_check ? 0 : 1;
	}

}
