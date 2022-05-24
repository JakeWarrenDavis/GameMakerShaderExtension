#include "pch.h"
#include "Manager.h"

Manager::Manager(ID3D11Device* _device, ID3D11DeviceContext* _context)
{
	device = _device;
	context = _context;
}

Manager::~Manager()
{
	//Free the texture objects from memory.
	for (int i = 0; i < textures.size(); i++)
	{
		TextureHolder* texture = textures.at(i);
		texture->shaderResourceView->Release();
		texture->texture->Release();
		delete[] texture->textureData;
	}
	//Now clear the textures list.
	textures.clear();
}

int Manager::createTexture(int _width, int _height, int _value)
{
	TextureHolder* texture = new TextureHolder();
	texture->width = (int)_width;
	texture->height = (int)_height;
	texture->textureData = new float[(texture->width * texture->height) * texture->components];
	texture->id = textureID;
	textureID += 1;

	//Fill texture data.
	for (int i = 0; i < (texture->width * texture->height) * texture->components; i++)
	{
		texture->textureData[i] = _value;
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = (void*)texture->textureData;
	initData.SysMemPitch = texture->width * texture->components * sizeof(float);
	initData.SysMemSlicePitch = NULL;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = texture->width;
	desc.Height = texture->height;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	device->CreateTexture2D(&desc, &initData, &texture->texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	HRESULT hr;
	hr = device->CreateShaderResourceView(texture->texture, &SRVDesc, &texture->shaderResourceView);

	if (FAILED(hr))
	{
		//Lmao get fucked.
		MessageBoxA(NULL, "Couldn't create shader resource view", NULL, MB_OK);
	}

	textures.push_back(texture);
	return texture->id;
}

void Manager::setTextureValue(int _id, int _index, float _value)
{
	TextureHolder* texture = findTexture(_id);
	texture->textureData[_index] = _value;
}

float Manager::getTextureValue(int _id, int _index)
{
	TextureHolder* texture = findTexture(_id);

	if (texture)
	{
		return texture->textureData[_index];
	}
	return 0.0;
}

void Manager::setPixelColour(int _id, int _x, int _y, float _r, float _g, float _b, float _a)
{
	TextureHolder* texture = findTexture(_id);

	if (texture)
	{
		int tmpIndex = 0;
		tmpIndex += texture->width * texture->components * _y;
		tmpIndex += _x * texture->components;
		texture->textureData[tmpIndex] = _r;
		texture->textureData[tmpIndex+1] = _g;
		texture->textureData[tmpIndex+2] = _b;
		texture->textureData[tmpIndex+3] = _a;
	}
}

float Manager::getPixelValue(int _id, int _x, int _y, int _channel)
{
	TextureHolder* texture = findTexture(_id);

	if (texture)
	{
		int tmpIndex = 0;
		tmpIndex += texture->width * texture->components * _y;
		tmpIndex += _x * texture->components;
		tmpIndex += _channel; //Technically you could access data in another pixel if this value goes over 3. IDK if I want to limit that though.
		return texture->textureData[tmpIndex];
	}
	return 0.0;
}

TextureHolder* Manager::findTexture(int _id)
{
	TextureHolder* texture = nullptr;
	for (int i = 0; i < textures.size(); i++)
	{
		texture = textures.at(i);

		if (texture->id == _id)
		{
			break;
		}
	}
	return texture;
}

void Manager::setTexture(int _id, int _slot)
{
	TextureHolder* texture = findTexture(_id);
	if (texture)
	{
		context->VSSetShaderResources(_slot, 1, &texture->shaderResourceView);
		//context->VSSetShaderResources(0, 1, &texture->shaderResourceView);
	}
	//context->VSSetShaderResources(_slot, 1, &texture->shaderResourceView);
}

void Manager::deleteTexture(int _id)
{
	TextureHolder* texture = findTexture(_id);

	if (texture)
	{
		texture->shaderResourceView->Release();
		texture->texture->Release();
		delete[] texture->textureData;
		delete texture;
	}
}

void Manager::updateTexture(int _id)
{
	TextureHolder* texture = findTexture(_id);

	if (texture)
	{
		//Update the texture.
		context->UpdateSubresource(texture->texture, 0, NULL, texture->textureData, texture->components * texture->width, texture->components * texture->height);
		
		texture->shaderResourceView->Release();

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		HRESULT hr;
		hr = device->CreateShaderResourceView(texture->texture, &SRVDesc, &texture->shaderResourceView);
	}
}

void Manager::clearResources(int _slot)
{
	context->VSSetShaderResources(_slot, 1, nullptr);
}

void Manager::recreateTexture(int _id)
{
	TextureHolder* texture = findTexture(_id);

	if (texture)
	{
		texture->shaderResourceView->Release();
		texture->texture->Release();

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = (void*)texture->textureData;
		initData.SysMemPitch = texture->width * texture->components * sizeof(float);
		initData.SysMemSlicePitch = NULL;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = texture->width;
		desc.Height = texture->height;
		desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		device->CreateTexture2D(&desc, &initData, &texture->texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		HRESULT hr;
		hr = device->CreateShaderResourceView(texture->texture, &SRVDesc, &texture->shaderResourceView);

		if (FAILED(hr))
		{
			//Lmao get fucked.
			MessageBoxA(NULL, "Couldn't create shader resource view", NULL, MB_OK);
		}
	}
}
