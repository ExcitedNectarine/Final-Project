#include "Lighting.h"

namespace ENG
{
	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Entities& entities, Shader& shader)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& lights = entities.getPool<CS::Light>();

		std::vector<EntityID> ents = entities.entitiesWith<CS::Transform, CS::Light>();
		for (std::size_t i = 0; i < ents.size(); i++)
		{
			shader.setUniform("lights[" + std::to_string(i) + "].position", transforms[ents[i]].position);
			shader.setUniform("lights[" + std::to_string(i) + "].colour", lights[ents[i]].colour);
			shader.setUniform("lights[" + std::to_string(i) + "].radius", lights[ents[i]].radius);
		}
	}
}