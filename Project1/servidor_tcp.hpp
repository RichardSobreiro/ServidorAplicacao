#pragma once

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

#include "memoria_compartilhada_gateway_servidor.hpp"
#include "util.hpp"

#define PORTA 1024

using boost::asio::ip::tcp;
using namespace boost::interprocess;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

void session(socket_ptr sock)
{
	memoria_compartilhada_gateway_servidor memoria_compartilhada;

	try
	{
		for (;;)
		{
			char request[1024];
			memset(request, 0, sizeof(request));
			string requisicao("");
			string resposta("");

			boost::system::error_code error;
			size_t length = sock->read_some(boost::asio::buffer(request), error);
			if (error == boost::asio::error::eof)
				break; // Conexão encerrada pela interface gráfica normalmente
			else if (error)
				throw boost::system::system_error(error); // Algum outro erro

			requisicao.assign(request);

			if(requisicao.find("REQ_ATIVOS"))
			{
				std::cout << "Requisicao: " << requisicao << std::endl;

				memoria_compartilhada.get_usarios_ativos(resposta);

				std::cout << "Resposta: " << resposta << std::endl;
			}
			else if(requisicao.find("REQ_HIST"))
			{
				std::cout << "Requisicao: " << requisicao << std::endl;

				vector<string> args;
				int id(0);
				int num_amostras(0);
				if (get_args(requisicao, args))
				{
					num_amostras = atoi(args.push_back.c_str());
					id = atoi(args.push_back.c_str());

					if (num_amostras == 1)
					{
						memoria_compartilhada.get_ultimo_registro(id, resposta);
					}
					else if (num_amostras > 1)
					{

					}
					else
					{

					}
				}
				else
				{
					resposta.assign("HIST;0");
				}

				std::cout << "Resposta: " << resposta << std::endl;
			}
			else
			{
				std::cerr << "A requisicao nao tem o formarto esperado: " << requisicao << std::endl;
			}

			boost::asio::write(*sock, boost::asio::buffer(resposta.c_str(), length));
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

void server(boost::asio::io_service& io_service, short port)
{
	tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
	for (;;)
	{
		socket_ptr sock(new tcp::socket(io_service));
		a.accept(*sock);
		boost::thread t(boost::bind(session, sock));
	}
}