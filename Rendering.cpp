#include "Rendering.h"

namespace ENG
{
	/**
	* Draws model components.
	*/
	void drawModels(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			//resources.shader(m.shader).setUniform("transform", transforms[id].get());
			resources.shader(m.shader).setUniform("transform", getWorld(entities, id));
			resources.shader(m.shader).bind();
			resources.mesh(m.mesh).bind();
			resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
		}
	}
}