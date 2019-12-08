varying vec3 out_uv;

uniform samplerCube skybox;

void main()
{
	gl_FragColor = texture(skybox, out_uv);
}