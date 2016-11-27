#pragma once

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
		struct shm_remove
		{
			shm_remove() { shared_memory_object::remove("memoria_compartilhada_gateway_servidor"); }
			~shm_remove() { shared_memory_object::remove("memoria_compartilhada_gateway_servidor"); }
		} remover;

		shm.reset(new shared_memory_object(create_only, nome.c_str(), read_write));

		struct active_users_t usuarios_ativos;
		(*shm).truncate(sizeof(usuarios_ativos));

		// Mapeia a região de memória compartilhada nesse processo
		region.reset(new mapped_region((*shm), read_write));
	}
	~memoria_compartilhada_gateway_servidor() {}

	void get_usarios_ativos(list<struct position_t> &lista_usuarios_ativos) 
	{
		struct active_users_t *usuarios_ativos = static_cast<struct active_users_t *>((*region).get_address());

		usuarios_ativos->mutex.lock();

		for each (struct position_t p in (*usuarios_ativos).list)
		{
			if (p.id != -1) lista_usuarios_ativos.push_back(p);
		}

		usuarios_ativos->mutex.unlock();
	}

	void get_ultimo_registro(list<struct position_t> &lista_usuarios_ativos)
	{
		struct active_users_t *usuarios_ativos = static_cast<struct active_users_t *>((*region).get_address());

		usuarios_ativos->mutex.lock();

		for each (struct position_t p in (*usuarios_ativos).list)
		{
			if (p.id != -1) lista_usuarios_ativos.push_back(p);
		}

		usuarios_ativos->mutex.unlock();
	}
};