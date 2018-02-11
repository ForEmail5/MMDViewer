#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <string>

#include "D3D.h"
#include "External\FreeImage\FreeImage.h"

class Texture2D
{
public:
	Texture2D(ID3D11Device* device)
	{
		this->device = device;
		this->texture2d = nullptr;
		this->shader_resource_view = nullptr;
	}

	~Texture2D()
	{
		SAFE_RELEASE(texture2d);
		SAFE_RELEASE(shader_resource_view);
	}

	static Texture2D* CreateTexture2D(const std::string& filename, ID3D11Device* device)
	{
		Texture2D* texture = nullptr;

		/* 读取图像数据 */
		int width, height;
		unsigned char* data = LoadImageData(filename, width, height);

		if ( data != nullptr )
		{
			bool res = true;
			/* 创建纹理 */
			texture = new Texture2D(device);
			res &= texture->CreateTexture2D(width, height, data);
			res &= texture->CreateShaderResourceView();
			if ( res == false )
			{
				delete texture;
				texture = nullptr;
			}
			FreeImageData(data);
		}
		return texture;
	}

	static unsigned char* LoadImageData(const std::string& filename, int& w, int& h)
	{
		FIBITMAP* bmp = nullptr;
		unsigned char* image_data = nullptr;

		/* 初始化 FreeImage */
		FreeImage_Initialise(TRUE);

		/* 获取图像文件类型 */
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.c_str());
		if ( fif == FIF_UNKNOWN )
		{
			fif = FreeImage_GetFIFFromFilename(filename.c_str());
		}

		/* 加载所支持图像类型的图像 */
		if ( fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif) )
		{
			bmp = FreeImage_Load(fif, filename.c_str(), JPEG_DEFAULT);
		}

		if ( bmp == nullptr ) return nullptr;

		/* 获取图像信息 */
		w = FreeImage_GetWidth(bmp);
		h = FreeImage_GetHeight(bmp);
		int pixel_num = w * h;
		int byte_per_pixel = FreeImage_GetLine(bmp) / w;

		image_data = ( unsigned char* ) malloc(sizeof( unsigned char ) * pixel_num * 4);
		unsigned char* image_ptr = FreeImage_GetBits(bmp);

		/* 获取图像数据 */
		int current_pixel = 0;
		if ( byte_per_pixel == 1 )
		{
			for ( int i = 0; i < pixel_num; i++ ) {
				image_data[i * 4 + 2] = image_ptr[current_pixel++];
				image_data[i * 4 + 1] = image_ptr[255];
				image_data[i * 4 + 0] = image_ptr[255];
				image_data[i * 4 + 3] = image_ptr[255];
			}
		}
		else if ( byte_per_pixel == 4 ) {
			for ( int i = 0; i < pixel_num; i++ ) {
				image_data[i * 4 + 2] = image_ptr[current_pixel++];
				image_data[i * 4 + 1] = image_ptr[current_pixel++];
				image_data[i * 4 + 0] = image_ptr[current_pixel++];
				image_data[i * 4 + 3] = image_ptr[current_pixel++];
			}
		}
		else {
			for ( int i = 0; i < pixel_num; i++ ) {
				image_data[i * 4 + 2] = image_ptr[current_pixel++];
				image_data[i * 4 + 1] = image_ptr[current_pixel++];
				image_data[i * 4 + 0] = image_ptr[current_pixel++];
				image_data[i * 4 + 3] = 255;
			}
		}
		FreeImage_Unload(bmp);
		FreeImage_DeInitialise();

		return image_data;
	}

	static void FreeImageData(unsigned char* data)
	{
		::free(data);
	}

	ID3D11ShaderResourceView* GetShaderResourceView()
	{
		return shader_resource_view;
	}

private:
	bool CreateTexture2D(int w, int h, void* data)
	{
		/* 创建纹理描述 */
		D3D11_TEXTURE2D_DESC tex_desc;
		tex_desc.Width = w;
		tex_desc.Height = h;
		tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; DXGI_FORMAT_R8_UNORM;
		tex_desc.MipLevels = 1;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.MiscFlags = 0;
		tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex_desc.ArraySize = 1;

		D3D11_SUBRESOURCE_DATA sub_data;
		sub_data.pSysMem = data;
		sub_data.SysMemPitch = w * 4;		// 行宽 2D、3D
		sub_data.SysMemSlicePitch = 0;		// 层宽 3D

		/* 创建纹理 */
		HRESULT result = device->CreateTexture2D(&tex_desc, &sub_data, &texture2d);
		if ( FAILED(result) ) return false;

		return true;
	}

	bool CreateShaderResourceView()
	{
		if ( texture2d == nullptr ) return false;

		D3D11_TEXTURE2D_DESC tex_desc;
		texture2d->GetDesc(&tex_desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		srv_desc.Format = tex_desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = -1;
		srv_desc.Texture2D.MostDetailedMip = 0;

		/*  创建着色器资源视图 */
		HRESULT result = device->CreateShaderResourceView(texture2d, &srv_desc, &shader_resource_view);
		if ( FAILED(result) ) return false;

		return true;
	}

private:
	ID3D11Device* device;
	ID3D11Texture2D* texture2d;
	ID3D11ShaderResourceView* shader_resource_view;
};

#endif