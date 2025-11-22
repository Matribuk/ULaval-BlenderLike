#version 120

uniform vec4 color;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 ambientColor;
uniform sampler2D tex0;
uniform bool hasTexture;  // True if a real texture is bound

// Material reflection components
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

    // Calculate diffuse lighting
    float diff = max(dot(N, L), 0.0);

    // Quantize the lighting into discrete bands (cel-shading effect)
    float toonLevels = 4.0;
    float toonDiff = floor(diff * toonLevels) / toonLevels;

    // Apply material reflection components to lighting equation
    vec3 ambient = ambientColor * ambientReflection;
    vec3 diffuse = lightColor * toonDiff * lightIntensity * diffuseReflection;
    vec3 emissive = emissiveReflection;

    vec3 lighting = ambient + diffuse + emissive;

    // Use texture if available, otherwise use object color
    vec3 baseColor;
    if (hasTexture) {
        baseColor = texture2D(tex0, vTexCoord).rgb;
    } else {
        baseColor = color.rgb;
    }

    // Apply toon lighting
    vec3 finalColor = lighting * baseColor;

    // Add edge detection for cartoon outline effect
    float edge = (diff > 0.1) ? 1.0 : 0.3;
    finalColor *= edge;

    gl_FragColor = vec4(finalColor, 1.0);
}
