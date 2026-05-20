#pragma once

#include "ISystem.h"
#include "Registry.h"


#include <thread>
#include <mutex>

namespace Engine {

	class DataBaseCommunicateSystem : public ISystem
	{
		void Init(ECS& e) override;

		void Update(ECS& e, float dt) override;


		void InitDataBase(ECS& e);

		struct entity_data {
			int id;
			float r, g, b;
		};

		std::vector<entity_data> m_entitysToChange;
		std::thread m_dbThread;
		std::mutex m_mutex;
		bool m_isRunning = true;
	public:

		~DataBaseCommunicateSystem();
	};

}
