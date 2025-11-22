#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;

uniform sampler2D tex0;
uniform bool hasTexture;

uniform vec3 ambientReflection;
uniform vec3 diffuseReflection;
uniform vec3 specularReflection;   // not used in this simplified shader
uniform vec3 emissiveReflection;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

void main()
{
    // Lighting vectors
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPosition - vPosition);

    float diff = max(dot(N, L), 0.0);

    // Base surface color (texture or uniform)
    vec3 baseColor = hasTexture 
        ? texture2D(tex0, vTexCoord).rgb 
        : color.rgb;

    // Lighting contributions
    vec3 ambient  = ambientColor * ambientReflection * baseColor;
    vec3 diffuse  = lightColor * diff * lightIntensity * diffuseReflection * baseColor;
    vec3 emissive = emissiveReflection;

    // Final color (no specular in this shader)
    vec3 finalColor = ambient + diffuse + emissive;

    gl_FragColor = vec4(finalColor, 1.0);
}
