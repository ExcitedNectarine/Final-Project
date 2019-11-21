uniform sampler2D texture;

varying vec3 uv_frag;
varying vec3 uv_frag2;
varying vec3 normal_frag;
varying vec3 light_pos;

uniform vec3 light_colour;
uniform vec3 view_pos;

void main()
{
	// Ambient
	float ambient_strength = 0.1;
	vec3 ambient = ambient_strength * light_colour;
	
	// Diffuse
	vec3 normal = normalize(normal_frag);
	vec3 dir = normalize(-uv_frag2);
	float diff = max(dot(normal, dir), 0.0);
	vec3 diffuse = diff * light_colour;
	
	// Specular
	float specular_strength = 0.5;
	vec3 view_dir = normalize(-uv_frag2);
	vec3 reflect_dir = reflect(-dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * light_colour;
	
	vec3 total_light = ambient + diffuse + specular;
	gl_FragColor = texture2D(texture, uv_frag) * vec4(total_light, 1.0);
}