﻿#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdint.h>
#include <new>
#include <future>


#ifdef UNIX
#include <pthread.h>
#include <semaphore.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include <map>
#include <sstream>

#include <errno.h>

#include "socket.hpp"
#include "log.hpp"
#include "sha1.h"

namespace amc::core
{
	auto close_sock(decltype(socket(AF_INET, SOCK_STREAM, 0)) s)->int
	{

#ifdef UNIX
		auto ret = close(s);
#endif
		return ret;
	}
	auto safe_recv(decltype(socket(AF_INET, SOCK_STREAM, 0)) s, char *data, int size) -> int
	{
		int result{ 0 };
		for (; result < size; )
		{
			int ret = recv(s, data + result, size - result, 0);
			if (ret <= 0)
			{
				close_sock(s);
				result = ret;
				break;
			}

			result += ret;
		}

		return result;
	};

	// please refer to 
	// https://www.cnblogs.com/chyingp/p/websocket-deep-in.html
	// sha1 hash 生成出来的是纯数字，可以把它改成2进制来保存
	// 
	std::string base64_encode2(unsigned char const* bytes_to_encode, unsigned int in_len)
	{
		static const std::string base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i <4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';

		}

		return ret;

	}
	auto pack_data_server(const char *data, int size)->std::string
	{
		std::string s;
		if (size < 126)
		{
			s.resize(size + 2);
			s[0] = char(0x82);// binary data, 0x81 is text data
			s[1] = char(size);
			std::copy_n(data, size, &s[2]);
		}
		else if (size < 0xFFFF)
		{
			s.resize(size + 4);
			s[0] = char(0x82);
			s[1] = char(126);
			s[2] = size >> 8;
			s[3] = size & 0xFF;
			std::copy_n(data, size, &s[4]);
		}
		else
		{
			s.resize(size + 10);
			s[0] = char(0x82);
			s[1] = char(127);
			s[2] = 0;
			s[3] = 0;
			s[4] = 0;
			s[5] = 0;
			s[6] = size >> 24;
			s[7] = size >> 16;
			s[8] = size >> 8;
			s[9] = size & 0xFF;
			std::copy_n(data, size, &s[10]);
		}

		return s;
	};
	auto pack_data_client(const char *data, int size)->std::string
	{
		std::string s;
		if (size < 126)
		{
			s.resize(size + 6);
			s[0] = char(0x82);// binary data, 0x81 is text data
			s[1] = char(size) | 0x80;
			std::copy_n(data, size, &s[6]);
		}
		else if (size < 0xFFFF)
		{
			s.resize(size + 8);
			s[0] = char(0x82);
			s[1] = char(126) | 0x80;
			s[2] = size >> 8;
			s[3] = size & 0xFF;
			std::copy_n(data, size, &s[8]);
		}
		else
		{
			s.resize(size + 14);
			s[0] = char(0x82);
			s[1] = char(127) | 0x80;
			s[2] = 0;
			s[3] = 0;
			s[4] = 0;
			s[5] = 0;
			s[6] = size >> 24;
			s[7] = size >> 16;
			s[8] = size >> 8;
			s[9] = size & 0xFF;
			std::copy_n(data, size, &s[14]);
		}

		std::fill_n(&s[s.size() - size - 4], 4, 0xf0);
		for (int i = 0; i < size; ++i)
		{
			s[i + s.size() - size] = s[i + s.size() - size] ^ s[s.size() - size - 4 + i % 4];
		}

		return s;
	};
	auto make_header_map(const std::string &hand_shake_text)->std::map<std::string, std::string>
	{
		std::istringstream istream(hand_shake_text);

		// 找到Sec-WebSocket-Key //
		std::map<std::string, std::string> header_map;
		std::string header;
		while (std::getline(istream, header) && header != "\r")
		{
			if (header[header.size() - 1] != '\r')
			{
				continue; //end
			}
			else
			{
				header.erase(header.end() - 1);    //remove last char
			}

			auto end = header.find(": ", 0);
			if (end != std::string::npos)
			{
				std::string key = header.substr(0, end);
				std::string value = header.substr(end + 2);
				header_map[key] = value;
			}
		}

		return header_map;
	}

	struct Socket::Imp
	{
		Socket* socket_;
		Socket::State state_;
		Socket::TYPE type_;
		bool is_server_;

		std::function<int(Socket *, amc::core::Msg &)> onReceivedMsg;
		std::function<int(Socket *, const char *data, int size)> onReceivedData;
		std::function<int(Socket *, const char *, int)> onReceivedConnection;
		std::function<int(Socket *)> onLoseConnection;

		decltype(socket(AF_INET, SOCK_STREAM, 0)) lisn_socket_, recv_socket_;  //也可以用SOCKET类型
		struct sockaddr_in server_addr_, client_addr_;
		socklen_t sin_size_;

		std::string remote_ip_, port_;

		// 线程同步变量 //
		std::recursive_mutex state_mutex_;

		std::thread recv_thread_, accept_thread_;
		std::mutex close_mutex_;

		// 连接的socket //

		~Imp() = default;
		Imp(Socket* sock) :socket_(sock), lisn_socket_(0), recv_socket_(0), sin_size_(sizeof(struct sockaddr_in)), state_(Socket::IDLE)
			, onReceivedMsg(nullptr), onReceivedData(nullptr), onReceivedConnection(nullptr), onLoseConnection(nullptr) {}

		static void receiveThread(Socket::Imp* imp, std::promise<void> receive_thread_ready);
		static void acceptThread(Socket::Imp* imp, std::promise<void> accept_thread_ready);

		auto lose_tcp()->void
		{
			// 需要用close_lck 来确保本段代码不会被stop中断 //
			std::unique_lock<std::mutex> close_lck(close_mutex_, std::defer_lock);
			if (!close_lck.try_lock())
			{
				close_sock(recv_socket_);
			}
			else
			{
				shutdown(recv_socket_, 2);
				close_sock(recv_socket_);
				
				std::unique_lock<std::recursive_mutex> state_lck(state_mutex_);
				state_ = IDLE;
				recv_thread_.detach();
				if (onLoseConnection)onLoseConnection(socket_);
				return;
			}
		}
	};
	auto Socket::Imp::acceptThread(Socket::Imp* imp, std::promise<void> accept_thread_ready)->void
	{
#ifdef UNIX
		signal(SIGPIPE, SIG_IGN);
#endif
		// 改变状态 //
		imp->state_ = WAITING_FOR_CONNECTION;

		// 通知主线程,accept线程已经拷贝完毕,准备监听 //
		accept_thread_ready.set_value();

		// 开始建立连接 //
		for (;;)
		{
			// 服务器阻塞,直到客户程序建立连接 //
			imp->recv_socket_ = accept(imp->lisn_socket_, (struct sockaddr *)(&imp->client_addr_), &imp->sin_size_);
			
			if (imp->recv_socket_ <= 0)
			{
				std::unique_lock<std::mutex> close_lck(imp->close_mutex_, std::defer_lock);
				if (!close_lck.try_lock())
				{
					close_sock(imp->lisn_socket_);
					imp->state_ = IDLE;
					return;
				}

				LOG_ERROR << "socket failed to accept" << std::endl;
				continue;
			}

			if (imp->type_ == WEB || imp->type_ == WEB_RAW)
			{
				std::this_thread::sleep_for(std::chrono::seconds(3));
				
				char recv_data[1024]{ 0 };
				int res = recv(imp->recv_socket_, recv_data, 1024, 0);
				if (res <= 0)
				{
					LOG_ERROR << "websocket shake hand failed : " << res << std::endl;
					shutdown(imp->recv_socket_, 2);
					close_sock(imp->recv_socket_);
					continue;
				}

				auto header_map = make_header_map(recv_data);
				std::string server_key;
				try
				{
					server_key = header_map.at("Sec-WebSocket-Key");
				}
				catch (std::exception &)
				{
					LOG_ERROR << "websocket shake hand failed : invalid key" << std::endl;
					shutdown(imp->recv_socket_, 2);
					close_sock(imp->recv_socket_);
					continue;
				}
				server_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

				// 找到返回的key //
				SHA1 checksum;
				checksum.update(server_key);
				std::string hash = checksum.final();

				std::uint32_t message_digest[5];
				for (int i = 0; i < 20; ++i)
				{
					char num[5] = "0x00";
					std::copy_n(hash.data() + i * 2, 2, num + 2);
					std::uint8_t n = std::stoi(num, 0, 16);
					*(reinterpret_cast<unsigned char*>(message_digest) + i) = n;
				}

				auto ret_hey = base64_encode2(reinterpret_cast<const unsigned char*>(message_digest), 20);

				std::string shake_hand;
				shake_hand = "HTTP/1.1 101 Switching Protocols\r\n"
					"Upgrade: websocket\r\n"
					"Connection: Upgrade\r\n"
					"Sec-WebSocket-Accept: " + ret_hey + std::string("\r\n\r\n");

				auto ret = send(imp->recv_socket_, shake_hand.c_str(), static_cast<int>(shake_hand.size()), 0);
				
				if (ret == -1)
				{
					LOG_ERROR << "websocket shake hand failed : lose connection before hand shake successful" << std::endl;
					shutdown(imp->recv_socket_, 2);
					close_sock(imp->recv_socket_);
					continue;
				};
			}

			break;
		}
		
		shutdown(imp->lisn_socket_, 2);
		close_sock(imp->lisn_socket_);

		// 否则,开始开启数据线程 //
		if (imp->onReceivedConnection)imp->onReceivedConnection(imp->socket_, inet_ntoa(imp->client_addr_.sin_addr), ntohs(imp->client_addr_.sin_port));

		std::promise<void> receive_thread_ready;
		auto fut = receive_thread_ready.get_future();
		imp->recv_thread_ = std::thread(receiveThread, imp, std::move(receive_thread_ready));
		fut.wait();

		return;
	}
	auto Socket::Imp::receiveThread(Socket::Imp* imp, std::promise<void> receive_thread_ready)->void
	{
#ifdef UNIX
		signal(SIGPIPE, SIG_IGN);
#endif
		// 改变状态 //
		imp->state_ = Socket::WORKING;

		// 通知accept或connect线程已经准备好,下一步开始收发数据 //
		receive_thread_ready.set_value();

		amc::core::Msg recv_msg;
		recv_msg.resize(1024);

		// 开启接受数据的循环 //
		for (;;)
		{
			switch (imp->type_)
			{
			case TCP:
			{
				if (safe_recv(imp->recv_socket_, reinterpret_cast<char *>(&recv_msg.header()), sizeof(MsgHeader)) <= 0) { imp->lose_tcp(); return; }
				recv_msg.resize(recv_msg.size());
				if (recv_msg.size() > 0 && safe_recv(imp->recv_socket_, recv_msg.data(), recv_msg.size()) <= 0) { imp->lose_tcp(); return; }
				if (imp->onReceivedMsg)imp->onReceivedMsg(imp->socket_, recv_msg);
				break;
			}
			case WEB:
			{
				std::int64_t real_length{ 0 };
				std::string payload_data;

				for (bool fin{ false }; !fin;)
				{
					// 接受头 //
					char web_head[2];
					if (safe_recv(imp->recv_socket_, web_head, 2) <= 0) { imp->lose_tcp(); return; }

					// 是否最后一帧 //
					fin = (web_head[0] & 0x80) == 0x80; // 1bit，1表示最后一帧    

					// 获取opcode //
					std::int8_t op_code = web_head[0] & 0x0f;
					if (op_code == 0x08) { imp->lose_tcp(); return; }

					// 获取数据长度
					std::int64_t payload_len = web_head[1] & 0x7F; // 数据长度 
					if (payload_len == 126)
					{
						char length_char[2];
						if (safe_recv(imp->recv_socket_, length_char, 2) <= 0) { imp->lose_tcp(); return; }

						union
						{
							std::uint16_t length;
							char reverse_char[2];
						};
						for (int i = 0; i < 2; ++i)reverse_char[i] = length_char[1 - i];
						payload_len = length;
					}
					else if (payload_len == 127)
					{
						char length_char[8];
						if (safe_recv(imp->recv_socket_, length_char, 8) <= 0) { imp->lose_tcp(); return; }

						char reverse_char[8];
						for (int i = 0; i < 8; ++i)reverse_char[i] = length_char[7 - i];
						std::copy_n(reverse_char, 8, reinterpret_cast<char*>(&payload_len));
					}

					//////////////////////////////////保护，数据不能太大///////////////////////////////
					if (payload_len > 0x00080000 || payload_len + payload_data.size() > 0x00100000)
					{
						LOG_ERROR << "websocket receive too large object" << std::endl;
						imp->lose_tcp();
						return;
					}

					// 获取掩码
					bool mask_flag = (web_head[1] & 0x80) == 0x80; // 是否包含掩码    
					char masks[4];
					if (safe_recv(imp->recv_socket_, masks, 4) <= 0) { imp->lose_tcp(); return; }

					// 用掩码读取出数据 //
					auto last_size = payload_data.size();
					payload_data.resize(payload_data.size() + static_cast<std::size_t>(payload_len));
					if (safe_recv(imp->recv_socket_, payload_data.data() + last_size, static_cast<int>(payload_len)) <= 0) { imp->lose_tcp(); return; }

					for (int i{ 0 }; i<payload_len; ++i)
					{
						payload_data[i + last_size] = payload_data[i + last_size] ^ masks[i % 4];
					}
				}

				// 把web sock 的东西转成 msg //
				recv_msg.resize(static_cast<amc::core::MsgSize>(payload_data.size() - sizeof(amc::core::MsgHeader)));
				std::copy(payload_data.data(), payload_data.data() + payload_data.size(), reinterpret_cast<char*>(&recv_msg.header()));

				if (recv_msg.size() != payload_data.size() - sizeof(amc::core::MsgHeader))
				{
					LOG_ERROR << "websocket receive wrong msg size" << std::endl;
					imp->lose_tcp();
					return;
				}

				if (imp->onReceivedMsg)imp->onReceivedMsg(imp->socket_, recv_msg);

				break;
			}
			case WEB_RAW:
			{
				std::int64_t real_length{ 0 };
				std::string payload_data;

				for (bool fin{ false }; !fin;)
				{
					// 接受头 //
					char web_head[2];
					if (safe_recv(imp->recv_socket_, web_head, 2) <= 0) { imp->lose_tcp(); return; }

					// 是否最后一帧 //
					fin = (web_head[0] & 0x80) == 0x80; // 1bit，1表示最后一帧    

					// 获取opcode //
					std::int8_t op_code = web_head[0] & 0x0f;
					if (op_code == 0x08) { imp->lose_tcp(); return; }

					// 获取数据长度
					std::int64_t payload_len = web_head[1] & 0x7F; // 数据长度 
					if (payload_len == 126)
					{
						char length_char[2];
						if (safe_recv(imp->recv_socket_, length_char, 2) <= 0) { imp->lose_tcp(); return; }

						union
						{
							std::uint16_t length;
							char reverse_char[2];
						};
						for (int i = 0; i < 2; ++i)reverse_char[i] = length_char[1 - i];
						payload_len = length;
					}
					else if (payload_len == 127)
					{
						char length_char[8];
						if (safe_recv(imp->recv_socket_, length_char, 8) <= 0) { imp->lose_tcp(); return; }

						char reverse_char[8];
						for (int i = 0; i < 8; ++i)reverse_char[i] = length_char[7 - i];
						std::copy_n(reverse_char, 8, reinterpret_cast<char*>(&payload_len));
					}

					//////////////////////////////////保护，数据不能太大///////////////////////////////
					if (payload_len > 65536 || payload_len + payload_data.size() > 0x00020000)
					{
						LOG_ERROR << "websocket receive too large object" << std::endl;
						imp->lose_tcp();
						return;
					}

					// 获取掩码
					bool mask_flag = (web_head[1] & 0x80) == 0x80; // 是否包含掩码    
					char masks[4];
					if (safe_recv(imp->recv_socket_, masks, 4) <= 0) { imp->lose_tcp(); return; }

					// 用掩码读取出数据 //
					auto last_size = payload_data.size();
					payload_data.resize(payload_data.size() + static_cast<std::size_t>(payload_len));
					if (safe_recv(imp->recv_socket_, payload_data.data() + last_size, static_cast<int>(payload_len)) <= 0) { imp->lose_tcp(); return; }

					for (int i{ 0 }; i<payload_len; ++i)
					{
						payload_data[i + last_size] = payload_data[i + last_size] ^ masks[i % 4];
					}
				}

				if (imp->onReceivedData)imp->onReceivedData(imp->socket_, payload_data.data(), payload_data.size());

				break;
			}
			case UDP:
			{
				int ret = recvfrom(imp->recv_socket_, reinterpret_cast<char *>(&recv_msg.header()), 1024, 0, (struct sockaddr *)(&imp->client_addr_), &imp->sin_size_);
				
				std::unique_lock<std::mutex> close_lck(imp->close_mutex_, std::defer_lock);
				if (ret <= 0 && !close_lck.try_lock()) return;
				if (ret != sizeof(MsgHeader) + recv_msg.size())
				{
					LOG_ERROR << "UDP msg size not correct" << std::endl;
					continue;
				}
				if(ret > 0 && imp->onReceivedMsg)imp->onReceivedMsg(imp->socket_, recv_msg);
				break;
			}
			case UDP_RAW:
			{
				char data[1024];
				int ret = recvfrom(imp->recv_socket_, data, 1024, 0, (struct sockaddr *)(&imp->client_addr_), &imp->sin_size_);
				std::unique_lock<std::mutex> close_lck(imp->close_mutex_, std::defer_lock);
				if (ret <= 0 && !close_lck.try_lock()) return;
				if (ret > 0 && imp->onReceivedData)imp->onReceivedData(imp->socket_, data, ret);
				break;
			}
			}
		}
	}
	auto Socket::loadXml(const amc::core::XmlElement &xml_ele)->void
	{
		setRemoteIP(attributeString(xml_ele, "remote_ip", std::string()));
		setPort(attributeString(xml_ele, "port", std::string()));
		
		if (auto type = attributeString(xml_ele, "connect_type", std::string("TCP")); type == "TCP")imp_->type_ = TCP;
		else if (type == "WEB")imp_->type_ = WEB;
		else if (type == "WEB_RAW")imp_->type_ = WEB_RAW;
		else if (type == "UDP")imp_->type_ = UDP;
		else if (type == "UDP_RAW")imp_->type_ = UDP_RAW;
		else throw std::runtime_error("unknown connect type");

		Object::loadXml(xml_ele);
	}
	auto Socket::saveXml(amc::core::XmlElement &xml_ele) const->void
	{
		Object::saveXml(xml_ele);

		switch (connectType())
		{
		case TCP:
			xml_ele.SetAttribute("connect_type", "TCP");
			break;
		case WEB:
			xml_ele.SetAttribute("connect_type", "WEB");
			break;
		case WEB_RAW:
			xml_ele.SetAttribute("connect_type", "WEB_RAW");
			break;
		case UDP:
			xml_ele.SetAttribute("connect_type", "UDP");
			break;
		case UDP_RAW:
			xml_ele.SetAttribute("connect_type", "UDP_RAW");
			break;
		}

		if (!imp_->remote_ip_.empty())xml_ele.SetAttribute("remote_ip", imp_->remote_ip_.c_str());
		if (!imp_->port_.empty())xml_ele.SetAttribute("port", imp_->port_.c_str());
	}
	auto Socket::stop()->void
	{
		std::lock(imp_->state_mutex_, imp_->close_mutex_);
		std::unique_lock<std::recursive_mutex> lck1(imp_->state_mutex_, std::adopt_lock);
		std::unique_lock<std::mutex> lck2(imp_->close_mutex_, std::adopt_lock);

		switch (imp_->state_)
		{
		case IDLE:
			break;
		case WAITING_FOR_CONNECTION:
			shutdown(imp_->lisn_socket_, 2);
			if (imp_->accept_thread_.joinable())imp_->accept_thread_.join();
			break;
		case WORKING:
			switch (connectType())
			{
			case TCP:
			case WEB:
			case WEB_RAW:
				if (shutdown(imp_->recv_socket_, 2) < 0) LOG_ERROR << "shutdown error:" << errno << std::endl;
				break;
			case UDP:
			case UDP_RAW:
				if (close_sock(imp_->recv_socket_) < 0) LOG_ERROR << "close error:" << errno << std::endl;
				break;
			}

			if(imp_->recv_thread_.joinable())imp_->recv_thread_.join();
			break;
		}

		imp_->state_ = Socket::IDLE;
	}
	auto Socket::startServer(const std::string &port)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);

		if (!port.empty())setPort(port);
		if (this->port().empty())throw std::runtime_error("Socket can't Start as server, because it has empty port\n");

		switch (imp_->state_)
		{
		case IDLE:
			break;
		default:
			throw(std::runtime_error("Socket can't Start as server, because it is not at idle state\n"));
		}

		imp_->is_server_ = true;

		//////////////////////////////////////////////////////////////////////////////////////////////
		int sock_type;
		switch (connectType())
		{
		case TCP:
		case WEB:
		case WEB_RAW:
			sock_type = SOCK_STREAM;
			break;
		case UDP:
		case UDP_RAW:
			sock_type = SOCK_DGRAM;
			break;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////

		// 服务器端开始建立socket描述符 //
		if (static_cast<int>(imp_->lisn_socket_ = socket(AF_INET, sock_type, 0)) == -1)throw(std::runtime_error("Socket can't Start as server, because it can't socket\n"));

		// linux 下设置keep alive
#ifdef UNIX
		if (sock_type == SOCK_STREAM)
		{
			int tcp_timeout = 10000; //10 seconds before aborting a write()
			if (setsockopt(imp_->lisn_socket_, SOL_TCP, TCP_USER_TIMEOUT, &tcp_timeout, sizeof(int)) < 0)
				THROW_FILE_AND_LINE("socket setsockopt TCP_USER_TIMEOUT FAILED");

			// Set the option active //
			int keepAlive = 1; // 开启keepalive属性
			int keepIdle = 5; // 如该连接在60秒内没有任何数据往来,则进行探测 
			int keepInterval = 1; // 探测时发包的时间间隔为5 秒
			int keepCount = 5; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

			if (setsockopt(imp_->lisn_socket_, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive)) < 0)
				THROW_FILE_AND_LINE("socket setsockopt SO_KEEPALIVE FAILED");
			if (setsockopt(imp_->lisn_socket_, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)) < 0)
				THROW_FILE_AND_LINE("socket setsockopt TCP_KEEPIDLE FAILED");
			if (setsockopt(imp_->lisn_socket_, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) < 0)
				THROW_FILE_AND_LINE("socket setsockopt TCP_KEEPINTVL FAILED");
			if (setsockopt(imp_->lisn_socket_, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)) < 0)
				THROW_FILE_AND_LINE("socket setsockopt TCP_KEEPCNT FAILED");
		}
#endif		


		// 设置socketopt选项,使得地址在程序结束后立即可用 //
		int nvalue = 1;
		if (::setsockopt(imp_->lisn_socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&nvalue), sizeof(int)) < 0)throw std::runtime_error("setsockopt failed: SO_REUSEADDR \n");

		// 服务器端填充server_addr_结构,并且bind //
		memset(&imp_->server_addr_, 0, sizeof(struct sockaddr_in));
		imp_->server_addr_.sin_family = AF_INET;
		imp_->server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
		imp_->server_addr_.sin_port = htons(std::stoi(this->port()));
		if (::bind(imp_->lisn_socket_, (struct sockaddr *)(&imp_->server_addr_), sizeof(struct sockaddr)) == -1)
		{

			throw(std::runtime_error("Socket can't Start as server, because it can't bind\n"));
		}
		
		if (connectType() == TCP || connectType() == WEB || connectType() == WEB_RAW)
		{
			// 监听lisn_socket_描述符 //
			if (listen(imp_->lisn_socket_, 5) == -1)throw(std::runtime_error("Socket can't Start as server, because it can't listen\n"));

			// 启动等待连接的线程 //
			std::promise<void> accept_thread_ready;
			auto ready = accept_thread_ready.get_future();
			imp_->accept_thread_ = std::thread(Socket::Imp::acceptThread, this->imp_.get(), std::move(accept_thread_ready));
			imp_->accept_thread_.detach();
			ready.wait();
		}
		else
		{
			// 因为UDP没法shutdown，所以用非阻塞模式 //

#ifdef UNIX
			struct timeval read_timeout;
			read_timeout.tv_sec = 0;
			read_timeout.tv_usec = 10000;
			if (::setsockopt(imp_->lisn_socket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&read_timeout), sizeof(read_timeout)) < 0)throw std::runtime_error("setsockopt failed: SO_RCVTIMEO \n");
#endif

			imp_->recv_socket_ = imp_->lisn_socket_;
			
			std::promise<void> receive_thread_ready;
			auto fut = receive_thread_ready.get_future();
			imp_->recv_thread_ = std::thread(Socket::Imp::receiveThread, imp_.get(), std::move(receive_thread_ready));
			fut.wait();
		}

		return;
	}
	auto Socket::connect(const std::string &remote_ip, const std::string &port)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);

		if (!remote_ip.empty())setRemoteIP(remote_ip);
		if (!port.empty())setPort(port);
		if (remoteIP().empty())throw std::runtime_error("Socket can't connect, because it empty ip address\n");
		if (this->port().empty())throw std::runtime_error("Socket can't connect, because it empty port\n");

		switch (imp_->state_)
		{
		case IDLE:
			break;
		default:
			throw std::runtime_error("Socket can't connect, because it is busy now, please close it\n");
		}

		imp_->is_server_ = false;

		///////////////////////////////////////////////////////////////////////////////
		int sock_type;
		switch (connectType())
		{
		case TCP:
		case WEB:
		case WEB_RAW:
			sock_type = SOCK_STREAM;
			break;
		case UDP:
		case UDP_RAW:
			sock_type = SOCK_DGRAM;
			break;
		}
		//////////////////////////////////////////////////////////////////////////////

		// 服务器端开始建立socket描述符 //
		if ((imp_->recv_socket_ = socket(AF_INET, sock_type, 0)) < 0)throw std::runtime_error("Socket can't connect, because can't socket\n");

		// 客户端填充server_addr_结构 //
		memset(&imp_->server_addr_, 0, sizeof(imp_->server_addr_));
		imp_->server_addr_.sin_family = AF_INET;
		imp_->server_addr_.sin_addr.s_addr = inet_addr(remoteIP().c_str());
		imp_->server_addr_.sin_port = htons(std::stoi(this->port()));

		switch (connectType())
		{
		case TCP:
		{
			// 连接 //
			if (::connect(imp_->recv_socket_, (const struct sockaddr *)&imp_->server_addr_, sizeof(imp_->server_addr_)) == -1)
				throw std::runtime_error("Socket can't connect, because can't connect\n");

			imp_->state_ = Socket::WORKING;

			// Start Thread //
			std::promise<void> receive_thread_ready;
			auto fut = receive_thread_ready.get_future();
			imp_->recv_thread_ = std::thread(Imp::receiveThread, imp_.get(), std::move(receive_thread_ready));
			fut.wait();

			return;
		}
		case WEB:
		case WEB_RAW:
		{
			if (::connect(imp_->recv_socket_, (const struct sockaddr *)&imp_->server_addr_, sizeof(imp_->server_addr_)) == -1)
				throw std::runtime_error("Socket can't connect, because can't connect\n");

			char handshake_text[]{
				"GET / HTTP/1.1\r\n"
				"Host: localhost:8080\r\n"
				"Origin: http://127.0.0.1:3000\r\n"
				"Connection: Upgrade\r\n"
				"Upgrade: websocket\r\n"
				"Sec-WebSocket-Version: 13\r\n"
				"Sec-WebSocket-Key: w4v7O6xFTi36lq3RNcgctw==\r\n\r\n" };

			if(send(imp_->recv_socket_, handshake_text, std::strlen(handshake_text), 0) == -1)
				throw std::runtime_error("Socket can't connect, web sock error 1\n");

			char recv_data[1024]{ 0 };
			int res = recv(imp_->recv_socket_, recv_data, 1024, 0);
			if(res <= 0)throw std::runtime_error("Socket can't connect, web sock error 2\n");

			auto header_map = make_header_map(recv_data);

			////////////   这里应该check更多，tbd //
			if(header_map.find("Sec-WebSocket-Accept") == header_map.end())throw std::runtime_error("Socket can't connect, web sock error 3\n");

			imp_->state_ = Socket::WORKING;

			// Start Thread //
			std::promise<void> receive_thread_ready;
			auto fut = receive_thread_ready.get_future();
			imp_->recv_thread_ = std::thread(Imp::receiveThread, imp_.get(), std::move(receive_thread_ready));
			fut.wait();

			return;
		}
		case UDP:
		case UDP_RAW:
			imp_->state_ = Socket::WORKING;
			return;
		}
	}
	auto Socket::sendMsg(const amc::core::MsgBase &data)->void
	{
#ifdef UNIX
		signal(SIGPIPE, SIG_IGN);
#endif

		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);

		switch (imp_->state_)
		{
		case WORKING:
		{
			switch (imp_->type_)
			{
			case TCP:
				if (send(imp_->recv_socket_, reinterpret_cast<const char *>(&data.header()), data.size() + sizeof(MsgHeader), 0) == -1)
					throw std::runtime_error("Socket failed sending data, because network failed\n");
				else
					return;
				break;
			case WEB:
			{
				auto packed_data = imp_->is_server_ 
					? pack_data_server(reinterpret_cast<const char*>(&data.header()), data.size() + sizeof(amc::core::MsgHeader))
					: pack_data_client(reinterpret_cast<const char*>(&data.header()), data.size() + sizeof(amc::core::MsgHeader));
				if (send(imp_->recv_socket_, packed_data.data(), packed_data.size(), 0) == -1)
					throw std::runtime_error("Socket failed sending data, because network failed\n");
				else
					return;
				break;
			}
			case UDP:
			{
				memset(&imp_->server_addr_, 0, sizeof(imp_->server_addr_));
				imp_->server_addr_.sin_family = AF_INET;
				imp_->server_addr_.sin_addr.s_addr = inet_addr(remoteIP().c_str());
				imp_->server_addr_.sin_port = htons(std::stoi(this->port()));
				
				if (sendto(imp_->recv_socket_, reinterpret_cast<const char *>(&data.header()), data.size() + sizeof(MsgHeader), 0, (const struct sockaddr *)&imp_->server_addr_, sizeof(imp_->server_addr_)) == -1)
					throw std::runtime_error("Socket failed sending data, because network failed\n");
				else
					return;

				break;
			}
			default:
				throw std::runtime_error("Socket failed send msg, because Socket is not at right MODE\n");
			}
		}
		default:
			throw std::runtime_error("Socket failed sending data, because Socket is not at right STATE\n");
		}
	}
	auto Socket::sendRawData(const char *data, int size)->void
	{
#ifdef UNIX
		signal(SIGPIPE, SIG_IGN);
#endif
		
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);

		switch (imp_->state_)
		{
		case WORKING:
		{
			switch (imp_->type_)
			{
			case WEB_RAW:
			{
				auto packed_data = imp_->is_server_ ? pack_data_server(data, size) : pack_data_client(data, size);
				if (send(imp_->recv_socket_, packed_data.data(), packed_data.size(), 0) == -1)
					throw std::runtime_error("Socket failed sending data, because network failed\n");
				else
					return;
				break;
			}
			case UDP_RAW:
			{
				memset(&imp_->server_addr_, 0, sizeof(imp_->server_addr_));
				imp_->server_addr_.sin_family = AF_INET;
				imp_->server_addr_.sin_addr.s_addr = inet_addr(remoteIP().c_str());
				imp_->server_addr_.sin_port = htons(std::stoi(this->port()));

				if (sendto(imp_->recv_socket_, data, size, 0, (const struct sockaddr *)&imp_->server_addr_, sizeof(imp_->server_addr_)) == -1)
					throw std::runtime_error("Socket failed sending data, because network failed\n");
				else
					return;

				break;
			}
			default:
				throw std::runtime_error("Socket failed send raw data, because Socket is not at right MODE\n");
			}
		}
		default:
			throw std::runtime_error("Socket failed send raw data, because Socket is not at right STATE\n");
		}
	}
	auto Socket::isConnected()->bool
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);

		switch (imp_->state_)
		{
		case WORKING:
			return true;
		default:
			return false;
		}
	}
	auto Socket::state()->State
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		return imp_->state_;
	};
	auto Socket::port()const->const std::string & 
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		return imp_->port_;
	}
	auto Socket::setPort(const std::string &port)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		imp_->port_ = port;
	}
	auto Socket::remoteIP()const->const std::string &
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		return imp_->remote_ip_;
	}
	auto Socket::setRemoteIP(const std::string &remote_ip)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		imp_->remote_ip_ = remote_ip;
	}
	auto Socket::connectType()const->TYPE { return imp_->type_; }
	auto Socket::setConnectType(const TYPE type)->void { imp_->type_ = type; }
	auto Socket::setOnReceivedMsg(std::function<int(Socket*, amc::core::Msg &)> OnReceivedData)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		imp_->onReceivedMsg = OnReceivedData;
	}
	auto Socket::setOnReceivedRawData(std::function<int(Socket*, const char *data, int size)> func)->void 
	{ 
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		imp_->onReceivedData = func;
	}
	auto Socket::setOnReceivedConnection(std::function<int(Socket*, const char*, int)> OnReceivedConnection)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		imp_->onReceivedConnection = OnReceivedConnection;
	}
	auto Socket::setOnLoseConnection(std::function<int(Socket*)> OnLoseConnection)->void
	{
		std::unique_lock<std::recursive_mutex> lck(imp_->state_mutex_);
		imp_->onLoseConnection = OnLoseConnection;
	}
	Socket::~Socket() 
	{ 
		stop();

	}
	Socket::Socket(const std::string &name, const std::string& remote_ip, const std::string& port, TYPE type) :Object(name), imp_(new Imp(this))
	{
		// 启动服务器 //

		
		setRemoteIP(remote_ip);
		setPort(port);
		setConnectType(type);
	}
}
