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
#include "fila_historiador_servidor.hpp"
#include "fila_servidor_historiador.hpp"
#include "util.hpp"

#define PORTA 1024

using boost::asio::ip::tcp;
using namespace boost::interprocess;

memoria_compartilhada_gateway_servidor memoria_compartilhada;
Fila_Historiador_Servidor fila_historiador_servidor;
Fila_Servidor_Historiador fila_servidor_historiador;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

void session(socket_ptr sock)
{
	try
	{
		for (;;)
		{
			char request[1024];
			memset(request, 0, sizeof(request));
			string requisicao("");
			string resposta("");

			boost::system::error_code error;
			memset(request, 0, sizeof(request));

			size_t length = sock->read_some(boost::asio::buffer(request), error);

			if (error == boost::asio::error::eof)
				break; // Conexão encerrada pela interface gráfica normalmente
			else if (error)
				throw boost::system::system_error(error); // Algum outro erro

			requisicao.assign(request);

			std::cout << "Requisicao: " << requisicao << std::endl;


			if(requisicao.find("REQ_ATIVOS") != std::string::npos)
			{
				memoria_compartilhada.get_usarios_ativos(resposta);
			}
			else if(requisicao.find("REQ_HIST") != std::string::npos)
			{
				vector<string> args;
				int id(0);
				int num_samples(0);
				if (get_args(requisicao, args))
				{
					num_samples = atoi(args.back().c_str());
					args.pop_back();
					id = atoi(args.back().c_str());
					args.pop_back();

					if (num_samples == 1)
					{
						memoria_compartilhada.get_ultimo_registro(id, resposta);
					}
					else if (num_samples > 1)
					{
						historical_data_request_t hist_request;
						historical_data_reply_t hist_reply;

						hist_request.id = id;
						hist_request.num_samples = num_samples;

						fila_servidor_historiador.write_historical_data_request_t(hist_request);

						fila_historiador_servidor.read_historical_data_reply_t(hist_reply);

						resposta_historical_data_request(hist_reply, resposta);
					}
				}
				else
				{
					resposta.assign("HIST;0\n");
				}
			}
			else
			{
				std::cerr << "A requisicao nao tem o formarto esperado: " << requisicao << std::endl;
			}

			std::cout << "Resposta: " << resposta << std::endl;
			boost::asio::write(*sock, boost::asio::buffer(resposta.c_str(), (resposta.length()+1)));
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