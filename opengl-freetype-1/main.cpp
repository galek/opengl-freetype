/*
 * main.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: ifth6590
 */
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <GL/gl.h>

#include <string>
#include <cstring>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

unsigned int* GlyphTexture(const FT_Bitmap& bitmap, const unsigned int& color)
{
	unsigned int* buffer = new unsigned int[bitmap.width * bitmap.rows*4];
	for(int y = 0 ; y< bitmap.rows; y++)
	{
		for (int x = 0 ; x < bitmap.width; x++)
		{
			unsigned int pixel = (color & 0xffffff00);
			unsigned int alpha = bitmap.buffer[(y * bitmap.pitch) + x];
			pixel |= alpha;
			buffer[(y*bitmap.width)+x] = pixel;
		}
	}
	return buffer;
}

void DrawText(const std::wstring& text,
		const int& x_start,
		const int& baseline,
		const unsigned int& color,
		const FT_Face& face) {


	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glTranslatef(x_start, baseline, 0);

	for (unsigned int i = 0; i < text.length(); i++) {
		FT_Load_Char(face, text[i], FT_LOAD_RENDER);

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(	GL_TEXTURE_2D, texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		unsigned int* buffer = GlyphTexture(face->glyph->bitmap, color);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			face->glyph->bitmap.pitch,
			face->glyph->bitmap.rows,
			0,
			GL_RGBA,
			GL_UNSIGNED_INT_8_8_8_8,
			buffer);

		delete[] buffer;

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texture);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glMatrixMode(GL_MODELVIEW);

		glTranslatef(face->glyph->metrics.horiBearingX /64.0f,
					face->glyph->metrics.horiBearingY /64.0f,
					0);

		glBegin(GL_TRIANGLE_FAN);
		  glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0 );
		  glTexCoord2f(1.0f, 0.0f); glVertex2f(face->glyph->bitmap.pitch, 0);
		  glTexCoord2f(1.0f, 1.0f); glVertex2f(face->glyph->bitmap.pitch, -face->glyph->bitmap.rows);
		  glTexCoord2f(0.0f, 1.0f); glVertex2f(0, -face->glyph->bitmap.rows);
		glEnd();

		glDeleteTextures(1, &texture);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glTranslatef(face->glyph->metrics.horiAdvance / 64.0f, 0, 0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** args)
{

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;

	window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 800, 600,SDL_WINDOW_OPENGL );

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, "./font/ThaiSansNeue-Black.otf", 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 64);

	glViewport(0,0,800,600);

	glMatrixMode(GL_PROJECTION|GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0.0, 800, 0.0, 600, 1.0f, 0.0f);


	while(true){
		SDL_Event event;
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
			break;

		int before = SDL_GetTicks();
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT );

		DrawText(L"ABCDEFG abcdefh 1234567",
				50,
				200,
				0xffffffff,
				face);

	    glFlush();

	    std::cout<<1000/( SDL_GetTicks() - before) <<"FPS"<<std::endl;

		SDL_GL_SwapWindow(window);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
