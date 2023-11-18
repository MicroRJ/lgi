#define _RX_STANDALONE
#include <rx.c>

void DrawCube(rlColor color, float x, float y, float r) {
	// rxtexture_bind(texture);
	// rxsampler_bind(rx.point_sampler);

	// rxIMP_applyMode(rxIMP_MODE_3D,TRUE);
	float w = r;
	float h = r;
	rxvec2_t xy0 = (rxvec2_t){x+0,y+0};
	rxvec2_t xy1 = (rxvec2_t){x+w,y+h};
	rxvec2_t uv0 = (rxvec2_t){0,0};
	rxvec2_t uv1 = (rxvec2_t){1,1};

	rxIMP_applyMode(rxIMP_MODE_QUAD,TRUE);
	Emu_imp_begin(6,4);
	rx.imp.attr.rgba = color;
	rxaddnvtx(4,
	rxvtx_xyuv(xy0.x,xy0.y, uv0.x,uv1.y),
	rxvtx_xyuv(xy0.x,xy1.y, uv0.x,uv0.y),
	rxvtx_xyuv(xy1.x,xy1.y, uv1.x,uv0.y),
	rxvtx_xyuv(xy1.x,xy0.y, uv1.x,uv1.y));
	rxaddnidx(6, 0,1,2, 0,2,3);
	Emu_imp_end();

	// Emu_imp_begin(36,24);
	// rx.imp.attr.rgba = color;
	// rxaddnvtx(24,
	// rxvtx_xyuv(xy0.x,xy0.y, uv0.x,uv1.y),
	// rxvtx_xyuv(xy0.x,xy1.y, uv0.x,uv0.y),
	// rxvtx_xyuv(xy1.x,xy1.y, uv1.x,uv0.y),
	// rxvtx_xyuv(xy1.x,xy0.y, uv1.x,uv1.y));
	// rxaddnidx(6, 0,1,2, 0,2,3);
	// Emu_imp_end();

	// rxIMP_Vertex vertices[] =
 //  { /* FRONT */
	// (rxIMP_Vertex){(rxvec4_t){-.5, -.5, -.5, 1.0},rxvec2_xy(0x00000 + M[4].x0,M[4].y0 + M[4].y1)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, 0.5, -.5, 1.0},rxvec2_xy(0x00000 + M[4].x0,M[4].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){0.5, 0.5, -.5, 1.0},rxvec2_xy(M[4].x0 + M[4].x1,M[4].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){0.5, -.5, -.5, 1.0},rxvec2_xy(M[4].x0 + M[4].x1,M[4].y0 + M[4].y1)},
 //    /* RIGHT */
	// (rxIMP_Vertex){(rxvec4_t){+.5, -.5, -.5, 1.0},rxvec2_xy(0x00000 + M[1].x0,M[1].y0 + M[1].y1)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, +.5, -.5, 1.0},rxvec2_xy(0x00000 + M[1].x0,M[1].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, +.5, +.5, 1.0},rxvec2_xy(M[1].x0 + M[1].x1,M[1].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, -.5, +.5, 1.0},rxvec2_xy(M[1].x0 + M[1].x1,M[1].y0 + M[1].y1)},
 //    /* BACK */
	// (rxIMP_Vertex){(rxvec4_t){+.5, -.5, +.5, 1.0},rxvec2_xy(0x00000 + M[5].x0,M[5].y0 + M[5].y1)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, +.5, +.5, 1.0},rxvec2_xy(0x00000 + M[5].x0,M[5].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, +.5, +.5, 1.0},rxvec2_xy(M[5].x0 + M[5].x1,M[5].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, -.5, +.5, 1.0},rxvec2_xy(M[5].x0 + M[5].x1,M[5].y0 + M[5].y1)},
 //    /* LEFT */
	// (rxIMP_Vertex){(rxvec4_t){-.5, -.5, +.5, 1.0},rxvec2_xy(0x00000 + M[0].x0,M[0].y0 + M[0].y1)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, +.5, +.5, 1.0},rxvec2_xy(0x00000 + M[0].x0,M[0].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, +.5, -.5, 1.0},rxvec2_xy(M[0].x0 + M[0].x1,M[0].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, -.5, -.5, 1.0},rxvec2_xy(M[0].x0 + M[0].x1,M[0].y0 + M[0].y1)},
 //    /* TOP */
	// (rxIMP_Vertex){(rxvec4_t){-.5, +.5, -.5, 1.0},rxvec2_xy(0x00000 + M[3].x0,M[3].y0 + M[3].y1)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, +.5, +.5, 1.0},rxvec2_xy(0x00000 + M[3].x0,M[3].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, +.5, +.5, 1.0},rxvec2_xy(M[3].x0 + M[3].x1,M[3].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, +.5, -.5, 1.0},rxvec2_xy(M[3].x0 + M[3].x1,M[3].y0 + M[3].y1)},
 //    /* BOTTOM */
	// (rxIMP_Vertex){(rxvec4_t){-.5, -.5, +.5, 1.0},rxvec2_xy(0x00000 + M[2].x0,M[2].y0 + M[2].y1)},
	// (rxIMP_Vertex){(rxvec4_t){-.5, -.5, -.5, 1.0},rxvec2_xy(0x00000 + M[2].x0,M[2].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, -.5, -.5, 1.0},rxvec2_xy(M[2].x0 + M[2].x1,M[2].y0 + 0x00000)},
	// (rxIMP_Vertex){(rxvec4_t){+.5, -.5, +.5, 1.0},rxvec2_xy(M[2].x0 + M[2].x1,M[2].y0 + M[2].y1)} };


	// for(int i = 0; i < ccCarrlenL(vertices); i += 1)
	// {
	// 	rxIMP_Vertex v = vertices[i];
	// 	v.rgba = RX_COLOR_WHITE;
	// 	v.xyzw = rxmul_matvec(rxmatrix_rotation(rot_x,rot_y,0),v.xyzw);
	// 	v.  z += 4;
	// 	v.  u /= (float) texture.size_x;
	// 	v.  v /= (float) texture.size_y;
	// 	vertices[i] = v;
	// }

	// rxsubmit_vertex_array(vertices,ccCarrlenL(vertices));

	// for(int i = 0; i < ccCarrlenL(vertices) / 4; i += 1)
	// {
	// 	rxsubmit_index(i*4 + 0);rxsubmit_index(i*4 + 1);rxsubmit_index(i*4 + 2);
	// 	rxsubmit_index(i*4 + 0);rxsubmit_index(i*4 + 2);rxsubmit_index(i*4 + 3);
	// }

	// rxvertex_mode_end();
}

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	init_windowed(0,0,"elang debugger");

	do {
		DrawCube(rxColor_WHITE,0,0,64);
	} while (rxtick());
}
