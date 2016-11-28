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
	string nome = "memoria_compartilhada_gateway_servidor";
	boost::scoped_ptr<shared_memory_object> shm;
	boost::scoped_ptr<mapped_region> region;
public:
	memoria_compartilhada_gateway_servidor()
	{
		try 
		{
			shared_memory_object::remove("memoria_compartilhada_gateway_servidor");
			struct shm_remove
			{
				shm_remove() { shared_memory_object::remove("memoria_compartilhada_gateway_servidor"); }
				~shm_remove() { shared_memory_object::remove("memoria_compartilhada_gateway_servidor"); }
			} remover;

			shm.reset(new shared_memory_object(create_only, nome.c_str(), read_write));

			struct active_users_t usuarios_ativos;
			(*shm).truncate(sizeof(struct active_users_t));

			// Mapeia a região de memória compartilhada nesse processo
			region.reset(new mapped_region((*shm), read_write));
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
		struct active_users_t *usuarios_ativos = static_cast<struct active_users_t *>((*region).get_address());

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
			resposta << "\n";
		}
		else
		{
			resposta << ";0\n";
		}

		usuarios_ativos->mutex.unlock();

		str.clear();
		str.assign(resposta.str());

	}

	void get_ultimo_registro(int id, string& resposta)
	{
		bool result(false);

		struct active_users_t *usuarios_ativos = static_cast<struct active_users_t *>((*region).get_address());

		stringstream resposta;
		resposta << "HIST";

		usuarios_ativos->mutex.lock();

		if (usuarios_ativos->list[id].id == -1) 
		{
			result = true;

			resposta << ";" << to_string(usuarios_ativos->list[id].id);
			resposta << ";" << to_string(usuarios_ativos->list[id].timestamp);
			resposta << ";" << to_string(usuarios_ativos->list[id].latitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].longitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].speed);
			resposta << ";1";

		}
		else if(usuarios_ativos->list[id].id != -1)
		{
			resposta << ";" << to_string(id);
			resposta << ";" << to_string(usuarios_ativos->list[id].timestamp);
			resposta << ";" << to_string(usuarios_ativos->list[id].latitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].longitude);
			resposta << ";" << to_string(usuarios_ativos->list[id].speed);
			resposta << ";0";
			result = false;
		}
		else
		{
			resposta << ";0";
		}

		usuarios_ativos->mutex.unlock();

		resposta.assign(resposta.str());
	}
};

void thread_procedimento_gateway_servidor(void * p, bool status)
{
	memoria_compartilhada_gateway_servidor mc;
	struct position_t *nova_posicao = static_cast<struct position_t *>(p);


}