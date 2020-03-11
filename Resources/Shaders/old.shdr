// FRAG
uniform sampler2D texture;

varying vec3 uv_frag;
varying vec3 uv_frag_t;
varying vec3 uv_frag_v;
varying vec3 normal_frag;

uniform vec3 view_pos;
uniform vec3 ambient;

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
	float distance = 0.0;
	float attenuation = 0.0;
	vec3 diffuse;
	vec3 total_light = ambient;
	vec4 tex = texture2D(texture, uv_frag);
	
	for (int i = 0; i < MAX_LIGHTS; i++)
	{	
		vec3 normal = normalize(normal_frag);
		vec3 dir = normalize(-uv_frag_v);
		float diff = max(dot(normal, dir), 0.0);

		distance = length(lights[i].position - uv_frag_t);
		attenuation = 1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance));
		//attenuation = max(1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance)) - 1.0f / (lights[i].radius * lights[i].radius), 0.0f);
		diffuse = diff * vec3(tex) * lights[i].colour;
		diffuse *= attenuation;
		total_light += diffuse;

		// // Diffuse
		// vec3 normal = normalize(normal_frag);
		// vec3 dir = normalize(-uv_frag_pos);
		// float diff = max(dot(normal, dir), 0.0);
		// vec3 diffuse = diff * lights[i].colour;
		
		// // Specular
		// // float specular_strength = 0.5;
		// // vec3 view_dir = normalize(-uv_frag_pos);
		// // vec3 reflect_dir = reflect(-dir, normal);
		// // float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
		// // vec3 specular = specular_strength * spec * lights[i].colour;
		
		// float dist = length(lights[i].position - uv_frag_pos);
		// //float attenuation = 1.0 / (CONSTANT + LINEAR * dist + QUADRATIC * (dist * dist));	
		// float attenuation = max(1.0 / (CONSTANT + LINEAR * dist + QUADRATIC * (dist * dist)) - 1.0f / (lights[i].radius * lights[i].radius), 0.0f);
		// //diffuse = vec3(tex) * 
		// diffuse *= attenuation;
		// //specular *= attenuation;
		
		// total_light += diffuse;// + specular;
	}
	
	gl_FragColor = tex * vec4(total_light, 1.0);
}

// VERT
attribute vec3 position;
attribute vec4 uv;
attribute vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

varying vec3 uv_frag;
varying vec3 uv_frag_t;
varying vec3 uv_frag_v;
varying vec3 normal_frag;

void main()
{
	gl_Position = projection * view * transform * vec4(position, 1.0);
	uv_frag = uv;
	normal_frag = normal;
	
	uv_frag_t = vec3(transform * vec4(position, 1.0));
	uv_frag_v = vec3(view * vec4(position, 1.0));
	normal_frag = mat3(transpose(inverse(view * transform))) * normal;
}