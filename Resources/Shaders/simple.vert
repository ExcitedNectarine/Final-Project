attribute vec3 position;
attribute vec4 uv;
attribute vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;
uniform vec3 lightpos;

varying vec3 uv_frag;
varying vec3 uv_frag2;
varying vec3 normal_frag;
varying vec3 light_pos;

void main()
{
	gl_Position = projection * view * transform * vec4(position, 1.0);
	uv_frag = uv;
	normal_frag = normal;
	light_pos = vec3(view * vec4(lightpos, 1.0));
	
	uv_frag2 = vec3(view * transform * vec4(position, 1.0));
	normal_frag = mat3(transpose(inverse(view * transform))) * normal;
}