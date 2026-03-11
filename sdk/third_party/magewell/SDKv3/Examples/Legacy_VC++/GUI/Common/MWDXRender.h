#ifndef RENDER_H
#define RENDER_H

#include <Windows.h>
#include "D3Dcommon.h"
// #include "D3DX11.h"
// #include "xnamath.h"
#include "D3Dcompiler.h"
#include <DirectXMath.h>
#include "d3d11.h"  
#include <d3d11_4.h>
#include <dxgi1_6.h>

#include "MWFOURCC.h"

#include <queue>
//#include <wrl/client.h>
using namespace std;

#define NUM_DATA 5

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) {(p)->Release();(p)=NULL;} }
#endif


typedef enum _color_space {
	MW_CSP_AUTO = 0,
	MW_CSP_BT_601,
	MW_CSP_BT_709,
	MW_CSP_BT_2020,
	MW_CSP_COUNT,
}mw_color_space_e;

typedef enum _color_space_level {
	MW_CSP_LEVELS_AUTO = 0,
	MW_CSP_LEVELS_TV,
	MW_CSP_LEVELS_PC,
	MW_CSP_LEVELS_COUNT,
}mw_color_space_level_e;


typedef struct yuvlevels { double ymin, ymax, cmax, cmid; }yuvlevels;

typedef struct rgblevels { double min, max; }rgblevels;


typedef struct _vertex{
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT2 m_tex;
}s_vertex;

typedef struct _rect {
	int m_cx;
	int m_cy;
	int t_temp;
	int t_temp1;
	float m_csp_coeff_0_0;
	float m_csp_coeff_0_1;
	float m_csp_coeff_0_2;
	float m_csp_coeff_1_0;
	float m_csp_coeff_1_1;
	float m_csp_coeff_1_2;
	float m_csp_coeff_2_0;
	float m_csp_coeff_2_1;
	float m_csp_coeff_2_2;
    float m_csp_const_0;
	float m_csp_const_1;
	float m_csp_const_2;
}s_rect;

typedef struct _csvs{
	unsigned char *m_p_code;
	int m_n_code_len;
}s_csvs;

//using Microsoft::WRL::ComPtr;

class MWDXRender{
public:
	MWDXRender();
	~MWDXRender();

public:
	bool initialize(int t_n_width,int t_n_height,DWORD dwFourcc,bool t_b_reverse, bool t_b_mirror,HWND t_hwnd,
		mw_color_space_e t_color_format, mw_color_space_level_e t_input_range, mw_color_space_level_e t_output_range);
	void cleanup_device();

	bool paint(unsigned char*data);
	void output_debug(bool bDebug);
	void set_buffer_count(int bBufferCount);
	BOOL get_max_latency(UINT* pMaxLatency);
	BOOL set_max_latency(UINT nMaxLatency);

protected:
	bool initialize_new(int t_n_width,int t_n_height,DWORD dwFourcc,bool t_b_reverse, bool t_b_mirror,HWND t_hwnd,
		mw_color_space_e t_color_format, mw_color_space_level_e t_input_range, mw_color_space_level_e t_output_range);
	bool initialize_old(int t_n_width,int t_n_height,DWORD dwFourcc,bool t_b_reverse, bool t_b_mirror,HWND t_hwnd,
		mw_color_space_e t_color_format, mw_color_space_level_e t_input_range, mw_color_space_level_e t_output_range);

	HRESULT compile_shader_from_file(WCHAR *t_sz_filepath,LPCSTR t_sz_entrypoint,LPCSTR t_sz_shader_model,ID3DBlob** t_pp_blob_out);
	HRESULT compile_shader_from_cs(unsigned char*t_p_data,int t_n_data_len, LPCSTR t_sz_entrypoint, LPCSTR t_sz_shader_model, ID3DBlob** t_pp_blob_out);

	bool create_render_with_fourcc(DWORD t_dw_fourcc, bool t_b_reverse, bool t_b_mirror,
		                           mw_color_space_e t_color_format, mw_color_space_level_e t_input_range, mw_color_space_level_e t_output_range);
	void destory_render_with_fourcc(DWORD t_dw_fourcc);

	bool create_render_nv12();

	bool get_vsfs(s_csvs *t_vs,s_csvs *t_ps,DWORD t_dw_fourcc);

protected:
	int m_n_width;
	int m_n_height;

	DWORD m_dw_fourcc;
	int m_n_stride;
	int m_n_size;
#define USE_NEW_D3D11 0
#if USE_NEW_D3D11
	IDXGISwapChain2* m_p_swap_chain;		//directx swap chain
	// DXGI_SWAP_CHAIN_DESC1 m_sd;			//swap chain description
	ID3D11Device5* m_p_device;			//d3d11 device
	ID3D11DeviceContext3* m_p_device_context;//device context
#else
	IDXGISwapChain* m_p_swap_chain;		//directx swap chain
	DXGI_SWAP_CHAIN_DESC m_sd;			//swap chain description

	ID3D11Device* m_p_device;			//d3d11 device
	ID3D11DeviceContext* m_p_device_context;//device context
#endif
	ID3D11Texture2D* m_p_render_buffer;
	ID3D11RenderTargetView* m_p_render_view;//render view

	D3D_FEATURE_LEVEL m_featurelevels;
	D3D_FEATURE_LEVEL m_featurelevel;

	D3D11_VIEWPORT m_vp;

	DirectX::XMFLOAT3 m_arr_vex[3];
	D3D11_BUFFER_DESC m_vb_bd;
	ID3D11Buffer* m_p_vertex_buffer;
	ID3D11VertexShader* m_p_vertex_shader;

	ID3DBlob* m_p_vs_blob;
	ID3D11InputLayout* m_p_vertex_layout;

	ID3DBlob* m_p_fs_blob;
	ID3D11PixelShader* m_p_fragment_shader;

	ID3D11Texture2D* m_p_texture_2d;
	ID3D11ShaderResourceView* m_p_shader_resource_view; //texture
	ID3D11SamplerState* m_p_sampler_state;
	D3D11_SAMPLER_DESC m_sampler_des;

	ID3D11Texture2D* m_p_texture_2d_2;
	ID3D11ShaderResourceView* m_p_shader_resource_view_2;

	ID3D11Texture2D* m_p_texture_2d_3;
	ID3D11ShaderResourceView* m_p_shader_resource_view_3;

	int m_n_rc_size;
	// unsigned char*		  m_array_data[NUM_DATA] = {nullptr};
	// queue<unsigned char*> m_queue_data;

	ID3D11Buffer* m_d3db_rect;

	bool m_b_set;
	// unsigned char* m_p_rgb24_buf;
	mw_color_space_e m_csp;

	mw_color_space_level_e m_csp_lvl_in;
	mw_color_space_level_e m_csp_lvl_out;
	HANDLE m_hLatency;

	bool m_bDebug;
	int m_nBufferCount;
#if HAS_VIDEO_PROCESSOR
	// Video processor components
	ComPtr<ID3D11VideoDevice> m_p_video_device;
	ComPtr<ID3D11VideoContext> m_p_video_context;
	ComPtr<ID3D11VideoProcessor> m_p_video_processor;
	ComPtr<ID3D11VideoProcessorEnumerator> m_p_video_enum;
	ComPtr<ID3D11VideoProcessorOutputView> m_p_video_output_view;
	ComPtr<ID3D11VideoProcessorInputView> m_p_video_input_view;
	ComPtr<ID3D11Texture2D> m_p_input_texture;
	ComPtr<ID3D11Texture2D> m_p_video_texture;
	// Flag to indicate if video processor is available
	bool m_b_video_processor_supported;

	bool paint_with_video_processor(unsigned char*data);
	bool create_video_processor();
	bool check_video_processor_support(DWORD t_dw_fourcc);
	DXGI_FORMAT get_dxgi_format_from_fourcc(DWORD fourcc);
	void LogVideoProcessorContent();
#endif
	bool paint_with_shader(unsigned char*data);
	// Helper methods for video processor

private:
};

#endif