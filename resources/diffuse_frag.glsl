#version 330 core

in vec3 worldPos;
in vec3 fragNor;
in vec2 texCoords;

uniform float matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float shine;

uniform vec3 camPos;
uniform vec3 lightDir;

uniform sampler2D textureImage;
uniform bool usesTexture;

uniform bool isToon;
uniform float silAngle;
uniform float numCells;
uniform sampler1D cellIntensities;
uniform sampler1D cellScales;

out vec4 color;

void main() {
    vec3 viewDir = camPos - worldPos;
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);
    vec3 N = normalize(fragNor);
    float lambert = dot(L, N);

    /* Diffuse */
    float diffuseContrib = clamp(lambert, matAmbient, 1.0);
    vec3 diffuseColor = matDiffuse;
    if (usesTexture) {
        diffuseColor = vec3(texture(textureImage, texCoords));
    }

    /* Specular using Blinn-Phong */
    vec3 H = (L + V) / 2.0;
    float specularContrib = pow(max(dot(H, N), 0.0), shine);

    /* Cell shading */
    if (isToon) {
        for(int i = 0; i < numCells; i++) {
            if(lambert > texelFetch(cellIntensities, i, 0).r) {
                float scale = texelFetch(cellScales, i, 0).r;
                diffuseContrib *= scale;
                specularContrib *= scale;
                break;
            }
        }
   }
 
    vec3 pColor = vec3(diffuseColor*diffuseContrib + matSpecular*specularContrib);

    /* Silhouettes */
    float edge = (isToon && (clamp(dot(N, V), 0.0, 1.0) < silAngle)) ? 0.0 : 1.0;

    color = vec4(edge * pColor, 1.0);
}
