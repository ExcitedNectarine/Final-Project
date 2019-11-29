attribute vec3 in_position;
attribute vec4 in_uv;
attribute vec3 in_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

varying vec3 out_uv;
varying vec3 out_normal;
varying vec3 out_frag_pos;

void main()
{
	out_uv = in_uv;
	out_normal = mat3(transpose(inverse(transform))) * in_normal;
	out_frag_pos = vec3(transform * vec4(in_position, 1.0));
	
	gl_Position = projection * view * vec4(out_frag_pos, 1.0);
}