#ifndef MWDXRENDERSHADER_H
#define MWDXRENDERSHADER_H


unsigned char g_cs_vs_grey[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";

unsigned char g_cs_ps_grey[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.5f;\n"
	"float v=0.5f;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_y16[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";
unsigned char g_cs_ps_y16[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.5f;\n"
	"float v=0.5f;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_rgb16[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";;
unsigned char g_cs_ps_rgb16[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"Texture2D txDiffuse;\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float r= txDiffuse.Sample(samLinear, input.Tex).b;\n"
	"float g= txDiffuse.Sample(samLinear, input.Tex).g;\n"
	"float b= txDiffuse.Sample(samLinear, input.Tex).r;\n"
	"float a= 1.0;\n"
	"return float4(r,g,b,a);\n"
	"}";

unsigned char g_cs_vs_bgr16[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";;
unsigned char g_cs_ps_bgr16[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"Texture2D txDiffuse;\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"float b= txDiffuse.Sample(samLinear, input.Tex).b;\n"
	"float g= txDiffuse.Sample(samLinear, input.Tex).g;\n"
	"float r= txDiffuse.Sample(samLinear, input.Tex).r;\n"
	"float a= 1.0;\n"
	"return float4(r,g,b,a);\n"
	"}";

unsigned char g_cs_vs_yuy2[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_yuy2[] = "struct VS_INPUT\
{\
	float4 Pos : POSITION;\
	float2 Tex : TEXCOORD0;\
};\
struct PS_INPUT {\
	float4 Pos : SV_POSITION;\
	float2 Tex : TEXCOORD0;\
};\
\
cbuffer cbNeverChanges : register(b0)\
{\
    	int m_cx;\
		int m_cy;\
		int t_temp;\
		int t_temp2;\
		float m_csp_coeff_0_0;\
		float m_csp_coeff_0_1;\
		float m_csp_coeff_0_2;\
		float m_csp_coeff_1_0;\
		float m_csp_coeff_1_1;\
		float m_csp_coeff_1_2;\
		float m_csp_coeff_2_0;\
		float m_csp_coeff_2_1;\
		float m_csp_coeff_2_2;\
		float m_csp_const_0;\
		float m_csp_const_1;\
		float m_csp_const_2;\
};\
\
static const float3 YUV_OFFSET = float3(0.0625, 0.5, 0.5);\
\
Texture2D txDiffuse   : register(t0);\
SamplerState samLinear: register(s0);\
\
float4 PS(PS_INPUT input) : SV_Target\
{\
	float3x3 coeffs = float3x3(\
        m_csp_coeff_0_0, m_csp_coeff_0_1, m_csp_coeff_0_2,\
        m_csp_coeff_1_0, m_csp_coeff_1_1, m_csp_coeff_1_2,\
        m_csp_coeff_2_0, m_csp_coeff_2_1, m_csp_coeff_2_2\
    );\
	float3 offset = float3(m_csp_const_0, m_csp_const_1, m_csp_const_2);\
	float4 yuy2 = txDiffuse.Sample(samLinear, input.Tex);\
    float3 yuv = float3(yuy2.y, yuy2.x, yuy2.z) ;\
	float3 rgb = mul(coeffs, yuv) + offset;\
    return float4(saturate(rgb), 1.0);\
}";
unsigned char g_cs_ps_yuy2t[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   cbuffer cbNeverChanges : register(b0) \
							   { \
							   int m_cx; \
							   int m_cy; \
							   int t_temp; \
							   int t_temp2; \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   int t_n_y = m_cy * input.Tex.y; \
							   int t_f_x = floor(float(m_cx) *input.Tex.x); \
							   int t_n_index = t_f_x % 2; \
							   if (t_n_index == 0) { \
							   float t_f_next = input.Tex.x + (1.0 / m_cx); \
							   float2 t_tex; \
							   t_tex.x = t_f_next; \
							   t_tex.y = input.Tex.y; \
							   float y = txDiffuse.Sample(samLinear, input.Tex).x; \
							   float u = txDiffuse.Sample(samLinear, input.Tex).y; \
							   float v = txDiffuse.Sample(samLinear, t_tex).y; \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   } \
							   else if (t_n_index == 1) { \
							   float t_f_next = input.Tex.x - (1.0 / m_cx); \
							   float2 t_tex; \
							   t_tex.x = t_f_next; \
							   t_tex.y = input.Tex.y; \
							   float y = txDiffuse.Sample(samLinear, input.Tex).x; \
							   float u = txDiffuse.Sample(samLinear, t_tex).y; \
							   float v = txDiffuse.Sample(samLinear, input.Tex).y; \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   } \
							   }";


unsigned char g_cs_vs_bgra[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_bgra[] = "\
struct VS_INPUT\
{\
    float4 Pos : POSITION;\
    float2 Tex : TEXCOORD0;\
};\
\
struct PS_INPUT\
{\
    float4 Pos : SV_POSITION;\
    float2 Tex : TEXCOORD0;\
};\
\
cbuffer cbNeverChanges : register(b0)\
{\
    	int m_cx;\
		int m_cy;\
		int t_temp;\
		int t_temp2;\
		float m_csp_coeff_0_0;\
		float m_csp_coeff_0_1;\
		float m_csp_coeff_0_2;\
		float m_csp_coeff_1_0;\
		float m_csp_coeff_1_1;\
		float m_csp_coeff_1_2;\
		float m_csp_coeff_2_0;\
		float m_csp_coeff_2_1;\
		float m_csp_coeff_2_2;\
		float m_csp_const_0;\
		float m_csp_const_1;\
		float m_csp_const_2;\
};\
\
Texture2D txDiffuse : register(t0);\
SamplerState samLinear : register(s0);\
\
float4 PS(PS_INPUT input) : SV_Target\
{\
    float4 color = txDiffuse.Sample(samLinear, input.Tex);\
	return color;\
}";

unsigned char g_cs_vs_argb[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_argb[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float a= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float r= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float b= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_abgr[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_abgr[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float a= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float b= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float r= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_rgba[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_rgba[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   Texture2D txDiffuse; \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float r= txDiffuse.Sample(samLinear, input.Tex).x; \
							   float g= txDiffuse.Sample(samLinear, input.Tex).y; \
							   float b= txDiffuse.Sample(samLinear, input.Tex).z; \
							   float a= txDiffuse.Sample(samLinear, input.Tex).w; \
							   return float4(r,g,b,a); \
							   }";

unsigned char g_cs_vs_bgr24[]="struct VS_INPUT\n \
							  { \n \
							  float4 Pos : POSITION;\n \
							  float2 Tex : TEXCOORD0;\n \
							  };\n \
							  struct PS_INPUT \n \
							  {\n \
							  float4 Pos : SV_POSITION;\n \
							  float2 Tex : TEXCOORD0;\n \
							  };\n \
							  PS_INPUT VS(VS_INPUT input)\n \
							  {\n \
							  PS_INPUT output;\n \
							  output.Pos = input.Pos;\n \
							  output.Tex = input.Tex;\n \
							  return output;\n \
							  }";
unsigned char g_cs_ps_bgr24[] = "struct VS_INPUT \
								{ \
								float4 Pos : POSITION; \
								float2 Tex : TEXCOORD0; \
								}; \
								struct PS_INPUT \
								{ \
								float4 Pos : SV_POSITION; \
								float2 Tex : TEXCOORD0; \
								}; \
								cbuffer cbNeverChanges : register(b0) \
								{ \
								int m_cx; \
								int m_cy; \
								};\
								Texture2D txDiffuse; \
								SamplerState samLinear; \
								float4 PS(PS_INPUT input) : SV_Target \
								{ \
								int nIdx = floor(input.Tex.x * float(m_cx));\
								float2 fLocate;\
								float b, g, r;\
								\
								if(nIdx % 4 == 0)\
								{\
								fLocate.x = input.Tex.x*3/4;\
								fLocate.y = input.Tex.y;\
								\
								b = txDiffuse.Sample(samLinear, fLocate).x;\
								g = txDiffuse.Sample(samLinear, fLocate).y;\
								r = txDiffuse.Sample(samLinear, fLocate).z;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else if(nIdx % 4 == 1)\
								{\
								fLocate.x = (input.Tex.x-1.0/m_cx)*3/4;\
								fLocate.y = input.Tex.y;\
								b = txDiffuse.Sample(samLinear, fLocate).w;\
								\
								fLocate.x = (input.Tex.x-1.0/m_cx)*3/4+1.0/m_cx;\
								fLocate.y = input.Tex.y;\
								g = txDiffuse.Sample(samLinear, fLocate).x;\
								r = txDiffuse.Sample(samLinear, fLocate).y;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else if(nIdx % 4 == 2)\
								{\
								fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+1.0/m_cx;\
								fLocate.y = input.Tex.y;\
								b = txDiffuse.Sample(samLinear, fLocate).z;\
								g = txDiffuse.Sample(samLinear, fLocate).w;\
								\
								fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+2.0/m_cx;\
								fLocate.y = input.Tex.y;\
								r = txDiffuse.Sample(samLinear, fLocate).x;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else\
								{\
								fLocate.x = (input.Tex.x-3.0/m_cx)*3/4+2.0/m_cx; \
								fLocate.y = input.Tex.y; \
								\
								b = txDiffuse.Sample(samLinear, fLocate).y; \
								g = txDiffuse.Sample(samLinear, fLocate).z; \
								r = txDiffuse.Sample(samLinear, fLocate).w; \
								\
								return float4(r, g, b, 1.0); \
								}\
								}";

unsigned char g_cs_vs_rgb24[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";

unsigned char g_cs_ps_rgb24[] = "struct VS_INPUT \
								{ \
								float4 Pos : POSITION; \
								float2 Tex : TEXCOORD0; \
								}; \
								struct PS_INPUT \
								{ \
								float4 Pos : SV_POSITION; \
								float2 Tex : TEXCOORD0; \
								}; \
								cbuffer cbNeverChanges : register(b0) \
								{ \
								int m_cx; \
								int m_cy; \
								};\
								Texture2D txDiffuse; \
								SamplerState samLinear; \
								float4 PS(PS_INPUT input) : SV_Target \
								{ \
								int nIdx = floor(input.Tex.x * float(m_cx));\
								float2 fLocate;\
								float b, g, r;\
								\
								if(nIdx % 4 == 0)\
								{\
								fLocate.x = input.Tex.x*3/4;\
								fLocate.y = input.Tex.y;\
								\
								r = txDiffuse.Sample(samLinear, fLocate).x;\
								g = txDiffuse.Sample(samLinear, fLocate).y;\
								b = txDiffuse.Sample(samLinear, fLocate).z;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else if(nIdx % 4 == 1)\
								{\
								fLocate.x = (input.Tex.x-1.0/m_cx)*3/4;\
								fLocate.y = input.Tex.y;\
								r = txDiffuse.Sample(samLinear, fLocate).w;\
								\
								fLocate.x = (input.Tex.x-1.0/m_cx)*3/4+1.0/m_cx;\
								fLocate.y = input.Tex.y;\
								g = txDiffuse.Sample(samLinear, fLocate).x;\
								b = txDiffuse.Sample(samLinear, fLocate).y;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else if(nIdx % 4 == 2)\
								{\
								fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+1.0/m_cx;\
								fLocate.y = input.Tex.y;\
								r = txDiffuse.Sample(samLinear, fLocate).z;\
								g = txDiffuse.Sample(samLinear, fLocate).w;\
								\
								fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+2.0/m_cx;\
								fLocate.y = input.Tex.y;\
								b = txDiffuse.Sample(samLinear, fLocate).x;\
								\
								return float4(r, g, b, 1.0);\
								}\
								else\
								{\
								fLocate.x = (input.Tex.x-3.0/m_cx)*3/4+2.0/m_cx; \
								fLocate.y = input.Tex.y; \
								\
								r = txDiffuse.Sample(samLinear, fLocate).y; \
								g = txDiffuse.Sample(samLinear, fLocate).z; \
								b = txDiffuse.Sample(samLinear, fLocate).w; \
								\
								return float4(r, g, b, 1.0); \
								}\
								}";
unsigned char g_cs_vs_uyvy[]="struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";


unsigned char g_cs_ps_uyvy[] = "struct VS_INPUT\
{\
	float4 Pos : POSITION;\
	float2 Tex : TEXCOORD0;\
};\
struct PS_INPUT {\
	float4 Pos : SV_POSITION;\
	float2 Tex : TEXCOORD0;\
};\
\
cbuffer cbNeverChanges : register(b0)\
{\
    	int m_cx;\
		int m_cy;\
		int t_temp;\
		int t_temp2;\
		float m_csp_coeff_0_0;\
		float m_csp_coeff_0_1;\
		float m_csp_coeff_0_2;\
		float m_csp_coeff_1_0;\
		float m_csp_coeff_1_1;\
		float m_csp_coeff_1_2;\
		float m_csp_coeff_2_0;\
		float m_csp_coeff_2_1;\
		float m_csp_coeff_2_2;\
		float m_csp_const_0;\
		float m_csp_const_1;\
		float m_csp_const_2;\
};\
\
static const float3 YUV_OFFSET = float3(0.0625, 0.5, 0.5);\
\
Texture2D txDiffuse   : register(t0);\
SamplerState samLinear: register(s0);\
\
float4 PS(PS_INPUT input) : SV_Target\
{\
	float3x3 coeffs = float3x3(\
        m_csp_coeff_0_0, m_csp_coeff_0_1, m_csp_coeff_0_2,\
        m_csp_coeff_1_0, m_csp_coeff_1_1, m_csp_coeff_1_2,\
        m_csp_coeff_2_0, m_csp_coeff_2_1, m_csp_coeff_2_2\
    );\
	float3 offset = float3(m_csp_const_0, m_csp_const_1, m_csp_const_2);\
	float4 yuy2 = txDiffuse.Sample(samLinear, input.Tex);\
    float3 yuv = float3(yuy2.y, yuy2.x, yuy2.z) ;\
	float3 rgb = mul(coeffs, yuv) + offset;\
    return float4(saturate(rgb), 1.0);\
}";

unsigned char g_cs_ps_uyvyt[]="struct VS_INPUT\n \
							 {\n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT\n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 cbuffer cbNeverChanges : register(b0)\n \
							 {\n \
							 int m_cx;\n \
							 int m_cy;\n \
							 int t_temp;\n \
							 int t_temp2;\n \
							 float m_csp_coeff_0_0;\n \
							 float m_csp_coeff_0_1;\n \
							 float m_csp_coeff_0_2;\n \
							 float m_csp_coeff_1_0;\n \
							 float m_csp_coeff_1_1;\n \
							 float m_csp_coeff_1_2;\n \
							 float m_csp_coeff_2_0;\n \
							 float m_csp_coeff_2_1;\n \
							 float m_csp_coeff_2_2;\n \
							 float m_csp_const_0;\n \
							 float m_csp_const_1;\n \
							 float m_csp_const_2;\n \
							 };\n \
							 Texture2D txDiffuse;\n \
							 SamplerState samLinear;\n \
							 float4 PS(PS_INPUT input) : SV_Target\n \
							 {\n \
							 int t_n_y = m_cy * input.Tex.y;\n \
							 int t_f_x = floor(m_cx *input.Tex.x);\n \
							 int t_n_index = floor(t_f_x) % 2;\n \
							 if (t_n_index == 0) {\n \
							 float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float u = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float v = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 else if (t_n_index == 1) {\n \
							 float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float u = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float v = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 }\n";

unsigned char g_cs_vs_yvyu[]="struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_yvyu[] = "struct VS_INPUT\
{\
	float4 Pos : POSITION;\
	float2 Tex : TEXCOORD0;\
};\
struct PS_INPUT {\
	float4 Pos : SV_POSITION;\
	float2 Tex : TEXCOORD0;\
};\
\
cbuffer cbNeverChanges : register(b0)\
{\
    	int m_cx;\
		int m_cy;\
		int t_temp;\
		int t_temp2;\
		float m_csp_coeff_0_0;\
		float m_csp_coeff_0_1;\
		float m_csp_coeff_0_2;\
		float m_csp_coeff_1_0;\
		float m_csp_coeff_1_1;\
		float m_csp_coeff_1_2;\
		float m_csp_coeff_2_0;\
		float m_csp_coeff_2_1;\
		float m_csp_coeff_2_2;\
		float m_csp_const_0;\
		float m_csp_const_1;\
		float m_csp_const_2;\
};\
\
static const float3 YUV_OFFSET = float3(0.0625, 0.5, 0.5);\
\
Texture2D txDiffuse   : register(t0);\
SamplerState samLinear: register(s0);\
\
float4 PS(PS_INPUT input) : SV_Target\
{\
	float3x3 coeffs = float3x3(\
        m_csp_coeff_0_0, m_csp_coeff_0_1, m_csp_coeff_0_2,\
        m_csp_coeff_1_0, m_csp_coeff_1_1, m_csp_coeff_1_2,\
        m_csp_coeff_2_0, m_csp_coeff_2_1, m_csp_coeff_2_2\
    );\
	float3 offset = float3(m_csp_const_0, m_csp_const_1, m_csp_const_2);\
	float4 yuy2 = txDiffuse.Sample(samLinear, input.Tex);\
    float3 yuv = float3(yuy2.y, yuy2.z, yuy2.x) ;\
	float3 rgb = mul(coeffs, yuv) + offset;\
    return float4(saturate(rgb), 1.0);\
}";

unsigned char g_cs_ps_yvyut[]="struct VS_INPUT\n \
							 {\n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT\n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 cbuffer cbNeverChanges : register(b0)\n \
							 {\n \
							 int m_cx;\n \
							 int m_cy;\n \
							 int t_temp;\n \
							 int t_temp2;\n \
							 float m_csp_coeff_0_0;\n \
							 float m_csp_coeff_0_1;\n \
							 float m_csp_coeff_0_2;\n \
							 float m_csp_coeff_1_0;\n \
							 float m_csp_coeff_1_1;\n \
							 float m_csp_coeff_1_2;\n \
							 float m_csp_coeff_2_0;\n \
							 float m_csp_coeff_2_1;\n \
							 float m_csp_coeff_2_2;\n \
							 float m_csp_const_0;\n \
							 float m_csp_const_1;\n \
							 float m_csp_const_2;\n \
							 };\n \
							 Texture2D txDiffuse;\n \
							 SamplerState samLinear;\n \
							 float4 PS(PS_INPUT input) : SV_Target\n \
							 {\n \
							 int t_n_y = m_cy * input.Tex.y;\n \
							 int t_f_x = floor(m_cx *input.Tex.x);\n \
							 int t_n_index = floor(t_f_x) % 2;\n \
							 if (t_n_index == 0) {\n \
							 float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float v = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float u = txDiffuse.Sample(samLinear, t_tex).y;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 else if (t_n_index == 1) {\n \
							 float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float u = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float v = txDiffuse.Sample(samLinear, t_tex).y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 }\n";

unsigned char g_cs_vs_vyuy[]="struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_vyuy[] = "struct VS_INPUT\
{\
	float4 Pos : POSITION;\
	float2 Tex : TEXCOORD0;\
};\
struct PS_INPUT {\
	float4 Pos : SV_POSITION;\
	float2 Tex : TEXCOORD0;\
};\
\
cbuffer cbNeverChanges : register(b0)\
{\
    	int m_cx;\
		int m_cy;\
		int t_temp;\
		int t_temp2;\
		float m_csp_coeff_0_0;\
		float m_csp_coeff_0_1;\
		float m_csp_coeff_0_2;\
		float m_csp_coeff_1_0;\
		float m_csp_coeff_1_1;\
		float m_csp_coeff_1_2;\
		float m_csp_coeff_2_0;\
		float m_csp_coeff_2_1;\
		float m_csp_coeff_2_2;\
		float m_csp_const_0;\
		float m_csp_const_1;\
		float m_csp_const_2;\
};\
\
static const float3 YUV_OFFSET = float3(0.0625, 0.5, 0.5);\
\
Texture2D txDiffuse   : register(t0);\
SamplerState samLinear: register(s0);\
\
float4 PS(PS_INPUT input) : SV_Target\
{\
	float3x3 coeffs = float3x3(\
        m_csp_coeff_0_0, m_csp_coeff_0_1, m_csp_coeff_0_2,\
        m_csp_coeff_1_0, m_csp_coeff_1_1, m_csp_coeff_1_2,\
        m_csp_coeff_2_0, m_csp_coeff_2_1, m_csp_coeff_2_2\
    );\
	float3 offset = float3(m_csp_const_0, m_csp_const_1, m_csp_const_2);\
	float4 yuy2 = txDiffuse.Sample(samLinear, input.Tex);\
    float3 yuv = float3(yuy2.y, yuy2.z, yuy2.x) ;\
	float3 rgb = mul(coeffs, yuv) + offset;\
    return float4(saturate(rgb), 1.0);\
}";
unsigned char g_cs_ps_vyuyt[]="struct VS_INPUT\n \
							 {\n \
							 float4 Pos : POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 struct PS_INPUT\n \
							 {\n \
							 float4 Pos : SV_POSITION;\n \
							 float2 Tex : TEXCOORD0;\n \
							 };\n \
							 cbuffer cbNeverChanges : register(b0)\n \
							 {\n \
							 int m_cx;\n \
							 int m_cy;\n \
							 int t_temp;\n \
							 int t_temp2;\n \
							 float m_csp_coeff_0_0;\n \
							 float m_csp_coeff_0_1;\n \
							 float m_csp_coeff_0_2;\n \
							 float m_csp_coeff_1_0;\n \
							 float m_csp_coeff_1_1;\n \
							 float m_csp_coeff_1_2;\n \
							 float m_csp_coeff_2_0;\n \
							 float m_csp_coeff_2_1;\n \
							 float m_csp_coeff_2_2;\n \
							 float m_csp_const_0;\n \
							 float m_csp_const_1;\n \
							 float m_csp_const_2;\n \
							 };\n \
							 Texture2D txDiffuse;\n \
							 SamplerState samLinear;\n \
							 float4 PS(PS_INPUT input) : SV_Target\n \
							 {\n \
							 int t_n_y = m_cy * input.Tex.y;\n \
							 int t_f_x = floor(m_cx *input.Tex.x);\n \
							 int t_n_index = floor(t_f_x) % 2;\n \
							 if (t_n_index == 0) {\n \
							 float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float v = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float u = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 else if (t_n_index == 1) {\n \
							 float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
							 float2 t_tex;\n \
							 t_tex.x = t_f_next;\n \
							 t_tex.y = input.Tex.y;\n \
							 float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
							 float v = txDiffuse.Sample(samLinear, t_tex).x;\n \
							 float u = txDiffuse.Sample(samLinear, input.Tex).x;\n \
							 float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							 float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							 float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							 return float4(r,g,b,1.0f);\n \
							 }\n \
							 }\n";


unsigned char g_cs_vs_nv12[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

/**/
unsigned char g_cs_ps_nv12[] = "struct VS_INPUT\n \
							   {\n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT\n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   cbuffer cbNeverChanges : register(b0)\n \
							   {\n \
							   int m_cx;\n \
							   int m_cy;\n \
							   int t_temp;\n \
							   int t_temp2;\n \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   };\n \
							   Texture2D txtYregister: register(t0);\n \
							   Texture2D txtUVregister:register(t1);\n \
							   SamplerState samLinear;\n \
							   float4 PS(PS_INPUT input) : SV_Target\n \
							   {\n \
							   int t_n_nx=input.Tex.x*m_cx;\n \
							   int t_n_ny=input.Tex.y*m_cy;\n \
							   float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n \
							   float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n \
							   float2 t_tex;\n \
							   t_tex.x = t_f_x;\n \
							   t_tex.y = t_f_y;\n \
							   float y=txtYregister.Sample(samLinear, input.Tex).x;\n \
							   float u=0.0f;\n \
							   float v=0.0f;\n \
							   u=txtUVregister.Sample(samLinear, t_tex).x;\n \
							   v=txtUVregister.Sample(samLinear, t_tex).y;\n \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   }\n";

unsigned char g_cs_vs_nv21[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

/**/
unsigned char g_cs_ps_nv21[] = "struct VS_INPUT\n \
							   {\n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT\n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   cbuffer cbNeverChanges : register(b0)\n \
							   {\n \
							   int m_cx;\n \
							   int m_cy;\n \
							   int t_temp;\n \
							   int t_temp2;\n \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   };\n \
							   Texture2D txtYregister: register(t0);\n \
							   Texture2D txtUVregister:register(t1);\n \
							   SamplerState samLinear;\n \
							   float4 PS(PS_INPUT input) : SV_Target\n \
							   {\n \
							   int t_n_nx=input.Tex.x*m_cx;\n \
							   int t_n_ny=input.Tex.y*m_cy;\n \
							   float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n \
							   float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n \
							   float2 t_tex;\n \
							   t_tex.x = t_f_x;\n \
							   t_tex.y = t_f_y;\n \
							   float y=txtYregister.Sample(samLinear, input.Tex).x;\n \
							   float u=0.0f;\n \
							   float v=0.0f;\n \
							   u=txtUVregister.Sample(samLinear, t_tex).y;\n \
							   v=txtUVregister.Sample(samLinear, t_tex).x;\n \
							   float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n \
							   float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n \
							   float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n \
							   return float4(r,g,b,1.0f);\n \
							   }\n";

unsigned char g_cs_vs_nv16[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";

unsigned char g_cs_ps_nv16[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUVregister:register(t1);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n"
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_nv61[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";

unsigned char g_cs_ps_nv61[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUVregister:register(t1);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n"
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_i422[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";
unsigned char g_cs_ps_i422[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 2 == 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUregister.Sample(samLinear, t_tex).x;\n"
	"v=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_yv16[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";
unsigned char g_cs_ps_yv16[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 2 == 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"v=txtUregister.Sample(samLinear, t_tex).x;\n"
	"u=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_i420[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";;
unsigned char g_cs_ps_i420[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 4 > 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"u=txtUregister.Sample(samLinear, t_tex).x;\n"
	"v=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_yv12[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";;
unsigned char g_cs_ps_yv12[]=
	"struct VS_INPUT\n"
	"{\n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT\n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"cbuffer cbNeverChanges : register(b0)\n"
	"{\n"
	"int m_cx;\n"
	"int m_cy;\n"
	"int t_temp;\n"
	"int t_temp2;\n"
	"float m_csp_coeff_0_0;\n"
	"float m_csp_coeff_0_1;\n"
	"float m_csp_coeff_0_2;\n"
	"float m_csp_coeff_1_0;\n"
	"float m_csp_coeff_1_1;\n"
	"float m_csp_coeff_1_2;\n"
	"float m_csp_coeff_2_0;\n"
	"float m_csp_coeff_2_1;\n"
	"float m_csp_coeff_2_2;\n"
	"float m_csp_const_0;\n"
	"float m_csp_const_1;\n"
	"float m_csp_const_2;\n"
	"};\n"
	"Texture2D txtYregister: register(t0);\n"
	"Texture2D txtUregister: register(t1);\n"
	"Texture2D txtVregister: register(t2);\n"
	"SamplerState samLinear;\n"
	"float4 PS(PS_INPUT input) : SV_Target\n"
	"{\n"
	"int t_n_nx=input.Tex.x*m_cx;\n"
	"int t_n_ny=input.Tex.y*m_cy;\n"
	"float t_f_x=floor(t_n_nx/2)/(m_cx);\n"
	"float t_f_y = floor(t_n_ny)/(m_cy);\n"
	"if(t_n_ny % 4 > 1){"
	"t_f_x=t_f_x+0.5;"
	"}"
	"float2 t_tex;\n"
	"t_tex.x = t_f_x;\n"
	"t_tex.y = t_f_y;\n"
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n"
	"float u=0.0f;\n"
	"float v=0.0f;\n"
	"v=txtUregister.Sample(samLinear, t_tex).x;\n"
	"u=txtVregister.Sample(samLinear, t_tex).x;\n"
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n"
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n"
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n"
	"return float4(r,g,b,1.0f);\n"
	"}\n";

unsigned char g_cs_vs_p010[]=
	"struct VS_INPUT\n"
	"{ \n"
	"float4 Pos : POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"struct PS_INPUT \n"
	"{\n"
	"float4 Pos : SV_POSITION;\n"
	"float2 Tex : TEXCOORD0;\n"
	"};\n"
	"PS_INPUT VS(VS_INPUT input)\n"
	"{\n"
	"PS_INPUT output;\n"
	"output.Pos = input.Pos;\n"
	"output.Tex = input.Tex;\n"
	"return output;\n"
	"}";;
unsigned char g_cs_ps_p010[]=
	"struct VS_INPUT\n"
	"{\n "
	"float4 Pos : POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"struct PS_INPUT\n "
	"{\n "
	"float4 Pos : SV_POSITION;\n "
	"float2 Tex : TEXCOORD0;\n "
	"};\n "
	"cbuffer cbNeverChanges : register(b0)\n "
	"{\n "
	"int m_cx;\n "
	"int m_cy;\n "
	"int t_temp;\n "
	"int t_temp2;\n "
	"float m_csp_coeff_0_0;\n "
	"float m_csp_coeff_0_1;\n "
	"float m_csp_coeff_0_2;\n "
	"float m_csp_coeff_1_0;\n "
	"float m_csp_coeff_1_1;\n "
	"float m_csp_coeff_1_2;\n "
	"float m_csp_coeff_2_0;\n "
	"float m_csp_coeff_2_1;\n "
	"float m_csp_coeff_2_2;\n "
	"float m_csp_const_0;\n "
	"float m_csp_const_1;\n "
	"float m_csp_const_2;\n "
	"};\n "
	"Texture2D txtYregister: register(t0);\n "
	"Texture2D txtUVregister:register(t1);\n "
	"SamplerState samLinear;\n "
	"float4 PS(PS_INPUT input) : SV_Target\n "
	"{\n "
	"int t_n_nx=input.Tex.x*m_cx;\n "
	"int t_n_ny=input.Tex.y*m_cy;\n "
	"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n "
	"float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n "
	"float2 t_tex;\n "
	"t_tex.x = t_f_x;\n "
	"t_tex.y = t_f_y;\n "
	"float y=txtYregister.Sample(samLinear, input.Tex).x;\n "
	"float u=0.0f;\n "
	"float v=0.0f;\n "
	"u=txtUVregister.Sample(samLinear, t_tex).x;\n "
	"v=txtUVregister.Sample(samLinear, t_tex).y;\n "
	"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
	"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
	"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
	"return float4(r,g,b,1.0f);\n "
	"}\n";

unsigned char g_cs_vs_p210[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";;
unsigned char g_cs_ps_p210[] =
"struct VS_INPUT\n"
"{\n "
"float4 Pos : POSITION;\n "
"float2 Tex : TEXCOORD0;\n "
"};\n "
"struct PS_INPUT\n "
"{\n "
"float4 Pos : SV_POSITION;\n "
"float2 Tex : TEXCOORD0;\n "
"};\n "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txtYregister: register(t0);\n "
"Texture2D txtUVregister:register(t1);\n "
"SamplerState samLinear;\n "
"float4 PS(PS_INPUT input) : SV_Target\n "
"{\n "
"int t_n_nx=input.Tex.x*m_cx;\n "
"int t_n_ny=input.Tex.y*m_cy;\n "
"float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n "
"float t_f_y = floor(t_n_ny)/(m_cy);\n "
"float2 t_tex;\n "
"t_tex.x = t_f_x;\n "
"t_tex.y = t_f_y;\n "
"float y=txtYregister.Sample(samLinear, input.Tex).x;\n "
"float u=0.0f;\n "
"float v=0.0f;\n "
"u=txtUVregister.Sample(samLinear, t_tex).x;\n "
"v=txtUVregister.Sample(samLinear, t_tex).y;\n "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,1.0f);\n "
"}\n";

unsigned char g_cs_vs_iyu2[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";;
unsigned char g_cs_ps_iyu2[] =
"struct VS_INPUT\n"
"{\n "
"float4 Pos : POSITION;\n "
"float2 Tex : TEXCOORD0;\n "
"};\n "
"struct PS_INPUT\n "
"{\n "
"float4 Pos : SV_POSITION;\n "
"float2 Tex : TEXCOORD0;\n "
"};\n "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txDiffuse: register(t0);\n "
"SamplerState samLinear;\n "
"float4 PS(PS_INPUT input) : SV_Target\n "
"{\n "
"int nIdx = floor(input.Tex.x * float(m_cx)); \n "
"float2 fLocate; \n "
"float y, u, v; \n "
"if (nIdx % 4 == 0)\n "
"{\n "
"fLocate.x = input.Tex.x*3/4; \n "
"fLocate.y = input.Tex.y; \n "
"u = txDiffuse.Sample(samLinear, fLocate).x; \n "
"y = txDiffuse.Sample(samLinear, fLocate).y; \n "
"v = txDiffuse.Sample(samLinear, fLocate).z; \n "
"}\n "
"else if (nIdx % 4 == 1)\n "
"{\n "
"fLocate.x = (input.Tex.x-1.0/m_cx)*3/4; \n "
"fLocate.y = input.Tex.y; \n "
"u = txDiffuse.Sample(samLinear, fLocate).w; \n "
"fLocate.x = (input.Tex.x-1.0/m_cx)*3/4+1.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"y = txDiffuse.Sample(samLinear, fLocate).x; \n "
"v = txDiffuse.Sample(samLinear, fLocate).y; \n "
"}\n "
"else if (nIdx % 4 == 2)\n "
"{\n "
"fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+1.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"u = txDiffuse.Sample(samLinear, fLocate).z; \n "
"y = txDiffuse.Sample(samLinear, fLocate).w; \n "
"fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+2.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"v = txDiffuse.Sample(samLinear, fLocate).x; \n "
"}\n "
"else\n "
"{\n "
"fLocate.x = (input.Tex.x-3.0/m_cx)*3/4+2.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"u = txDiffuse.Sample(samLinear, fLocate).y; \n "
"y = txDiffuse.Sample(samLinear, fLocate).z; \n "
"v = txDiffuse.Sample(samLinear, fLocate).w; \n "
"}\n "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,1.0f);\n "
"}\n";

unsigned char g_cs_vs_v308[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_v308[] =
"struct VS_INPUT\n"
"{\n "
"float4 Pos : POSITION;\n "
"float2 Tex : TEXCOORD0;\n "
"};\n "
"struct PS_INPUT\n "
"{\n "
"float4 Pos : SV_POSITION;\n "
"float2 Tex : TEXCOORD0;\n "
"};\n "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txDiffuse: register(t0);\n "
"SamplerState samLinear;\n "
"float4 PS(PS_INPUT input) : SV_Target\n "
"{\n "
"int nIdx = floor(input.Tex.x * float(m_cx)); \n "
"float2 fLocate; \n "
"float y, u, v; \n "
"if (nIdx % 4 == 0)\n "
"{\n "
"fLocate.x = input.Tex.x*3/4; \n "
"fLocate.y = input.Tex.y; \n "
"v = txDiffuse.Sample(samLinear, fLocate).x; \n "
"y = txDiffuse.Sample(samLinear, fLocate).y; \n "
"u = txDiffuse.Sample(samLinear, fLocate).z; \n "
"}\n "
"else if (nIdx % 4 == 1)\n "
"{\n "
"fLocate.x = (input.Tex.x-1.0/m_cx)*3/4; \n "
"fLocate.y = input.Tex.y; \n "
"v = txDiffuse.Sample(samLinear, fLocate).w; \n "
"fLocate.x = (input.Tex.x-1.0/m_cx)*3/4+1.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"y = txDiffuse.Sample(samLinear, fLocate).x; \n "
"u = txDiffuse.Sample(samLinear, fLocate).y; \n "
"}\n "
"else if (nIdx % 4 == 2)\n "
"{\n "
"fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+1.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"v = txDiffuse.Sample(samLinear, fLocate).z; \n "
"y = txDiffuse.Sample(samLinear, fLocate).w; \n "
"fLocate.x = (input.Tex.x-2.0/m_cx)*3/4+2.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"u = txDiffuse.Sample(samLinear, fLocate).x; \n "
"}\n "
"else\n "
"{\n "
"fLocate.x = (input.Tex.x-3.0/m_cx)*3/4+2.0/m_cx; \n "
"fLocate.y = input.Tex.y; \n "
"v = txDiffuse.Sample(samLinear, fLocate).y; \n "
"y = txDiffuse.Sample(samLinear, fLocate).z; \n "
"u = txDiffuse.Sample(samLinear, fLocate).w; \n "
"}\n "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,1.0f);\n "
"}\n";

unsigned char g_cs_vs_ayuv[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_ayuv[] = 
"struct VS_INPUT"
"{ "
"float4 Pos : POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"struct PS_INPUT "
"{ "
"float4 Pos : SV_POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txDiffuse; "
"SamplerState samLinear; "
"float4 PS(PS_INPUT input) : SV_Target "
"{ "
"float a= txDiffuse.Sample(samLinear, input.Tex).x; "
"float y= txDiffuse.Sample(samLinear, input.Tex).y; "
"float u= txDiffuse.Sample(samLinear, input.Tex).z; "
"float v= txDiffuse.Sample(samLinear, input.Tex).w; "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,a);\n "
"}";

unsigned char g_cs_vs_uyva[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_uyva[] = 
"struct VS_INPUT"
"{ "
"float4 Pos : POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"struct PS_INPUT "
"{ "
"float4 Pos : SV_POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txDiffuse; "
"SamplerState samLinear; "
"float4 PS(PS_INPUT input) : SV_Target "
"{ "
"float u= txDiffuse.Sample(samLinear, input.Tex).x; "
"float y= txDiffuse.Sample(samLinear, input.Tex).y; "
"float v= txDiffuse.Sample(samLinear, input.Tex).z; "
"float a= txDiffuse.Sample(samLinear, input.Tex).w; "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,a);\n "
"}";

unsigned char g_cs_vs_vyua[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_vyua[] = 
"struct VS_INPUT"
"{ "
"float4 Pos : POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"struct PS_INPUT "
"{ "
"float4 Pos : SV_POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txDiffuse; "
"SamplerState samLinear; "
"float4 PS(PS_INPUT input) : SV_Target "
"{ "
"float v= txDiffuse.Sample(samLinear, input.Tex).x; "
"float y= txDiffuse.Sample(samLinear, input.Tex).y; "
"float u= txDiffuse.Sample(samLinear, input.Tex).z; "
"float a= txDiffuse.Sample(samLinear, input.Tex).w; "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,a);\n "
"}";

unsigned char g_cs_vs_y410[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_y410[] =
"struct VS_INPUT"
"{ "
"float4 Pos : POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"struct PS_INPUT "
"{ "
"float4 Pos : SV_POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"cbuffer cbNeverChanges : register(b0)\n "
"{\n "
"int m_cx;\n "
"int m_cy;\n "
"int t_temp;\n "
"int t_temp2;\n "
"float m_csp_coeff_0_0;\n "
"float m_csp_coeff_0_1;\n "
"float m_csp_coeff_0_2;\n "
"float m_csp_coeff_1_0;\n "
"float m_csp_coeff_1_1;\n "
"float m_csp_coeff_1_2;\n "
"float m_csp_coeff_2_0;\n "
"float m_csp_coeff_2_1;\n "
"float m_csp_coeff_2_2;\n "
"float m_csp_const_0;\n "
"float m_csp_const_1;\n "
"float m_csp_const_2;\n "
"};\n "
"Texture2D txDiffuse; "
"SamplerState samLinear; "
"float4 PS(PS_INPUT input) : SV_Target "
"{ "
"float u= txDiffuse.Sample(samLinear, input.Tex).r; "
"float y= txDiffuse.Sample(samLinear, input.Tex).g; "
"float v= txDiffuse.Sample(samLinear, input.Tex).b; "
"float a= txDiffuse.Sample(samLinear, input.Tex).a; "
"float r = y*m_csp_coeff_0_0 + u*m_csp_coeff_0_1 + v*m_csp_coeff_0_2 + m_csp_const_0;\n "
"float g = y*m_csp_coeff_1_0 + u*m_csp_coeff_1_1 + v*m_csp_coeff_1_2 + m_csp_const_1;\n "
"float b = y*m_csp_coeff_2_0 + u*m_csp_coeff_2_1 + v*m_csp_coeff_2_2 + m_csp_const_2;\n "
"return float4(r,g,b,a);\n "
"}";

unsigned char g_cs_vs_rgb10[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_rgb10[] =
"struct VS_INPUT"
"{ "
"float4 Pos : POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"struct PS_INPUT "
"{ "
"float4 Pos : SV_POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"Texture2D txDiffuse; "
"SamplerState samLinear; "
"float4 PS(PS_INPUT input) : SV_Target "
"{ "
"float r= txDiffuse.Sample(samLinear, input.Tex).r; "
"float g= txDiffuse.Sample(samLinear, input.Tex).g; "
"float b= txDiffuse.Sample(samLinear, input.Tex).b; "
"float a= txDiffuse.Sample(samLinear, input.Tex).a; "
"return float4(r,g,b,a);\n "
"}";

unsigned char g_cs_vs_bgr10[] =
"struct VS_INPUT\n"
"{ \n"
"float4 Pos : POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"struct PS_INPUT \n"
"{\n"
"float4 Pos : SV_POSITION;\n"
"float2 Tex : TEXCOORD0;\n"
"};\n"
"PS_INPUT VS(VS_INPUT input)\n"
"{\n"
"PS_INPUT output;\n"
"output.Pos = input.Pos;\n"
"output.Tex = input.Tex;\n"
"return output;\n"
"}";
unsigned char g_cs_ps_bgr10[] =
"struct VS_INPUT"
"{ "
"float4 Pos : POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"struct PS_INPUT "
"{ "
"float4 Pos : SV_POSITION; "
"float2 Tex : TEXCOORD0; "
"}; "
"Texture2D txDiffuse; "
"SamplerState samLinear; "
"float4 PS(PS_INPUT input) : SV_Target "
"{ "
"float b= txDiffuse.Sample(samLinear, input.Tex).r; "
"float g= txDiffuse.Sample(samLinear, input.Tex).g; "
"float r= txDiffuse.Sample(samLinear, input.Tex).b; "
"float a= txDiffuse.Sample(samLinear, input.Tex).a; "
"return float4(r,g,b,a);\n "
"}";

unsigned char g_cs_vs_y210[] = "struct VS_INPUT\n \
							   { \n \
							   float4 Pos : POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   struct PS_INPUT \n \
							   {\n \
							   float4 Pos : SV_POSITION;\n \
							   float2 Tex : TEXCOORD0;\n \
							   };\n \
							   PS_INPUT VS(VS_INPUT input)\n \
							   {\n \
							   PS_INPUT output;\n \
							   output.Pos = input.Pos;\n \
							   output.Tex = input.Tex;\n \
							   return output;\n \
							   }";

unsigned char g_cs_ps_y210[] = "struct VS_INPUT \
							   { \
							   float4 Pos : POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   struct PS_INPUT \
							   { \
							   float4 Pos : SV_POSITION; \
							   float2 Tex : TEXCOORD0; \
							   }; \
							   cbuffer cbNeverChanges : register(b0) \
							   { \
							   int m_cx; \
							   int m_cy; \
							   int t_temp; \
							   int t_temp2; \
							   float m_csp_coeff_0_0;\n \
							   float m_csp_coeff_0_1;\n \
							   float m_csp_coeff_0_2;\n \
							   float m_csp_coeff_1_0;\n \
							   float m_csp_coeff_1_1;\n \
							   float m_csp_coeff_1_2;\n \
							   float m_csp_coeff_2_0;\n \
							   float m_csp_coeff_2_1;\n \
							   float m_csp_coeff_2_2;\n \
							   float m_csp_const_0;\n \
							   float m_csp_const_1;\n \
							   float m_csp_const_2;\n \
							   }; \
								Texture2D txtRGregister : register(t0); \n \
								Texture2D txtRGBAregister: register(t1);\n \
							   SamplerState samLinear; \
							   float4 PS(PS_INPUT input) : SV_Target \
							   { \
							   float y = txtRGregister.Sample(samLinear, input.Tex).r; \
							   float u = txtRGBAregister.Sample(samLinear, input.Tex).g; \
							   float v = txtRGBAregister.Sample(samLinear, input.Tex).a; \
							   y = (y * 256.0 - 16.0) * 255.0 / 219.0; \
							   u = (u * 256.0 - 16.0) * 255.0 / 224.0; \
							   v = (v * 256.0 - 16.0) * 255.0 / 224.0; \
			                   float r = y + 1.402 * (v - 128.0); \
							   float g = y - 0.344 * (u - 128.0) - 0.714 * (v - 128.0); \
			                   float b = y + 1.772 * (u - 128.0); \
							   r = r / 256.0; \
							   g = g / 256.0; \
							   b = b / 256.0; \
							   if(r>1.0)   r=1.0; \
							   if(r<0.0)   r=0.0; \
							   if(g>1.0)   g=1.0; \
							   if(g<0.0)   g=0.0; \
							   if(b>1.0)   b=1.0; \
							   if(b<0.0)   b=0.0; \
							   return float4(r,g,b,1.0f);\n \
							   }";
#endif