// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


#include "Manager.h"

#define GMEXPORT extern "C" __declspec(dllexport)

Manager* manager;

GMEXPORT double shader_set_device(void* _device, void* _deviceContext)
{
	manager = new Manager((ID3D11Device*)_device, (ID3D11DeviceContext*)_deviceContext);
	return 1.0;
}

GMEXPORT double shader_set_texture(double _id, double _slot)
{
	//Set the texture to the active vertex shader.
	manager->setTexture(_id, _slot);
	return 1.0;
}

GMEXPORT double shader_delete_texture(double _id)
{
	manager->deleteTexture(_id);
	return 1.0;
}

GMEXPORT double shader_free_slot(double _slot)
{
	manager->clearResources(_slot);
	return 1.0;
}

GMEXPORT double shader_update_texture(double _id)
{
	//Updates the SRV and texture. Done in a seperate function, instead of when setting a texture value, so only one "refresh" is needed for all the new data.
	//manager->updateTexture(_id);
	manager->recreateTexture(_id); //Alternative method.
	return 1.0;
}

//Set a value in the texture data array. Literally just a massive 1D array of floats.
GMEXPORT double shader_set_texture_value(double _id, double _index, double _value)
{
	manager->setTextureValue(_id, _index, _value);
	return 1.0;
}

//Same as above, but does some fancy pants stuff to make it how you'd imagine a 2d array (i.e. uses x and y).
GMEXPORT double shader_set_texture_pixel(double _id, double _x, double _y, double _r, double _g, double _b, double _a)
{
	manager->setPixelColour(_id, _x, _y, _r, _g, _b, _a);
	return 1.0;
}

GMEXPORT double shader_get_texture_value(double _id, double _index)
{
	return manager->getTextureValue(_id, _index);
}

GMEXPORT double shader_get_texture_pixel(double _id, double _x, double _y, double _channel)
{
	return manager->getPixelValue(_id, _x, _y, _channel);
}

GMEXPORT double shader_create_vertex_texture(double _width, double _height, double _value)
{
	return manager->createTexture(_width, _height, _value); //_value is a default value to initalize all pixels with.
}

GMEXPORT double shader_clear_memory()
{
	manager->~Manager();
	return 1.0;
}

GMEXPORT double shader_set_surface(double _slot, void* _surfacePointer, double _width, double _height)
{
	manager->setSurface(_slot, _surfacePointer, _width, _height);
	return 1.0;
}

GMEXPORT double shader_copy_surface_to_texture(double _textureID, void* _data)
{
	manager->copy_surface_to_texture(_textureID, _data);
	return 1.0;
}