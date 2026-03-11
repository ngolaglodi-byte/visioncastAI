from OpenGL.GL import *
from PyQt5.QtWidgets import QOpenGLWidget
from PyQt5 import QtCore
import numpy as np
import copy
from mwcapture.libmwcapture import *

g_vertex_shader_str ="""
#version 130\n
in vec2 vertexIn;\n
in vec2 textureIn;\n
out vec2 textureOut;\n
void main(void)\n
{\n
   gl_Position = vec4(vertexIn,0.0,1.0);\n
   textureOut = textureIn;\n
}\n
"""

g_rgb_fragment_shader_str ="""
#version 130\n
uniform sampler2D tex_rgb;\n
uniform vec3 rgb_rng;\n
in vec2 textureOut;\n
void main(void)\n
{\n
   float r,g,b;\n
   vec4 t_color = texture2D(tex_rgb,textureOut);\n
   r = (t_color.r-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   g = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   b = (t_color.b-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   gl_FragColor = vec4(r,g,b,1.0f);\n
}\n
"""
g_bgr_fragment_shader_str ="""
#version 130\n
uniform sampler2D tex_rgb;\n
uniform vec3 rgb_rng;\n
in vec2 textureOut;\n
void main(void)\n
{\n
   float r,g,b;\n
   vec4 t_color = texture2D(tex_rgb,textureOut);\n
   r = (t_color.b-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   g = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   b = (t_color.r-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   gl_FragColor = vec4(r,g,b,1.0f);\n
}\n
"""

g_argb_fragment_shader_str ="""
#version 130\n
uniform sampler2D tex_rgb;\n
uniform vec3 rgb_rng;\n
in vec2 textureOut;\n
void main(void)\n
{\n
   float r,g,b;\n
   vec4 t_color = texture2D(tex_rgb,textureOut);\n
   r = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   g = (t_color.r-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   b = (t_color.a-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   gl_FragColor = vec4(r,g,b,1.0f);\n
}\n
"""
g_abgr_fragment_shader_str ="""
#version 130\n
uniform sampler2D tex_rgb;\n
uniform vec3 rgb_rng;\n
in vec2 textureOut;\n
void main(void)\n
{\n
   float r,g,b;\n
   vec4 t_color = texture2D(tex_rgb,textureOut);\n
   b = (t_color.g-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   g = (t_color.r-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   r = (t_color.a-rgb_rng.x)*rgb_rng.y+rgb_rng.z;\n
   gl_FragColor = vec4(r,g,b,1.0f);\n
}\n
"""
g_yuy2_fragment_shader_str ="""
#version 130 \n
uniform sampler2D tex_yuv_rg; \n
uniform sampler2D tex_yuv_rgba; \n
uniform mat3 cspmat;\n
uniform vec3 cspconst;\n
in vec2 textureOut; \n
void main(void) \n
{ \n
   vec3 rgb; \n
   vec3 yuv; \n
   yuv.r = texture2D(tex_yuv_rg, textureOut).r; \n
   yuv.g = texture2D(tex_yuv_rgba, textureOut).g; \n
   yuv.b = texture2D(tex_yuv_rgba, textureOut).a; \n
   rgb = yuv*cspmat+cspconst;\n
   gl_FragColor=vec4(rgb,1.0); \n
} \n
"""
g_nv12_fragment_shader_str = """
#version 130 \n
uniform sampler2D tex_y; \n
uniform sampler2D tex_uv; \n
uniform mat3 cspmat;\n
uniform vec3 cspconst;\n
in vec2 textureOut; \n
void main(void) \n
{ \n
   vec3 rgb; \n
   vec3 yuv; \n
   yuv.r = texture2D(tex_y, textureOut).r; \n
   yuv.g = texture2D(tex_uv, textureOut).r; \n
   yuv.b = texture2D(tex_uv, textureOut).g; \n
   rgb = yuv*cspmat+cspconst;\n
   gl_FragColor=vec4(rgb,1.0); \n
} \n
"""
g_nv21_fragment_shader_str ="""
#version 130 \n
uniform sampler2D tex_y; \n
uniform sampler2D tex_uv; \n
uniform mat3 cspmat;\n
uniform vec3 cspconst;\n
in vec2 textureOut; \n
void main(void) \n
{ \n
   vec3 rgb; \n
   vec3 yuv; \n
   yuv.r = texture2D(tex_y, textureOut).r; \n
   yuv.g = texture2D(tex_uv, textureOut).g; \n
   yuv.b = texture2D(tex_uv, textureOut).r; \n
   rgb = yuv*cspmat+cspconst;\n
   gl_FragColor=vec4(rgb,1.0); \n
} \n
"""

g_i420_fragment_shader_str ="""
#version 130 \n
uniform sampler2D tex_y; \n
uniform sampler2D tex_u; \n
uniform sampler2D tex_v; \n
uniform mat3 cspmat;\n
uniform vec3 cspconst;\n
in vec2 textureOut; \n
void main(void) \n
{ \n
   vec3 rgb; \n
   vec3 yuv; \n
   yuv.r = texture2D(tex_y, textureOut).r; \n
   yuv.g = texture2D(tex_u, textureOut).r; \n
   yuv.b = texture2D(tex_v, textureOut).r; \n
   rgb = yuv*cspmat+cspconst;\n
   gl_FragColor=vec4(rgb,1.0); \n
} \n
"""

g_yv12_fragment_shader_str ="""
#version 130 \n
uniform sampler2D tex_y; \n
uniform sampler2D tex_u; \n
uniform sampler2D tex_v; \n
uniform mat3 cspmat;\n
uniform vec3 cspconst;\n
in vec2 textureOut; \n
void main(void) \n
{ \n
   vec3 rgb; \n
   vec3 yuv; \n
   yuv.r = texture2D(tex_y, textureOut).r; \n
   yuv.b = texture2D(tex_u, textureOut).r; \n
   yuv.g = texture2D(tex_v, textureOut).r; \n
   rgb = yuv*cspmat+cspconst;\n
   gl_FragColor=vec4(rgb,1.0); \n
} \n
"""
g_p010_fragment_shader_str ="""
#version 130 \n
uniform sampler2D tex_y; \n
uniform sampler2D tex_uv; \n
uniform mat3 cspmat;\n
uniform vec3 cspconst;\n
uniform int val_ctrl; \n
uniform int threshold; \n
in vec2 textureOut; \n
float A = 0.15, B = 0.50, C = 0.10, D = 0.20, E = 0.02, F = 0.30; \n
float hable(float x) { \n
  return (x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F)  - E/F; \n
} \n
void main(void) \n
{ \n
   vec3 rgb; \n
   vec3 yuv; \n
   float max_rgb=0.0; \n
   float fval = float(val_ctrl); \n
   yuv.r = texture2D(tex_y, textureOut).r; \n
   yuv.g = texture2D(tex_uv, textureOut).r; \n
   yuv.b = texture2D(tex_uv, textureOut).g; \n
   rgb = yuv*cspmat+cspconst;\n
   rgb.r = fval*pow(rgb.r,2.4); \n
   rgb.g = fval*pow(rgb.g,2.4); \n
   rgb.b = fval*pow(rgb.b,2.4); \n
   if(rgb.r>max_rgb) max_rgb = rgb.r; \n
   if(rgb.g>max_rgb) max_rgb = rgb.g; \n
   if(rgb.b>max_rgb) max_rgb = rgb.b; \n
   float ratio = hable(max_rgb) / hable(fval); \n
   if(val_ctrl <= threshold) \n
   { \n
       ratio = hable(rgb.r) / hable(fval); \n
   } \n
   rgb.r = rgb.r * ratio / max_rgb; \n
   if(val_ctrl <= threshold) \n
   { \n
       ratio = hable(rgb.g) / hable(fval); \n
   } \n
   rgb.g = rgb.g * ratio / max_rgb; \n
   if(val_ctrl <= threshold) \n
   { \n
       ratio = hable(rgb.b) / hable(fval); \n
   } \n
   rgb.b = rgb.b * ratio / max_rgb; \n
   gl_FragColor=vec4(rgb,1.0); \n
} \n
"""

g_ver_vertices = np.array([-1.0,-1.0,1.0,-1.0,-1.0,1.0,1.0,1.0],dtype=np.float32)
g_ver_textures = np.array([0.0,1.0,1.0,1.0,0.0,0.0,1.0,0.0],dtype=np.float32)


class CRenderWid(QOpenGLWidget):
    def __init__(self, parent=None, flags = QtCore.Qt.WindowFlags()):
        super().__init__(parent=parent, flags=flags)
        self.m_rgb_trans = []
        self.m_csp_const = [0.1 for i in range(3)]
        self.m_csp_mat = [[0.2] * 3 for i in range(3) ]
        self.m_data = 0
        self.m_tex_num = 0
        self.m_tex_offset = []
        self.m_tex_size = []
        self.m_tex = []
        self.m_mw_fourcc = 0
        self.m_width = 0
        self.m_height = 0
        self.m_program = 0
        self.m_program_hdr = 0
        self.m_program_sdr = 0
        self.m_buffers_num = 0
        self.m_fragment_shader = 0
        self.m_fragment_shader_hdr = 0
        self.m_vertex_shader = 0
        self.m_vertex_shader_hdr = 0

    def __del__(self):
        self.clean_up()
        pass

    def initializeGL(self):
        print(glGetString(GL_VERSION))

    def paintGL(self):
        if self.m_data!= 0:
            self.render(False)
        else:
            glClear(GL_COLOR_BUFFER_BIT)
            glClearColor(0.0,0.0,0.0,1.0)

    def resizeGL(self, x, y):
        self.makeCurrent()
        glViewport(0,0,x,y)

    def open_render(self,mw_fourcc,width,height):
        self.makeCurrent()
        self.m_width = width
        self.m_height = height
        self.m_mw_fourcc = mw_fourcc
        self.calc_rgb_trans(False, False)
        self.calc_yuv_trans(False, False, MWCAP_VIDEO_COLOR_FORMAT_YUV601)
        self.clean_up()
        if False == self.gen_textures():
            self.clean_up()
            return False
        if False == self.build_program():
            self.clean_up()
            return False
        verLocation = glGetAttribLocation(self.m_program_sdr, "vertexIn")
        texLocation = glGetAttribLocation(self.m_program_sdr, "textureIn")
        glVertexAttribPointer(verLocation, 2, GL_FLOAT, 0, 0, g_ver_vertices)
        glEnableVertexAttribArray(verLocation)

        glVertexAttribPointer(texLocation, 2, GL_FLOAT, 0, 0, g_ver_textures)
        glEnableVertexAttribArray(texLocation)

        if self.m_program_hdr:
            verLocation = glGetAttribLocation(self.m_program_hdr, "vertexIn")
            texLocation = glGetAttribLocation(self.m_program_hdr, "textureIn")
            glVertexAttribPointer(verLocation, 2, GL_FLOAT, 0, 0, g_ver_vertices)
            glEnableVertexAttribArray(verLocation)
            glVertexAttribPointer(texLocation, 2, GL_FLOAT, 0, 0, g_ver_textures)
            glEnableVertexAttribArray(texLocation)
        glClearColor(0.9, 0.0, 0.0, 1.0)
        return True 
    def calc_rgb_trans(self, is_in_limited, is_out_limited):
        in_min = 0
        in_max = 255
        out_min = 0
        out_max = 255
        if is_in_limited:
            in_min = 16
            in_max = 255
        
        if is_out_limited:
            out_min = 16
            out_max = 255
        
        self.m_rgb_trans.append(in_min*1.0 / (in_max - in_min))
        self.m_rgb_trans.append((out_max - out_min)*1.0 / (in_max - in_min))
        self.m_rgb_trans.append(out_min*1.0 / 255)

    def calc_yuv_trans(self, is_in_limited,  is_out_limited,  m_csp_in):
        f_r = 0.2126
        f_g = 0.7152
        f_b = 0.0722

        if MWCAP_VIDEO_COLOR_FORMAT_YUV601 == m_csp_in:
            f_r = 0.299
            f_g = 0.587
            f_b = 0.114
        elif MWCAP_VIDEO_COLOR_FORMAT_YUV2020 == m_csp_in:
            f_r = 0.2627
            f_g = 0.6780
            f_b = 0.0593
        
        self.m_csp_mat[0][0] = 1
        self.m_csp_mat[0][1] = 0
        self.m_csp_mat[0][2] = 2*(1-f_r)
        self.m_csp_mat[1][0] = 1
        self.m_csp_mat[1][1] = -2 * (1 - f_b)*f_b / f_g
        self.m_csp_mat[1][2] = -2 * (1 - f_r)*f_r / f_g
        self.m_csp_mat[2][0] = 1
        self.m_csp_mat[2][1] = 2 * (1 - f_b)
        self.m_csp_mat[2][2] = 0
        '''
        self.m_csp_mat[0] = 1
        self.m_csp_mat[1] = 0
        self.m_csp_mat[2] = 2*(1-f_r)
        self.m_csp_mat[3] = 1
        self.m_csp_mat[4] = -2 * (1 - f_b)*f_b / f_g
        self.m_csp_mat[5] = -2 * (1 - f_r)*f_r / f_g
        self.m_csp_mat[6] = 1
        self.m_csp_mat[7] = 2 * (1 - f_b)
        self.m_csp_mat[8] = 0
        '''
        if is_in_limited:
            yuv_rng_ymin = 16.0 / 255.0
            yuv_rng_ymax = 235.0 / 255.0
            yuv_rng_cmax = 240.0 / 255.0
            yuv_rng_cmid = 128.0 / 255.0
        else:
            yuv_rng_ymin = 0.0
            yuv_rng_ymax = 255.0 / 255.0
            yuv_rng_cmax = 255.0 / 255.0
            yuv_rng_cmid = 128.0 / 255.0

        if is_out_limited:
            rgb_rng_min = 16.0 / 255.0
            rgb_rng_max = 235.0 / 255.0
        else:
            rgb_rng_min = 0.0
            rgb_rng_max = 255.0 / 255.0

        d_ymul = (rgb_rng_max - rgb_rng_min) / (yuv_rng_ymax - yuv_rng_ymin)
        d_cmul = (rgb_rng_max - rgb_rng_min) / (yuv_rng_cmax - yuv_rng_cmid) / 2.0
        for i in range(3):
            self.m_csp_mat[i][0] *= d_ymul
            self.m_csp_mat[i][1] *= d_cmul
            self.m_csp_mat[i][2] *= d_cmul
            self.m_csp_const[i] = rgb_rng_min - self.m_csp_mat[i][0] * yuv_rng_ymin - (self.m_csp_mat[i][1] + self.m_csp_mat[i][2])*yuv_rng_cmid

    def gen_textures(self):
        if (MWFOURCC_RGB24 == self.m_mw_fourcc) or (MWFOURCC_BGR24 == self.m_mw_fourcc):
            self.m_tex_num = 1
            self.m_tex_offset.append(0)
            self.m_tex_size.append(self.m_width*self.m_height*3)
        elif (MWFOURCC_BGRA == self.m_mw_fourcc) or (MWFOURCC_RGBA == self.m_mw_fourcc) or (MWFOURCC_ABGR == self.m_mw_fourcc) or (MWFOURCC_ARGB == self.m_mw_fourcc):
            self.m_tex_num = 1
            self.m_tex_offset.append(0)
            self.m_tex_size.append(self.m_width*self.m_height*4)
        elif MWFOURCC_YUY2 == self.m_mw_fourcc:
            self.m_tex_num = 2
            self.m_tex_offset.append(0)
            self.m_tex_size.append(self.m_width*self.m_height*2)
            self.m_tex_offset.append(0)
            self.m_tex_size.append(self.m_width*self.m_height*2)
        elif(MWFOURCC_NV12 == self.m_mw_fourcc) or (MWFOURCC_NV21 == self.m_mw_fourcc):
            self.m_tex_num = 2
            self.m_tex_offset.append(0)
            self.m_tex_offset.append(self.m_width*self.m_height)
            self.m_tex_size.append(self.m_width*self.m_height)
            self.m_tex_size.append(self.m_width*self.m_height/2)
        elif MWFOURCC_P010 == self.m_mw_fourcc:
            self.m_tex_num = 2
            self.m_tex_offset.append(0)
            self.m_tex_offset.append(self.m_width*self.m_height*2)
            self.m_tex_size.append(self.m_width*self.m_height*2)
            self.m_tex_size.append(self.m_width*self.m_height)
        elif (MWFOURCC_I420 == self.m_mw_fourcc) or (MWFOURCC_YV12 == self.m_mw_fourcc):
            self.m_tex_num = 3
            self.m_tex_offset.append(0)
            self.m_tex_offset.append(self.m_width*self.m_height)
            self.m_tex_offset.append(self.m_width*self.m_height * 5 / 4)
            self.m_tex_size.append(self.m_width*self.m_height)
            self.m_tex_size.append(self.m_width*self.m_height/4)
            self.m_tex_size.append(self.m_width*self.m_height/4)

        
        if 1 == self.m_tex_num:
            self.m_tex.append(glGenTextures(self.m_tex_num))
        else:
            self.m_tex = glGenTextures(self.m_tex_num)
        for i in range(self.m_tex_num):
            if 0 == self.m_tex[i]:
                return False
            glBindTexture(GL_TEXTURE_2D, self.m_tex[i])
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        return True
    def build_program_hdr(self):
        self.m_vertex_shader_hdr = glCreateShader(GL_VERTEX_SHADER)
        if 0 == self.m_vertex_shader_hdr:
            print("hdr GL_VERTEX_SHADER")
            return False
        self.m_fragment_shader_hdr = glCreateShader(GL_FRAGMENT_SHADER)
        if 0 == self.m_fragment_shader_hdr:
            print("hdr GL_FRAGMENT_SHADER")
            return False

        glShaderSource(self.m_vertex_shader_hdr, g_vertex_shader_str)

        glShaderSource(self.m_fragment_shader_hdr, g_p010_fragment_shader_str)

        glCompileShader(self.m_vertex_shader_hdr)
        status = glGetShaderiv(self.m_vertex_shader_hdr, GL_COMPILE_STATUS)
        if not(status):
            print("hdr vertCompiled %s" %(glGetShaderInfoLog(self.m_vertex_shader_hdr)))
            return False
        glCompileShader(self.m_fragment_shader_hdr)
        status = glGetShaderiv(self.m_fragment_shader_hdr, GL_COMPILE_STATUS)
        if not(status):
            print("hdr fragCompiled %s" %(glGetShaderInfoLog(self.m_fragment_shader_hdr)))
            return False

        self.m_program_hdr = glCreateProgram()
        if 0 == self.m_program_hdr:
            print("hdr glCreateProgram")
            return False

        glAttachShader(self.m_program_hdr, self.m_vertex_shader_hdr)
        glAttachShader(self.m_program_hdr, self.m_fragment_shader_hdr)
        glLinkProgram(self.m_program_hdr)
        status = glGetProgramiv(self.m_program_hdr, GL_LINK_STATUS)
        if not(status):
            print("hdr linked fail")
            return False
        return True


    def build_program(self):
        self.m_vertex_shader = glCreateShader(GL_VERTEX_SHADER)
        if 0 == self.m_vertex_shader:
            print("GL_VERTEX_SHADER")
            return False
        self.m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER)
        if 0 == self.m_fragment_shader:
            print("GL_VERTEX_SHADER")
            return False
        glShaderSource(self.m_vertex_shader, g_vertex_shader_str)
        if(MWFOURCC_RGB24 == self.m_mw_fourcc) or (MWFOURCC_RGBA == self.m_mw_fourcc):
            glShaderSource(self.m_fragment_shader, g_rgb_fragment_shader_str)
        elif(MWFOURCC_BGR24 == self.m_mw_fourcc) or (MWFOURCC_BGRA == self.m_mw_fourcc):
            glShaderSource(self.m_fragment_shader, g_bgr_fragment_shader_str)
        elif MWFOURCC_ABGR == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_abgr_fragment_shader_str)
        elif MWFOURCC_ARGB == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_argb_fragment_shader_str)
        elif MWFOURCC_YUY2 == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_yuy2_fragment_shader_str)
        elif MWFOURCC_NV12 == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_nv12_fragment_shader_str)
        elif MWFOURCC_NV21 == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_nv21_fragment_shader_str)
        elif MWFOURCC_P010 == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_nv12_fragment_shader_str)
            self.build_program_hdr()
        elif MWFOURCC_I420 == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_i420_fragment_shader_str)
        elif MWFOURCC_YV12 == self.m_mw_fourcc:
            glShaderSource(self.m_fragment_shader, g_yv12_fragment_shader_str)
        

        glCompileShader(self.m_vertex_shader)
        status = glGetShaderiv(self.m_vertex_shader, GL_COMPILE_STATUS)
        if not(status):
            print("vertCompiled %s" %(glGetShaderInfoLog(self.m_vertex_shader)))
            return False
        glCompileShader(self.m_fragment_shader)
        status = glGetShaderiv(self.m_fragment_shader, GL_COMPILE_STATUS)
        if not(status):
            print("fragCompiled %s" %(glGetShaderInfoLog(self.m_fragment_shader)))
            return False

        self.m_program_sdr = glCreateProgram()
        if 0 == self.m_program_sdr:
            print("glCreateProgram")
            return False
        glAttachShader(self.m_program_sdr, self.m_vertex_shader)
        glAttachShader(self.m_program_sdr, self.m_fragment_shader)
        glLinkProgram(self.m_program_sdr)
        status = glGetProgramiv(self.m_program_sdr, GL_LINK_STATUS)
        if not(status):
            print("linked fail")
            return False
        return True

    def clean_up(self):
        if 0 != self.m_fragment_shader:
            glDeleteShader(self.m_fragment_shader)
            self.m_fragment_shader = 0

        if 0 != self.m_vertex_shader:
            glDeleteShader(self.m_vertex_shader)
            self.m_vertex_shader = 0

        if 0 != self.m_program:
            glDeleteProgram(self.m_program)
            self.m_program = 0

        if 0 != self.m_fragment_shader_hdr:
            glDeleteShader(self.m_fragment_shader_hdr)
            self.m_fragment_shader_hdr = 0
        if 0 != self.m_vertex_shader_hdr:
            glDeleteShader(self.m_vertex_shader_hdr)
            self.m_vertex_shader_hdr = 0

        if 0 != self.m_program_hdr:
            glDeleteProgram(self.m_program_hdr)
            self.m_program_hdr = 0

        if 0 != self.m_tex_num:
            glDeleteTextures(self.m_tex_num, self.m_tex)
            self.m_tex = 0
            self.m_tex_num = 0

    def render_tex(self,tex_index, internalformat, tex_w, tex_h, format, tex_type, p_tex_name):
        glActiveTexture(GL_TEXTURE0 + tex_index)
        glBindTexture(GL_TEXTURE_2D, self.m_tex[tex_index])
        if 0 == self.m_tex_offset[tex_index]:
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, tex_w, tex_h, 0, format, tex_type, self.m_data)
        else:
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, tex_w, tex_h, 0, format, tex_type, self.m_data[self.m_tex_offset[tex_index]:])
        glUniform1i(glGetUniformLocation(self.m_program, p_tex_name), tex_index)


    def render(self, hdr_on):
        self.makeCurrent()
        set_rgb = False
        set_csp = False
        self.m_program = self.m_program_sdr
        if hdr_on and 0 != self.m_program_hdr:
            self.m_program = self.m_program_hdr
        glUseProgram(self.m_program)
        
        if (MWFOURCC_RGB24 == self.m_mw_fourcc) or (MWFOURCC_BGR24 == self.m_mw_fourcc):
            self.render_tex(0, GL_RGB, self.m_width, self.m_height, GL_RGB, GL_UNSIGNED_BYTE, "tex_rgb")
            set_rgb = 1
        elif (MWFOURCC_BGRA == self.m_mw_fourcc) or (MWFOURCC_RGBA == self.m_mw_fourcc):
            self.render_tex(0, GL_BGRA, self.m_width, self.m_height, GL_RGBA, GL_UNSIGNED_BYTE, "tex_rgb")
            set_rgb = 1
        elif (MWFOURCC_ABGR == self.m_mw_fourcc) or (MWFOURCC_ARGB == self.m_mw_fourcc):
            self.render_tex(0, GL_BGRA, self.m_width, self.m_height, GL_BGRA, GL_UNSIGNED_BYTE, "tex_rgb")
            set_rgb = 1
        elif MWFOURCC_YUY2 == self.m_mw_fourcc:
            self.render_tex(0, GL_RG, self.m_width, self.m_height, GL_RG, GL_UNSIGNED_BYTE, "tex_yuv_rg")
            self.render_tex(1, GL_RGBA, self.m_width/2, self.m_height, GL_RGBA, GL_UNSIGNED_BYTE, "tex_yuv_rgba")
            set_csp = True
        elif (MWFOURCC_NV12 == self.m_mw_fourcc) or (MWFOURCC_NV21 == self.m_mw_fourcc):
            self.render_tex(0, GL_RED, self.m_width, self.m_height, GL_RED, GL_UNSIGNED_BYTE, "tex_y")
            self.render_tex(1, GL_RG, self.m_width/2, self.m_height/2, GL_RG, GL_UNSIGNED_BYTE, "tex_uv")
            set_csp = True 
        elif MWFOURCC_P010 == self.m_mw_fourcc:
            self.render_tex(0, GL_RED, self.m_width, self.m_height, GL_RED, GL_UNSIGNED_SHORT, "tex_y")
            self.render_tex(1, GL_RG, self.m_width/2, self.m_height/2, GL_RG, GL_UNSIGNED_SHORT, "tex_uv")
            glUniform1i(glGetUniformLocation(self.m_program, "threshold"), 40)
            glUniform1i(glGetUniformLocation(self.m_program, "val_ctrl"), 40)
            set_csp = True
        elif (MWFOURCC_I420 == self.m_mw_fourcc) or (MWFOURCC_YV12 == self.m_mw_fourcc):
            self.render_tex(0, GL_RED, self.m_width, self.m_height, GL_RED, GL_UNSIGNED_BYTE, "tex_y")
            self.render_tex(1, GL_RED, self.m_width/2, self.m_height/2, GL_RED, GL_UNSIGNED_BYTE, "tex_u")
            self.render_tex(2, GL_RED, self.m_width/2, self.m_height/2, GL_RED, GL_UNSIGNED_BYTE, "tex_v")
            set_csp = True

        if set_rgb:
            glUniform3f(glGetUniformLocation(self.m_program, "rgb_rng"),
            self.m_rgb_trans[0],
            self.m_rgb_trans[1],
            self.m_rgb_trans[2])

        if set_csp:
            glUniformMatrix3fv(glGetUniformLocation(self.m_program, "cspmat"), 1, GL_FALSE, self.m_csp_mat)
            glUniform3f(glGetUniformLocation(self.m_program, "cspconst"), self.m_csp_const[0], self.m_csp_const[1], self.m_csp_const[2])
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)
        return True

    def put_frame(self,pbframe):
        self.m_data = copy.deepcopy(pbframe)
        self.update()
        
    def set_black(self):
        self.m_data = 0
        self.update()
