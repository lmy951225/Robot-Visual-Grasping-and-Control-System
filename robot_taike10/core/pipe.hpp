#ifndef AMC_CORE_PIPE_H
#define AMC_CORE_PIPE_H

#include <object.hpp>
#include <msg.hpp>

namespace amc::core
{
	class Pipe :public amc::core::Object
	{
	public:
		auto virtual loadXml(const amc::core::XmlElement &xml_ele)->void override;
		auto sendMsg(const amc::core::MsgBase &)->bool;
		auto recvMsg(amc::core::MsgBase &)->bool;

		virtual ~Pipe();
		Pipe(const std::string &name = "pipe", std::size_t pool_size = 16384);
		Pipe(const Pipe&) = delete;
		Pipe(Pipe&&);
		Pipe& operator=(const Pipe&) = delete;
		Pipe& operator=(Pipe&&);
		AMC_REGISTER_TYPE(Pipe);

	private:
		struct Imp;
		amc::core::ImpPtr<Imp> imp_;
	};
}

#endif
