
//@ add transparency to text

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lib.h"
#include "opengl.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <x86intrin.h>


struct glyphInfo
{
	float TexX0, TexX1, TexY0, TexY1;
	int Width, Height;
	int OffsetLeft, OffsetTop, AdvanceX;
};

struct fontAtlas
{
	GLuint TextureID;
	unsigned int Width;
	unsigned int Height;
	glyphInfo Glyphs[95];
};


int WindowWidth = 1000;
int WindowHeight = 600;

FT_Library library;
//FT_Face face;

////GLuint CreateShader(const char *VertexShaderFile, const char *FragmentShaderFile)
//GLuint CreateShader(char *vertexShaderText, char *fragmentShaderText)
//{
////	printf("VERTEX SHADER:\n%s", vertexShaderText);
////	printf("FRAGMENT SHADER:\n%s", fragmentShaderText);
//	int success;
//	char info_log[512];
//
////	char *vertexShaderText;
////	if(!ReadTextFile(VertexShaderFile, &vertexShaderText)){
////		fprintf(stderr, "Error: couldnt read shader file: %s\n", VertexShaderFile);
////		return 0; //@
////	}
//
//	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
//	glCompileShader(vertexShader);
//
//	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(vertexShader, sizeof(info_log), NULL, info_log);
////		printf("Shader error: %s: %s\n", VertexShaderFile, info_log);
//		printf("vertex shader error: %s\n", info_log);
//	}
//
////	char *fragmentShaderText;
////	if(!ReadTextFile(FragmentShaderFile, &fragmentShaderText)){
////		fprintf(stderr, "Error: couldnt read shader file: %s\n", fragmentShaderText);
////		return 0; //@
////	}
//
//	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, &fragmentShaderText, NULL);
//	glCompileShader(fragmentShader);
//
//	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(fragmentShader, sizeof(info_log), NULL, info_log);
////		printf("Shader error: %s: %s\n", FragmentShaderFile, info_log);
//		printf("fragment shader error: %s\n", info_log);
//	}
//
//	GLuint shaderProgram = glCreateProgram();
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//	glLinkProgram(shaderProgram);
//
//	//@ delete shaders?
////	free(vertexShaderText);
////	free(fragmentShaderText);
//
//	return shaderProgram;
//}

//GLuint make_shader(const char *VertexSrc, const char *FragmentSrc, const char *GeometrySrc = NULL)
//{
//	int success;
//	char info_log[512];
//
//	GLuint VertexShader   = glCreateShader(GL_VERTEX_SHADER);
//	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(VertexShader, 1, &VertexSrc, NULL);
//	glShaderSource(FragmentShader, 1, &FragmentSrc, NULL);
//
//	glCompileShader(VertexShader);
//	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(VertexShader, sizeof(info_log), NULL, info_log);
//		printf("vertex shader error: %s\n", info_log);
//		//@ error handling
//	}
//
//	glCompileShader(FragmentShader);
//	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(FragmentShader, sizeof(info_log), NULL, info_log);
//		printf("fragment shader error: %s\n", info_log);
//		//@ error handling
//	}
//
//	GLuint ShaderProgram = glCreateProgram();
//	glAttachShader(ShaderProgram, VertexShader);
//	glAttachShader(ShaderProgram, FragmentShader);
//
//	if(GeometrySrc)
//	{
//		GLuint GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
//		glShaderSource(GeometryShader, 1, &GeometrySrc, NULL);
//		glCompileShader(GeometryShader);
//		glGetShaderiv(GeometryShader, GL_COMPILE_STATUS, &success);
//		if (!success)
//		{
//			glGetShaderInfoLog(GeometryShader, sizeof(info_log), NULL, info_log);
//			printf("geometry shader error: %s\n", info_log);
//		//@ error handling
//		}
//		glAttachShader(ShaderProgram, GeometryShader);
//	}
//
//	glLinkProgram(ShaderProgram);
//
//	//@ delete shaders?
////	free(vertexShaderText);
////	free(fragmentShaderText);
//
//	return ShaderProgram;
//}
//
//void pass_to_shader(GLuint Shader, const char *Name, int Value)
//{
//	GLint UniformLocation = glGetUniformLocation(Shader, Name);
//	glUniform1i(UniformLocation, Value);
//}
//
//void pass_to_shader(GLuint Shader, const char *Name, float Value)
//{
//	GLint UniformLocation = glGetUniformLocation(Shader, Name);
//	glUniform1f(UniformLocation, Value);
//}

bool AreEqual(const char *Str1, const char *Str2)
{
	bool Result = true;
	int i = 0;
	while(true)
	{
		if(Str1[i] != Str2[i])
		{
			Result = false;
			break;
		}
		if(Str1[i] == '\0')
		{
			break;
		}
		
		i += 1;
	}
	
	return Result;
}

void PrintSlice(const char *Str, int Start, int End)
{
	for(int i = Start; i < End; ++i)
	{
		putchar(Str[i]);
//		printf("%c (%d), ", Str[i], Str[i]);
	}
//	printf("\n");
}

void PrintSlice(const char *Str, int Start)
{
	for(int i = Start; Str[i]; ++i)
	{
		putchar(Str[i]);
//		printf("%c (%d), ", Str[i], Str[i]);
	}
//	printf("\n");
}

void PrintFormattedLine(const char *Str, int LineNum, int Start, int End)
{
	printf("%d: ", LineNum);
	PrintSlice(Str, Start, End);
	printf("\n");
}

void PrintFormattedLine(const char *Str, int LineNum, int Start)
{
	printf("%d: ", LineNum);
	PrintSlice(Str, Start);
	printf("\n");
}

bool AreEqual(const char *SlicedStr, int Start, int End, const char *Str)
{
	bool Result = true;
	int i = 0;
	while(true)
	{
		if(Str[i] == '\0')
		{
			if(Start+i != End)
			{
				Result = false;
			}
			break;
		}
		if(Str[i] != SlicedStr[Start+i])
		{
			Result = false;
			break;
		}

		i += 1;
	}

	return Result;
}

GLuint CompileShaders(const char *File)
{
	GLuint ShaderId;
	char *FileContents;

	char *VertexShaderSrc;
	char *FragmentShaderSrc;

	if(ReadTextFile(File, &FileContents))
	{
		int LineStart = 0;
//		int LineCount = 1;
		for(int i = 0; ; ++i)
		{
			if(FileContents[i] == '\0')
			{
				if(i > LineStart)
				{
//					PrintFormattedLine(FileContents, LineCount, LineStart);
				}
				break;
			}
			if(FileContents[i] == '\n')
			{
//				PrintFormattedLine(FileContents, LineCount, LineStart, i);
//				LineCount += 1;
				if(AreEqual(FileContents, LineStart, i, "---"))
				{
//					printf("VERTEX SHADER:\n");
//					PrintSlice(FileContents, 0, LineStart);
//					printf("FRAGMENT SHADER:\n");
//					PrintSlice(FileContents, i+1);
					FileContents[LineStart] = '\0';
					VertexShaderSrc = FileContents;
					FragmentShaderSrc = &(FileContents[i+1]);
					break;
				}
				LineStart = i + 1;
			}
		}
	}
	else
	{
		fprintf(stderr, "Error: ReadTextFile()\n");
		return -1; //@ random
	}

//	printf("VERTEX SHADER:\n%s", VertexShaderSrc);
//	printf("FRAGMENT SHADER:\n%s", FragmentShaderSrc);

	ShaderId = make_shader(VertexShaderSrc, FragmentShaderSrc);
	free(FileContents);
	return ShaderId;
}

//GLuint make_text_shader()
//{
//	const char *VertexShader = R"(
//		#version 330 core
//	
//		layout(location = 0) in vec2 position;
//		layout(location = 1) in vec2 texture;
//		layout(location = 2) in vec3 color;
//	
//		out vec2 ftexture;
//		out vec3 fcolor;
//	
//		void main()
//		{
//			gl_Position = vec4(position, 0.0, 1.0);
//			fcolor = color;
//			ftexture = texture;
//		}
//	)";
//
//	const char *FragmentShader = R"(
//		#version 330 core
//		
//		in vec2 ftexture;
//		in vec3 fcolor;
//		
//		out vec4 color;
//		
//		uniform sampler2D s;
//		
//		void main()
//		{
//			color = vec4(fcolor, texture(s, ftexture).r);
//		}
//	)";
//
//	return make_shader(VertexShader, FragmentShader);
//}

//GLuint make_text_shader_geometry_style()
//{
//	const char *VertexShader = R"(
//		#version 330 core
//		
//		layout(location = 0) in vec2 XY0;
//		layout(location = 1) in vec2 XY1;
//		layout(location = 2) in vec2 TXY0;
//		layout(location = 3) in vec2 TXY1;
//		layout(location = 4) in vec3 color;
//		
//		out vec2 gXY0;
//		out vec2 gXY1;
//		out vec2 gTXY0;
//		out vec2 gTXY1;
//		out vec3 gColor;
//
////		uniform float MyUniform;
//		uniform float PixelWidth;
//		uniform float PixelHeight;
//		
//		void main()
//		{
////		float _X = X * PixelWidth - 1.0f;
////		float _Y = 1.0f - Y * PixelHeight;
////		float _Width = Width * PixelWidth;
////		float _Height = Height * PixelHeight;
//
////			gl_Position = vec4(upperLeft, 0.0, 1.0);
////			gXY0 = XY0;
////			gXY1 = XY1;
//
//			gXY0.x = XY0.x * PixelWidth - 1.0;
//			gXY0.y = 1.0 - XY0.y * PixelHeight;
//			gXY1.x = XY1.x * PixelWidth - 1.0;
//			gXY1.y = 1.0 - XY1.y * PixelHeight;
//
//			gTXY0 = TXY0;
//			gTXY1 = TXY1;
//			gColor = color;
////			fColor = color;
//
////			gXY0.x += MyUniform;
////			gXY1.x += MyUniform;
//		}
//	)";
//
//	const char *GeometryShader = R"(
//		#version 330 core
//		
//		layout(points) in;
//		layout(triangle_strip, max_vertices=6) out;
//		
////		in vec2 gtexture[];
//
//		in vec2 gXY0[];
//		in vec2 gXY1[];
//		in vec2 gTXY0[];
//		in vec2 gTXY1[];
//		in vec3 gColor[];
//
//		//in vertexData
//		//{
//		//	vec4 color;
//		//} GS[];
//		
//		out vec2 fTexture;
//		out vec3 fColor;
//		
//		void main()
//		{
//			fColor = gColor[0];
//
//			gl_Position = vec4(gXY0[0], 0.0, 1.0);
//			fTexture = vec2(gTXY0[0].x, gTXY1[0].y);
//			EmitVertex();
//
//			gl_Position = vec4(gXY0[0].x, gXY1[0].y, 0.0, 1.0);
//			fTexture = vec2(gTXY0[0].x, gTXY0[0].y);
//			EmitVertex();
//
//			gl_Position = vec4(gXY1[0], 0.0, 1.0);
//			fTexture = vec2(gTXY1[0].x, gTXY0[0].y);
//			EmitVertex();
//
//			gl_Position = vec4(gXY0[0], 0.0, 1.0);
//			fTexture = vec2(gTXY0[0].x, gTXY1[0].y);
//			EmitVertex();
//
//			gl_Position = vec4(gXY1[0].x, gXY0[0].y, 0.0, 1.0);
//			fTexture = vec2(gTXY1[0].x, gTXY1[0].y);
//			EmitVertex();
//
//			gl_Position = vec4(gXY1[0], 0.0, 1.0);
//			fTexture = vec2(gTXY1[0].x, gTXY0[0].y);
//			EmitVertex();
//		
//			EndPrimitive();
//		}
//	)";
//
//	const char *FragmentShader = R"(
//		#version 330 core
//		
//		in vec3 fColor;
//		in vec2 fTexture;
//		
//		out vec4 color;
//
//		uniform sampler2D s;
//		void main()
//		{
//			color = vec4(fColor, texture(s, fTexture));
////			color = vec4(1.0, 1.0, 1.0, 1.0);
//		}
//	)";
//
//	return make_shader(VertexShader, FragmentShader, GeometryShader);
////	return make_shader(VertexShader, FragmentShader);
//}

///* X and Y specify the top left corner of the bitmap in window coordinates */
//void RenderGlyphQuad(FT_Bitmap *Bitmap, int X, int Y)
//{
//	// Create texture that contains the glyph bitmap.
//	GLuint myTexture;
//	glGenTextures(1, &myTexture);
//	glBindTexture(GL_TEXTURE_2D, myTexture);
//
//	// What to do when texture coordinates are outside of the texture:
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// What to do when the texture is minified/magnified:
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
////	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
////	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Bitmap->width, Bitmap->rows, 0, GL_RED, GL_UNSIGNED_BYTE, Bitmap->buffer);
//
//	float PixelWidth = 2.0f / WindowWidth;
//	float PixelHeight = 2.0f / WindowHeight;
//	float _X = X * PixelWidth - 1;
//	float _Y = 1 - Y * PixelHeight;
//	float _Width = Bitmap->width * PixelWidth;
//	float _Height = Bitmap->rows * PixelHeight;
//	float Vertices[] = {
//		_X, _Y, 0.0f, 0.0f, 0.0f,
//		_X, _Y - _Height, 0.0f, 0.0f, 1.0f,
//		_X + _Width, _Y - _Height, 0.0f, 1.0f, 1.0f,
//		_X + _Width, _Y - _Height, 0.0f, 1.0f, 1.0f,
//		_X + _Width, _Y, 0.0f, 1.0f, 0.0f,
//		_X, _Y, 0.0f, 0.0f, 0.0f,
//	};
//
//
//	// Do OpenGL rendering stuff.
//
//	GLuint VertexArray, VertexBuffer;
//	glGenVertexArrays(1, &VertexArray);
//	glGenBuffers(1, &VertexBuffer);
//
//	glBindVertexArray(VertexArray);
//	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)0);
//	glEnableVertexAttribArray(0);
//
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *) (sizeof(float) * 3));
//	glEnableVertexAttribArray(1);
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
//
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//
//	glDeleteTextures(1, &myTexture);
//	glDeleteBuffers(1, &VertexBuffer);
//	glDeleteVertexArrays(1, &VertexArray);
//}
//
//void DrawText(const char *Text, int X, int Y)
//{
//	int OriginalX = X;
//	for(int i = 0; Text[i]; ++i)
//	{
//		if(Text[i] == '\n' || X > WindowWidth)
//		{
//			Y += 10;
//			if(Y > WindowHeight)
//			{
//				break;
//			}
//			X = OriginalX;
//			continue;
//		}
//		unsigned int index = FT_Get_Char_Index(face, Text[i]);
//		FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
//		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
//		FT_GlyphSlot slot = face->glyph;
//		FT_Bitmap *bitmap = &slot->bitmap;
////		printf("rows: %u, width: %u, pitch: %d, num_grays: %u\n", bitmap->rows, bitmap->width, bitmap->pitch, bitmap->num_grays);
//
////		printf("left: %d, top: %d\n", slot->bitmap_left, slot->bitmap_top);
//		RenderGlyphQuad(bitmap, X + slot->bitmap_left, Y - slot->bitmap_top);
////		GLuint VertexArray;
////		glGenVertexArrays(1, &VertexArray);
////		glBindVertexArray(VertexArray);
////
////		// x0, y0 -- lower left corner
////		float width = bitmap->width * 2.0f / WindowWidth;
////		float height = bitmap->rows * 2.0f / WindowHeight;
////		float x1 = x0 + width;
////		float y1 = y0 + height;
////		float tx0 = 0.0f;
////		float ty0 = 0.0f;
////		float tx1 = 1.0f;
////		float ty1 = 1.0f;
////		float Vertices[] =
////		{
////			x0, y0, 0.0f, tx0, ty1,
////			x1, y0, 0.0f, tx1, ty1,
////			x0, y1, 0.0f, tx0, ty0,
////	
////			x0, y1, 0.0f, tx0, ty0,
////			x1, y0, 0.0f, tx1, ty1,
////			x1, y1, 0.0f, tx1, ty0,
////		};
////		GLuint VertexBuffer;
////		glGenBuffers(1, &VertexBuffer);
////		glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
////		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
////	
////		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
////		glEnableVertexAttribArray(0);
////	
////		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *) (3 * sizeof(float)));
////		glEnableVertexAttribArray(1);
////
////		glDrawArrays(GL_TRIANGLES, 0, 6);
////
////		glDeleteTextures(1, &myTexture);
////		glDeleteBuffers(1, &VertexBuffer);
////		glDeleteVertexArrays(1, &VertexArray);
//
//		X += slot->advance.x >> 6;
//	}
//}

/*
@try these, skip the intermediary buffer:
void glBufferData(	GLenum 	target,
	GLsizeiptr 	size,
	const GLvoid * 	data,
	GLenum 	usage);

void glBufferSubData(	GLenum 	target,
	GLintptr 	offset,
	GLsizeiptr 	size,
	const GLvoid * 	data);

void glCopyBufferSubData(	GLenum readtarget,
	GLenum writetarget,
	GLintptr readoffset,
	GLintptr writeoffset,
	GLsizeiptr size);

Use an array of buffer objects. If the first buffer is full, create another. Maintain a list. Eventually render them. No copying needed. Need to make multiple render calls I think.

Use one vertex buffer. Copy data directly into the vertex buffer one the fly. If it becomes full then *expand*. Need to copy data but can do away with just one render call.

---

void * glMapBuffer(	GLenum target,
	GLenum access);

GL_WRITE_ONLY

GLboolean glUnmapBuffer(	GLenum target);

---

Need to think about what kind of interface we want to provide. For a text editor we might want a widget that display's the text buffer's contents. What about labels?
*/
void draw_text_fast(const char *Text, int X, int Y, color Color, fontAtlas *FontAtlas, GLuint Shader)
{
double T1, T2; //@ temp.
double t1 = 0.0, t2 = 0.0; //@ temp.
unsigned long C; //@ temp.
unsigned long c; //@ temp.

//@ temp:
double ElapsedInit = 0.0;
double ElapsedLoop = 0.0;
double ElapsedDraw = 0.0;

//@ temp.
double ElapsedChecks = 0.0;
double ElapsedCalc = 0.0;
double ElapsedVertices = 0.0;
double ElapsedOther = 0.0;

unsigned long ElapsedLoopC;
unsigned long ElapsedChecksC;
unsigned long ElapsedCalcC;
unsigned long ElapsedVerticesC;

	int PenX = X;
	int PenY = Y;

	float PixelWidth = 2.0f / WindowWidth;
	float PixelHeight = 2.0f / WindowHeight;

T1 = glfwGetTime();

	int TextLen = strlen(Text);
	printf("TEXT LENGTH: %d\n", TextLen);

	int NumFloatsPerVertex = 8;
	int NumVerticesPerChar = 6;
	int NumFloatsPerChar = NumVerticesPerChar * NumFloatsPerVertex;
	int NumBytesPerChar = NumFloatsPerChar * sizeof(float);
//	int InitiallyAllocatedMemory = (42+6) * 14356 * sizeof(float); // avoid allocating memory, this is the exact amount we need
	int InitiallyAllocatedMemory = (42+6) * 3 * sizeof(float); // avoid allocating memory, this is the exact amount we need
//	int InitiallyAllocatedMemory = (42+6) * TextLen * sizeof(float); // avoid allocating memory, this is the exact amount we need

	GLuint VB;
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, InitiallyAllocatedMemory, 0, GL_DYNAMIC_DRAW);

//	float *Ptr = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	int BufferOffset = 0;
	int VerticesCount = 0;

T2 = glfwGetTime();
ElapsedInit = (T2 - T1) * 1000;

T1 = glfwGetTime();
//C = __rdtsc();

	for (int i = 0; Text[i]; ++i)
	{
		t1 = glfwGetTime();

		if (t2 > 0.0)
		{
			ElapsedOther += (t1 - t2);
		}
		else
		{
			assert(i == 0);
		}

		if(Text[i] == '\n' || PenX > WindowWidth)
		{
//			PenY += 16;
			PenY += 10;
			if(PenY > WindowHeight)
			{
				break;
			}
			PenX = X;

			t2 = glfwGetTime();
			ElapsedChecks += (t2 - t1);
			continue;
		}
		if(Text[i] == '\t')
		{
			PenX += 24;
			continue;
		}

		t2 = glfwGetTime();
		ElapsedChecks += (t2 - t1);

		int Index = Text[i] - ' ';
		glyphInfo Glyph = FontAtlas->Glyphs[Index];

		int X = PenX + Glyph.OffsetLeft;
		int Y = PenY - Glyph.OffsetTop;
		int Width = Glyph.Width;
		int Height = Glyph.Height;

		float _X = X * PixelWidth - 1.0f;
		float _Y = 1.0f - Y * PixelHeight;
		float _Width = Width * PixelWidth;
		float _Height = Height * PixelHeight;

		float X0 = _X;
		float X1 = _X + _Width;
		float Y0 = _Y;
		float Y1 = _Y - _Height;
//		printf("[%c] x0: %f, x1: %f, y0: %f, y1: %f\n", Text[i], X0, X1, Y0, Y1);
//		printf("[%c] tx0: %f, tx1: %f, ty0: %f, ty1: %f\n",
//			Text[i], Glyph.TexX0, Glyph.TexX1, Glyph.TexY0, Glyph.TexY1);

		t1 = glfwGetTime();
		ElapsedCalc += (t1 - t2);

		float Vertices[] =
		{
			// upper-left
			X0, Y0, Glyph.TexX0, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A,
	
			// upper-right
			X1, Y0, Glyph.TexX1, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A,
	
			// lower-left
			X0, Y1, Glyph.TexX0, Glyph.TexY0, Color.R, Color.G, Color.B, Color.A,
	
			// lower-left
			X0, Y1, Glyph.TexX0, Glyph.TexY0, Color.R, Color.G, Color.B, Color.A,
	
			// lower-right
			X1, Y1, Glyph.TexX1, Glyph.TexY0, Color.R, Color.G, Color.B, Color.A,
	
			// upper-right
			X1, Y0, Glyph.TexX1, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A,
		};
//		int NumBytesPerChar = NumFloatsPerChar * sizeof(float);
		glBufferSubData(GL_ARRAY_BUFFER, BufferOffset, NumBytesPerChar, Vertices);
		BufferOffset += NumBytesPerChar;

//		// upper-left
//		Ptr[0]=X0; Ptr[1]=Y0; Ptr[2]=Glyph.TexX0; Ptr[3]=Glyph.TexY1; Ptr[4]=Color.R; Ptr[5]=Color.G; Ptr[6]=Color.B; Ptr[7]=Color.A;
//
//		// upper-right
//		Ptr[8]=X1; Ptr[9]=Y0; Ptr[10]=Glyph.TexX1; Ptr[11]=Glyph.TexY1; Ptr[12]=Color.R; Ptr[13]=Color.G; Ptr[14]=Color.B; Ptr[15]=Color.A;
//
//		// lower-left
//		Ptr[16]=X0; Ptr[17]=Y1; Ptr[18]=Glyph.TexX0; Ptr[19]=Glyph.TexY0; Ptr[20]=Color.R; Ptr[21]=Color.G; Ptr[22]=Color.B; Ptr[23]=Color.A;
//
//		// lower-left
//		Ptr[24]=X0; Ptr[25]=Y1; Ptr[26]=Glyph.TexX0; Ptr[27]=Glyph.TexY0; Ptr[28]=Color.R; Ptr[29]=Color.G; Ptr[30]=Color.B; Ptr[31]=Color.A;
//
//		// lower-right
//		Ptr[32]=X1; Ptr[33]=Y1; Ptr[34]=Glyph.TexX1; Ptr[35]=Glyph.TexY0; Ptr[36]=Color.R; Ptr[37]=Color.G; Ptr[38]=Color.B;  Ptr[39]=Color.A;
//
//		// upper-right
//		Ptr[40]=X1; Ptr[41]=Y0; Ptr[42]=Glyph.TexX1; Ptr[43]=Glyph.TexY1; Ptr[44]=Color.R; Ptr[45]=Color.G; Ptr[46]=Color.B; Ptr[47]=Color.A;
//
////		memcpy(Ptr, Vertices, sizeof(Vertices));
//		Ptr += NumFloatsPerChar;
		VerticesCount += NumVerticesPerChar;
		
		t2 = glfwGetTime();
		ElapsedVertices += (t2 - t1);

//		t1 = glfwGetTime();

		PenX += Glyph.AdvanceX;
	}

T2 = glfwGetTime();
//ElapsedLoopC = __rdtsc() - C;
ElapsedLoop = (T2 - T1);

//	printf("\t\tElapsedCalculations: %f, ElapsedAddingVertices: %f\n", ElapsedCalculations, ElapsedAddingVertices);

T1 = glfwGetTime();

//	glUnmapBuffer(GL_ARRAY_BUFFER);


	// Do OpenGL rendering stuff.

	glBindTexture(GL_TEXTURE_2D, FontAtlas->TextureID);

	glUseProgram(Shader);

//	GLuint VertexArray, VertexBuffer;
	GLuint VertexArray;
	glGenVertexArrays(1, &VertexArray);
//	glGenBuffers(1, &VertexBuffer);

	glBindVertexArray(VertexArray);
//	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *) (sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *) (sizeof(float) * 4));
	glEnableVertexAttribArray(2);

//	glBufferData(GL_ARRAY_BUFFER, VerticesToBeRendered.Count * sizeof(float), VerticesToBeRendered.Data, GL_STATIC_DRAW);
//	glBufferData(GL_ARRAY_BUFFER, VerticesToBeRendered.Count * sizeof(float), VerticesToBeRendered.Data, GL_STREAM_DRAW);

//	int NumVertices = VerticesToBeRendered.Count / 7;
//	int NumVertices = BufferOffset / (NumFloatsPerVertex * sizeof(float));
	int NumVertices = VerticesCount;
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

//	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &VB);
	glDeleteVertexArrays(1, &VertexArray);
//	free(VerticesToBeRendered.Data);

T2 = glfwGetTime();
ElapsedDraw = (T2 - T1) * 1000;

printf("\t\tElapsedInit: %f ms\n", ElapsedInit);
printf("\t\tElapsedLoop: %f ms\n", ElapsedLoop * 1000);
printf("\t\t\tElapsedChecks: %f ms\n", ElapsedChecks * 1000);
printf("\t\t\tElapsedCalc: %f ms\n", ElapsedCalc * 1000);
printf("\t\t\tElapsedVertices: %f ms\n", ElapsedVertices * 1000);
printf("\t\t\tElapsedOther: %f ms\n", ElapsedOther * 1000);
printf("\t\tElapsedDraw: %f ms\n", ElapsedDraw);
}

void draw_text(const char *Text, int X, int Y, color Color, fontAtlas *FontAtlas, GLuint Shader)
{
	int PenX = X;
	int PenY = Y;

	float PixelWidth = 2.0f / WindowWidth;
	float PixelHeight = 2.0f / WindowHeight;

//	int InitiallyAllocatedMemory = 42 * 14356; // avoid allocating memory, this is the exact amount we need
//	array<float> VerticesToBeRendered; ArrayInit(&VerticesToBeRendered, InitiallyAllocatedMemory);
	array<float> VerticesToBeRendered; ArrayInit(&VerticesToBeRendered, 1024);

//	int CharCount = 0;
	for (int i = 0; Text[i]; ++i)
	{
		if(Text[i] == '\n' || PenX > WindowWidth)
		{
//			PenY += 16;
			PenY += 10;
			if(PenY > WindowHeight)
			{
				break;
			}
			PenX = X;
			continue; //@ I think we are skipping a character if wrapping text
		}
		if(Text[i] == '\t')
		{
			PenX += 24;
			continue;
		}

		int Index = Text[i] - ' ';
		glyphInfo Glyph = FontAtlas->Glyphs[Index];

		int X = PenX + Glyph.OffsetLeft;
		int Y = PenY - Glyph.OffsetTop;
		int Width = Glyph.Width;
		int Height = Glyph.Height;

		float _X = X * PixelWidth - 1.0f;
		float _Y = 1.0f - Y * PixelHeight;
		float _Width = Width * PixelWidth;
		float _Height = Height * PixelHeight;

		float X0 = _X;
		float X1 = _X + _Width;
		float Y0 = _Y;
		float Y1 = _Y - _Height;
//		printf("[%c] x0: %f, x1: %f, y0: %f, y1: %f\n", Text[i], X0, X1, Y0, Y1);
//		printf("[%c] tx0: %f, tx1: %f, ty0: %f, ty1: %f\n",
//			Text[i], Glyph.TexX0, Glyph.TexX1, Glyph.TexY0, Glyph.TexY1);
		
		float Vertices[] =
		{
			// upper-left
			X0, Y0, Glyph.TexX0, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A,
	
			// upper-right
			X1, Y0, Glyph.TexX1, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A,
	
			// lower-left
			X0, Y1, Glyph.TexX0, Glyph.TexY0, Color.R, Color.G, Color.B, Color.A,
	
			// lower-left
			X0, Y1, Glyph.TexX0, Glyph.TexY0, Color.R, Color.G, Color.B, Color.A,
	
			// lower-right
			X1, Y1, Glyph.TexX1, Glyph.TexY0, Color.R, Color.G, Color.B, Color.A,
	
			// upper-right
			X1, Y0, Glyph.TexX1, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A,
		};
		unsigned int NumItems = sizeof(Vertices) / sizeof(float);
		ArrayAddFast(&VerticesToBeRendered, Vertices, NumItems);
//		ArrayAdd(&VerticesToBeRendered, Vertices, NumItems);
		
//		CharCount += 1;

		PenX += Glyph.AdvanceX;
	}
//	printf("CharCount: %d\n", CharCount);

	// Do OpenGL rendering stuff.

	glBindTexture(GL_TEXTURE_2D, FontAtlas->TextureID);

	glUseProgram(Shader);

	GLuint VertexArray, VertexBuffer;
	glGenVertexArrays(1, &VertexArray);
	glGenBuffers(1, &VertexBuffer);

	glBindVertexArray(VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *) (sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *) (sizeof(float) * 4));
	glEnableVertexAttribArray(2);

	glBufferData(GL_ARRAY_BUFFER, VerticesToBeRendered.Count * sizeof(float), VerticesToBeRendered.Data, GL_STATIC_DRAW);

	int NumVertices = VerticesToBeRendered.Count / 8;
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	glDeleteBuffers(1, &VertexBuffer);
	glDeleteVertexArrays(1, &VertexArray);
	free(VerticesToBeRendered.Data);
}

//@ sometimes it does couple of fast frames and then 1 really slow one
void draw_text_geo(const char *Text, int X, int Y, color Color, fontAtlas *FontAtlas, GLuint Shader)
{
	double Time1, Time2, Elapsed; //@ temp.

	int PenX = X;
	int PenY = Y; // baseline

	float PixelWidth = 2.0f / WindowWidth;
	float PixelHeight = 2.0f / WindowHeight;

	int FloatsInVertex = 12;
	int InitiallyAllocatedMemory = FloatsInVertex * 14356; // avoid allocating memory, this is the exact amount we need
	array<float> VerticesToBeRendered; ArrayInit(&VerticesToBeRendered, InitiallyAllocatedMemory);

	Time1 = glfwGetTime();

	for (int i = 0; Text[i]; ++i)
	{
		if(Text[i] == '\n' || PenX > WindowWidth)
		{
//			PenY += 16;
			PenY += 10;
			if(PenY > WindowHeight)
			{
				break;
			}
			PenX = X;
			continue;
		}
		if(Text[i] == '\t')
		{
			PenX += 24;
			continue;
		}

		int Index = Text[i] - ' ';
		glyphInfo Glyph = FontAtlas->Glyphs[Index];

		float X = (float) (PenX + Glyph.OffsetLeft);
		float Y = (float) (PenY - Glyph.OffsetTop);
		float Width = (float) Glyph.Width;
		float Height = (float) Glyph.Height;

		float Vertices[] =
		{
//			X0, Y0, X1, Y1, Glyph.TexX0, Glyph.TexY0, Glyph.TexX1, Glyph.TexY1, Color.R, Color.G, Color.B
			X, Y,
			X + Width, Y + Height,
			Glyph.TexX0, Glyph.TexY0, Glyph.TexX1, Glyph.TexY1, Color.R, Color.G, Color.B, Color.A
		};
		unsigned int NumVertices = sizeof(Vertices) / sizeof(float);
//		ArrayAdd(&VerticesToBeRendered, Vertices, NumVertices);
		ArrayAddFast(&VerticesToBeRendered, Vertices, NumVertices);

		PenX += Glyph.AdvanceX;
	}

	Time2 = glfwGetTime();
	printf("\t\tloop: %f ms\n", 1000 * (Time2 - Time1));

	// Do OpenGL rendering stuff.

	glUseProgram(Shader);
//	pass_to_shader(Shader, "MyUniform", 1.0f);
	pass_to_shader(Shader, "PixelWidth", PixelWidth);
	pass_to_shader(Shader, "PixelHeight", PixelHeight);

	glBindTexture(GL_TEXTURE_2D, FontAtlas->TextureID);

	Time1 = glfwGetTime();

	GLuint VertexArray, VertexBuffer;
	glGenVertexArrays(1, &VertexArray);
	glGenBuffers(1, &VertexBuffer);

	glBindVertexArray(VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *)0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, FloatsInVertex * sizeof(float), (const void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, FloatsInVertex * sizeof(float), (const void *) (sizeof(float) * 2));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FloatsInVertex * sizeof(float), (const void *) (sizeof(float) * 4));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, FloatsInVertex * sizeof(float), (const void *) (sizeof(float) * 6));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, FloatsInVertex * sizeof(float), (const void *) (sizeof(float) * 8));

	glBufferData(GL_ARRAY_BUFFER, VerticesToBeRendered.Count * sizeof(float), VerticesToBeRendered.Data, GL_STATIC_DRAW);

	Time2 = glfwGetTime();
	printf("\t\tvaovbo creation: %f ms\n", 1000 * (Time2 - Time1));

	int NumVertices = VerticesToBeRendered.Count / FloatsInVertex; assert((VerticesToBeRendered.Count % FloatsInVertex) == 0);
//	printf("NumVertices: %d\n", NumVertices);
	Time1 = glfwGetTime();
	glDrawArrays(GL_POINTS, 0, NumVertices);
	Time2 = glfwGetTime();
	printf("\t\tglDrawArrays: %f ms\n", 1000 * (Time2 - Time1));

	glDeleteBuffers(1, &VertexBuffer);
	glDeleteVertexArrays(1, &VertexArray);
	free(VerticesToBeRendered.Data);
}

void MakeFontAtlas(fontAtlas *FontAtlas, const char *FontFile, int FontSize)
{
	FT_Face Face;
	FT_Error Error = FT_New_Face(library, FontFile, 0, &Face);
	if(Error != FT_Err_Ok)
	{
		fprintf(stderr, "MakeFontAtlas: Error!\n");
		exit(1);
	}
//	FT_Set_Char_Size(face, 0, 16*64, 300, 300);
	FT_Set_Pixel_Sizes(Face, 0, FontSize);

//	printf("bitmap_left: %d, bitmap_top: %d\nwidth: %u, rows: %u, pitch: %u\n",
//			GlyphSlot->bitmap_left, GlyphSlot->bitmap_top,
//			GlyphSlot->bitmap.width, GlyphSlot->bitmap.rows, GlyphSlot->bitmap.pitch);

	int Width = 0;
	int Height = 0;
	const char *Glyphs = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
//	const char *Glyphs = "abc";
	for(int i = 0; Glyphs[i]; ++i)
	{
		unsigned int Index = FT_Get_Char_Index(Face, Glyphs[i]);
		FT_Load_Glyph(Face, Index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);

		FT_GlyphSlot GlyphSlot = Face->glyph;
//		printf("%c: width: %d, height: %d\n", Glyphs[i], GlyphSlot->bitmap.width, GlyphSlot->bitmap.rows);

		Width += GlyphSlot->bitmap.width;
		Height = (GlyphSlot->bitmap.rows > Height) ? (GlyphSlot->bitmap.rows) : (Height);
	}
//	printf("Width: %d, Height: %d\n", Width, Height);

	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
//	printf("TextureID: %u\n", TextureID);

	// What to do when texture coordinates are outside of the texture:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// What to do when the texture is minified/magnified:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(
		GL_TEXTURE_2D,            // target
		0,                        // level
		GL_RED,                   // internal format
		Width,                    // width
		Height,                   // height
		0,                        // must be 0
		GL_RED,                   // format
		GL_UNSIGNED_BYTE,         // type
		NULL                      // data
		);

	GLint XOffset = 0;
	for(int i = 0; Glyphs[i]; ++i)
	{
		unsigned int CharIndex = FT_Get_Char_Index(Face, Glyphs[i]);
		FT_Load_Glyph(Face, CharIndex, FT_LOAD_DEFAULT);
		FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);

		FT_GlyphSlot GlyphSlot = Face->glyph;

//		float _Width = GlyphSlot->bitmap.width / Width;
//		float _Height = GlyphSlot->bitmap.rows / Height;

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			XOffset,                 // xoffset
			Height - GlyphSlot->bitmap.rows,                       // yoffset
//			0,                       // yoffset
			GlyphSlot->bitmap.width, // width
			GlyphSlot->bitmap.rows,  // height
			GL_RED,                  // format
			GL_UNSIGNED_BYTE,        // type
			GlyphSlot->bitmap.buffer // data
		);


		// store glyph info so that we can later find the glyph in the texture

		int Index = Glyphs[i] - ' ';

		FontAtlas->Glyphs[Index].Width = GlyphSlot->bitmap.width;
		FontAtlas->Glyphs[Index].Height = GlyphSlot->bitmap.rows;

		float TexX0 = ((float) XOffset) / Width;
		float TexX1 = TexX0 + (GlyphSlot->bitmap.width / ((float) Width));
		float TexY0 = 1.0f;
		float TexY1 = 1.0f - (GlyphSlot->bitmap.rows / ((float) Height));
		FontAtlas->Glyphs[Index].TexX0 = TexX0;
		FontAtlas->Glyphs[Index].TexX1 = TexX1;
		FontAtlas->Glyphs[Index].TexY0 = TexY0;
		FontAtlas->Glyphs[Index].TexY1 = TexY1;

		FontAtlas->Glyphs[Index].OffsetLeft = GlyphSlot->bitmap_left;
		FontAtlas->Glyphs[Index].OffsetTop = GlyphSlot->bitmap_top;
		FontAtlas->Glyphs[Index].AdvanceX = GlyphSlot->advance.x >> 6;
//		printf("%c: %d\n", Index + ' ', FontAtlas->Glyphs[Index].AdvanceX);

		XOffset += GlyphSlot->bitmap.width;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FontAtlas->TextureID = TextureID;
	FontAtlas->Width = Width;
	FontAtlas->Height = Height;
}

void RenderFontAtlas(fontAtlas *FontAtlas)
{
	glBindTexture(GL_TEXTURE_2D, FontAtlas->TextureID);

	int _X = 0;
	int _Y = 0;
	int _Width = FontAtlas->Width;
	int _Height = FontAtlas->Height;

	float PixelWidth = 2.0f / WindowWidth;
	float PixelHeight = 2.0f / WindowHeight;

	float X = _X * PixelWidth - 1.0f;
	float Y = 1.0f - _Y * PixelHeight;
	float Width = _Width * PixelWidth;
	float Height = _Height * PixelHeight;

	float X0 = X;
	float X1 = X + Width;
	float Y0 = Y;
	float Y1 = Y - Height;

//	printf("x0: %f, y0: %f, x1: %f, y1: %f\n", X0, Y0, X1, Y1);

	float Vertices[] =
	{
		// upper-left
		X0, Y0, 0.0f, 0.0f, 1.0f,

		// upper-right
		X1, Y0, 0.0f, 1.0f, 1.0f,

		// lower-left
		X0, Y1, 0.0f, 0.0f, 0.0f,

		// lower-left
		X0, Y1, 0.0f, 0.0f, 0.0f,

		// lower-right
		X1, Y1, 0.0f, 1.0f, 0.0f,

		// upper-right
		X1, Y0, 0.0f, 1.0f, 1.0f,
	};


 	// Do OpenGL rendering stuff.

	GLuint VertexArray, VertexBuffer;
	glGenVertexArrays(1, &VertexArray);
	glGenBuffers(1, &VertexBuffer);

	glBindVertexArray(VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *) (sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDeleteBuffers(1, &VertexBuffer);
	glDeleteVertexArrays(1, &VertexArray);
}

void OnWindowResized(GLFWwindow *Window, int Width, int Height)
{
	glViewport(0, 0, Width, Height);
	WindowWidth = Width;
	WindowHeight = Height;
}

/* Returns time elapsed since last call (in seconds) */
double get_elapsed_time_since_last_call()
{
	static double Previous = 0.0; // initialized to 0.0 first time around

	double Now = glfwGetTime();
	double Elapsed = Now - Previous;
	Previous = Now;

	return Elapsed;
}

/*
Mean = SUM(Values[i]) / NumValues
*/
double calculate_mean(array<double> *Values)
{
	double Sum = 0.0;
	for(int i = 0; i < Values->Count; ++i)
	{
		Sum += Values->Data[i];
	}
	return (Sum / Values->Count);
}

/*
Mean Deviation = SUM(ABS(Values[i] - Mean)) / NumValues

For a deeper dive: Central Limit Theorem, Normal Distribution, Standard Deviation, Variance etc.
*/
double calculate_mean_deviation(array<double> *Values)
{
	double Mean = calculate_mean(Values);

	double SumDeviations = 0.0;
	for(int i = 0; i < Values->Count; ++i)
	{
		double Difference = Values->Data[i] - Mean;
		double AbsDifference = (Difference < 0) ? -Difference : Difference;
		SumDeviations += AbsDifference;
	}

	return (SumDeviations / Values->Count);
}

int main(void)
{
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor *Monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *VideoMode = glfwGetVideoMode(Monitor);
	printf("screen width: %d, screen height: %d, screen refresh rate: %d\n", VideoMode->width, VideoMode->height, VideoMode->refreshRate);
	int RefreshRate = VideoMode->refreshRate;

// FULLSCREEN WINDOW:
//	WindowWidth = VideoMode->width;
//	WindowHeight = VideoMode->height;
//	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Hello World", glfwGetPrimaryMonitor(), NULL);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE); // make window maximized
	GLFWwindow* window = glfwCreateWindow(1, 1, "Hello World", NULL, NULL);
	if (!window)
	{
		fprintf(stderr, "error: glfwCreateWindow()\n");
		glfwTerminate();
		return -1;
	}

	// Dimensions given by glfwGetWindowSize() might not be in pixels
	glfwGetFramebufferSize(window, &WindowWidth, &WindowHeight);
	printf("window width: %d, window height: %d\n", WindowWidth, WindowHeight);

	glfwSetFramebufferSizeCallback(window, OnWindowResized);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // VSync off

	gladLoadGL();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Error Error = FT_Init_FreeType(&library);
	if(Error != FT_Err_Ok)
	{
		fprintf(stderr, "Error!\n");
		return 0;
	}

//	const int FontSize = 64;
	const int FontSize = 13;
//	const int FontSize = 16;
//	const char *FontFile = "/home/eero/.local/share/fonts/DroidSansMono.ttf";
	const char *FontFile = "/home/eero/.local/share/fonts/YaHei.Consolas.1.12.ttf";
	fontAtlas ConsolasFont;
	MakeFontAtlas(&ConsolasFont, FontFile, FontSize);

	fontAtlas ComicShannsFont;
	MakeFontAtlas(&ComicShannsFont, "/home/eero/.local/share/fonts/ComicShanns-Regular.ttf", 64);

	fontAtlas RobotoFont;
//	MakeFontAtlas(&RobotoFont, "/home/eero/.local/share/fonts/Roboto-Regular.ttf", 32);
	MakeFontAtlas(&RobotoFont, "/home/eero/Desktop/novem___.ttf", 64);

//	GLuint Shader = CreateShader("vertex-shader", "fragment-shader");
//	GLuint Shader = CompileShaders("./shaders");
	GLuint ImageShader = make_texture_shader();
	GLuint Shader = make_text_shader();
	GLuint ShaderGeo = make_text_shader_geometry_style();

	char *FileContents;
	if(!ReadTextFile("/home/eero/all/test/Lorem-ipsum-dolor-sit-amet.txt", &FileContents))
//	if(!ReadTextFile("main.cpp", &FileContents))
	{
		fprintf(stderr, "Failed to read the file!\n");
		return 0;
	}
//	printf("%s\n", FileContents);

	int NthFrame = 0;
	array<double> FrameTimings; ArrayInit(&FrameTimings);
	while (!glfwWindowShouldClose(window))
	{
		double T1, T2; //@temp
		NthFrame += 1;

		double ElapsedTimeSecs = get_elapsed_time_since_last_call();
		double ElapsedTimeMs = ElapsedTimeSecs * 1000;
		printf("#%d: %f ms\n", NthFrame, ElapsedTimeMs);

		// Do some (very simple) statistics on frame timings.
		if (NthFrame > 1)
		{
			ArrayAdd(&FrameTimings, ElapsedTimeMs);
//			if(NthFrame == 5000) break;
			if(NthFrame == 1000) break;
//			if(NthFrame == 100) break;
		}

		T1 = glfwGetTime();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		T2 = glfwGetTime();
		printf("\tdraw background: %f ms\n", (T2 - T1) * 1000);

//		T1 = glfwGetTime();
//		unsigned long C1 = __rdtsc();
//		draw_text_fast(FileContents, 10, 10, {0.0f, 0.0f, 1.0f, 1.0f}, &ConsolasFont, Shader);
//		T2 = glfwGetTime();
//		unsigned long C2 = __rdtsc();
//		printf("\tDrawText: %f ms\n", (T2 - T1) * 1000);
//		printf("\tDrawText: %lu cycles\n", (C2 - C1));
//		draw_text_fast("Hello, world!", 10, 300, {1.0f, 1.0f, 1.0f, 0.7f}, &ComicShannsFont, Shader);
//		draw_text_fast("Is this monospaced?", 50, 50, {1.0f, 0.0f, 0.0f, 1.0f}, &RobotoFont, Shader);

//		T1 = glfwGetTime();
//		draw_text(FileContents, 10, 10, {0.0f, 0.0f, 1.0f, 1.0f}, &ConsolasFont, Shader);
////		T2 = glfwGetTime();
////		printf("\tDrawText: %f ms\n", (T2 - T1) * 1000);
//		draw_text("Hello, world!", 10, 300, {1.0f, 1.0f, 1.0f, 0.7f}, &ComicShannsFont, Shader);
//		draw_text("Is this monospaced?", 50, 50, {1.0f, 0.0f, 0.0f, 1.0f}, &RobotoFont, Shader);

		T1 = glfwGetTime();
		draw_text_geo(FileContents, 10, 10, {0.0f, 0.0f, 1.0f, 1.0f}, &ConsolasFont, ShaderGeo);
		T2 = glfwGetTime();
		printf("\tdraw_text_geo: %f\n", (T2 - T1) * 1000);
		draw_text_geo("Hello, world!", 10, 300, {1.0f, 1.0f, 1.0f, 0.3f}, &ComicShannsFont, ShaderGeo);
		draw_text_geo("Is this monospaced?", 50, 50, {1.0f, 0.0f, 0.0f, 1.0f}, &RobotoFont, ShaderGeo);

//		glUseProgram(ImageShader);
//		RenderFontAtlas(&ConsolasFont);
//		RenderFontAtlas(&ComicShannsFont);

		/* Swap front and back buffers */
		T1 = glfwGetTime();
		glfwSwapBuffers(window);
		T2 = glfwGetTime();
		printf("\tglfwSwapBuffers: %f ms\n", (T2 - T1) * 1000);

		/* Poll for and process events */
		T1 = glfwGetTime();
		glfwPollEvents();
		T2 = glfwGetTime();
		printf("\tglfwPollEvents: %f ms\n", (T2 - T1) * 1000);
	}

	// Most frames are counted as "missed frames" when vsync is on.
	// So clearly the refresh rate is lower than what is reported by glfw
	// unless we are actually missing all these frames (which is probably not happening)
	//@ Am I not understanding something here? Maybe look into it a bit more.
	// Rather: why is the vsynced framerate so inconsistent?
	int NumMissedFrames = 0;
	double MaxFrameTime = 1000.0 / RefreshRate;
	printf("MaxFrameTime: %f\n", MaxFrameTime);
	for(int i = 0; i < FrameTimings.Count; ++i)
	{
		if (FrameTimings.Data[i] > MaxFrameTime) NumMissedFrames += 1;
	}
	printf("NumMissedFrames: %d\n", NumMissedFrames);

	double MeanFrameTime = calculate_mean(&FrameTimings);
	double MeanDeviation = calculate_mean_deviation(&FrameTimings);
	printf("average frame time: %f ms\n", MeanFrameTime);
	printf("average frame time deviation: %f ms\n", MeanDeviation);
	printf("(%d frames)\n", FrameTimings.Count);

	glfwTerminate();
	return 0;
}