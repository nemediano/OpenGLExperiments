#ifndef TEXTURE_HANDLER_H_
#define TEXTURE_HANDLER_H_

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <IL/il.h>
#include <IL/ilu.h>

namespace texture {

	class TextureHandler {
	public:
		TextureHandler();
		//Interface
		bool load_texture(std::wstring file_name);
		void bind();
		//Getters
		int get_width();
		int get_height();
		unsigned char* get_texture_data();
		//To comply with the rule of three
		TextureHandler(const TextureHandler& copy);
		TextureHandler operator= (const TextureHandler& rhs);
		virtual ~TextureHandler();
	protected:
		unsigned int m_width;
		unsigned int m_height;
		unsigned char* m_data;
		GLuint m_texture_id;
	private:
		bool send_to_gpu();
		bool get_DevIL_error();
		ILuint m_devil_image_id;
	};

}

#endif