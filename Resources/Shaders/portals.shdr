[VERTEX]
attribute vec3 in_position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

varying vec3 out_uv;
varying mat4 out_pvt;

void main()
{
	out_pvt = projection * view * transform;
	gl_Position = out_pvt * vec4(in_position, 1.0);
	out_uv = in_position;
}

[FRAGMENT]
uniform sampler2D texture;
uniform vec3 ambient;

varying vec3 out_uv;
varying mat4 out_pvt;

void main()
{
	vec4 clip = out_pvt * vec4(out_uv, 1.0);
	vec2 new_uv = clamp(vec2(clip / clip.w) / 2.0 + 0.5, vec2(0.0), vec2(1.0));
	gl_FragColor = texture2D(texture, new_uv);
}