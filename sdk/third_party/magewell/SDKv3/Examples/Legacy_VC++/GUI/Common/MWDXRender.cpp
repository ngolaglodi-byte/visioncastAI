#include "stdafx.h"
#include "MWDXRender.h"
#include <Windows.h>
#include <WinGDI.h>
// #include <d3d9.h>

using namespace DirectX;

#include "MWDXRenderShader.h"

#include "utils.h"

bool MWDXRender::get_vsfs(s_csvs *t_vs, s_csvs *t_ps, DWORD t_dw_fourcc) {
	bool t_ret = false;

	switch (t_dw_fourcc) {
	case MWFOURCC_GREY:
	case MWFOURCC_Y800:
	case MWFOURCC_Y8:
		t_vs->m_p_code = g_cs_vs_grey;
		t_vs->m_n_code_len = sizeof(g_cs_vs_grey);
		t_ps->m_p_code = g_cs_ps_grey;
		t_ps->m_n_code_len = sizeof(g_cs_ps_grey);
		t_ret = true;
		break;
	case MWFOURCC_Y16:
		t_vs->m_p_code = g_cs_vs_y16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_y16);
		t_ps->m_p_code = g_cs_ps_y16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_y16);
		t_ret = true;
		break;
	case MWFOURCC_RGB16:
		t_vs->m_p_code = g_cs_vs_rgb16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_rgb16);
		t_ps->m_p_code = g_cs_ps_rgb16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_rgb16);
		t_ret = true;
		break;
	case MWFOURCC_BGR16:
		t_vs->m_p_code = g_cs_vs_bgr16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgr16);
		t_ps->m_p_code = g_cs_ps_bgr16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgr16);
		t_ret = true;
		break;
	case MWFOURCC_RGBA:
		t_vs->m_p_code = g_cs_vs_rgba;
		t_vs->m_n_code_len = sizeof(g_cs_vs_rgba);
		t_ps->m_p_code = g_cs_ps_rgba;
		t_ps->m_n_code_len = sizeof(g_cs_ps_rgba);
		t_ret = true;
		break;
	case MWFOURCC_ARGB:
		t_vs->m_p_code = g_cs_vs_argb;
		t_vs->m_n_code_len = sizeof(g_cs_vs_argb);
		t_ps->m_p_code = g_cs_ps_argb;
		t_ps->m_n_code_len = sizeof(g_cs_ps_argb);
		t_ret = true;
		break;
	case MWFOURCC_BGRA:
		t_vs->m_p_code = g_cs_vs_bgra;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgra);
		t_ps->m_p_code = g_cs_ps_bgra;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgra);
		t_ret = true;
		break;
	case MWFOURCC_ABGR:
		t_vs->m_p_code = g_cs_vs_abgr;
		t_vs->m_n_code_len = sizeof(g_cs_vs_abgr);
		t_ps->m_p_code = g_cs_ps_abgr;
		t_ps->m_n_code_len = sizeof(g_cs_ps_abgr);
		t_ret = true;
		break;
	case MWFOURCC_YUY2:
	case MWFOURCC_YUYV:
		t_vs->m_p_code = g_cs_vs_yuy2;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yuy2);
		t_ps->m_p_code = g_cs_ps_yuy2;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yuy2);
		t_ret = true;
		break;
	case MWFOURCC_BGR24:
		t_vs->m_p_code = g_cs_vs_bgr24;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgr24);
		t_ps->m_p_code = g_cs_ps_bgr24;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgr24);
		t_ret = true;
		break;
	case MWFOURCC_RGB24:
		t_vs->m_p_code = g_cs_vs_rgb24;
		t_vs->m_n_code_len = sizeof(g_cs_vs_rgb24);
		t_ps->m_p_code = g_cs_ps_rgb24;
		t_ps->m_n_code_len = sizeof(g_cs_ps_rgb24);
		t_ret = true;
		break;
	case MWFOURCC_UYVY:
		t_vs->m_p_code = g_cs_vs_uyvy;
		t_vs->m_n_code_len = sizeof(g_cs_vs_uyvy);
		t_ps->m_p_code = g_cs_ps_uyvy;
		t_ps->m_n_code_len = sizeof(g_cs_ps_uyvy);
		t_ret = true;
		break;
	case MWFOURCC_YVYU:
		t_vs->m_p_code = g_cs_vs_yvyu;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yvyu);
		t_ps->m_p_code = g_cs_ps_yvyu;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yvyu);
		t_ret = true;
		break;
	case MWFOURCC_VYUY:
		t_vs->m_p_code = g_cs_vs_vyuy;
		t_vs->m_n_code_len = sizeof(g_cs_vs_vyuy);
		t_ps->m_p_code = g_cs_ps_vyuy;
		t_ps->m_n_code_len = sizeof(g_cs_ps_vyuy);
		t_ret = true;
		break;
	case MWFOURCC_NV12:
		t_vs->m_p_code = g_cs_vs_nv12;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv12);
		t_ps->m_p_code = g_cs_ps_nv12;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv12);
		t_ret = true;
		break;
	case MWFOURCC_NV21:
		t_vs->m_p_code = g_cs_vs_nv21;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv21);
		t_ps->m_p_code = g_cs_ps_nv21;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv21);
		t_ret = true;
		break;
	case MWFOURCC_NV16:
		t_vs->m_p_code = g_cs_vs_nv16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv16);
		t_ps->m_p_code = g_cs_ps_nv16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv16);
		t_ret = true;
		break;
	case MWFOURCC_NV61:
		t_vs->m_p_code = g_cs_vs_nv61;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv61);
		t_ps->m_p_code = g_cs_ps_nv61;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv61);
		t_ret = true;
		break;
	case MWFOURCC_I422:
		t_vs->m_p_code = g_cs_vs_i422;
		t_vs->m_n_code_len = sizeof(g_cs_vs_i422);
		t_ps->m_p_code = g_cs_ps_i422;
		t_ps->m_n_code_len = sizeof(g_cs_ps_i422);
		t_ret = true;
		break;
	case MWFOURCC_YV16:
		t_vs->m_p_code = g_cs_vs_yv16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yv16);
		t_ps->m_p_code = g_cs_ps_yv16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yv16);
		t_ret = true;
		break;
	case MWFOURCC_I420:
	case MWFOURCC_IYUV:
		t_vs->m_p_code = g_cs_vs_i420;
		t_vs->m_n_code_len = sizeof(g_cs_vs_i420);
		t_ps->m_p_code = g_cs_ps_i420;
		t_ps->m_n_code_len = sizeof(g_cs_ps_i420);
		t_ret = true;
		break;
	case MWFOURCC_YV12:
		t_vs->m_p_code = g_cs_vs_yv12;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yv12);
		t_ps->m_p_code = g_cs_ps_yv12;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yv12);
		t_ret = true;
		break;
	case MWFOURCC_P010:
		t_vs->m_p_code = g_cs_vs_p010;
		t_vs->m_n_code_len = sizeof(g_cs_vs_p010);
		t_ps->m_p_code = g_cs_ps_p010;
		t_ps->m_n_code_len = sizeof(g_cs_ps_p010);
		t_ret = true;
		break;
	case MWFOURCC_P210:
		t_vs->m_p_code = g_cs_vs_p210;
		t_vs->m_n_code_len = sizeof(g_cs_vs_p210);
		t_ps->m_p_code = g_cs_ps_p210;
		t_ps->m_n_code_len = sizeof(g_cs_ps_p210);
		t_ret = true;
		break;
	case MWFOURCC_IYU2:
		t_vs->m_p_code = g_cs_vs_iyu2;
		t_vs->m_n_code_len = sizeof(g_cs_vs_iyu2);
		t_ps->m_p_code = g_cs_ps_iyu2;
		t_ps->m_n_code_len = sizeof(g_cs_ps_iyu2);
		t_ret = true;
		break;
	case MWFOURCC_V308:
		t_vs->m_p_code = g_cs_vs_v308;
		t_vs->m_n_code_len = sizeof(g_cs_vs_v308);
		t_ps->m_p_code = g_cs_ps_v308;
		t_ps->m_n_code_len = sizeof(g_cs_ps_v308);
		t_ret = true;
		break;
	case MWFOURCC_AYUV:
		t_vs->m_p_code = g_cs_vs_ayuv;
		t_vs->m_n_code_len = sizeof(g_cs_vs_ayuv);
		t_ps->m_p_code = g_cs_ps_ayuv;
		t_ps->m_n_code_len = sizeof(g_cs_ps_ayuv);
		t_ret = true;
		break;
	case MWFOURCC_UYVA:
	case MWFOURCC_V408:
		t_vs->m_p_code = g_cs_vs_uyva;
		t_vs->m_n_code_len = sizeof(g_cs_vs_uyva);
		t_ps->m_p_code = g_cs_ps_uyva;
		t_ps->m_n_code_len = sizeof(g_cs_ps_uyva);
		t_ret = true;
		break;
	case MWFOURCC_VYUA:
		t_vs->m_p_code = g_cs_vs_vyua;
		t_vs->m_n_code_len = sizeof(g_cs_vs_vyua);
		t_ps->m_p_code = g_cs_ps_vyua;
		t_ps->m_n_code_len = sizeof(g_cs_ps_vyua);
		t_ret = true;
		break;
	case MWFOURCC_Y210:
		t_vs->m_p_code = g_cs_vs_y210;
		t_vs->m_n_code_len = sizeof(g_cs_vs_y210);
		t_ps->m_p_code = g_cs_ps_y210;
		t_ps->m_n_code_len = sizeof(g_cs_ps_y210);
		t_ret = true;
		break;
	case MWFOURCC_Y410:
		t_vs->m_p_code = g_cs_vs_y410;
		t_vs->m_n_code_len = sizeof(g_cs_vs_y410);
		t_ps->m_p_code = g_cs_ps_y410;
		t_ps->m_n_code_len = sizeof(g_cs_ps_y410);
		t_ret = true;
		break;
	case MWFOURCC_RGB10:
		t_vs->m_p_code = g_cs_vs_rgb10;
		t_vs->m_n_code_len = sizeof(g_cs_vs_rgb10);
		t_ps->m_p_code = g_cs_ps_rgb10;
		t_ps->m_n_code_len = sizeof(g_cs_ps_rgb10);
		t_ret = true;
		break;
	case MWFOURCC_BGR10:
		t_vs->m_p_code = g_cs_vs_bgr10;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgr10);
		t_ps->m_p_code = g_cs_ps_bgr10;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgr10);
		t_ret = true;
		break;
	default:
		t_vs->m_p_code = g_cs_vs_bgra;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgra);
		t_ps->m_p_code = g_cs_ps_bgra;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgra);
		t_ret = false;
		break;
	}

	return t_ret;
}

MWDXRender::MWDXRender()
{
	m_hLatency = NULL;
	m_p_device_context = NULL;
	m_p_vertex_buffer = NULL;
	m_p_vertex_layout = NULL;
	m_p_vertex_shader = NULL;
	m_p_sampler_state = NULL;
	m_d3db_rect = NULL;
	m_p_texture_2d = NULL;
	m_p_shader_resource_view = NULL;
	m_p_texture_2d_2 = NULL;
	m_p_shader_resource_view_2 = NULL;
	m_p_texture_2d_3 = NULL;
	m_p_shader_resource_view_3 = NULL;
	m_p_fragment_shader= NULL;
	m_p_render_view = NULL;
	m_p_swap_chain = NULL;
	m_p_device_context = NULL;
	m_p_device = NULL;
	m_bDebug = false;
	m_nBufferCount = 2;
#if HAS_VIDEO_PROCESSOR
	m_b_video_processor_supported = false;
#endif
}

MWDXRender::~MWDXRender()
{
}


HRESULT MWDXRender::compile_shader_from_file(WCHAR *t_sz_filepath, LPCSTR t_sz_entrypoint, LPCSTR t_sz_shader_model, ID3DBlob** t_pp_blob_out)
{
	HRESULT t_hr = S_FALSE;

	DWORD t_dw_shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *t_p_errorblod;
	t_hr = D3DCompileFromFile(
		t_sz_filepath,
		NULL,
		NULL,
		t_sz_entrypoint,
		t_sz_shader_model,
		t_dw_shaderflags,
		0,
		t_pp_blob_out,
		&t_p_errorblod
	);
	if (FAILED(t_hr)) {
		if (t_p_errorblod != NULL) {
			//output the err
			//release
			//TRACE("%s\n",t_p_errorblod->GetBufferPointer());
			t_p_errorblod->Release();
			t_p_errorblod = NULL;
		}
	}
	if (t_p_errorblod) {
		t_p_errorblod->Release();
		t_p_errorblod = NULL;
	}
	return t_hr;
}

HRESULT MWDXRender::compile_shader_from_cs(unsigned char*t_p_data, int t_n_data_len, LPCSTR t_sz_entrypoint, LPCSTR t_sz_shader_model, ID3DBlob** t_pp_blob_out) {
	HRESULT t_hr = S_FALSE;

	DWORD t_dw_shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *t_p_errorblod;
	t_hr = D3DCompile(t_p_data, t_n_data_len, NULL, NULL, NULL, t_sz_entrypoint,
		t_sz_shader_model, t_dw_shaderflags, 0,
		t_pp_blob_out, &t_p_errorblod);
	if (FAILED(t_hr)) {
		if (t_p_errorblod != NULL) {
			//output the err
			//release
			MW_LOG("compile shader failed, %s\n, shader=%s\n", t_p_errorblod->GetBufferPointer(), t_p_data);
			t_p_errorblod->Release();
			t_p_errorblod = NULL;
		}
	}
	if (t_p_errorblod) {
		t_p_errorblod->Release();
		t_p_errorblod = NULL;
	}
	return t_hr;
}

bool MWDXRender::create_render_with_fourcc(DWORD t_dw_fourcc, bool t_b_reverse, bool t_b_mirror, mw_color_space_e t_color_format, mw_color_space_level_e input_range, mw_color_space_level_e output_range)
{
	HRESULT t_hr = S_FALSE;
	// Create vertex buffer
	s_vertex vertices[] =
	{

		XMFLOAT3(-1.0f, 1.0f, 0.5f),XMFLOAT2(0.0f,0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.5f),XMFLOAT2(1.0f,0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.5f),XMFLOAT2(0.0f,1.0f),
		XMFLOAT3(1.0f, -1.0f, 0.5f),XMFLOAT2(1.0f,1.0f),

	};

	if (t_b_reverse&&t_b_mirror) {
		vertices[0].m_tex = XMFLOAT2(1.0f, 1.0f);
		vertices[1].m_tex = XMFLOAT2(0.0f, 1.0f);
		vertices[2].m_tex = XMFLOAT2(1.0f, 0.0f);
		vertices[3].m_tex = XMFLOAT2(0.0f, 0.0f);
	}
	else if (t_b_reverse) {
		vertices[0].m_tex = XMFLOAT2(0.0f, 1.0f);
		vertices[1].m_tex = XMFLOAT2(1.0f, 1.0f);
		vertices[2].m_tex = XMFLOAT2(0.0f, 0.0f);
		vertices[3].m_tex = XMFLOAT2(1.0f, 0.0f);
	}
	else if (t_b_mirror) {
		vertices[0].m_tex = XMFLOAT2(1.0f, 0.0f);
		vertices[1].m_tex = XMFLOAT2(0.0f, 0.0f);
		vertices[2].m_tex = XMFLOAT2(1.0f, 1.0f);
		vertices[3].m_tex = XMFLOAT2(0.0f, 1.0f);
	}



	//ZeroMemory(&m_vb_bd,sizeof(m_vb_bd));
	m_vb_bd.Usage = D3D11_USAGE_DEFAULT;
	m_vb_bd.ByteWidth = sizeof(s_vertex) * 4;
	m_vb_bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vb_bd.CPUAccessFlags = 0;
	m_vb_bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA t_initial_data;
	ZeroMemory(&t_initial_data, sizeof(t_initial_data));
	t_initial_data.pSysMem = vertices;
	t_hr = m_p_device->CreateBuffer(&m_vb_bd, &t_initial_data, &m_p_vertex_buffer);
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "CreateBuffer");
		return false;
	}

	if (t_dw_fourcc == MWFOURCC_GREY || \
		t_dw_fourcc == MWFOURCC_Y800 || \
		t_dw_fourcc == MWFOURCC_Y8 || \
		t_dw_fourcc == MWFOURCC_Y16 || \
		t_dw_fourcc == MWFOURCC_RGB16 || \
		t_dw_fourcc == MWFOURCC_BGR16 || \
		t_dw_fourcc == MWFOURCC_YUY2 || \
		t_dw_fourcc == MWFOURCC_YUYV || \
		t_dw_fourcc == MWFOURCC_UYVY || \
		t_dw_fourcc == MWFOURCC_YVYU || \
		t_dw_fourcc == MWFOURCC_VYUY || \
		t_dw_fourcc == MWFOURCC_BGR24 || \
		t_dw_fourcc == MWFOURCC_RGB24 || \
		t_dw_fourcc == MWFOURCC_NV12 || \
		t_dw_fourcc == MWFOURCC_NV21 || \
		t_dw_fourcc == MWFOURCC_NV16 || \
		t_dw_fourcc == MWFOURCC_NV61 || \
		t_dw_fourcc == MWFOURCC_I422 || \
		t_dw_fourcc == MWFOURCC_YV16 || \
		t_dw_fourcc == MWFOURCC_I420 || \
		t_dw_fourcc == MWFOURCC_IYUV || \
		t_dw_fourcc == MWFOURCC_YV12 || \
		t_dw_fourcc == MWFOURCC_P010 || \
		t_dw_fourcc == MWFOURCC_P210 || \
		t_dw_fourcc == MWFOURCC_IYU2 || \
		t_dw_fourcc == MWFOURCC_V308 || \
		t_dw_fourcc == MWFOURCC_AYUV || \
		t_dw_fourcc == MWFOURCC_UYVA || \
		t_dw_fourcc == MWFOURCC_V408 || \
		t_dw_fourcc == MWFOURCC_VYUA || \
		t_dw_fourcc == MWFOURCC_Y210 || \
		t_dw_fourcc == MWFOURCC_Y410) {
		//create cb_buffer
		m_d3db_rect = NULL;
		// Create the constant buffers
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(s_rect);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		t_hr = m_p_device->CreateBuffer(&bd, NULL, &m_d3db_rect);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateBuffer");
			return false;
		}
	}
	//set vertex buffer
	UINT t_stride = sizeof(s_vertex);
	UINT t_offset = 0;
	m_p_device_context->IASetVertexBuffers(0, 1, &m_p_vertex_buffer, &t_stride, &t_offset);

	//set primitive topology
	m_p_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//compile the vertex shader
	s_csvs t_vs, t_ps;
	bool t_b_ret = get_vsfs(&t_vs, &t_ps, t_dw_fourcc);

	m_p_vs_blob = NULL;
	t_hr = compile_shader_from_cs(t_vs.m_p_code, t_vs.m_n_code_len, "VS", "vs_4_0", &m_p_vs_blob);
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "compile_shader_from_cs");
		return false;
	}

	//create the vertex shader
	t_hr = m_p_device->CreateVertexShader(m_p_vs_blob->GetBufferPointer(), m_p_vs_blob->GetBufferSize(), NULL, &m_p_vertex_shader);
	if (FAILED(t_hr)) {
		m_p_vs_blob = NULL;
		MW_LOG_HR_ERROR(t_hr, "CreateVertexShader");
		return false;
	}

	//define the input layout
	D3D11_INPUT_ELEMENT_DESC t_arr_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT t_num_elements = ARRAYSIZE(t_arr_layout);

	// Create the input layout
	t_hr = m_p_device->CreateInputLayout(t_arr_layout, t_num_elements, m_p_vs_blob->GetBufferPointer(),
		m_p_vs_blob->GetBufferSize(), &m_p_vertex_layout);
	m_p_vs_blob = NULL;
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "CreateInputLayout");
		return false;
	}

	// Set the input layout
	m_p_device_context->IASetInputLayout(m_p_vertex_layout);

	// Compile the pixel shader
	t_hr = compile_shader_from_cs(t_ps.m_p_code, t_ps.m_n_code_len, "PS", "ps_4_0", &m_p_fs_blob);
	if (FAILED(t_hr))
	{
		MW_LOG_HR_ERROR(t_hr, "compile_shader_from_cs");
		return false;
	}

	// Create the pixel shader
	t_hr = m_p_device->CreatePixelShader(m_p_fs_blob->GetBufferPointer(), m_p_fs_blob->GetBufferSize(), NULL, &m_p_fragment_shader);
	m_p_fs_blob = NULL;
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "CreatePixelShader");
		return false;
	}

	/*if(t_dw_fourcc==MWFOURCC_BGR24|| \
	t_dw_fourcc==MWFOURCC_RGB24|| \
	t_dw_fourcc==MWFOURCC_IYU2|| \
	t_dw_fourcc==MWFOURCC_V308){
	m_n_stride=FOURCC_CalcMinStride(MWFOURCC_BGRA,m_n_width,4);
	int t_n_stride=FOURCC_CalcMinStride(t_dw_fourcc,m_n_width,4);
	m_n_size=FOURCC_CalcImageSize(t_dw_fourcc,m_n_width,m_n_height,t_n_stride);
	int t_n_size=FOURCC_CalcImageSize(MWFOURCC_BGRA,m_n_width,m_n_height,m_n_stride);
	m_n_rc_size =t_n_size;
	}
	else*/ {
		m_n_stride = FOURCC_CalcMinStride(t_dw_fourcc, m_n_width, 4);
		m_n_size = FOURCC_CalcImageSize(t_dw_fourcc, m_n_width, m_n_height, m_n_stride);
		m_n_rc_size = m_n_size;
	}

	// Create the render target texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_n_width;
	desc.Height = m_n_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	switch (t_dw_fourcc) {
	case MWFOURCC_YUY2:
	case MWFOURCC_YUYV:
	case MWFOURCC_YVYU:
		desc.Format = DXGI_FORMAT_G8R8_G8B8_UNORM;
		break;
	case MWFOURCC_UYVY:
	case MWFOURCC_VYUY:
		desc.Format = DXGI_FORMAT_R8G8_B8G8_UNORM;
		break;
	case MWFOURCC_Y210:
		desc.Format = DXGI_FORMAT_R16G16_UNORM;
		break;
	case MWFOURCC_BGRA:
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		break;
	case MWFOURCC_RGBA:
	case MWFOURCC_ABGR:
	case MWFOURCC_ARGB:
	case MWFOURCC_BGR24:
	case MWFOURCC_RGB24:
	case MWFOURCC_IYU2:
	case MWFOURCC_V308:
	case MWFOURCC_AYUV:
	case MWFOURCC_UYVA:
	case MWFOURCC_V408:
	case MWFOURCC_VYUA:
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case MWFOURCC_Y410:
	case MWFOURCC_RGB10:
	case MWFOURCC_BGR10:
		desc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		break;
	case MWFOURCC_GREY:
	case MWFOURCC_Y800:
	case MWFOURCC_Y8:
	case MWFOURCC_NV12:
	case MWFOURCC_NV21:
	case MWFOURCC_NV16:
	case MWFOURCC_NV61:
	case MWFOURCC_I422:
	case MWFOURCC_YV16:
	case MWFOURCC_I420:
	case MWFOURCC_IYUV:
	case MWFOURCC_YV12:
		desc.Format = DXGI_FORMAT_R8_UNORM;
		break;
	case MWFOURCC_Y16:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;
	case MWFOURCC_RGB16:
	case MWFOURCC_BGR16:
		desc.Format = DXGI_FORMAT_B5G6R5_UNORM;
		break;
	case MWFOURCC_P010:
	case MWFOURCC_P210:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;
	default:
		desc.Format = DXGI_FORMAT_B5G6R5_UNORM;
		break;
	}


	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
	// desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	m_p_texture_2d = NULL;
	t_hr = m_p_device->CreateTexture2D(&desc, NULL, &m_p_texture_2d);
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
		return false;
	}

	// Create the shader-resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;//1; //1;

	t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d, &srDesc, &m_p_shader_resource_view);
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
		return false;
	}

	if (t_dw_fourcc == MWFOURCC_NV12 ||
		t_dw_fourcc == MWFOURCC_NV21) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height / 2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8G8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;
		// desc2.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		// desc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
	}
	else if (t_dw_fourcc == MWFOURCC_YUY2 ||
		t_dw_fourcc == MWFOURCC_YUYV ||
		t_dw_fourcc == MWFOURCC_UYVY ||
		t_dw_fourcc == MWFOURCC_YVYU ||
		t_dw_fourcc == MWFOURCC_VYUY) {
		// yuy2 no need to create the second texture
#if 0
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		m_p_texture_2d_2.Reset();// = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2.Get(), &srDesc2, m_p_shader_resource_view_2.GetAddressOf());
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
#endif
	}
	else if (t_dw_fourcc == MWFOURCC_Y210)
	{
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R16G16B16A16_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;
	}
	else if (
		t_dw_fourcc == MWFOURCC_NV16 ||
		t_dw_fourcc == MWFOURCC_NV61) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8G8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		// desc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
	}
	else if (t_dw_fourcc == MWFOURCC_I422 || \
		t_dw_fourcc == MWFOURCC_YV16) {
		// Create the render target texture 2
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width;
		desc2.Height = m_n_height / 2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}

		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc3;
		ZeroMemory(&desc3, sizeof(desc3));
		desc3.Width = m_n_width;
		desc3.Height = m_n_height / 2;
		desc3.MipLevels = 1;
		desc3.ArraySize = 1;

		desc3.Format = DXGI_FORMAT_R8_UNORM;

		desc3.SampleDesc.Count = 1;
		desc3.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc3.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_3 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc3, NULL, &m_p_texture_2d_3);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc3;
		srDesc3.Format = desc3.Format;
		srDesc3.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc3.Texture2D.MostDetailedMip = 0;
		srDesc3.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_3, &srDesc3, &m_p_shader_resource_view_3);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
	}
	else if (t_dw_fourcc == MWFOURCC_I420 || \
		t_dw_fourcc == MWFOURCC_IYUV || \
		t_dw_fourcc == MWFOURCC_YV12) {
		// Create the render target texture 2
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width;
		desc2.Height = m_n_height / 4;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		// desc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}

		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc3;
		ZeroMemory(&desc3, sizeof(desc3));
		desc3.Width = m_n_width;
		desc3.Height = m_n_height / 4;
		desc3.MipLevels = 1;
		desc3.ArraySize = 1;

		desc3.Format = DXGI_FORMAT_R8_UNORM;

		desc3.SampleDesc.Count = 1;
		desc3.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc3.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_3 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc3, NULL, &m_p_texture_2d_3);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc3;
		srDesc3.Format = desc3.Format;
		srDesc3.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc3.Texture2D.MostDetailedMip = 0;
		srDesc3.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_3, &srDesc3, &m_p_shader_resource_view_3);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
	}
	else if (t_dw_fourcc == MWFOURCC_P010) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height / 2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R16G16_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //1;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
	}
	else if (t_dw_fourcc == MWFOURCC_P210) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R16G16_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		// desc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		m_p_texture_2d_2  = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
			return false;
		}

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = 1;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr)) {
			MW_LOG_HR_ERROR(t_hr, "CreateShaderResourceView");
			return false;
		}
	}

	ZeroMemory(&m_sampler_des, sizeof(m_sampler_des));
	// m_sampler_des.Filter = D3D11_FILTER_ANISOTROPIC;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_sampler_des.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_sampler_des.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_sampler_des.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; //D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;// D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.ComparisonFunc = D3D11_COMPARISON_ALWAYS;// D3D11_COMPARISON_NEVER;
	m_sampler_des.MinLOD = 0;
	m_sampler_des.MaxLOD = D3D11_FLOAT32_MAX;

	if (t_dw_fourcc == MWFOURCC_BGR24 || \
		t_dw_fourcc == MWFOURCC_RGB24 || \
		t_dw_fourcc == MWFOURCC_IYU2 || \
		t_dw_fourcc == MWFOURCC_V308)
	{
		m_sampler_des.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}

	t_hr = m_p_device->CreateSamplerState(&m_sampler_des, &m_p_sampler_state);
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "CreateSamplerState");
		return false;
	}


	m_p_device_context->VSSetShader(m_p_vertex_shader, NULL, 0);
	m_p_device_context->PSSetShader(m_p_fragment_shader, NULL, 0);

	s_rect t_rect;
	memset(&t_rect, 0, sizeof(s_rect));
	t_rect.m_cx = m_n_width;
	t_rect.m_cy = m_n_height;
	///////////////////////////////////////////////////////////////
	float csp_coeff[3][3] = { 0.0 };
	float csp_const[3] = { 0.0 };
	float lr = 0.0;
	float lg = 0.0;
	float lb = 0.0;

	m_csp = t_color_format;

	switch (m_csp)
	{
	case MW_CSP_BT_601:
		lr = 0.299, lg = 0.587, lb = 0.114;
		break;
	case MW_CSP_BT_709:
		lr = 0.2126, lg = 0.7152, lb = 0.0722;
		break;
	case MW_CSP_BT_2020:
		lr = 0.2627, lg = 0.6780, lb = 0.0593;
		break;
	}

	csp_coeff[0][0] = 1;
	csp_coeff[0][1] = 0;
	csp_coeff[0][2] = 2 * (1 - lr);
	csp_coeff[1][0] = 1;
	csp_coeff[1][1] = -2 * (1 - lb) * lb / lg;
	csp_coeff[1][2] = -2 * (1 - lr) * lr / lg;
	csp_coeff[2][0] = 1;
	csp_coeff[2][1] = 2 * (1 - lb);
	csp_coeff[2][2] = 0;

	yuvlevels yuvlev = { 0.0 };
	rgblevels rgblev = { 0.0 };
	rgblevels t_rgblev_tv = { 16.0 / 255.0, 235.0 / 255.0 };
	rgblevels t_rgblev_pc = { 0.0, 255.0 / 255.0 };
	yuvlevels t_lev_tv = { 16.0 / 255.0, 235.0 / 255.0, 240.0 / 255.0, 128.0 / 255.0 };
	yuvlevels t_lev_pc = { 0.0 , 255.0 / 255.0, 255.0 / 255.0, 128.0 / 255.0 };

	m_csp_lvl_in = input_range;
	m_csp_lvl_out = output_range;

	switch (m_csp_lvl_in)
	{
	case MW_CSP_LEVELS_AUTO:
		yuvlev = t_lev_pc;
		break;
	case MW_CSP_LEVELS_TV:
		yuvlev = t_lev_tv;
		break;
	case MW_CSP_LEVELS_PC:
		yuvlev = t_lev_pc;
		break;
	}
	switch (m_csp_lvl_out)
	{
	case MW_CSP_LEVELS_AUTO:
		yuvlev = t_lev_pc;
		break;
	case MW_CSP_LEVELS_TV:
		rgblev = t_rgblev_tv;
		break;
	case MW_CSP_LEVELS_PC:
		rgblev = t_rgblev_pc;
		break;
	}

	double ymul = (rgblev.max - rgblev.min) / (yuvlev.ymax - yuvlev.ymin);
	double cmul = (rgblev.max - rgblev.min) / (yuvlev.cmax - yuvlev.cmid) / 2.0;

	for (int i = 0; i < 3; i++) {
		csp_coeff[i][0] *= ymul;
		csp_coeff[i][1] *= cmul;
		csp_coeff[i][2] *= cmul;

		csp_const[i] = rgblev.min - csp_coeff[i][0] * yuvlev.ymin
			- (csp_coeff[i][1] + csp_coeff[i][2]) * yuvlev.cmid;
	}
	t_rect.m_csp_coeff_0_0 = csp_coeff[0][0];
	t_rect.m_csp_coeff_0_1 = csp_coeff[0][1];
	t_rect.m_csp_coeff_0_2 = csp_coeff[0][2];
	t_rect.m_csp_coeff_1_0 = csp_coeff[1][0];
	t_rect.m_csp_coeff_1_1 = csp_coeff[1][1];
	t_rect.m_csp_coeff_1_2 = csp_coeff[1][2];
	t_rect.m_csp_coeff_2_0 = csp_coeff[2][0];
	t_rect.m_csp_coeff_2_1 = csp_coeff[2][1];
	t_rect.m_csp_coeff_2_2 = csp_coeff[2][2];

	t_rect.m_csp_const_0 = csp_const[0];
	t_rect.m_csp_const_1 = csp_const[1];
	t_rect.m_csp_const_2 = csp_const[2];


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (t_dw_fourcc == MWFOURCC_GREY || \
		t_dw_fourcc == MWFOURCC_Y800 || \
		t_dw_fourcc == MWFOURCC_Y8 || \
		t_dw_fourcc == MWFOURCC_Y16 || \
		t_dw_fourcc == MWFOURCC_RGB16 || \
		t_dw_fourcc == MWFOURCC_BGR16 || \
		t_dw_fourcc == MWFOURCC_YUY2 || \
		t_dw_fourcc == MWFOURCC_YUYV || \
		t_dw_fourcc == MWFOURCC_BGR24 || \
		t_dw_fourcc == MWFOURCC_RGB24 || \
		t_dw_fourcc == MWFOURCC_UYVY || \
		t_dw_fourcc == MWFOURCC_YVYU || \
		t_dw_fourcc == MWFOURCC_VYUY || \
		t_dw_fourcc == MWFOURCC_NV12 || \
		t_dw_fourcc == MWFOURCC_NV21 || \
		t_dw_fourcc == MWFOURCC_NV16 || \
		t_dw_fourcc == MWFOURCC_NV61 || \
		t_dw_fourcc == MWFOURCC_I422 || \
		t_dw_fourcc == MWFOURCC_YV16 || \
		t_dw_fourcc == MWFOURCC_I420 || \
		t_dw_fourcc == MWFOURCC_IYUV || \
		t_dw_fourcc == MWFOURCC_YV12 || \
		t_dw_fourcc == MWFOURCC_P010 || \
		t_dw_fourcc == MWFOURCC_P210 || \
		t_dw_fourcc == MWFOURCC_IYU2 || \
		t_dw_fourcc == MWFOURCC_V308 || \
		t_dw_fourcc == MWFOURCC_AYUV || \
		t_dw_fourcc == MWFOURCC_UYVA || \
		t_dw_fourcc == MWFOURCC_V408 || \
		t_dw_fourcc == MWFOURCC_VYUA || \
		t_dw_fourcc == MWFOURCC_Y210 || \
		t_dw_fourcc == MWFOURCC_Y410) {
		m_p_device_context->PSSetConstantBuffers(0, 1, &m_d3db_rect);
		m_p_device_context->UpdateSubresource(m_d3db_rect, 0, NULL, &t_rect, 0, 0);
	}


	if (t_dw_fourcc == MWFOURCC_NV12 ||
		t_dw_fourcc == MWFOURCC_NV21 ||
		t_dw_fourcc == MWFOURCC_NV16 ||
		t_dw_fourcc == MWFOURCC_NV61 ||
		t_dw_fourcc == MWFOURCC_P010 ||
		t_dw_fourcc == MWFOURCC_P210 ||
		t_dw_fourcc == MWFOURCC_Y210
		// t_dw_fourcc == MWFOURCC_YUY2 ||
		// t_dw_fourcc == MWFOURCC_YUYV ||
		// t_dw_fourcc == MWFOURCC_UYVY ||
		// t_dw_fourcc == MWFOURCC_YVYU ||
		// t_dw_fourcc == MWFOURCC_VYUY
		) {
		m_p_device_context->PSSetShaderResources(0, 1, &m_p_shader_resource_view);
		m_p_device_context->PSSetShaderResources(1, 1, &m_p_shader_resource_view_2);
	}
	else if (t_dw_fourcc == MWFOURCC_I422 || \
		t_dw_fourcc == MWFOURCC_YV16 || \
		t_dw_fourcc == MWFOURCC_I420 || \
		t_dw_fourcc == MWFOURCC_IYUV || \
		t_dw_fourcc == MWFOURCC_YV12) {
		m_p_device_context->PSSetShaderResources(0, 1, &m_p_shader_resource_view);
		m_p_device_context->PSSetShaderResources(1, 1, &m_p_shader_resource_view_2);
		m_p_device_context->PSSetShaderResources(2, 1, &m_p_shader_resource_view_3);
	}
	else {
		m_p_device_context->PSSetShaderResources(0, 1, &m_p_shader_resource_view);
	}
	m_p_device_context->PSSetSamplers(0, 1, &m_p_sampler_state);
	m_b_set = true;
	return m_b_set;
	return false;
}

void MWDXRender::destory_render_with_fourcc(DWORD t_dw_fourcc)
{

}

bool MWDXRender::initialize(int t_n_width, int t_n_height, DWORD dwFourcc, bool t_b_reverse, bool t_b_mirror, HWND t_hwnd,
    mw_color_space_e t_color_format, mw_color_space_level_e t_input_range, mw_color_space_level_e t_output_range)
{
    bool t_b_ret = false;
    m_b_set = false;
    HRESULT t_hr = S_FALSE;

    m_n_width = t_n_width;
    m_n_height = t_n_height;
    m_dw_fourcc = dwFourcc;
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

#if USE_NEW_D3D11
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	t_hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
        featureLevels, ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &pDevice,
        &m_featurelevel,
        &pContext
    );

    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "D3D11CreateDevice");
        return false;
    }

	IDXGIDevice4* pDXGIDevice;
	t_hr = pDevice->QueryInterface(__uuidof(IDXGIDevice4), (void**)&pDXGIDevice);
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "QueryInterface for IDXGIDevice4");
        return false;
    }

    t_hr = pDXGIDevice->SetMaximumFrameLatency(1);
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "SetMaximumFrameLatency");
        return false;
    }

    // 3. »ñÈ¡ DXGI Adapter ºÍ Factory
    IDXGIAdapter* pAdapter;
    t_hr = pDXGIDevice->GetAdapter(&pAdapter);
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "GetAdapter");
        return false;
    }

    IDXGIFactory7* pFactory;
    t_hr = pAdapter->GetParent(IID_PPV_ARGS(&pFactory));
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "GetParent for IDXGIFactory");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = t_n_width;
    swapChainDesc.Height = t_n_height;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = m_nBufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    IDXGISwapChain1* pSwapChain1;
    t_hr = pFactory->CreateSwapChainForHwnd(
        pDevice,
        t_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &pSwapChain1
    );

    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "CreateSwapChainForHwnd");
        return false;
    }

	t_hr = pDevice->QueryInterface(__uuidof(ID3D11Device5), (void**)&m_p_device);
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "QueryInterface for ID3D11Device5");
        return false;
    }

	t_hr = pContext->QueryInterface(__uuidof(ID3D11DeviceContext3), (void**)&m_p_device_context);
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "QueryInterface for ID3D11DeviceContext4");
        return false;
    }

	t_hr = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&m_p_swap_chain);
    if (FAILED(t_hr)) {
        MW_LOG_HR_ERROR(t_hr, "QueryInterface for IDXGISwapChain4");
        return false;
    }

#else
    ZeroMemory(&m_sd, sizeof(m_sd));
    m_sd.BufferCount = m_nBufferCount;
    m_sd.BufferDesc.Width = t_n_width;
    m_sd.BufferDesc.Height = t_n_height;
    m_sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    m_sd.BufferDesc.RefreshRate.Numerator = 0;
    m_sd.BufferDesc.RefreshRate.Denominator = 1;
    m_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_sd.OutputWindow = t_hwnd;
    m_sd.SampleDesc.Count = 1;
    m_sd.SampleDesc.Quality = 0;
    m_sd.Windowed = TRUE;
    m_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    m_sd.Flags = 0;

    m_featurelevels = D3D_FEATURE_LEVEL_11_0;

    t_hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &m_sd,
        &m_p_swap_chain,
        &m_p_device,
        &m_featurelevels,
        &m_p_device_context);

    if (FAILED(t_hr)) {
    	MW_LOG_HR_ERROR(t_hr, "D3D11CreateDeviceAndSwapChain");
	    return false;
    }
#endif
    // m_p_render_buffer = NULL;
	m_p_render_buffer = NULL;
    t_hr = m_p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&m_p_render_buffer));
    if (FAILED(t_hr))
        return false;
#if USE_NEW_D3D11
	m_hLatency = m_p_swap_chain->GetFrameLatencyWaitableObject();
	if (!m_hLatency && m_bDebug) {
		MW_LOG("can not get frame latency waitable object");
	}
#endif
    t_hr = m_p_device->CreateRenderTargetView(m_p_render_buffer, NULL, &m_p_render_view);
    m_p_render_buffer = NULL;
    if (FAILED(t_hr))
        return false;

    m_p_device_context->OMSetRenderTargets(1, &m_p_render_view, NULL);


    ZeroMemory(&m_vp, sizeof(m_vp));
    m_vp.Width = (float)t_n_width;
    m_vp.Height = (float)t_n_height;
    m_vp.MinDepth = 0.0f;
    m_vp.MaxDepth = 1.0f;
    m_vp.TopLeftX = 0.0f;
    m_vp.TopLeftY = 0.0f;
    m_p_device_context->RSSetViewports(1, &m_vp);

#if HAS_VIDEO_PROCESSOR
    m_b_video_processor_supported = check_video_processor_support(dwFourcc);

    if (m_b_video_processor_supported) {
    	D3D11_TEXTURE2D_DESC input_tex_desc;
    	ZeroMemory(&input_tex_desc, sizeof(input_tex_desc));
    	input_tex_desc.Width = m_n_width;
    	input_tex_desc.Height = m_n_height;
    	input_tex_desc.MipLevels = 1;
    	input_tex_desc.ArraySize = 1;
    	input_tex_desc.Format = get_dxgi_format_from_fourcc(m_dw_fourcc);
    	input_tex_desc.SampleDesc.Count = 1;
    	input_tex_desc.SampleDesc.Quality = 0;
    	input_tex_desc.Usage = D3D11_USAGE_DEFAULT;
    	input_tex_desc.BindFlags = D3D11_BIND_DECODER | D3D11_BIND_SHADER_RESOURCE;
    	input_tex_desc.CPUAccessFlags = 0;
    	input_tex_desc.MiscFlags = 0;

    	HRESULT hr = m_p_device->CreateTexture2D(&input_tex_desc, NULL, &m_p_input_texture);
    	if (FAILED(hr)) {
    		MW_LOG_HR_ERROR(hr, "CreateTexture2D for input");
    		return false;
    	}

    	D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC input_view_desc = {};
    	input_view_desc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
    	input_view_desc.Texture2D.MipSlice = 0;
    	input_view_desc.Texture2D.ArraySlice = 0;

    	hr = m_p_video_device->CreateVideoProcessorInputView(
			m_p_input_texture.Get(),
			m_p_video_enum.Get(),
			&input_view_desc,
			&m_p_video_input_view);

    	if (FAILED(hr)) {
    		MW_LOG_HR_ERROR(hr, "CreateVideoProcessorInputView");
    		return false;
    	}

    	D3D11_TEXTURE2D_DESC output_tex_desc = {};
        output_tex_desc.Width = t_n_width;
        output_tex_desc.Height = t_n_height;
        output_tex_desc.MipLevels = 1;
        output_tex_desc.ArraySize = 1;
        output_tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        output_tex_desc.SampleDesc.Count = 1;
        output_tex_desc.SampleDesc.Quality = 0;
        output_tex_desc.Usage = D3D11_USAGE_DEFAULT;
        output_tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        output_tex_desc.CPUAccessFlags = 0;
        output_tex_desc.MiscFlags = 0;

        ID3D11Texture2D* p_output_texture = nullptr;
        t_hr = m_p_device->CreateTexture2D(&output_tex_desc, nullptr, &p_output_texture);
        if (FAILED(t_hr)) {
        	MW_LOG_HR_ERROR(t_hr, "CreateTexture2D");
            m_b_video_processor_supported = false;
        } else {
            D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC output_view_desc = {};
            output_view_desc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
            output_view_desc.Texture2D.MipSlice = 0;

            t_hr = m_p_video_device->CreateVideoProcessorOutputView(
                p_output_texture,
                m_p_video_enum.Get(),
                &output_view_desc,
                &m_p_video_output_view);

            if (FAILED(t_hr)) {
            	MW_LOG_HR_ERROR(t_hr, "CreateVideoProcessorOutputView");
                m_b_video_processor_supported = false;
            }
        	m_p_video_texture = p_output_texture;
        }

        if (m_b_video_processor_supported) {
        	m_csp = t_color_format;
        	m_csp_lvl_in = t_input_range;
        	m_csp_lvl_out = t_output_range;
            if (!create_video_processor()) {
                m_b_video_processor_supported = false;
            }
        }
    }

    // if video processor is not supported, create render with fourcc
    if (!m_b_video_processor_supported)
#endif
    {
        memset(m_arr_vex, 0, sizeof(XMFLOAT3) * 3);
        m_arr_vex[0] = XMFLOAT3(0.0f, 0.5f, 0.5f);
        m_arr_vex[1] = XMFLOAT3(0.5f, -0.5f, 0.5f);
        m_arr_vex[2] = XMFLOAT3(-0.5f, -0.5f, 0.5f);

        t_b_ret = create_render_with_fourcc(dwFourcc, t_b_reverse, t_b_mirror,
                                          t_color_format, t_input_range, t_output_range);

        if (!t_b_ret)
            return false;
    }


	// already init in create_render_with_fourcc
    //m_n_stride = FOURCC_CalcMinStride(m_dw_fourcc, m_n_width, 4);
    //m_n_size = FOURCC_CalcImageSize(m_dw_fourcc, m_n_width, m_n_height, m_n_stride);
    //m_n_rc_size = m_n_size;

	//
    // for (int i = 0; i < NUM_DATA; i++) {
    //     m_array_data[i] = new unsigned char[m_n_rc_size];
    //     if (m_array_data[i] != NULL) {
    //         memset(m_array_data[i], 0, m_n_rc_size);
    //         m_queue_data.push(m_array_data[i]);
    //     }
    // }

    m_b_set = true;
	MW_LOG("initialize success, width: %d, height: %d, fourcc %s: , color format: %d, input range: %d, output range: %d",
		t_n_width, t_n_height, fourcc2string(dwFourcc).c_str(), t_color_format, t_input_range, t_output_range);

    return m_b_set;
}

void MWDXRender::cleanup_device()
{
	if (m_hLatency) {
		CloseHandle(m_hLatency);
		m_hLatency = NULL;
	}
	if(m_p_device_context)
	{
		m_p_device_context->Release();
		m_p_device_context = NULL;
	}
	if(m_p_vertex_buffer)
	{
		m_p_vertex_buffer->Release();
		m_p_vertex_buffer = NULL;
	}
	if(m_p_vertex_layout)
	{
		m_p_vertex_layout->Release();
		m_p_vertex_layout = NULL;
	}
	if(m_p_vertex_shader)
	{
		m_p_vertex_shader->Release();
		m_p_vertex_shader = NULL;
	}
	if(m_p_sampler_state)
	{
		m_p_sampler_state->Release();
		m_p_sampler_state = NULL;
	}
	if(m_d3db_rect)
	{
		m_d3db_rect->Release();
		m_d3db_rect = NULL;
	}
	if(m_p_texture_2d)
	{
		m_p_texture_2d->Release();
		m_p_texture_2d = NULL;
	}
	if(m_p_shader_resource_view)
	{
		m_p_shader_resource_view->Release();
		m_p_shader_resource_view = NULL;
	}
	if(m_p_texture_2d_2)
	{
		m_p_texture_2d_2->Release();
		m_p_texture_2d_2 = NULL;
	}
	if(m_p_shader_resource_view_2)
	{
		m_p_shader_resource_view_2->Release();
		m_p_shader_resource_view_2 = NULL;
	}
	if(m_p_texture_2d_3)
	{
		m_p_texture_2d_3->Release();
		m_p_texture_2d_3 = NULL;
	}
	if(m_p_shader_resource_view_3)
	{
		m_p_shader_resource_view_3->Release();
		m_p_shader_resource_view_3 = NULL;
	}
	if(m_p_fragment_shader)
	{
		m_p_fragment_shader->Release();
		m_p_fragment_shader= NULL;
	}
	if(m_p_render_view)
	{
		m_p_render_view->Release();
		m_p_render_view = NULL;
	}
	if(m_p_swap_chain)
	{
		m_p_swap_chain->Release();
		m_p_swap_chain = NULL;
	}

#if HAS_VIDEO_PROCESSOR
	m_p_input_texture.Reset();
	m_p_video_input_view.Reset();
	m_p_video_texture.Reset();
	m_p_video_output_view.Reset();
	m_p_video_processor.Reset();
	m_p_video_enum.Reset();
	m_p_video_context.Reset();
	m_p_video_device.Reset();
#endif
	if(m_p_device_context)
	{
		m_p_device_context->Release();
		m_p_device_context = NULL;
	}
	if(m_p_device)
	{
		m_p_device->Release();
		m_p_device = NULL;
	}

}

bool MWDXRender::paint(unsigned char* data) {
	if (!data)
		return false;
#if HAS_VIDEO_PROCESSOR
	if (m_b_video_processor_supported) {
		return paint_with_video_processor(data);
	}
#endif
	// fallback to shader rendering
	return paint_with_shader(data);
}

void MWDXRender::output_debug(bool bDebug)
{
	m_bDebug = bDebug;
}

void MWDXRender::set_buffer_count(int bBufferCount)
{
	m_nBufferCount = bBufferCount;
}

BOOL MWDXRender::get_max_latency(UINT* pMaxLatency) {
#if USE_NEW_D3D11
	if (!m_p_swap_chain || !pMaxLatency) {
		return FALSE;
	}

	HRESULT t_hr = m_p_swap_chain->GetMaximumFrameLatency(pMaxLatency);
	return t_hr==S_OK ? TRUE : FALSE;
#else
	return FALSE;
#endif
}

BOOL MWDXRender::set_max_latency(UINT nMaxLatency) {
#if USE_NEW_D3D11
	if (!m_p_swap_chain || nMaxLatency <= 0) {
		return FALSE;
	}

	HRESULT t_hr = m_p_swap_chain->SetMaximumFrameLatency(nMaxLatency);

	return t_hr == S_OK ? TRUE : FALSE;
#else
	return FALSE;
#endif
}

bool MWDXRender::paint_with_shader(unsigned char*data)
{
	bool t_b_ret = true;
	HRESULT t_hr = S_OK;

	//clear buffer
	if (m_b_set == false)
		return false;

	if (m_hLatency) {
		if (WaitForSingleObject(m_hLatency, 5) == WAIT_TIMEOUT) {
			if (m_bDebug)
			{
				MW_LOG("swap chain is busy, skip rendering");
			}

			return false;
		}
	}
	m_p_device_context->OMSetRenderTargets(1, &m_p_render_view, NULL);

	float t_clearcolor[4] = { 1.0f,1.0f,0.0f,1.0f };
	m_p_device_context->ClearRenderTargetView(m_p_render_view, t_clearcolor);

	unsigned char* t_p_data = NULL;
	t_p_data = data;
	if (t_p_data == NULL)
		return false;

	if (m_dw_fourcc == MWFOURCC_NV12 || \
		m_dw_fourcc == MWFOURCC_NV21 || \
		m_dw_fourcc == MWFOURCC_NV16 || \
		m_dw_fourcc == MWFOURCC_NV61) {
		auto size = m_n_width * m_n_height;
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width, 0);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_2,  0, NULL, t_p_data + m_n_width*m_n_height, m_n_width, 0);
	}
	else if (m_dw_fourcc == MWFOURCC_I422 || \
		m_dw_fourcc == MWFOURCC_YV16) {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width, m_n_height);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_2,  0, NULL, t_p_data + m_n_width*m_n_height, m_n_width, m_n_height / 2);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_3, 0, NULL, t_p_data + m_n_width*m_n_height + m_n_width*m_n_height / 2, m_n_width, 0);
	}
	else if (m_dw_fourcc == MWFOURCC_I420 || \
		m_dw_fourcc == MWFOURCC_IYUV || \
		m_dw_fourcc == MWFOURCC_YV12) {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width, m_n_height);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_2,  0, NULL, t_p_data + m_n_width*m_n_height, m_n_width, m_n_height / 4);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_3, 0, NULL, t_p_data + m_n_width*m_n_height + m_n_width*m_n_height / 4, m_n_width, 0);
	}
	else if (m_dw_fourcc == MWFOURCC_P010 || \
		m_dw_fourcc == MWFOURCC_P210) {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width * 2, m_n_width * 2 * m_n_height);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_2,  0, NULL, t_p_data + m_n_width*m_n_height * 2, m_n_width * 2, 0);
	}
	else if (m_dw_fourcc == MWFOURCC_YUY2 ||
		m_dw_fourcc == MWFOURCC_YUYV ||
		m_dw_fourcc == MWFOURCC_UYVY ||
		m_dw_fourcc == MWFOURCC_YVYU ||
		m_dw_fourcc == MWFOURCC_VYUY) {
		// MAP_AND_COPY(m_p_texture_2d, t_p_data, m_n_stride, m_n_height);
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_stride, 0);
		// m_p_device_context->UpdateSubresource(m_p_texture_2d_2.Get(),  0, NULL, t_p_data, m_n_stride, 0);
	}
	else if (m_dw_fourcc == MWFOURCC_Y210)
	{
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_stride, 0);
		m_p_device_context->UpdateSubresource(m_p_texture_2d_2, 0, NULL, t_p_data, m_n_stride, 0);
	}
	else if (m_dw_fourcc == MWFOURCC_BGR24 || \
		m_dw_fourcc == MWFOURCC_RGB24) {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_stride, 0);
	} else {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_stride, 0);
	}

	m_p_device_context->Draw(4, 0);
	t_hr = m_p_swap_chain->Present(1, 0);
	if (FAILED(t_hr)) {
		MW_LOG_HR_ERROR(t_hr, "Present");
		t_b_ret = false;
	}
	return t_b_ret;
}
#if HAS_VIDEO_PROCESSOR
DXGI_FORMAT MWDXRender::get_dxgi_format_from_fourcc(DWORD fourcc)
{
	switch (fourcc) {
		case MWFOURCC_NV12:
			return DXGI_FORMAT_NV12;
		case MWFOURCC_YUY2:
			return DXGI_FORMAT_YUY2;
		case MWFOURCC_RGBA:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case MWFOURCC_BGRA:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		default:
			return DXGI_FORMAT_UNKNOWN;
	}
}


void MWDXRender::LogVideoProcessorContent() {
	if (m_p_video_input_view) {
		D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC input_desc;
		m_p_video_input_view->GetDesc(&input_desc);
		MW_LOG("Input view dimension: %d, MipSlice: %d",
			input_desc.ViewDimension,
			input_desc.Texture2D.MipSlice);
	}

	if (m_p_video_output_view) {
		D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC output_desc;
		m_p_video_output_view->GetDesc(&output_desc);
		MW_LOG("Output view dimension: %d, MipSlice: %d",
			output_desc.ViewDimension,
			output_desc.Texture2D.MipSlice);
	}
}

bool MWDXRender::paint_with_video_processor(unsigned char* data) {
    if (!data || !m_p_input_texture || !m_p_video_input_view)
        return false;

	HRESULT hr;

    m_p_device_context->UpdateSubresource(
        m_p_input_texture.Get(),
        0,
        NULL,
        data,
        m_n_stride,
        0);

	D3D11_VIDEO_PROCESSOR_STREAM stream = {0};
	stream.Enable = TRUE;
	stream.pInputSurface = m_p_video_input_view.Get();

	hr = m_p_video_context->VideoProcessorBlt(
		m_p_video_processor.Get(),
		m_p_video_output_view.Get(),
		0, // OutputFrame
		1, // StreamCount
		&stream);

	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "VideoProcessorBlt");
		return false;
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "GetBuffer");
		return false;
	}

	m_p_device_context->CopyResource(pBackBuffer, m_p_video_texture.Get());
	SAFE_RELEASE(pBackBuffer);

	hr = (m_p_swap_chain->Present(1, 0));
	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "Present");
		return false;
	}
	return true;
}


bool MWDXRender::check_video_processor_support(DWORD t_dw_fourcc)
{
	if (!m_p_device)
		return false;

	char value[256];
	bool enable_video_process = true;
	if (GetEnvironmentVariableA("__ENABLE_VIDEO_PROCESSOR", value, sizeof(value)) > 0) {
		enable_video_process = (strcmp(value, "1") == 0);
	}
	if (!enable_video_process) {
		MW_LOG("disable video processor by __ENABLE_VIDEO_PROCESSOR=%s", value);
		return false;
	}
	HRESULT hr = m_p_device->QueryInterface(__uuidof(ID3D11VideoDevice), (void**)&m_p_video_device);
	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "QueryInterface ID3D11VideoDevice");
		return false;
	}

	hr = m_p_device_context->QueryInterface(__uuidof(ID3D11VideoContext), (void**)&m_p_video_context);
	if (FAILED(hr)) {
		// SAFE_RELEASE(m_p_video_device);
		m_p_video_device.Reset();
		MW_LOG_HR_ERROR(hr, "QueryInterface ID3D11VideoContext");
		return false;
	}

	D3D11_VIDEO_PROCESSOR_CONTENT_DESC content_desc = {};
	content_desc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
	content_desc.InputWidth = m_n_width;
	content_desc.InputHeight = m_n_height;
	content_desc.OutputWidth = m_n_width;
	content_desc.OutputHeight = m_n_height;
	content_desc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

	hr = m_p_video_device->CreateVideoProcessorEnumerator(&content_desc, &m_p_video_enum);
	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "CreateVideoProcessorEnumerator");
		m_p_video_context.Reset();
		m_p_video_device.Reset();
		return false;
	}

	UINT flags;
	DXGI_FORMAT input_format;
	switch (t_dw_fourcc) {
		case MWFOURCC_NV12:
			input_format = DXGI_FORMAT_NV12;
		break;
		// please add more supported fourcc here
		default:
			m_p_video_enum.Reset();
			m_p_video_context.Reset();
			m_p_video_device.Reset();
			MW_LOG("unsupported fourcc: %d", t_dw_fourcc);
		return false;
	}

	hr = m_p_video_enum->CheckVideoProcessorFormat(input_format, &flags);
	if (FAILED(hr) || !(flags & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT)) {
		m_p_video_enum.Reset();
		m_p_video_context.Reset();
		m_p_video_device.Reset();
		MW_LOG_HR_ERROR(hr, "CheckVideoProcessorFormat input");
		return false;
	}
	hr = m_p_video_enum->CheckVideoProcessorFormat(DXGI_FORMAT_B8G8R8A8_UNORM, &flags);
	if (FAILED(hr) || !(flags & D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT)) {
		m_p_video_enum.Reset();
		m_p_video_context.Reset();
		m_p_video_device.Reset();
		MW_LOG_HR_ERROR(hr, "CheckVideoProcessorFormat output bgra");
		return false;
	}
	MW_LOG("create video processor success");
	return true;
}

void LogVideoProcessorCaps(const D3D11_VIDEO_PROCESSOR_CAPS& caps) {
    MW_LOG("Video Processor Capabilities:");

    MW_LOG("  Device Caps: 0x%08X", caps.DeviceCaps);
    if (caps.DeviceCaps & D3D11_VIDEO_PROCESSOR_DEVICE_CAPS_LINEAR_SPACE)
        MW_LOG("    - Linear Space");
    if (caps.DeviceCaps & D3D11_VIDEO_PROCESSOR_DEVICE_CAPS_xvYCC)
        MW_LOG("    - xvYCC");
    if (caps.DeviceCaps & D3D11_VIDEO_PROCESSOR_DEVICE_CAPS_RGB_RANGE_CONVERSION)
        MW_LOG("    - RGB Range Conversion");
    if (caps.DeviceCaps & D3D11_VIDEO_PROCESSOR_DEVICE_CAPS_YCbCr_MATRIX_CONVERSION)
        MW_LOG("    - YCbCr Matrix Conversion");

    MW_LOG("  Feature Caps: 0x%08X", caps.FeatureCaps);
    if (caps.FeatureCaps & D3D11_VIDEO_PROCESSOR_FEATURE_CAPS_ALPHA_FILL)
        MW_LOG("    - Alpha Fill");
    if (caps.FeatureCaps & D3D11_VIDEO_PROCESSOR_FEATURE_CAPS_ALPHA_PALETTE)
        MW_LOG("    - Alpha Palette");
    if (caps.FeatureCaps & D3D11_VIDEO_PROCESSOR_FEATURE_CAPS_LEGACY)
        MW_LOG("    - Legacy");
    if (caps.FeatureCaps & D3D11_VIDEO_PROCESSOR_FEATURE_CAPS_STEREO)
        MW_LOG("    - Stereo");
    if (caps.FeatureCaps & D3D11_VIDEO_PROCESSOR_FEATURE_CAPS_ROTATION)
        MW_LOG("    - Rotation");
    if (caps.FeatureCaps & D3D11_VIDEO_PROCESSOR_FEATURE_CAPS_SHADER_USAGE)
        MW_LOG("    - Shader Usage");

    MW_LOG("  Filter Caps: 0x%08X", caps.FilterCaps);
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_BRIGHTNESS)
        MW_LOG("    - Brightness");
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_CONTRAST)
        MW_LOG("    - Contrast");
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_HUE)
        MW_LOG("    - Hue");
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_SATURATION)
        MW_LOG("    - Saturation");
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_NOISE_REDUCTION)
        MW_LOG("    - Noise Reduction");
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_EDGE_ENHANCEMENT)
        MW_LOG("    - Edge Enhancement");
    if (caps.FilterCaps & D3D11_VIDEO_PROCESSOR_FILTER_CAPS_ANAMORPHIC_SCALING)
        MW_LOG("    - Anamorphic Scaling");

    MW_LOG("  Input Format Caps: 0x%08X", caps.InputFormatCaps);

    MW_LOG("  Auto Stream Caps: 0x%08X", caps.AutoStreamCaps);

    MW_LOG("  Stereo Caps: 0x%08X", caps.StereoCaps);

    MW_LOG("  Rate Conversion Caps Count: %u", caps.RateConversionCapsCount);

    MW_LOG("  Stream Information:");
    MW_LOG("    - Max Input Streams: %u", caps.MaxInputStreams);
    MW_LOG("    - Max Stream States: %u", caps.MaxStreamStates);
}

bool MWDXRender::create_video_processor() {
	if (!m_p_video_enum || !m_p_video_device)
		return false;

	D3D11_VIDEO_PROCESSOR_CAPS caps;
	HRESULT hr = m_p_video_enum->GetVideoProcessorCaps(&caps);
	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "GetVideoProcessorCaps");
		return false;
	}

	LogVideoProcessorCaps(caps);

	hr = m_p_video_device->CreateVideoProcessor(
		m_p_video_enum.Get(),
		0,
		&m_p_video_processor);
	if (FAILED(hr)) {
		MW_LOG_HR_ERROR(hr, "CreateVideoProcessor");
		return false;
	}

	D3D11_VIDEO_PROCESSOR_COLOR_SPACE input_color_space = {};
	input_color_space.Usage = 0;
	input_color_space.RGB_Range = (m_csp_lvl_in == MW_CSP_LEVELS_PC) ? 0 : 1;
	input_color_space.YCbCr_Matrix = (m_csp == MW_CSP_BT_601) ? 0 : 1;
	input_color_space.YCbCr_xvYCC = 0;
	input_color_space.Nominal_Range = (m_csp_lvl_in == MW_CSP_LEVELS_PC) ? 0 : 1;
	MW_LOG("Input color space settings:");
	MW_LOG("RGB_Range: %d, YCbCr_Matrix: %d, Nominal_Range: %d",
		input_color_space.RGB_Range,
		input_color_space.YCbCr_Matrix,
		input_color_space.Nominal_Range);
	m_p_video_context->VideoProcessorSetStreamColorSpace(
		m_p_video_processor.Get(),
		0,  // StreamIndex
		&input_color_space);

	D3D11_VIDEO_PROCESSOR_COLOR_SPACE output_color_space = {};
	output_color_space.Usage = 0;
	output_color_space.RGB_Range = (m_csp_lvl_out == MW_CSP_LEVELS_PC) ? 0 : 1;
	output_color_space.YCbCr_Matrix = (m_csp == MW_CSP_BT_601) ? 0 : 1;
	output_color_space.YCbCr_xvYCC = 0;
	output_color_space.Nominal_Range = (m_csp_lvl_out == MW_CSP_LEVELS_PC) ? 0 : 1;

	m_p_video_context->VideoProcessorSetOutputColorSpace(
		m_p_video_processor.Get(),
		&output_color_space);

	D3D11_VIDEO_COLOR backgroundColor = {};
	backgroundColor.RGBA.A = 1.0f;
	backgroundColor.RGBA.R = 0.0f;
	backgroundColor.RGBA.G = 0.0f;
	backgroundColor.RGBA.B = 1.0f;

	m_p_video_context->VideoProcessorSetOutputBackgroundColor(
		m_p_video_processor.Get(),
		FALSE,
		&backgroundColor
	);
	return true;
}
#endif
