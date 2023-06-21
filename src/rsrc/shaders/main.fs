#version 330 core
out vec4 FragColor;
in vec2 texcoords;
uniform vec2 size;
uniform usampler1D board;

bool sampleCircle(vec2 cur, vec2 pos) {
	float l = pos.x * 0.48f;
	float r = pos.y * 0.48f;
	l = cur.x-l;
	l *= l;
	r = cur.y-r;
	r *= r;
	r += l;
	return r < 0.024f && r > 0.012f;
}

void drawCircle(vec2 cur, vec2 pos, vec2 offsets) {
	float alpha = sampleCircle(vec2(cur.x + offsets.x, cur.y + offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleCircle(vec2(cur.x + offsets.x, cur.y - offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleCircle(vec2(cur.x - offsets.x, cur.y + offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleCircle(vec2(cur.x - offsets.x, cur.y - offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleCircle(cur, pos) ? 0.2f : 0.f;
	FragColor = vec4(mix(FragColor.xyz, vec3(0.23f, 0.66f, 0.84f), alpha), 1.f);
}

bool sampleX(vec2 cur, vec2 pos) {
	cur.x = cur.x-(pos.x * 0.48f);
	cur.y = cur.y-(pos.y * 0.48f);
	return (cur.y > cur.x-0.035f && cur.y < cur.x+0.035f && cur.y < -cur.x+0.286f && cur.y > -cur.x-0.286f) ||
			(cur.y > -cur.x-0.035f && cur.y < 0.035f-cur.x && cur.y < cur.x+0.286f && cur.y > cur.x-0.286f);
}

void drawX(vec2 cur, vec2 pos, vec2 offsets) {
	float alpha = sampleX(vec2(cur.x + offsets.x, cur.y + offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleX(vec2(cur.x + offsets.x, cur.y - offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleX(vec2(cur.x - offsets.x, cur.y + offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleX(vec2(cur.x - offsets.x, cur.y - offsets.y), pos) ? 0.2f : 0.f;
	alpha += sampleX(cur, pos) ? 0.2f : 0.f;
	FragColor = vec4(mix(FragColor.xyz, vec3(0.97f, 0.19f, 0.34f), alpha), 1.f);
}

void main() {
	vec2 offsets = vec2(1/(size.x*4), 1/(size.y*4));
	float x = texcoords.x * size.x / size.y;
	
	FragColor = vec4(1.f, 1.f, 1.f, 1.f);
	if(x > -0.7f && x < 0.7f) {
		if(texcoords.y > -0.25f && texcoords.y < -0.215f) {
			FragColor = vec4(0.78f, 0.78f, 0.78f, 1.f);
		} else if(texcoords.y < 0.25f && texcoords.y > 0.215f) {
			FragColor = vec4(0.78f, 0.78f, 0.78f, 1.f);
		}
	}
	if(texcoords.y > -0.7f && texcoords.y < 0.7f) {
		if(x < -0.215f && x > -0.25f) {
			FragColor = vec4(0.78f, 0.78f, 0.78f, 1.f);
		} else if(x > 0.21f && x < 0.245f) {
			FragColor = vec4(0.78f, 0.78f, 0.78f, 1.f);
		}
	}
	vec2 cur = vec2(x, texcoords.y);
	//FragColor = vec4(0.f, texture(board, 0.5f * texcoords.xy + vec2(0.5f), 0).r/2.f, 0.f, 1.f);
	//vec2 coord = 0.5f * texcoords.xy + vec2(0.5f);
	//FragColor = vec4(0.f, coord.x, coord.y, 1.f);

	for(int x = 0;x < 3;++x) {
		for(int y = 0;y < 3;++y) {
			switch(texelFetch(board, y * 3 + x, 0).r) {
				case 1u:
					drawX(cur, vec2(x-1, y-1), offsets);
					break;
				case 2u:
					drawCircle(cur, vec2(x-1, y-1), offsets);
					break;
				case 0u:
					break;
				default:
					drawCircle(cur, vec2(x-1, y-1), offsets);
					drawX(cur, vec2(x-1, y-1), offsets);
					break;
			}
		}
	}
} 