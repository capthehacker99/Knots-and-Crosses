//Thanks derhass, https://stackoverflow.com/questions/2588875/whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2
#version 330 core
out vec2 texcoords;
void main() {
	vec2 vertices[3]=vec2[3](vec2(-1,-1), vec2(3,-1), vec2(-1, 3));
	gl_Position = vec4(vertices[gl_VertexID],0,1);
	texcoords = gl_Position.xy;
}