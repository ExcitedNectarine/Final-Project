uniform sampler2D texture;

varying vec3 uv_frag;
varying vec3 uv_frag_pos;
varying vec3 normal_frag;

uniform vec3 view_pos;

#define MAX_LIGHTS 128
#define CONSTANT 1.0
#define LINEAR 0.0
#define QUADRATIC 0.5

struct Light
{
	vec3 position;
	vec3 colour;
	float radius;
};

uniform Light lights[MAX_LIGHTS];

void main()
{
	vec3 total_light;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{	
		// Ambient
		float ambient_strength = 0.1;
		vec3 ambient = ambient_strength * lights[i].colour;
		
		// Diffuse
		vec3 normal = normalize(normal_frag);
		vec3 dir = normalize(-uv_frag_pos);
		float diff = max(dot(normal, dir), 0.0);
		vec3 diffuse = diff * lights[i].colour;
		
		// Specular
		float specular_strength = 0.5;
		vec3 view_dir = normalize(-uv_frag_pos);
		vec3 reflect_dir = reflect(-dir, normal);
		float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
		vec3 specular = specular_strength * spec * lights[i].colour;
		
		//float distance = length(lights[i].position, uv_frag_pos);
		//float attenuation = 1.0 / (CONSTANT + LINEAR * distance + QUADRATIC * (distance * distance));
		
		float attenuation = 1.0;
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		
		total_light += ambient + diffuse + specular;
	}
	
	gl_FragColor = texture2D(texture, uv_frag) * vec4(total_light, 1.0);
}