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
    // Normalized vectors
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPosition - vPosition);
    vec3 V = normalize(cameraPosition - vPosition);
    vec3 R = reflect(-L, N);

    // Lambert diffuse
    float diff = max(dot(N, L), 0.0);

    // Phong specular
    float spec = pow(max(dot(R, V), 0.0), shininess);

    // Fetch texture or fallback to object color
    vec3 baseColor = hasTexture ? texture2D(tex0, vTexCoord).rgb : color.rgb;

    // Lighting components (physically consistent)
    vec3 ambient  = ambientColor * ambientReflection * baseColor;
    vec3 diffuse  = lightColor * diff * lightIntensity * diffuseReflection * baseColor;
    vec3 specular = lightColor * spec * lightIntensity * specularReflection; // specular not tinted by baseColor
    vec3 emissive = emissiveReflection;

    vec3 finalColor = ambient + diffuse + specular + emissive;

    gl_FragColor = vec4(finalColor, 1.0);
}
