#include "Rendering.h"

namespace ENG
{
	namespace CS
	{
		glm::mat4 Camera::get() { return glm::perspective(glm::radians(fov), aspect, near, far); }
	}

	/**
	* Draws model components.
	*/
	void drawModels(Entities& entities, Resources& resources, const glm::vec3& view_pos)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		// Sort objects so that multiple of the same model and texture are drawn in 1 draw call.
		//std::map<std::string, std::map<std::string, std::vector<EntityID>>> em;
		//for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		//	em[models[id].mesh][models[id].texture].push_back(id);

		//resources.shader("default2.shader").bind();
		//for (const auto& m : em)
		//{
		//	resources.mesh(m.first).bind();

		//	for (const auto& t : m.second)
		//	{
		//		resources.texture(t.first).bind();

		//		int i = 0;
		//		for (i; i < t.second.size(); i++)
		//			resources.shader("default2.shader").setUniform("transforms[" + std::to_string(i) + "]", getWorldT(entities, t.second[i]));

		//		glDrawArraysInstanced(GL_TRIANGLES, 0, resources.mesh(m.first).vertexCount(), i);
		//	}
		//}

		// Draw objects from closest to furthest, std::map stores keys ordered by operator<
		//std::map<float, EntityID> distances;
		//for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		//	distances[glm::distance(view_pos, transforms[id].position)] = id;

		//for (auto& f : distances)
		//{
		//	CS::Model& m = models[f.second];

		//	resources.shader("default.shdr").setUniform("transform", getWorldT(entities, f.second));
		//	resources.shader("default.shdr").bind();
		//	resources.mesh(m.mesh).bind();
		//	resources.texture(m.texture).bind();

		//	glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
		//}

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			resources.shader("default.shdr").setUniform("transform", getWorldT(entities, id));
			resources.shader("default.shdr").bind();
			resources.mesh(m.mesh).bind();
			resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
		}
	}

	void drawToCameras(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& cameras = entities.getPool<CS::Camera>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Camera>())
		{
			resources.shader("default.shdr").setUniform("projection", cameras[id].get());
			resources.shader("default.shdr").setUniform("view", glm::inverse(getWorldT(entities, id)));
			resources.shader("default.shdr").setUniform("view_pos", transforms[id].position);
			resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(glm::inverse(getWorldT(entities, id)))));

			cameras[id].frame.bind();

			drawSkybox(resources);
			drawModels(entities, resources, transforms[id].position);

			cameras[id].frame.unbind();
		}
	}

	void drawSkybox(Resources& resources)
	{
		glDepthMask(GL_FALSE);
		resources.shader("skybox.shdr").bind();
		resources.mesh("cube_inverted.obj").bind();
		glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube_inverted.obj").vertexCount());
		glDepthMask(GL_TRUE);
	}
}