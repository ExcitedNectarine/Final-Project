[VERTEX]
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

[FRAGMENT]
uniform sampler2D texture;

varying vec3 out_uv;
varying vec3 out_normal;
varying vec3 out_frag_pos;

uniform vec3 ambient;
uniform vec3 view_pos;

#define MAX_LIGHTS 128
#define CONSTANT 1.0
#define LINEAR 0.7
#define QUADRATIC 1.8

struct Light
{
	vec3 position;
	vec3 colour;
	float radius;
};

uniform Light lights[MAX_LIGHTS];

void main()
{
	float distance;
	float attenuation;
	vec3 diffuse;
	vec3 specular;
	vec3 total_light = ambient;
	vec4 tex = texture2D(texture, out_uv);
	
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		// Calculate diffuse
		vec3 norm = normalize(out_normal);
		vec3 light_dir = normalize(lights[i].position - out_frag_pos);
		float diff = max(dot(norm, light_dir), 0.0);
		diffuse = diff * vec3(tex) * lights[i].colour;
		
		// Calculate specular
		vec3 view_dir = normalize(view_pos - out_frag_pos);
		vec3 reflect_dir = normalize(-light_dir - norm);
		float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
		specular = 0.5 * spec * lights[i].colour;
		
		distance = length(lights[i].position - out_frag_pos);
		attenuation = 1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance));
		
		diffuse *= attenuation;
		specular *= attenuation;
		
		total_light += diffuse + specular;
	}
	
	gl_FragColor = tex * vec4(total_light, 1.0);
}