#ifndef _TEXTURE__H_
#define _TEXTURE__H_

#include <stdint.h>
#include <string>

class Texture
{
private:
	uint32_t _rendererID;
	std::string _filepath;
	uint8_t* _localBuffer;
	int _width;
	int _height;
	int _bpp;

public:
	Texture(std::string filepath);
	~Texture(void);

	void bind(uint32_t slot = 0) const;
	void unbind(void) const;

};

#endif