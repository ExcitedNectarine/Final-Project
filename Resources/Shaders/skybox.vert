attribute vec3 in_position;

varying vec3 out_uv;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    out_uv = in_position;
    gl_Position = projection * view * vec4(in_position, 1.0);
}  