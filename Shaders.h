#pragma once


const char* uiVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"

"out vec2 TexCoord;\n"

"uniform vec4 spriteOffsets[50];\n"
"uniform float textCol[50];\n"
"uniform float bgCol[50];\n"
"uniform vec3 uiColors[18];\n"

"out vec3 textC;\n"
"out vec3 bgC;\n"

"uniform float windowRatio;\n"

"void main()\n"
"{\n"




"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"

"	vec2 offset = vec2(spriteOffsets[gl_InstanceID].x, spriteOffsets[gl_InstanceID].y);\n"


"	gl_Position.x -= 1.0;\n"
"	gl_Position.x += 1.0 / 92.0;\n"
"	gl_Position.x += offset.x * (1.0 / 46.0);\n"

"	gl_Position.y += 1.0;\n"
"	gl_Position.y -= 1.0 / 57.0;\n"
"	gl_Position.y -= offset.y * (1.0 / 28.5);\n"

"	if (windowRatio < 1)\n"
"		gl_Position.x *= windowRatio;\n"
"	else\n"
"		gl_Position.y /= windowRatio;\n"


"	TexCoord = aTexCoord;\n"
"	TexCoord.x += (8.0f / 256.0f) * spriteOffsets[gl_InstanceID].z;\n"
"	TexCoord.y += (8.0f / 256.0f) * spriteOffsets[gl_InstanceID].w;\n"

"	textC = uiColors[int(textCol[gl_InstanceID])];\n"
"	bgC = uiColors[int(bgCol[gl_InstanceID])];\n"

"}\0";





// Fragment shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"

"in vec2 TexCoord;\n" // we set this variable in the OpenGL code.

"in vec3 textC;\n"
"in vec3 bgC;\n"

"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"


"   FragColor = texture(ourTexture, TexCoord); \n"

"   if (FragColor.r == 1 && FragColor.g == 0 && FragColor.b == 1)\n"
"   {"
"		FragColor.rgb = bgC;\n"
"   }"

"   if (FragColor.r == 0 && FragColor.g == 1 && FragColor.b == 0)\n"
"   {"
"		FragColor.rgb = textC;\n"
"   }"

"   if (FragColor.r == 0 && FragColor.g == 1 && FragColor.b == 1)\n"
"   {"
"		discard;\n"
"   }"

"}\0";




const char* sampleVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"

"uniform vec2 offset;\n"
"uniform float windowRatio;\n"

"out vec2 TexCoord;\n"

"void main()\n"
"{\n"



"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"




"	gl_Position.x -= 1.0;\n"
"	gl_Position.x += 1.0 / 92.0;\n"
"	gl_Position.x += offset.x * (1.0 / 46.0);\n"

"	gl_Position.y += 1.0;\n"
"	gl_Position.y -= 1.0 / 57.0;\n"
"	gl_Position.y -= offset.y * (1.0 / 28.5);\n"

"	if (windowRatio < 1)\n"
"		gl_Position.x *= windowRatio;\n"
"	else\n"
"		gl_Position.y /= windowRatio;\n"


"	TexCoord = aTexCoord;\n"

"}\0";



// Fragment shader
const char* sampleFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n" // we set this variable in the OpenGL code.

"uniform sampler2D ourTexture;\n"

"void main()\n"
"{\n"
"   FragColor = texture(ourTexture, TexCoord); \n"
"}\0";