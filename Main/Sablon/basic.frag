#version 330 core //Koju verziju GLSL jezika da koristim (za programabilni pipeline: GL verzija * 100) i koji profil (core - samo programabilni pipeline, compatibility - core + zastarjele stvari)

in vec4 channelCol; 
in vec3 colOffset; 
out vec4 outCol; 

void main() //Glavna funkcija sejdera
{
	outCol = vec4(channelCol.r+colOffset.r,channelCol.g+colOffset.g,channelCol.b+colOffset.b,1.0);
}