#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;
uniform float shininess;
uniform sampler2D tex0;
uniform samplerCube envMap;
uniform float reflectivity;
uniform vec3 reflectionTint;

varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexCoord;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPosition - vPosition);
    vec3 V = normalize(cameraPosition - vPosition);
    vec3 R_phong = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R_phong, V), 0.0), shininess);

    vec3 ambient = ambientColor;
    vec3 diffuse = lightColor * diff * lightIntensity;
    vec3 specular = lightColor * spec * lightIntensity;

    vec4 texColor = texture2D(tex0, vTexCoord);
    float brightness = dot(texColor.rgb, vec3(0.333));
    vec3 baseColor = (brightness < 0.01) ? color.rgb : texColor.rgb * color.rgb;

    vec3 litColor = (ambient + diffuse) * baseColor + specular;

    vec3 finalColor = litColor;

    if (reflectivity > 0.001) {
        vec3 I = normalize(vPosition - cameraPosition);
        vec3 R = reflect(I, N);
        vec3 reflectionColor = textureCube(envMap, R).rgb;
        reflectionColor *= reflectionTint;
        finalColor = mix(litColor, reflectionColor, reflectivity);
    }

    gl_FragColor = vec4(finalColor, 1.0);
}
