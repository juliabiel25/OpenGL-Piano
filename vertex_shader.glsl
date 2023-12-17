#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
layout ( location = 0 ) in vec4 vertex;     //współrzędne wierzcholka w przestrzeni modelu
layout ( location = 1 ) in vec4 normal;     //wektor normalny w przestrzeni modelu
layout ( location = 2 ) in vec2 texCoord0;

//Zmienne interpolowane

out vec4 l;
out vec4 n;
out vec4 v;

out vec4 l2;
out vec4 n2;
out vec4 v2;

out vec2 iTexCoord0; 
out vec2 iTexCoord1;

void main(void) {


    vec4 lp = vec4(2.5, -0.5, 2.2, 1);                       // pozcyja światła, przestrzeń świata
    l = normalize(V * lp - V*M*vertex);                     // wektor do światła w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex);       // wektor do obserwatora w przestrzeni oka
    n = normalize(M * normal);                              // wektor normalny w przestrzeni oka

    
    vec4 lp2 = vec4(-2.5, 0.5, -2.2, 1);    
    l2 = normalize(V * lp2 - V*M*vertex);                    
    v2 = normalize(vec4(0, 0, 0, 1) - V * M * vertex);       
    n2 = normalize(M * normal);                              



    iTexCoord0 = texCoord0;
    iTexCoord1 = (n.xy + 1) / 2;

    gl_Position=P*V*M*vertex;
}
