/*
Copyright (c) 2009 Francisco Mu�oz 'Hermes' <www.elotrolado.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are 
permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this list of 
  conditions and the following disclaimer. 
- Redistributions in binary form must reproduce the above copyright notice, this list 
  of conditions and the following disclaimer in the documentation and/or other 
  materials provided with the distribution. 
- The names of the contributors may not be used to endorse or promote products derived 
  from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "gfx.h"

extern Mtx	modelView;

int scroll_text=-20;

GXTlutObj palette_icon;
GXTexObj text_icon[10];

GXTexObj text_button[4], default_game_texture, default_game_texture2, text_background[4], text_background2,text_game_empty[4];
GXTexObj text_screen_fx;

u32 *screen_fx=NULL;

GXTexObj text_move_chan;

GXTexObj png_texture;

void *mem_move_chan= NULL;

void *background_png=NULL;

void splash_scr()
{
			autocenter=1;
			draw_background();

			guMtxIdentity(modelView);
			GX_LoadPosMtxImm(modelView,	GX_PNMTX0); // carga la matriz mundial como identidad
			GX_SetCurrentMtx(GX_PNMTX0); // selecciona la matriz
			
			ChangeProjection(0,SCR_HEIGHT<=480 ? -12: 0,SCR_WIDTH,SCR_HEIGHT+(SCR_HEIGHT<=480 ? 16: 0));

			#ifndef ALTERNATIVE_VERSION
			PX=0; PY= 16; color= 0xffffffff; 
			#else
			PX=0; PY= 16; color= INK0; 
			#endif

			letter_size(32,64);
			SetTexture(NULL);
			#ifndef ALTERNATIVE_VERSION
			DrawRoundFillBox((SCR_WIDTH-260)/2, 16, 260, 64, 0, 0x5f00c03f);
			#else
			DrawRoundFillBox((SCR_WIDTH-260)/2, 16, 260, 64, 0, 0xffffffff);
			#endif

			SelectFontTexture(1);
			s_printf("%s","uLoader");
			color= INK0;
			SetTexture(NULL);
			DrawRoundBox((SCR_WIDTH-260)/2, 16, 260, 64, 0, 4, INK0);

			SelectFontTexture(0);
			PY+=80;
			letter_size(16,32);
			s_printf("%s","\251 2009, Hermes (www.elotrolado.net)");
			PY+=40;
			
			#ifndef ALTERNATIVE_VERSION

			s_printf("%s","Based in YAL \251 2009, Kwiirk");
			PY+=40;
			s_printf("%s","and USBLoader \251 2009, Waninkoko");
			PY+=34;
			letter_size(8,32);
			SelectFontTexture(1);
			s_printf("%s","Ocarina and some game patch added from FISHEARS usbloader version");

			#else

			letter_size(8,16);			
			s_printf("%s","Modifications by: Vrsquid, buhosoft, Josete2k and ManuMtz");
			PY+=44;
            letter_size(8,16);			
			s_printf("%s","Based in Waninkoko's USBLoader and Kwiirk's YAL");
 			PY+=40;

			s_printf("%s","Ocarina and some game patch added from FISHEARS usbloader version");
 			PY+=34;
 			letter_size(8,32);
 			SelectFontTexture(1);
			s_printf("%s","");
			#endif
			
			
			
			autocenter=0;
			PX=20; PY= 32; color= INK0; 
			letter_size(8,16);
			SelectFontTexture(1);
			#ifndef ALTERNATIVE_VERSION
			s_printf("v%s",uloader_version);
		    #endif
			PX=SCR_WIDTH-20-32;
			s_printf("v%s",uloader_version);
			autocenter=1;
			//letter_size(12,16);
			PX=20; PY= 480-40; color= INK0;

			#ifndef ALTERNATIVE_VERSION
			s_printf("%s","40 Years Old - 25 Years Programming (11-06-1969)");
			#else
			s_printf("%s","Alternative uLoader");
			#endif
		
}

void splash_scr_send()
{
	splash_scr();
	Screen_flip();
}

void splash2_scr()
{

		draw_background();
		letter_size(16,32);
		PX=0; PY= SCR_HEIGHT/2+32; color= INK0; 
				
		bkcolor=0;
		autocenter=1;
		SetTexture(NULL);

}
//---------------------------------------------------------------------------------
/* procedural texture */
//---------------------------------------------------------------------------------

#define RGB(r,g,b) ((r) |(g<<8) | (b<<16))

static float InterPol(float a, float b, float x)
{
return a+(b-a)*x*x*(3-2*x);
}

static float InterLin(float a, float b, float x)
{
return a+(b-a)*x;
}

static float IntNoise(register int x)
{
	x = (x<<13)^x;

return (((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0);
}


static float Cellular(float x,float y,int width,int tam_cas, int seed)
{
double primero=2*tam_cas, segundo=2*tam_cas,tercero=2*tam_cas,dist_aux;
int casilla_pto;
double xpunto, ypunto;
int n_casillas=(int) (width/tam_cas)+1;
int casillax=(int)(x/tam_cas);
int casillay=(int)(y/tam_cas);
int casilla=n_casillas*casillay+casillax;
int j,i;

	for(j=-1;j<2;j++)
		{
		for(i=-1;i<2;i++)
			{
			casilla_pto=casilla+i+j*n_casillas;
			xpunto=(casillax+i)*tam_cas+IntNoise(casilla_pto+seed)*tam_cas;
			ypunto=(casillay+j)*tam_cas+IntNoise(casilla_pto+10+seed)*tam_cas;
			dist_aux=sqrt((x-xpunto)*(x-xpunto)+(y-ypunto)*(y-ypunto));

			if (primero>dist_aux)
				{
				tercero=segundo;
				segundo=primero;
				primero=dist_aux;
				}
			else 
				{
				if (segundo>dist_aux) 
					{
					tercero= segundo;
					segundo=dist_aux;
					} 
				else 
					{
					if(tercero>dist_aux)
						{tercero=dist_aux;}
					}
				} 
			}
		}

return primero*primero/(segundo*tercero);
}


static float PerlinNoise(float x,float y,int width,int octaves,int seed){

double a,b,valor=0.0f,freq,cachox,cachoy;
int casilla,num_pasos,pasox,pasoy;
int amplitud=256;
int periodo=256;
int s;

	if(octaves>12) {octaves=12;}

	for(s=0;s<octaves;s++)
		{

		amplitud>>=1;
		periodo>>=1;
		freq=1/(float) (periodo);
		num_pasos=(int)(width*freq);
		pasox=(int)(x*freq);
		pasoy=(int)(y*freq);
		cachox=x*freq-pasox;
		cachoy=y*freq-pasoy;
		casilla=pasox+pasoy*num_pasos;
		a=InterPol(IntNoise(casilla+seed),IntNoise(casilla+1+seed),cachox);
		b=InterPol(IntNoise(casilla+num_pasos+seed),IntNoise(casilla+1+num_pasos+seed),cachox);
		valor+=InterPol(a,b,cachoy)*amplitud;

		}

return valor;
}
int flag_snow=0;

static int color_agua(float valor)
{

int r;
int g;
int b;

if(flag_snow)
	{
	r=InterLin(20, 200, valor);
	g=InterLin(68, 240, valor);
	b=InterLin(82, 251, valor);
	}
else
	{
	r=InterLin(20, 91, valor);
	g=InterLin(68, 187, valor);
	b=InterLin(82, 251, valor);
	}

	if(r>255) r=255;
	if(g>255) g=255;
	if(b>255) b=255;

return RGB(r,g,b);
}


static float rango(float v,float a,float b)
{
	if(v<a) v=a;
	if(v>b) v=b;

return v;
}

void create_background_text(int seed, int width,int height,unsigned *t)
{
//int seed = 666;

int dispx,dispy;
double colr,colr1;
unsigned color;
int j,i;

for(j=0; j<height; j++)
	{
    for(i=0; i< width; i++)
		{
	
		dispx= PerlinNoise(i,j+100,width,3,seed)/3;
		dispy= PerlinNoise(i,j+105,width,3,seed)/3;
		colr = Cellular(i*0.6+dispx,j+dispy,width,64/8,seed);
		colr *= (colr);
		colr1 = Cellular(i*0.6+dispy,j+dispx,width,48/8,seed);
		colr1 *= (colr1);
		colr=(colr*0.60+colr1*0.30);
		
		colr=rango(colr,0.0f,1.0f)+0.9f;
		color=color_agua(colr);
		
		t[i+j*width]=color | 0xff000000;

		}
   
    }

}

//---------------------------------------------------------------------------------
/* DRAW SNOW */
//---------------------------------------------------------------------------------

struct
{
int x,y;
int velocity;
int pad;
}
tab_snow[128];

int tab_random[1024];
int pos_random=0;

int get_rnd()
{
	pos_random=(pos_random+1) & 1023;
	return tab_random[pos_random];
}


void draw_snow()
{
static int init=1;
int n;
int y,dx;

	if(!flag_snow || (in_black & 3)==1) return;
/*
	ChangeProjection(0,SCR_HEIGHT<=480 ? -12: 0,SCR_WIDTH,SCR_HEIGHT+(SCR_HEIGHT<=480 ? 16: 0));
	guMtxIdentity(modelView);
	GX_LoadPosMtxImm(modelView,	GX_PNMTX0); // carga la matriz mundial como identidad
	GX_SetCurrentMtx(GX_PNMTX0); // selecciona la matriz
*/

	SetTexture(&text_icon[4]);

	if(init)
		{
		for(n=0;n<1024;n++) tab_random[n]=rand();
		}

	for(n=0;n<80;n++)
		{
		
		y=tab_snow[n].y>>3;
		
		if(y>=SCR_HEIGHT || init)
			{
			
			if(init) tab_snow[n].y=(-(get_rnd() % SCR_HEIGHT)-32)<<3;
			else tab_snow[n].y=(-(get_rnd() & 63)-32)<<3;
			tab_snow[n].x=(get_rnd() % (SCR_WIDTH+32+208*is_16_9))-32-108*is_16_9;
			tab_snow[n].velocity=2+(get_rnd() & 3);
			y=tab_snow[n].y>>3;
			}

		switch(tab_snow[n].velocity)
			{
			case 1:
				{
				dx= (y & 31);
				if(y & 32) dx=31-dx;
				dx>>=1;
				dx+=7+14;
				DrawRoundFillBox(tab_snow[n].x+dx , y, 28 , 28, 0, 0xffffffff);
				break;
				}
			case 2:
				{
				dx= (y & 31);
				if(y & 32) dx=31-dx;
				dx>>=1;
				dx+=7+10;
				DrawRoundFillBox(tab_snow[n].x+dx , y, 20 , 20, 0, 0xffffffff);
				break;
				}
			case 3:
				{
				dx= (y & 15);
				if(y & 16) dx=15-dx;
				dx+=7+8;
				DrawRoundFillBox(tab_snow[n].x+dx , y, 16 , 16, 0, 0xffffffff);
				break;
				}
			case 4:
				{
				dx= (y & 15);
				if(y & 16) dx=15-dx;
				dx>>=1;
				dx+=3+6;
				DrawRoundFillBox(tab_snow[n].x+dx , y, 12 , 12, 0, 0xffffffff);
				break;
				}
			}

		
		tab_snow[n].y+=2+tab_snow[n].velocity;
		
		}
	init=0;

	SetTexture(NULL);
}

//---------------------------------------------------------------------------------
/* GUI routines and datas*/
//---------------------------------------------------------------------------------

float angle_icon=1.0f;
Mtx	temp_mtx;

void DrawIcon(int px,int py, u32 frames2)
{
float f;

	#ifndef ALTERNATIVE_VERSION
	f=angle_icon+ ((use_icon2 & 3)!=0 ? 0.0f : 20.0f);
	#else
	f=angle_icon;
	#endif

	f=(2.0f*3.141592f)*f/360.0f;
	
	guMtxIdentity(temp_mtx);
	guMtxRotRad(temp_mtx,'Z',f);

	#ifndef ALTERNATIVE_VERSION
	guMtxTransApply(temp_mtx, temp_mtx, (float) (px+8), (float) (py+8), 0.0f);
	#else
	guMtxTransApply(temp_mtx, temp_mtx, (float) (px+8), (float) (py+8), 0.0f);
	#endif

	guMtxConcat(modelView, temp_mtx, temp_mtx);
	GX_LoadPosMtxImm(temp_mtx,	GX_PNMTX0); // carga la matriz mundial como identidad
	GX_SetCurrentMtx(GX_PNMTX0); // selecciona la matriz

	#ifndef ALTERNATIVE_VERSION
	if(!(use_icon2 & 3))
		DrawSurface(&text_icon[(frames2 & 8)!=0 && ( (frames2 & (255))<16) ], -24,-24, (use_icon2 & 4) ? 64 : 48, (use_icon2 & 4) ? 64 : 48, 0, 0xffffffff);
	else
		DrawSurface(&text_icon[4+(use_icon2 & 3)+((use_icon2 & 3)>1)+(((frames2 & 4)!=0 && ((frames2 & 128)>110 || (frames2 & 32)!=0)) && (use_icon2 & 3)==1) ],
		-24,-24, (use_icon2 & 4) ? 64 : 48, (use_icon2 & 4) ? 64 : 48, 0, 0xffffffff);
	#else
	//DrawSurface(&text_icon[(frames2 & 4)!=0 && ((frames2 & 128)>110 || (frames2 & 32)!=0)],/*px*/-24,/*py*/-24, 64, 64, 0, 0xffffffff); //OGG
	if(!(use_icon2 & 3))
		DrawSurface(&text_icon[(frames2 & 8)!=0 && ( (frames2 & (255))<16) ], -24,-24, (use_icon2 & 4) ? 64 : 48, (use_icon2 & 4) ? 64 : 48, 0, 0xffffffff);
	else
		DrawSurface(&text_icon[4+(use_icon2 & 3)+((use_icon2 & 3)>1)+(((frames2 & 4)!=0 && ((frames2 & 128)>110 || (frames2 & 32)!=0)) && (use_icon2 & 3)==1) ],
		-24,-24, (use_icon2 & 4) ? 64 : 48, (use_icon2 & 4) ? 64 : 48, 0, 0xffffffff);
	#endif

	GX_LoadPosMtxImm(modelView,	GX_PNMTX0); // carga la matriz mundial como identidad
}

void draw_text(char *text)
{
int n,m,len;
len=strlen(text);


for(n=0;n<13;n++)
	{
    if(scroll_text<0) m=n;
	else
		m=(n+scroll_text) % (len+4);

	if(m<len)
		s_printf("%c",text[m]);
	else
		s_printf(" ");
	}
}

void draw_box_text(char *text)
{
int n,m,len,len2;

len=strlen(text);
len2=len/68;

for(n=0;n<68;n++)
	{
	if(len<=68 || scroll_text<0) m=n;
	else m=(n+scroll_text) % (len+4); //m=n+68*((scroll_text>>5)  % len2);

	if(m<len)
		s_printf("%c",text[m]);
	else
		s_printf(" ");
	}
}

// Draw_button variables

int step_button=0;

int x_temp=0;

int Draw_button(int x,int y,char *cad)
{
int len=strlen(cad);

	SetTexture(&text_button[(step_button>>4) & 3]);
	
	if(px>=x && px<=x+len*8+32 && py>=y && py<y+56) DrawRoundFillBox(x-8, y-8, len*8+32+16, 56+16, 0, 0xffcfcfcf);
		else DrawRoundFillBox(x, y, len*8+32, 56, 0, 0xffcfcfcf);
	SetTexture(NULL);
	PX=x+16; PY= y+12; color= INK0;
	letter_size(8,32);

	s_printf("%s",cad);

	x_temp=x+len*8+32;

	if(px>=x && px<=x+len*8+32 && py>=y && py<y+56)
		{DrawRoundBox(x-8, y-8, len*8+32+16, 56+16, 0, 5, 0xfff08000);return 1;}
	
	DrawRoundBox(x, y, len*8+32, 56, 0, 4, 0xff606060);
	

return 0;
}

int Draw_button2(int x,int y,char *cad, int selected)
{
int len=strlen(cad);
unsigned color=0xffcfcfcf;

if(selected) color= 0xff3fcf3f;
if(selected==128) color= 0xff3f3fcf;

if(selected==129) color= 0xff3fcfcf;
if(selected==130) color= 0xffcf8fcf;

if(selected<0) color=0x80cfcfcf;

	SetTexture(&text_button[(step_button>>4) & 3]);
	
	if(selected>=0)
		{
		if(px>=x && px<=x+len*8+32 && py>=y && py<y+48) DrawRoundFillBox(x-8, y-8, len*8+32+16, 48+16, 0, color);
			else DrawRoundFillBox(x, y, len*8+32, 48, 0, color);
		}

	SetTexture(NULL);
	PX=x+16; PY= y+8; color= INK0;
	letter_size(8,32);

	s_printf("%s",cad);

	x_temp=x+len*8+32;

	if(selected>=0)
		if(px>=x && px<=x+len*8+32 && py>=y && py<y+48)
			{DrawRoundBox(x-8, y-8, len*8+32+16, 48+16, 0, 5, 0xfff08000);return 1;}
	
	DrawRoundBox(x, y, len*8+32, 48, 0, 4, 0xff606060);
	

return 0;
}

extern void* SYS_AllocArena2MemLo(u32 size,u32 align);

void * create_png_texture(GXTexObj *texture, void *png, int repeat)
{
PNGUPROP imgProp;
IMGCTX ctx;
char *texture_buff;
s32 ret;

	/* Select PNG data */
	ctx = PNGU_SelectImageFromBuffer(png);
	if (!ctx)
		{return NULL;}

	/* Get image properties */
	ret = PNGU_GetImageProperties(ctx, &imgProp);
	if (ret != PNGU_OK)
		{return NULL;}
    
	if(repeat & 32)
	{
		texture_buff= (void *) SYS_AllocArena2MemLo(imgProp.imgWidth * imgProp.imgHeight *4+2048,32);

		repeat&=~32;
	}
	else
		texture_buff=memalign(32, imgProp.imgWidth * imgProp.imgHeight *4+2048);
	if(!texture_buff) {return NULL;}

	
	PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, texture_buff, 255);
	PNGU_ReleaseImageContext(ctx);

	DCFlushRange( texture_buff, imgProp.imgWidth * imgProp.imgHeight *4);

	if(repeat) GX_InitTexObj(texture,texture_buff, imgProp.imgWidth, imgProp.imgHeight, GX_TF_RGBA8, GX_REPEAT ,  GX_REPEAT , GX_FALSE);
	else GX_InitTexObj(texture,texture_buff, imgProp.imgWidth, imgProp.imgHeight, GX_TF_RGBA8, GX_CLAMP ,  GX_CLAMP , GX_FALSE);

	GX_InitTexObjLOD(texture, // objeto de textura
						 GX_LINEAR, // filtro Linear para cerca
						 GX_LINEAR, // filtro Linear para lejos
						 0, 0, 0, 0, 0, GX_ANISO_1);

return texture_buff;
}


//---------------------------------------------------------------------------------
/* GUI background */
//---------------------------------------------------------------------------------

static int spinner_mode,spinner_percent;
static char spinner_str[256]="";
int spinner_ctrl=0;


void draw_background()
{
static int frames2=0;
static int frames=0;
static int frames1=0;

static int vaiven=0;
static float anglez=0.0f;


guMtxIdentity(modelView);
GX_LoadPosMtxImm(modelView,	GX_PNMTX0); // carga la matriz mundial como identidad
GX_SetCurrentMtx(GX_PNMTX0); // selecciona la matriz

ChangeProjection(0,SCR_HEIGHT<=480 ? -12: 0,SCR_WIDTH,SCR_HEIGHT+(SCR_HEIGHT<=480 ? 16: 0));
if(frames==(12+2*(SCR_HEIGHT<=480))) {frames1++;frames=0;}
SetTexture(&text_background[(frames1 % 3)]);

frames++;
	if((in_black & 3)==1)
		{

		if(background_png) {SetTexture(&text_background[3]);DrawRoundFillBox(0, -12, SCR_WIDTH, SCR_HEIGHT+24, 999, 0xffffffff);}
		else {SetTexture(NULL);DrawRoundFillBox(0, -12, SCR_WIDTH, SCR_HEIGHT+24, 999, 0xff000000);}

		
		}
	else
		{
		ConfigureForTexture(10);
		GX_Begin(GX_QUADS,  GX_VTXFMT0, 4);
		AddTextureVertex(0, -12, 999, BACK_COLOR, 0, (frames2 & 1023));
		AddTextureVertex(SCR_WIDTH, -12, 999, BACK_COLOR, 1023, (frames2 & 1023)); 
		AddTextureVertex(SCR_WIDTH, SCR_HEIGHT+24, 999, BACK_COLOR, 1023, 1024+(frames2 & 1023)); 
		AddTextureVertex(0, SCR_HEIGHT+24, 999, BACK_COLOR, 0, 1024+(frames2 & 1023)); 
		GX_End();
		}


if(is_16_9)
	{
	ChangeProjection(0,SCR_HEIGHT<=480 ? -12: 0,848,SCR_HEIGHT+(SCR_HEIGHT<=480 ? 16: 0));
	guMtxIdentity(modelView);

	GX_SetCurrentMtx(GX_PNMTX0); // selecciona la matriz
	guMtxTrans(modelView, 104.0f, 0.0f, 0.0f);
	GX_LoadPosMtxImm(modelView,	GX_PNMTX0); // carga la matriz mundial como identidad
	
	}

if(time_sleep==1)
	{
	if(vaiven==0)
		{
		anglez+=0.002f;
		if(anglez>=0.261f) {vaiven=1;}
		}
	else
		{
		anglez-=0.002f;
		if(anglez<=-0.261f) {vaiven=0;}
		}
	guMtxIdentity(temp_mtx);
	guMtxTransApply(modelView, modelView, -320.0f, (float) (-SCR_HEIGHT/2), 0.0f);

	guMtxRotRad(temp_mtx,'Z',anglez);
	guMtxTransApply(temp_mtx,temp_mtx, 320.0f, (float) (SCR_HEIGHT/2), 0.0f);

	guMtxConcat(temp_mtx, modelView, modelView);
	GX_LoadPosMtxImm(modelView,	GX_PNMTX0); // carga la matriz mundial como identidad
	GX_SetCurrentMtx(GX_PNMTX0); // selecciona la matriz
	}
else {vaiven=0;anglez=0.0f;}



}

void display_spinner_draw()
{

//Screen_flip();

	autocenter=1;
	/*SetTexture(&text_background2);
	DrawRoundFillBox(0, 0, SCR_WIDTH, SCR_HEIGHT, 999, 0xffa0a0a0);*/

	draw_background();

	SetTexture(NULL);
	
	DrawFillSlice(SCR_WIDTH/2, SCR_HEIGHT/2, 210, 210, 10, 0, 360, 0x8f60a0a0);
	SetTexture(MOSAIC_PATTERN);
	DrawFillSlice(SCR_WIDTH/2, SCR_HEIGHT/2, 210, 210, 10, 0, 360*spinner_percent/100, 0xffa06000);
	SetTexture(NULL);
	DrawSlice(SCR_WIDTH/2, SCR_HEIGHT/2, 210, 210, 10, 4, 0, 360,  0xcf000000);
    

	PX=0; PY=SCR_HEIGHT/2-16; color= INK0; 
	letter_size(16,32);
	SelectFontTexture(1);
	if(spinner_mode)
		color=0xffffffff;
	else
		color=INK0;

	s_printf("%s", spinner_str);
	color=INK0;
	autocenter=0;

	draw_snow();
	
	Screen_flip();
	if(!spinner_ctrl) remote_call_abort();

}


int thread_in_second_plane=0;

char my_perror_error[256]="";

void my_perror(char * err)
{
int n;

if(thread_in_second_plane) {sprintf(my_perror_error,"Error: %s",err);return;}
	Screen_flip();

	for(n=0;n<240;n++)
		{
		autocenter=1;
		draw_background();

		SetTexture(NULL);
		DrawRoundFillBox((SCR_WIDTH-540)/2, SCR_HEIGHT/2-32, 540, 64, 999, 0xa00000ff);
		DrawRoundBox((SCR_WIDTH-540)/2, SCR_HEIGHT/2-32, 540, 64, 999, 4, 0xa0000000);

		PX=0; PY=SCR_HEIGHT/2-16; color= INK0; 
		letter_size(8,32);
		SelectFontTexture(1);
		s_printf("Error: %s",err);
		autocenter=0;

		draw_snow();
		Screen_flip();
		}
//	sleep(4);
}


void display_spinner(int mode, int percent, char *str)
{
	spinner_mode=mode;
	spinner_percent=percent;
	if(str)
		strcpy(spinner_str,str);
	else spinner_str[0]=0;
    
	if(mode==0) 
		{spinner_ctrl=1;remote_call(display_spinner_draw);}

	// abort
	if(exit_by_reset) {exit_by_reset=0;abort_signal=1;sleep(4);spinner_ctrl=0;remote_call_abort();usleep(100*1000);}
	SYS_SetResetCallback(reset_call);

	if(mode)
	{sleep(4);spinner_ctrl=0;remote_call_abort();usleep(100*1000);}

}


char cabecera2_str[128]="";

int signal_draw_cabecera2=0;

void draw_cabecera2()
{
static int frames2=0;
int n;

int ylev=(SCR_HEIGHT-440);

#define SLICE_LEN 180


	if(SCR_HEIGHT>480) ylev=(SCR_HEIGHT-440)/2;

	draw_background();
		

	/*SetTexture(NULL);
    DrawRoundFillBox(20, ylev, 148*4, 352, 0, 0xffafafaf);
	DrawRoundBox(20, ylev, 148*4, 352, 0, 4, 0xff303030);*/
    SetTexture(&text_icon[3]);

	//DrawFillBox(20, ylev, 148*4, 352, 0, 0xffffffff);
    for(n=0;n<8;n++)
	{
	u32 color;
	int vel=7-(frames2>>6);
	if(vel<0) vel=0;
	int ang=(((frames2>>vel) & 127)<<7)-2048+(n<<11);
	//vel=7;
	//int ang=((frames2) & 31)<<10;
	int xx1=SCR_WIDTH/2+(SLICE_LEN*seno2((ang) & 16383))/16384,yy1=SCR_HEIGHT/2-(SLICE_LEN*coseno2((ang) & 16383))/16384;
	int xx2=SCR_WIDTH/2+(SLICE_LEN*seno2((ang+4096) & 16383))/16384,yy2=SCR_HEIGHT/2-(SLICE_LEN*coseno2((ang+4096) & 16383))/16384;
	int xx3=SCR_WIDTH/2+(SLICE_LEN*seno2((ang+4096*2) & 16383))/16384,yy3=SCR_HEIGHT/2-(SLICE_LEN*coseno2((ang+4096*2) & 16383))/16384;
	int xx4=SCR_WIDTH/2+(SLICE_LEN*seno2((ang+4096*3) & 16383))/16384,yy4=SCR_HEIGHT/2-(SLICE_LEN*coseno2((ang+4096*3) & 16383))/16384;

	
	if(n==0) color=0xffffffff; else color=0x27ffffff;

	if(mode_disc & 2) color=(color & 0xff000000) | 0x20ff6f;

	SetTexture(&text_icon[3]);
	ConfigureForTexture(10);
	GX_Begin(GX_TRIANGLESTRIP,  GX_VTXFMT0, 5);

	AddTextureVertex(xx1, yy1, 999, color, 1, 1);
	AddTextureVertex(xx2, yy2, 999, color, 1024, 1); 
	AddTextureVertex(xx3, yy3, 999, color, 1024, 1024); 
	AddTextureVertex(xx4, yy4, 999, color, 1, 1024);
	AddTextureVertex(xx1, yy1, 999, color, 1, 1);
	GX_End();

	if(vel!=0 || signal_draw_cabecera2) break;

	}


	SetTexture(NULL);



	PX= 0; PY=ylev-32; color= INK1; 
				
	bkcolor=0;
	letter_size(16,32);

	autocenter=1;
	bkcolor=num_partitions=0;//0xb0f0f0f0;
	s_printf("%s", cabecera2_str);
	bkcolor=0;
#undef SLICE_LEN	
	draw_snow();
	Screen_flip();
	frames2+=20;

}

void cabecera(char *cab)
{
	int ylev=(SCR_HEIGHT-440);


	if(SCR_HEIGHT>480) ylev=(SCR_HEIGHT-440)/2;

	draw_background();
		

	SetTexture(&text_button[0]);
    DrawRoundFillBox(20, ylev, 148*4, 352, 0, 0xffafafaf);
	SetTexture(NULL);
	DrawRoundBox(20, ylev, 148*4, 352, 0, 4, 0xff303030);

	PX= 0; PY=ylev-32; color= INK1; 
				
	bkcolor=0;
	letter_size(16,32);

	autocenter=1;
	bkcolor=num_partitions=0;//0xb0f0f0f0;
	s_printf("%s", cab);
	bkcolor=0;
	

}

void cabecera2(char *str)
{
	strcpy(cabecera2_str, str);
	remote_call(draw_cabecera2);
}

void draw_add_game_mess()
{
int ylev=(SCR_HEIGHT-440);

	cabecera( &letrero[idioma][23][0]);
	PX=0;PY=ylev+352/2-16;
	color=INK0;
	s_printf("%s",&letrero[idioma][41][0]);
	draw_snow();
	Screen_flip();
}

int altdol_frames2=0;

void draw_altdolscr()
{

int n;

int ylev=(SCR_HEIGHT-440);

#define SLICE_LEN 180


	if(SCR_HEIGHT>480) ylev=(SCR_HEIGHT-440)/2;

	draw_background();

	SetTexture(&text_button[0]);
    DrawRoundFillBox(20, ylev, 148*4, 352, 999, 0xffafafaf);
	SetTexture(NULL);
	DrawRoundBox(20, ylev, 148*4, 352, 999, 4, 0xff303030);

	PX= 0; PY=ylev-32; color= INK0; 
				
	bkcolor=0;
	letter_size(16,32);

	autocenter=1;
	bkcolor=0;
	s_printf("%s", &letrero[idioma][31][0]);
	bkcolor=0;
	
	
	autocenter=0;
	letter_size(16,32);
		

	/*SetTexture(NULL);
    DrawRoundFillBox(20, ylev, 148*4, 352, 0, 0xffafafaf);
	DrawRoundBox(20, ylev, 148*4, 352, 0, 4, 0xff303030);*/
    SetTexture(&text_icon[3]);

	//DrawFillBox(20, ylev, 148*4, 352, 0, 0xffffffff);
    for(n=0;n<8;n++)
	{
	u32 color;
	int vel=7-(altdol_frames2>>6);
	if(vel<0) vel=0;
	int ang=(((altdol_frames2>>vel) & 127)<<7)-2048+(n<<11);
	//vel=7;
	//int ang=((frames2) & 31)<<10;
	int xx1=SCR_WIDTH/2+(SLICE_LEN*seno2((ang) & 16383))/16384,yy1=ylev+352/2-(SLICE_LEN*coseno2((ang) & 16383))/16384;
	int xx2=SCR_WIDTH/2+(SLICE_LEN*seno2((ang+4096) & 16383))/16384,yy2=ylev+352/2-(SLICE_LEN*coseno2((ang+4096) & 16383))/16384;
	int xx3=SCR_WIDTH/2+(SLICE_LEN*seno2((ang+4096*2) & 16383))/16384,yy3=ylev+352/2-(SLICE_LEN*coseno2((ang+4096*2) & 16383))/16384;
	int xx4=SCR_WIDTH/2+(SLICE_LEN*seno2((ang+4096*3) & 16383))/16384,yy4=ylev+352/2-(SLICE_LEN*coseno2((ang+4096*3) & 16383))/16384;

	
	if(n==0) color=0xffffffff; else color=0x27ffffff;

	if(mode_disc & 2) color=(color & 0xff000000) | 0x20ff6f;

	SetTexture(&text_icon[3]);
	ConfigureForTexture(10);
	GX_Begin(GX_TRIANGLESTRIP,  GX_VTXFMT0, 5);

	AddTextureVertex(xx1, yy1, 999, color, 1, 1);
	AddTextureVertex(xx2, yy2, 999, color, 1024, 1); 
	AddTextureVertex(xx3, yy3, 999, color, 1024, 1024); 
	AddTextureVertex(xx4, yy4, 999, color, 1, 1024);
	AddTextureVertex(xx1, yy1, 999, color, 1, 1);
	GX_End();

	if(vel!=0 /*|| signal_draw_cabecera2*/) break;

	}


	SetTexture(NULL);

	PX=0;PY=ylev+352/2-32;
	autocenter=1;letter_size(16,32);
	s_printf("%s", &letrero[idioma][51][0]);
	autocenter=0;
		

#undef SLICE_LEN	
    draw_snow();
	Screen_flip();
	altdol_frames2+=20;

}

void set_tex_pix_fx(int x, int y, unsigned color)
{
if(x<0 || x>127 || y<0 || y>127) return;

	screen_fx[x+(y<<7)]=color;
}

void set_text_screen_fx()
{
int n,m;
static int pos=256;
unsigned color1=0x80ffffff;

if(pos<-128) pos=1024;
memset(screen_fx,0,128*128*4);

for(n=0;n<128;n++)
	{

	color1=0x40ffffc0;
    for(m=0;m<4;m++)
		set_tex_pix_fx(pos-(n>>1)+m,n,color1);
	color1=((0x50-m)<<24) | 0xffffd0;
	for(m=8;m<20;m++)
		set_tex_pix_fx(pos-(n>>1)+m,n,color1);
	color1=0x40ffffc0;
	for(m=24;m<28;m++)
		set_tex_pix_fx(pos-(n>>1)+m,n,color1);
	
	}

CreateTexture(&text_screen_fx, TILE_SRGBA8, screen_fx, 128, 128, 0);
pos-=6;
}


void circle_select(int x, int y, char symb, int selected)
{
	SetTexture(NULL);

	if(!in_black)
	{
	if(selected)
		{
		DrawFillEllipse(x, y, 50, 50, 0, 0xc0f0f0f0);
		letter_size(32,64);
		PX= x-16; PY= y-32; color= INK0; bkcolor=0;
		s_printf("%c", symb);
		DrawEllipse(x, y, 50, 50, 0, 6, 0xc0f0f000);
		}
	else
		{
		DrawFillEllipse(x, y, 40, 40, 0, 0xc0f0f0f0);
		letter_size(32,48);
		PX= x-16; PY= y-24; color= INK0; bkcolor=0;
		s_printf("%c", symb);
		DrawEllipse(x, y, 40, 40, 0, 6, 0xc0000000);
		}
	}
	else
	{
    
	if(selected)
		{
		DrawFillEllipse(x, y, 40, 40, 0, 0xc0f0f0f0);
		letter_size(32,64);
		PX= x-16; PY= y-32; color= INK0; bkcolor=0;
		s_printf("%c", symb);
		DrawEllipse(x, y, 40, 40, 0, 6, 0xc0f0f000);
		}
	else
		{
		DrawFillEllipse(x, y, 30, 30, 0, 0xc0f0f0f0);
		letter_size(32,48);
		PX= x-16; PY= y-24; color= INK0; bkcolor=0;
		s_printf("%c", symb);
		DrawEllipse(x, y, 32, 32, 0, 2, 0xc0f00000);
		}
	}
}

char *down_mess=NULL;

int down_frame=0;

void wait_splash_scr()
{
unsigned color2;

int n;

	splash_scr();

	SelectFontTexture(1); // selecciona la fuente de letra extra

	letter_size(12,32);
					
	PX=0; PY= SCR_HEIGHT/2-32+64; color= INK0; 
					
	bkcolor=0;
	autocenter=1;
	SetTexture(NULL);
	
	DrawRoundFillBox((SCR_WIDTH-540)/2, SCR_HEIGHT/2-16-32+64, 540, 64, 0, 0xa00000ff);
	DrawRoundBox((SCR_WIDTH-540)/2, SCR_HEIGHT/2-16-32+64, 540, 64, 0, 4, 0xa0000000);
		
	if(down_mess)
		{
		if(strlen(down_mess)>40) letter_size(8,32);
		s_printf("%s",down_mess);
		}

	autocenter=0;

	if(down_frame>=0)
		for(n=0;n<((down_frame>>4) % 10)+1;n++)
			{
			color2=((1+(((down_frame>>2)+n) & 3))*64)-1;

			color2=0xd8000000 | (color2<<0) | ((15-n)<<20);
			DrawFillEllipse(140+n*40,SCR_HEIGHT/2-16+32+80+32, 16, 16, 0, color2);
			DrawEllipse(140+n*40,SCR_HEIGHT/2-16+32+80+32, 16, 16, 0, 2, (color2 & 0xff000000));
			}

	draw_snow();
	Screen_flip();

	if(down_frame>=0) down_frame++;
}

void down_uload_gfx()
{
unsigned color2;

int n;
	draw_background();

	SelectFontTexture(1); // selecciona la fuente de letra extra

	letter_size(12,32);
					
	PX=0; PY= SCR_HEIGHT/2-32; color= INK0; 
					
	bkcolor=0;
	autocenter=1;
	SetTexture(NULL);
	
	DrawRoundFillBox((SCR_WIDTH-540)/2, SCR_HEIGHT/2-16-32, 540, 64, 999, 0xa00000ff);
	DrawRoundBox((SCR_WIDTH-540)/2, SCR_HEIGHT/2-16-32, 540, 64, 999, 4, 0xa0000000);
		
	if(down_mess)
		s_printf("%s",down_mess);

	autocenter=0;

	if(down_frame>=0)
		for(n=0;n<((down_frame>>4) % 10)+1;n++)
			{
			color2=((1+(((down_frame>>2)+n) & 3))*64)-1;

			color2=0xd8000000 | (color2<<0) | ((15-n)<<20);
			DrawFillEllipse(140+n*40,SCR_HEIGHT/2-16+32+80, 16, 16, 0, color2);
			DrawEllipse(140+n*40,SCR_HEIGHT/2-16+32+80, 16, 16, 0, 2, (color2 & 0xff000000));
			}

	draw_snow();
	Screen_flip();

	if(down_frame>=0) down_frame++;
}


void happy_new_year(int nyear)
{
int frame=0;
int n,m;

u32 color2;
s16 xx,yy;
float f;

char year[16];
char happy[]="Happy New Year!!!";
char press_any[]="Press Any Button To Exit";

s16 stars[256][2];

struct _fuegos
	{
	float x,y;
	int fuerza;
	u32 color;
	}
fuegos[8];

struct _particles_d
	{
	float dx,dy;
	}
particles_d[256];

	time_sleep=0;
	SetVideoSleep(0);

	SelectFontTexture(0);
	
	if(nyear>=0)
		sprintf(year,"%i",nyear+1900);

	srand(1);

	for(m=0;m<8;m++) fuegos[m].fuerza=-1;



	for(n=0;n<256;n++)
		{
		
		int ang=rand() & 16383;

		stars[n][0]= (rand() % (848+128))-128;
		stars[n][1]= (rand() % SCR_HEIGHT);

		f=((float)((1+(rand() & 255))*2))/128.0f;

		particles_d[n].dx=f*((float) (seno2((ang) & 16383))/16384.0f);
		particles_d[n].dy=f*((float) (coseno2((ang) & 16383))/16384.0f);
		
		}

	
	while(frame<60*30 || nyear<0)
		{

		if(rumble)
			{
			if(rumble<=7) wiimote_rumble(0); 
			rumble--;
			}
		else wiimote_rumble(0);

		WPAD_ScanPads(); // esto lee todos los wiimotes


		draw_background();
		DrawFillBox(-128, -32, 848+128, SCR_HEIGHT+32, 900, 0xff100000);
		
		SetTexture(NULL);

		ConfigureForColor();

		// dibuja estrellas

		for(n=0;n<256;n++)
		{
		
			xx=(s16) (stars[n][0]);
			yy=(s16) (stars[n][1]);

			if(n & 1) color2=0xffffffff;
			else color2=0xffcfcfcf;
			
			if((rand() & 31)!=1) // tililar
				{
				GX_Begin(GX_TRIANGLES,  GX_VTXFMT0, 3);
				

				AddColorVertex(xx+1, yy  , 0, color2);
				AddColorVertex(xx+2, yy+2, 0, color2);
				AddColorVertex(xx  , yy+2, 0, color2);

				GX_End();
				}
		}

		for(m=0;m<8;m++)
			{
			
			if(fuegos[m].fuerza<=-1 && ((rand()>>8) & 15)==0)
				{
				fuegos[m].x=SCR_WIDTH/2+(rand() & 511)-255;
				fuegos[m].y=SCR_HEIGHT/2+(rand() & 255)-127;
				fuegos[m].fuerza=127;
				switch((rand()>>8) % 5)
					{
					case 0:
						fuegos[m].color=0xffffff;break;
					case 1:
						fuegos[m].color=0x2f2fff;break;
					case 2:
						fuegos[m].color=0x2fff2f;break;
					case 3:
						fuegos[m].color=0x2fffff;break;
					case 4:
						fuegos[m].color=0xff3f3f;break;


					}

				snd_explo(m, fuegos[m].x);

				}


			f=((float ) (128-fuegos[m].fuerza))/2.0f;
			
			if(fuegos[m].fuerza>0)
				{
				int alpha;
				
					
				color2=fuegos[m].color;
				
				alpha=fuegos[m].fuerza;
				if(alpha<0) alpha=0;
				color2|=alpha<<25;
				
				for(n=0;n<256;n++)
					{
					xx=(s16) (fuegos[m].x+particles_d[n].dx*f);
					yy=(s16) (fuegos[m].y+particles_d[n].dy*f);

						GX_Begin(GX_TRIANGLESTRIP,  GX_VTXFMT0, 4);

						AddColorVertex(xx+3, yy  , 0, color2);
						AddColorVertex(xx+6, yy+3, 0, color2);
						AddColorVertex(xx  , yy+3, 0, color2);
						AddColorVertex(xx+3, yy+6  , 0, color2);
						
						GX_End();	
					}
				
				fuegos[m].fuerza-=2;
				}


			}
		
		
        if(nyear>=0)
			{
			letter_size(64,128);	

			PX= 0; 
			
						
			bkcolor=0;
			PX= SCR_WIDTH/2-(strlen(year)*64)/2;	
			for(n=0; n<strlen(year); n++)
				{
				if(((frame & 32)!=0) ^ (n & 1))
					color= 0xffffffff; 
				else
					color= 0xffcfcfcf; 
				PY=100+ (8*seno2((((n +((frame>>2) & 15))*2048)) & 16383))/16384;
				s_printf("%c", year[n]);
				}

			PX= SCR_WIDTH/2-(strlen(happy)*32)/2; color= 0xffffffff; 
						
			bkcolor=0;
			letter_size(32,64);	
			autocenter=0;
		
			for(n=0; n<strlen(happy); n++)
				{
				switch((n+(frame>>2)) & 3)
					{
					case 0:
						color=0xff4fff4f;break;
					case 1:
						color=0xffff4f4f;break;
					case 2:
						color=0xff4fffff;break;
					case 3:
						color=0xff4f4fff;break;
					}

				PY=280+ (8*seno2((((n +((frame>>2) & 15))*2048)) & 16383))/16384;
				s_printf("%c", happy[n]);
				}

			letter_size(12,32);	

			PX= 0; color= 0xffffffff; 
						
			bkcolor=0;
			if(frame>60*10)
				{
				PX= SCR_WIDTH/2-(strlen(press_any)*12)/2;	
				for(n=0; n<strlen(press_any); n++)
					{
					PY=430+ (2*seno2((((n +((frame>>2) & 15))*2048)) & 16383))/16384;
					s_printf("%c", press_any[n]);
					}
				}
			}
		

		frame++;
		Screen_flip();

		wiimote_read();

		if(new_pad && frame>10)
				{
				time_sleep=60*5;
				SetVideoSleep(0);
				break;
				}

		//if(frame>60*5 && new_pad) 
		}

	SelectFontTexture(1);

time_sleep=60*5;
}

extern u32 sd_clusters;
extern u32 usb_clusters;

void cluster_warning()
{
int n=0;

while(n<15*60)
	{
	int ylev=(SCR_HEIGHT-440);

	if(rumble)
			{
			if(rumble<=7) wiimote_rumble(0); 
			rumble--;
			}
		else wiimote_rumble(0);

		WPAD_ScanPads(); // esto lee todos los wiimotes

	splash_scr();

	SetTexture(NULL);
    DrawRoundFillBox(20, ylev, 148*4, 352+32, 0, 0xcf600060);
	DrawRoundBox(20, ylev, 148*4, 352+32, 0, 4, 0xff900090);

	PX= 0; PY=ylev+8; color= 0xff00ffff; 
				
	bkcolor=0;

	letter_size(32,64);
		
	autocenter=1;
	s_printf("%s","WARNING!");
	

	letter_size(12,24);
	PY+=80;
    color= 0xffffffff; 

	if(sd_clusters && sd_clusters<32)
		{
		s_printf("The SD Device uses %uKB as cluster size", (sd_clusters*512)/1024); 
		PY+=32;
		}
	if(usb_clusters && usb_clusters<32)
		{
		s_printf("The USB Device uses %uKB as cluster size", (usb_clusters*512)/1024);
		PY+=32;
		}
	s_printf("To increase the speed using FAT you must use");
	PY+=32;
	s_printf("at least 16KB as cluster size and remember you");
	PY+=32;
	s_printf("to watch the file fragmentation!!!");
	PY+=32;
	s_printf("use 'format unit: /FS:FAT32 /Q /A:32k'");
	PY+=32;
	s_printf("from 'CMD' in Windows to resolve it.");

	PY+=32;color= 0xff00ffff; 
	if(n & 8) s_printf("< Press a button >");
	
	
	autocenter=0;
	Screen_flip();
	
	wiimote_read();
    
	if(exit_by_reset) break;

	if(new_pad && n>10)
				{
				time_sleep=60*5;
				SetVideoSleep(0);
				break;
				}

    n++;
	}
}

