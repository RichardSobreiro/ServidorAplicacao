#pragma once

#include <iostream>
#include <string>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/scoped_ptr.hpp>

#include "util.hpp"

using namespace std;
using namespace boost::interprocess;


class Fila_Servidor_Historiador
{
	string nome = "servapp_historiador";
	boost::scoped_ptr<message_queue> mq;
public:
	Fila_Servidor_Historiador()
	{
		message_queue::remove(nome.c_str());
		mq.reset(new message_queue(create_only, nome.c_str(), 1, sizeof(struct historical_data_request_t)));
	}
	~Fila_Servidor_Historiador()
	{
		message_queue::remove(nome.c_str());
	}

	void write_historical_data_request_t(struct historical_data_request_t &historical_data_request_t)
	{
		try
		{
			(*mq).send(&historical_data_request_t, sizeof(historical_data_request_t), 1);
		}
		catch (interprocess_exception &ex)
		{
			message_queue::remove(nome.c_str());
			cout << ex.what() << endl;
			return;
		}
	}

	size_t read_historical_data_request_t(struct historical_data_request_t &item)
	{
		size_t msg_size;
		unsigned int priority(1);

		try
		{
			(*mq).receive(&item, sizeof(item), msg_size, priority);
			return msg_size;
		}
		catch (interprocess_exception &ex)
		{
			message_queue::remove(nome.c_str());
			cout << ex.what() << endl;
			return 0;
		}
		catch (const std::exception &e)
		{
			cerr << "Erro: " << e.what() << endl;
			return 0;
		}
	}
};
