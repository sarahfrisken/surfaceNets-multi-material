//
// glShaders.h
//
// OpenGL Shaders used by glView
//
// TWO-SIDED SHADERS
static const char *vertexShader =
"uniform mat4 u_projMatrix;\n"
"uniform mat4 u_modelMatrix;\n"
"uniform mat4 u_viewMatrix;\n"
"uniform mat3 u_normalMatrix;\n"

"attribute vec4 a_position;\n"
"attribute vec3 a_normal;\n"
"attribute vec2 a_texcoord;\n"

"varying vec3 v_vert;\n"
"varying vec3 v_vertNormal;\n"

"varying vec4 v_frontColor; \n"
"varying vec4 v_backColor; \n"
"uniform vec4 colorMap[256];\n"

"void main() {\n"
"   v_vert = vec3(u_viewMatrix * u_modelMatrix * a_position);\n"
"   v_vertNormal = u_normalMatrix * a_normal;\n"
"   gl_Position = u_projMatrix * u_viewMatrix * u_modelMatrix * a_position;\n"
"   v_frontColor = vec4(colorMap[int(a_texcoord.x)]); \n"
"   v_backColor = vec4(colorMap[int(a_texcoord.y)]); \n"
"}\n";

static const char *fragmentShader =
"uniform vec3 u_lightPos;\n"

"varying vec3 v_vert;\n"
"varying vec3 v_vertNormal;\n"
"varying vec4 v_frontColor; \n"
"varying vec4 v_backColor; \n"

"void main() {\n"
"   vec3 L = normalize(v_vert - u_lightPos);\n"
"   float NL = dot(normalize(v_vertNormal), L);\n"
"   if (gl_FrontFacing) {\n"
"       if (v_frontColor.a == 0.0) discard; \n"
"		gl_FragColor = clamp(v_frontColor * 0.4 + v_frontColor * 0.6 * NL, 0.0, 1.0); \n"
"   } \n"
"	else {\n"
"       if (v_backColor.a == 0.0) discard; \n"
"       gl_FragColor = clamp(v_backColor * 0.4 - v_backColor * 0.6 * NL, 0.0, 1.0); \n"
"   } \n"
"}\n";