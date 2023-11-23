#pragma once

#include "../stage.h"
#include "ente/entity/character/enemy/Skeleton/skeleton.h"
#include "ente/entity/character/enemy/Demon/demon.h"
#include "ente/entity/obstacle/tile/tile.h"

namespace stage
{
	class InitialStage : public Stage
	{
	public:
		InitialStage();
		~InitialStage();

	protected:
		 void Initialize() override;
	};
}