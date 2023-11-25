#version 330 core 

//Kanali (in, out, uniform)
layout(location = 0) in vec2 inPos; 
layout(location = 1) in vec4 inCol; 
out vec4 channelCol;
out vec3 colOffset; 
uniform vec2 uPos;
uniform float timeColorRed;
uniform float timeColorGreen;
uniform float timeColorBlue;
uniform int rotate;


void main() //Glavna funkcija sejdera
{	

	if(rotate == 1){
		gl_Position = vec4(inPos.x-uPos.x , inPos.y+uPos.y, 0.0, 1.0);   
		colOffset = vec3(timeColorRed, timeColorGreen, timeColorBlue);
		channelCol = inCol;
	}
	else{
		gl_Position = vec4(inPos.x , inPos.y, 0.0, 1.0);   
		colOffset = vec3(timeColorRed, timeColorGreen, timeColorBlue);
		channelCol = inCol;
	}


	
}