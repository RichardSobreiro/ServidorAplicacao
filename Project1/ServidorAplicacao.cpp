#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "gateway_historiador.h"
#include "Servidor_Historiador.h"
#include "servidor_tcp.hpp"

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;

		server(io_service, PORTA);
	}
	catch (std::exception& e)
	{
		std::cerr << "Excecao: " << e.what() << std::endl;
	}

	return 0;
}