/*
 /home/kk/amc-1.5_taike/test/xml_test.cpp
 本例子展示Taike机器人基于str和xml解析Ethercat pdo映射的过程，xml文件位于：/home/kk/amc-1.5_taike/Taike_Master_Slave.xml
 by LMY
 date:2022.10.20
*/

#include <iostream>
#include <./core/core.hpp>
#include <./control/control.hpp>
#include <./dynamic/dynamic.hpp>
#include <./plan/plan.hpp>
#include <./sensor/sensor.hpp>
#include <./server/server.hpp>
#include <./robot/ur.hpp>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace amc::control;

namespace amc::control
{
	void read_bit(char *data, int bit_size, const char *pd, int offset, int bit_position);
	void write_bit(const char *data, int bit_size, char *pd, int offset, int bit_position);


	void read_bit2(char *data, int bit_size, const char *pd, int offset, int bit_position);
	void write_bit2(const char *data, int bit_size, char *pd, int offset, int bit_position);
}

void test_pdo()
{
	try
	{
		std::cout << "test pdo" << std::endl;

		amc::control::EthercatMaster mst;
		std::string xml_str =
			"<EthercatSlave phy_id=\"0\" product_code=\"0x00001030\" vendor_id=\"0x000000ab\" revision_num=\"0x03\" dc_assign_activate=\"0x0300\">\
				<SyncManagerPoolObject>\
					<SyncManager  is_tx=\"false\"/>\
					<SyncManager  is_tx=\"true\"/>\
					<SyncManager  is_tx=\"false\">\
						<Pdo index=\"0x1700\" is_tx=\"false\">\
							<PdoEntry name=\"control_word\" index=\"0x6040\" subindex=\"0x00\" size=\"16\"/>\
							<PdoEntry name=\"mode_of_operation\" index=\"0x6060\" subindex=\"0x00\" size=\"8\"/>\
							<PdoEntry name=\"target_pos\" index=\"0x607A\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"offset_vel\" index=\"0x60B1\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"offset_tor\" index=\"0x60B2\" subindex=\"0x00\" size=\"16\"/>\
						</Pdo>\
						<Pdo type=\"Pdo\" index=\"0x1701\" is_tx=\"false\">\
							<PdoEntry name=\"control_word\" index=\"0x6040\" subindex=\"0x00\" size=\"16\"/>\
							<PdoEntry name=\"mode_of_operation\" index=\"0x6060\" subindex=\"0x00\" size=\"9\"/>\
							<PdoEntry name=\"target_vel\" index=\"0x60FF\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"offset_tor\" index=\"0x60B2\" subindex=\"0x00\" size=\"16\"/>\
						</Pdo>\
						<Pdo type=\"Pdo\" index=\"0x1702\" is_tx=\"false\">\
							<PdoEntry name=\"control_word\" index=\"0x6040\" subindex=\"0x00\" size=\"16\"/>\
							<PdoEntry name=\"mode_of_operation\" index=\"0x6060\" subindex=\"0x00\" size=\"10\"/>\
							<PdoEntry name=\"target_vel\" index=\"0x60FF\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"offset_tor\" index=\"0x60B2\" subindex=\"0x00\" size=\"16\"/>\
						</Pdo>\
					</SyncManager>\
					<SyncManager is_tx=\"true\">\
						<Pdo type=\"Pdo\" index=\"0x1B00\" is_tx=\"true\">\
							<PdoEntry name=\"status_word\" index=\"0x6041\" subindex=\"0x00\" size=\"16\"/>\
							<PdoEntry name=\"pos_actual_value\" index=\"0x6064\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"pos_loop_error\" index=\"0x60F4\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"vel_actual_value\" index=\"0x606F\" subindex=\"0x00\" size=\"32\"/>\
							<PdoEntry name=\"tor_actual_value\" index=\"0x6077\" subindex=\"0x00\" size=\"16\"/>\
						</Pdo>\
					</SyncManager>\
				</SyncManagerPoolObject>\
			 </EthercatSlave>";


		mst.slavePool().add<amc::control::EthercatSlave>().loadXmlStr(xml_str);


		mst.setControlStrategy([&]()
		{
			static int count{ 0 };
			if (++count % 1000 == 0)
			{
				mst.mout() << "count:" << std::dec << count << std::endl;
				for (auto &sla : mst.ecSlavePool())
				{
					for (auto &sm : sla.smPool())
					{
						if (sm.tx())
						{
							for (auto &pdo : sm)
							{
								for (auto &entry : pdo)
								{
									if (entry.index())
									{
										std::uint32_t data;
										sla.readPdo(entry.index(), entry.subindex(), &data, entry.bitSize());
										mst.mout() << "  index:0x" << std::setfill('0') << std::setw(sizeof(std::int16_t) * 2) << std::hex << static_cast<std::uint32_t>(entry.index())
											<< "  subindex:0x" << std::setfill('0') << std::setw(sizeof(std::int8_t) * 2) << std::hex << static_cast<std::uint32_t>(entry.subindex())
											<< "  value:0x" << std::setfill('0') << std::setw(sizeof(std::uint32_t) * 2) << std::hex << data << std::endl;
									}
								}
							}
						}

					}
				}
			}
		}
		);
		mst.start();

		std::this_thread::sleep_for(std::chrono::seconds(10));

		mst.stop();
	std::cout << "test pdo finished" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

void test_pdo_xml()
{
	try
	{
		std::cout << "test pdo xml" << std::endl;
		
		amc::control::EthercatMaster m;

		auto &s1 = m.slavePool().add<EthercatSlave>();
		// s1.loadXmlStr(
		// 	"<EthercatSlave phy_id=\"0\" product_code=\"0x00001030\" vendor_id=\"0x000000ab\" revision_num=\"0x03\" dc_assign_activate=\"0x0300\">"
		// 	"	<SyncManagerPoolObject>"
		// 	"		<SyncManager  is_tx=\"false\"/>"
		// 	"		<SyncManager  is_tx=\"true\"/>"
		// 	"		<SyncManager  is_tx=\"true\">"
		// 	"			<Pdo index=\"0x1B00\" is_tx=\"true\">"
		// 	"				<PdoEntry name=\"pos_actual_value\" index=\"0x6064\" subindex=\"0x00\" size=\"32\"/>"
		// 	"			</Pdo>"
		// 	"		</SyncManager>"
		// 	"	</SyncManagerPoolObject>"
		// 	"</EthercatSlave>");
		s1.loadXmlFile(std::string("/home/kk/amc-1.5_taike/Taike_Master_Slave.xml"));
		m.setControlStrategy([&]()
		{
			static int count{ 0 };

			std::int32_t value{ 0 };
			s1.readPdo(0x6064, 0x00, value);

			if (++count % 1000 == 0)
			{
				m.mout() << "count " << count << " : pos " << value << std::endl;
			}
			
		});
		m.start();
		std::this_thread::sleep_for(std::chrono::seconds(10));
		m.stop();
		std::cout << "test pdo xml finished" << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

int main()
{
	//test_pdo();
	test_pdo_xml();
	return 0;
}
