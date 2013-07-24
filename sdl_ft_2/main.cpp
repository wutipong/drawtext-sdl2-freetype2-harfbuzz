#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <string>

std::wstring text = L"Font EDBazaar by ED_krub";

#define USE_OPENCL
#ifdef USE_OPENCL

#include <CL/cl.h>
#include <cstring>
const char* opencl_kernel = 
"__kernel void update_alpha(__global const uchar *alpha, \n"\
"		__global uchar4 *in_pixels, \n"\
"		__global uchar4 *out_pixels) \n"\
"{\n" \
"	int i = get_global_id(0);\n" \
"	out_pixels[i].s3 = alpha[i];\n" \
"	out_pixels[i].s012 = in_pixels[i].s012;\n" \
"}";

cl_command_queue command_queue;
cl_kernel kernel;
cl_program program;
cl_context context ;

bool InitOpenCl()
{
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;   
	cl_uint ret_num_devices = 0;
	cl_uint ret_num_platforms = 0;

	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

	ret = clGetDeviceIDs( 
			platform_id, 
			CL_DEVICE_TYPE_DEFAULT,
			1, 
			&device_id, 
			&ret_num_devices);
		
	context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	size_t length = strlen(opencl_kernel);

	cl_program program = clCreateProgramWithSource(
			context, 
			1, 
			&opencl_kernel, 
			&length, 
			&ret);

	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	kernel = clCreateKernel(program, "update_alpha", &ret);

	return true;
}

void CreateSurfaceFromFT_Bitmap(const FT_Bitmap& bitmap,
		const unsigned int& color, 
		SDL_Surface*& output) 
{
	output = SDL_CreateRGBSurface(0, 
			bitmap.width, 
			bitmap.rows, 
			32, 
			0x000000ff,
			0x0000ff00, 
			0x00ff0000, 
			0xff000000);

	SDL_FillRect(output, NULL, color);

	SDL_LockSurface(output);

	cl_int ret;
	size_t global_item_size = bitmap.width*bitmap.rows; 
	
	cl_mem alpha_mem_obj = clCreateBuffer(context, 
			CL_MEM_READ_ONLY,
			global_item_size * sizeof(cl_uchar), 
			NULL, 
			&ret);
			
	cl_mem in_pixels_mem_obj = clCreateBuffer(context, 
			CL_MEM_READ_ONLY,
			global_item_size * sizeof(cl_uchar4), 
			NULL, 
			&ret);
	
	cl_mem out_pixels_mem_obj = clCreateBuffer(context, 
			CL_MEM_WRITE_ONLY,
			global_item_size * sizeof(cl_uchar4), 
			NULL, 
			&ret);
	
	ret = clEnqueueWriteBuffer(
			command_queue, 
			alpha_mem_obj, 
			CL_TRUE, 
			0,
			global_item_size * sizeof(cl_uchar), 
			bitmap.buffer, 
			0, 
			NULL, 
			NULL);
			
	ret = clEnqueueWriteBuffer(
			command_queue, 
			in_pixels_mem_obj, 
			CL_TRUE, 
			0, 
			global_item_size * sizeof(cl_uchar4), 
			output->pixels, 
			0, 
			NULL, 
			NULL);
			
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&alpha_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&in_pixels_mem_obj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&out_pixels_mem_obj);
	
	ret = clEnqueueNDRangeKernel(
			command_queue, 
			kernel, 
			1, 
			NULL, 
			&global_item_size, 
			NULL, 
			0, 
			NULL, 
			NULL);
	
	ret = clEnqueueReadBuffer(
			command_queue, 
			out_pixels_mem_obj, 
			CL_TRUE, 
			0, 
	global_item_size * sizeof(cl_uchar4), 
			output->pixels, 
			0, 
			NULL, 
			NULL);
			
	ret = clReleaseMemObject(alpha_mem_obj);
	ret = clReleaseMemObject(in_pixels_mem_obj);
	ret = clReleaseMemObject(out_pixels_mem_obj);
	
	SDL_UnlockSurface(output);
}

void CleanUpOpenCL()
{
	cl_int ret;
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

}
#else
bool InitOpenCl() { return true;}
void CleanUpOpenCL(){}

void CreateSurfaceFromFT_Bitmap(const FT_Bitmap& bitmap,
		const unsigned int& color, 
		SDL_Surface*& output) 
{
	output = SDL_CreateRGBSurface(0, 
			bitmap.width, 
			bitmap.rows, 
			32, 
			0x000000ff,
			0x0000ff00, 
			0x00ff0000, 
			0xff000000);

	SDL_FillRect(output, NULL, color);

	SDL_LockSurface(output);

	unsigned char *src_pixels = bitmap.buffer;
	unsigned int *target_pixels =
			reinterpret_cast<unsigned int*>(output->pixels);

	for (int i = 0; i < bitmap.rows; i++) {
		for (int j = 0; j < bitmap.width; j++) {
			unsigned int pixel = target_pixels[i * output->w + j];
			unsigned int alpha = src_pixels[i * bitmap.pitch + j];

			pixel &= (alpha << 24) | 0x00ffffff;

			target_pixels[i * output->w + j] = pixel;
		}
	}
	SDL_UnlockSurface(output);
}
#endif

void DrawText(const std::wstring& text, 
		const unsigned int& color,
		const int& baseline, 
		const int& x_start, 
		const FT_Face& face,
		SDL_Renderer*& renderer) 
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	int x = x_start;

	for (unsigned int i = 0; i < text.length(); i++) {
		FT_Load_Char(face, text[i], FT_LOAD_RENDER);

		SDL_Surface* surface = NULL;
		CreateSurfaceFromFT_Bitmap(face->glyph->bitmap, color, surface);
		SDL_Texture* glyph_texture = SDL_CreateTextureFromSurface(renderer,
				surface);

		SDL_Rect dest;
		dest.x = x + (face->glyph->metrics.horiBearingX >> 6);

		dest.y = baseline - (face->glyph->metrics.horiBearingY >> 6);
		dest.w = surface->w;
		dest.h = surface->h;

		SDL_RenderCopy(renderer, glyph_texture, NULL, &dest);

		x += (face->glyph->metrics.horiAdvance >> 6);

		SDL_DestroyTexture(glyph_texture);
		SDL_FreeSurface(surface);
	}
}

int main(int argc, char **argv) {
	
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Test Window",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	FT_Library library;
	FT_Init_FreeType(&library);

	FT_Face face;
	FT_New_Face(library, "./EDBazaar.ttf", 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 64);

	InitOpenCl();
	
	uint32_t frame = 0;
	while (true) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				break;
		}

		SDL_SetRenderDrawColor(renderer, 0x50, 0x40, 0x00, 0xff);
		SDL_RenderClear(renderer);

		uint32_t before = SDL_GetTicks();
		DrawText(text, 0xffffffff, 300, 0, face, renderer);
		uint32_t after = SDL_GetTicks();
		
		if(frame % 50 == 0)
		{
			std::cout<<"Time taken for render text = "<<after - before <<"ms"<<std::endl;
			frame = 0;
		}
		frame++;
		SDL_RenderPresent(renderer);
		SDL_Delay(0);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	CleanUpOpenCL();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
