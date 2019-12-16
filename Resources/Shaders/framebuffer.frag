uniform sampler2D texture;

varying vec2 out_uv;

void main()
{
	gl_FragColor = texture2D(texture, out_uv);
}