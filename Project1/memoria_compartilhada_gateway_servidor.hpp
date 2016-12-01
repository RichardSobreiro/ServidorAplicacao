#pragma once

//#define __STDC_WANT_LIB_EXT1__ 1

#include <iostream>
#include <list>
// Bibliotecas Boost
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/thread.hpp>
// Bibliotecas do projeto
#include "util.hpp"


class memoria_compartilhada_gateway_servidor
{
public:
	string nome = "memoria_compartilhada_gateway_servidor";
	boost::scoped_ptr<shared_memory_object> shm;
	boost::scoped_ptr<mapped_region> region;
	void *addr;
	struct active_users_t *usuarios_ativos;

	memoria_compartilhada_gateway_servidor()
	{
		try 
		{
			shm.reset(new shared_memory_object(open_or_create, nome.c_str(), read_write));

			(*shm).truncate(sizeof(struct active_users_t));

			// Mapeia a região de memória compartilhada nesse processo
			region.reset(new mapped_region((*shm), read_write));

			addr = (*region).get_address();

			usuarios_ativos = new (addr) struct active_users_t;
		}
		catch (exception& e)
		{
			std::cerr << "Excecao: " << e.what() << std::endl;
		}
	}
	~memoria_compartilhada_gateway_servidor() 
	{
		shared_memory_object::remove("memoria_compartilhada_gateway_servidor");
	}

	void get_usarios_ativos(string& str) 
	{
		usuarios_ativos->mutex.lock();

		stringstream resposta;
		resposta << "ATIVOS";

		if (usuarios_ativos->num_active_users > 0)
		{
			resposta << ";" << std::to_string(usuarios_ativos->num_active_users);

			for each (struct position_t p in (*usuarios_ativos).list)
			{
				if (p.id != -1)
				{	
					resposta << ";" << std::to_string(p.id);
					resposta << ";" << std::to_string(p.id);
				}
			}
		}
		else
		{
			resposta << ";0";
		}

		usuarios_ativos->mutex.unlock();

		resposta << "\n";

		str.clear();
		str.assign(resposta.str());

	}

	void get_ultimo_registro(int id, string& str)
	{
		bool result(false);

		struct active_users_t *usuarios_ativos = static_cast<struct active_users_t *>((*region).get_address());

		stringstream resposta;
		resposta.clear();

		resposta << "HIST";

		usuarios_ativos->mutex.lock();

		if (usuarios_ativos->list[id].id == -1) 
		{
			result = true;

			resposta << ";" << to_string(usuarios_ativos->list[id].id);
			resposta << ";" << "POS";

			struct tm *tm = new struct tm;
			gmtime_s(tm, &(usuarios_ativos->list[id].timestamp));
			char date[30];
			strftime(date, sizeof(date), "%d%m%Y%H%M%S", tm);

			resposta << ";" << date;
			resposta << ";" << to_string(usuarios_ativos->list[id].latitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].longitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].speed);
			resposta << ";0";

		}
		else if(usuarios_ativos->list[id].id != -1)
		{
			resposta << ";" << to_string(id);
			resposta << ";" << "POS";

			struct tm *tm = new struct tm;
			gmtime_s(tm, &(usuarios_ativos->list[id].timestamp));
			char date[30];
			strftime(date, sizeof(date), "%d%m%Y%H%M%S", tm);

			resposta << ";" << date;
			resposta << ";" << to_string(usuarios_ativos->list[id].latitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].longitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].speed);
			resposta << ";1";
			result = false;
		}
		else
		{
			resposta << ";0";
		}

		resposta << "\n";

		usuarios_ativos->mutex.unlock();

		str.clear();
		str.assign(resposta.str());
	}
};
