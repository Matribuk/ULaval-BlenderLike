#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;
uniform sampler2D tex0;
<<<<<<< HEAD
uniform bool hasTexture;
=======
uniform bool hasTexture;  // True if a real texture is bound

// Material reflection components
>>>>>>> 1d070a7 ([ADD] multi prossece shader)
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
    float diff = max(dot(N, L), 0.0);

<<<<<<< HEAD
    vec3 baseColor;
    if (hasTexture) {
        baseColor = texture2D(tex0, vTexCoord).rgb;
    } else {
        baseColor = color.rgb;
    }

    vec3 ambient = ambientColor * ambientReflection * baseColor;
    vec3 diffuse = lightColor * diff * lightIntensity * diffuseReflection * baseColor;
    vec3 emissive = emissiveReflection;
    vec3 finalColor = ambient + diffuse + emissive;
=======
    // Apply material reflection components to lighting equation
    vec3 ambient = ambientColor * ambientReflection;
    vec3 diffuse = lightColor * diff * lightIntensity * diffuseReflection;
    vec3 emissive = emissiveReflection;

    vec3 lighting = ambient + diffuse + emissive;

    // Use texture if available, otherwise use object color
    vec3 baseColor;
    if (hasTexture) {
        baseColor = texture2D(tex0, vTexCoord).rgb;
    } else {
        baseColor = color.rgb;
    }

    vec3 finalColor = lighting * baseColor;
>>>>>>> 1d070a7 ([ADD] multi prossece shader)

    gl_FragColor = vec4(finalColor, 1.0);
}
