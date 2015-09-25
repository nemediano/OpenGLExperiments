
#include <iostream>
#include "TextureHandler.h"

namespace texture {
	
	TextureHandler::TextureHandler() : m_data(nullptr), m_texture_id(0), m_width(0), m_height(0) {
		glGenTextures(1, &m_texture_id);
	}

	bool TextureHandler::load_texture(std::wstring file_name) {
		m_devil_image_id = ilGenImage();
		ilBindImage(m_devil_image_id);
		ilLoadImage(file_name.c_str());
		get_DevIL_error();
		m_width = ilGetInteger(IL_IMAGE_WIDTH);
		m_height= ilGetInteger(IL_IMAGE_HEIGHT);
		ILint format = ilGetInteger(IL_IMAGE_FORMAT);
		ILint type = ilGetInteger(IL_IMAGE_TYPE);
		ilLoadImage(file_name.c_str());
		get_DevIL_error();
		//we want to load unsigned char RGBA images
		m_data = new unsigned char[m_width * m_height * 4];
		ilCopyPixels(0, 0, 0, m_width, m_height, 1, format, type, m_data);
		get_DevIL_error();
		ilDeleteImage(m_devil_image_id);
		send_to_gpu();
		return true;
	}

	void TextureHandler::bind() {
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
	}

	int TextureHandler::get_width() {
		return m_width;
	}

	int TextureHandler::get_height() {
		return m_height;
	}

	unsigned char* TextureHandler::get_texture_data() {
		return m_data;
	}

	TextureHandler::TextureHandler(const TextureHandler& copy) {
		//Allocate new memory
		m_data = new unsigned char[copy.m_width * copy.m_height * 4];
		//Copy the data
		std::copy(copy.m_data, copy.m_data + (copy.m_width * copy.m_height * 4), m_data);

		m_devil_image_id = copy.m_devil_image_id;
		m_height = copy.m_height;
		m_width = copy.m_width;
	}

	TextureHandler TextureHandler::operator= (const TextureHandler& rhs) {
		//Prevent self assignation
		if (this == &rhs) {
			return *this;
		}
		//Release resources
		delete [] m_data;
		//Allocate new memorry
		m_data = new unsigned char[rhs.m_width * rhs.m_height * 4];
		//Copy the data
		std::copy(rhs.m_data, rhs.m_data + (rhs.m_width * rhs.m_height * 4), m_data);
		
		m_devil_image_id = rhs.m_devil_image_id;
		m_height = rhs.m_height;
		m_width = rhs.m_width;

		return *this;
	}

	TextureHandler::~TextureHandler() {
		//Release the sharks!!!
		if (m_data) {
			delete [] m_data;
			m_data = nullptr;
		}
	}

	bool TextureHandler::send_to_gpu() {
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
		return get_DevIL_error();
	}

	bool TextureHandler::get_DevIL_error() {
		ILenum status = ilGetError();
		bool error = false;
		switch (status) {
		case IL_COULD_NOT_OPEN_FILE:
			std::cerr << "Cannot find or use the file" << std::endl;
			ilBindImage(0);
			ilDeleteImage(m_devil_image_id);
			error = true;
			break;

		case IL_ILLEGAL_OPERATION:
			std::cerr << "Library has not been initializated" << std::endl;
			ilBindImage(0);
			ilDeleteImage(m_devil_image_id);
			error = true;
			break;

		case IL_INVALID_EXTENSION:
			std::cerr << "The header and extension of the image don't match" << std::endl;
			ilBindImage(0);
			ilDeleteImage(m_devil_image_id);
			error = true;
			break;

		case IL_INVALID_PARAM:
			std::cerr << "File name is not valid!!" << std::endl;
			ilBindImage(0);
			ilDeleteImage(m_devil_image_id);
			error = true;
			break;

		case IL_OUT_OF_MEMORY:
			std::cerr << "Could not allocate memmory for image" << std::endl;
			ilBindImage(0);
			ilDeleteImage(m_devil_image_id);
			error = true;
			break;

		default:
			break;
		}
		return error;
	}

}