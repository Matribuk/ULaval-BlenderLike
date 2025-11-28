#version 120

void main()
{
    // OpenGL écrit automatiquement la profondeur dans gl_FragDepth
    // On n'a besoin de rien faire ici pour une simple shadow map
    gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}
