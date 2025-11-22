#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;
uniform float shininess;
uniform sampler2D tex0;
uniform bool hasTexture;
uniform vec3 ambientReflection;
uniform vec3 diffuseReflection;
uniform vec3 specularReflection;
uniform vec3 emissiveReflection;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPosition - vPosition);
    vec3 V = normalize(cameraPosition - vPosition);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), shininess);

    vec3 baseColor;
    if (hasTexture) {
        baseColor = texture2D(tex0, vTexCoord).rgb;
    } else {
        baseColor = color.rgb;
    }

    vec3 ambient = ambientColor * ambientReflection * baseColor;
    vec3 diffuse = lightColor * diff * lightIntensity * diffuseReflection * baseColor;
    vec3 specular = lightColor * spec * lightIntensity * specularReflection;
    vec3 emissive = emissiveReflection;

    vec3 finalColor = ambient + diffuse + specular + emissive;

    gl_FragColor = vec4(finalColor, 1.0);
}
