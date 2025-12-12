#version 120

uniform sampler2D normalMap;
uniform float normalStrength;
uniform int useNormalMap;

varying vec3 vNormal;
varying vec2 vTexCoord;
varying vec3 vTangent;
varying vec3 vBitangent;

void main()
{
    if (useNormalMap == 1) {
        vec3 tangentNormal = texture2D(normalMap, vTexCoord).rgb * 2.0 - 1.0;
        tangentNormal.xy *= normalStrength;
        tangentNormal = normalize(tangentNormal);

        mat3 TBN = mat3(normalize(vTangent), normalize(vBitangent), normalize(vNormal));
        vec3 N = normalize(TBN * tangentNormal);

        gl_FragColor = vec4(N * 0.5 + 0.5, 1.0);
    } else {
        vec3 N = normalize(vNormal);
        gl_FragColor = vec4(N * 0.5 + 0.5, 1.0);
    }
}
