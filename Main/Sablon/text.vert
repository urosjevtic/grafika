#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex; //Koordinate texture, propustamo ih u FS kao boje
out vec2 chTex;
uniform vec2 uPos;
uniform int rotation;

void main()
{
	if(rotation == 1)
	{
		gl_Position = vec4(inPos.x-uPos.x , inPos.y+uPos.y, 0.0, 1.0);           
		chTex = inTex;
	}else
	{
		gl_Position = vec4(inPos.x , inPos.y, 0.0, 1.0);          
		chTex = inTex;
	}


}