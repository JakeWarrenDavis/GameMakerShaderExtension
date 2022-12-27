#pragma once
#include <Windows.h>
#include <vector>
#include <d3d11.h>

//Stores the SRV, Texture and texture data of a texture.
class TextureHolder
{
public:
	float* textureData; //The actual data the texture is built from.
	ID3D11Texture2D* texture; //The texture to populate.
	ID3D11ShaderResourceView* shaderResourceView; //The actual resource the vertex shader will be using.
	int width, height; //Width and height of the texture.
	int components = 4; //Hard set to 4, as a colour is made from 4 floats.
	int id; //Keep track of what object it is for gamemaker.
};

class Manager
{
private:
	std::vector<TextureHolder*> textures; //List of all the TextureHolder objects, so they can be updated and deleted later on.
	int textureID = 0; //Start counting the texture IDs from 0.
	ID3D11DeviceContext* context;
	ID3D11Device* device;
public:
	Manager(ID3D11Device* _device, ID3D11DeviceContext* _context);
	~Manager();
	int createTexture(int _width, int _height, int _value);
	void setTextureValue(int _id, int _index, float _value);
	float getTextureValue(int _id, int _index);
	void setPixelColour(int _id, int _x, int _y, float _r, float _g, float _b, float _a);
	float getPixelValue(int _id, int _x, int _y, int _channel); //Channel is 0 to 3, where 0 is r and 3 is a.
	TextureHolder* findTexture(int _id);
	void setTexture(int _id, int _slot);
	void deleteTexture(int _id);
	void updateTexture(int _id);
	void clearResources(int _slot); //Free up a SRV slot, not sure if its nessecary, but just in case it crashes if you delete a texture's srv that is currently in use.
	void recreateTexture(int _id);
	void setSurface(int _slot, void* _surfacePointer, int _width, int _height);
	void copy_surface_to_texture(int _id, void* _data);
};

