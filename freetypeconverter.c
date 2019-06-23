#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include <ctype.h>

FT_Library library;
FT_Face face;
unsigned int rows_max=0;
unsigned int width_max=0;
unsigned int buffer[40]={0};

char *strlwr(char *str)
{
	unsigned char *p = (unsigned char *)str;

	while (*p) 
	{
		*p = tolower((unsigned char)*p);
		p++;
	}

	return str;
}

void draw_char(char c, int baseline, int debug)
{
	int error=0;
	int glyph_index=0, j=0;
	unsigned int i=0, k=0, width_count=0;
	unsigned char x;

	glyph_index=FT_Get_Char_Index(face, c);	// look for c
//	printf("glyph_index: %i\n", glyph_index);
	error=FT_Load_Glyph(face,glyph_index,FT_LOAD_TARGET_MONO);	// load glyph, optimized for mono hinting
	if(error)
	{
		printf("error load glyph\n");
		exit(-1);
	}
	error=FT_Render_Glyph(face->glyph,FT_RENDER_MODE_MONO);	// render mono pixmap
	if(error)
	{
		printf("error render glyph\n");
		exit(-1);
	}
//	printf("glyph_index: %i\n", face->glyph->glyph_index);	// not available in my version
//	printf("format: %i\n", face->glyph->format);
//	printf("bitmap_left: %i\n", face->glyph->bitmap_left);
	if(debug) printf("bitmap_top: %i\t", face->glyph->bitmap_top);
	int top_offset=face->glyph->bitmap_top;

	FT_Bitmap bitmap=face->glyph->bitmap;
//	printf("bitmap-rows:       %i\n", bitmap.rows);
	if(debug) printf("bitmap-width:      %i\n", bitmap.width);
//	printf("bitmap-pitch:      %i\n", bitmap.pitch);
//	printf("bitmap-num_grays:  %i\n", bitmap.num_grays);
//	printf("bitmap-pixel_mode: %i\n", bitmap.pixel_mode);

	if(bitmap.rows>rows_max) rows_max=bitmap.rows;
	if(bitmap.width>width_max) width_max=bitmap.width;

	unsigned char *s=bitmap.buffer;
	for(i=0; i<bitmap.rows; i++)	// go thruogh each row
	{
		width_count=0;
		for(j=0; j<bitmap.pitch; j++)	// get every byte. one byte stands for 8 pixels
		{
			x=*s;
//			printf("%02x - ",(unsigned char) x);
			s++;
			k=0x80;	// start width top bit because its the left most
			do
			{
				if(width_count<bitmap.width)	// as long as smaller than width of character bitmap
				{
//					if(k&x) printf("X");
//					else printf(".");
					if(k&x) buffer[width_count]|=(1<<(i+baseline-top_offset));
			//		printf("%02x! ",(unsigned char) k);
				}
				k=k>>1;		// move one pixel right
				width_count++;
			}while(k>=1);	// until the end of the byte
		}
//		printf("-\n");
	}
}


int main(int argc, char* argv[])
{
	int error=0;
	int i=0,j=0;
	int baseline=12, fontsize=16, heightbyte=2, debug=0;
	int fontpointer=0, namepointer=0;
	char fontname[200]={0};
	char fontfilename[200]={0};
	FILE *fp;

	const char *versionstring={"freetype converter 0.1 - 2019-06-23"};

	for(i=1;i<argc;i++)
	{
		if(strncmp(argv[i], "-f", 2)==0)	// font
		{
			i++;
			fontpointer=i;
		}
		if(strncmp(argv[i], "-n", 2)==0)	// name
		{
			i++;
			namepointer=i;
		}
		if(strncmp(argv[i], "-b", 2)==0)	// baseline
		{
			i++;
			baseline=atoi(argv[i]);
		}
		if(strncmp(argv[i], "-s", 2)==0)	// fontsize
		{
			i++;
			fontsize=atoi(argv[i]);
		}
		if(strncmp(argv[i], "-h", 2)==0)	// height in bytes
		{
			i++;
			heightbyte=atoi(argv[i]);
		}
		if(strncmp(argv[i], "-d", 2)==0)	// debug
		{
			debug=1;
		}
	}

	printf("%s\n",versionstring);
	if(fontpointer==0)	// check if font was defined
	{
		printf("define font file!\n");
		exit(-1);
	}
// print settings
	if(namepointer==0)
	{
		sprintf(fontname,"FONT");
		sprintf(fontfilename,"font.h");
	}
	else
	{
		sprintf(fontname,"%s",argv[namepointer]);
		sprintf(fontfilename,"%s.h",argv[namepointer]);
		strlwr(fontfilename);
	}
	printf("Font:            %s\n",argv[fontpointer]);
	printf("Font size:       %i\n",fontsize);
	printf("baseline:        %i\n",baseline);
	printf("height in bytes: %i\n", heightbyte);
	printf("name:            %s (%s)\n", fontname, fontfilename);

// start with freetype library
	error=FT_Init_FreeType(&library);
	if(error)
	{
		printf("an error occured during library initialization!\n");
		exit(-1);
	}
	printf("FreeType Version %i.%i.%i\n",FREETYPE_MAJOR,FREETYPE_MINOR,FREETYPE_PATCH);	// show freetype version
	error=FT_New_Face(library, argv[fontpointer],0,&face);
	if(error==FT_Err_Unknown_File_Format)
	{
		printf("the font file could be opened and read, but it appears that its font format is unsupported!\n");
		exit(-1);
	}
	else if(error)
	{
		printf("another error code means that the font file could not be opened or read, or that it is broken!\n");
		exit(-1);
	}
	fp=fopen(fontfilename, "w");	//open output file for writing
	if(fp==NULL)
	{
		printf("could not open output file!\n");
		exit(-1);
	}
	if(debug) printf("fonts in this file: %li\n", face->num_faces);
	if(debug) printf("num glyphse: %li\n", face->num_glyphs);
	if(debug) printf("num fixed sizes: %i\n", face->num_fixed_sizes);
//	error=FT_Set_Char_Size(face,0,12*64,72,72);	// 72 dpi - 1 point is 1 pixel
	error=FT_Set_Pixel_Sizes(face,0,fontsize);	// set pixel size direct
	if(error)
	{
		printf("error set char size\n");
		exit(-1);
	}

	int start_char=0x20;
	int stop_char=0x7e;

	for(char z=start_char; z<=stop_char; z++)
	{
		draw_char(z,baseline, debug);
	}
	printf("width_max: %i\n",width_max);
	printf("rows_max:  %i\n",rows_max);

//writing some comments into the file
	fprintf(fp,"/* file generated by %s */\n",versionstring);
	fprintf(fp,"/* find the latest version on https://github.com/generationmake/freetypeconverter */\n");
	fprintf(fp,"/* converter called with: */\n/* ");
	for(i=0;i<argc;i++) fprintf(fp, "%s ", argv[i]);
	fprintf(fp," */\n\n");

	int array_length=(stop_char-start_char+1)*width_max*heightbyte+8;
	fprintf(fp,"#define %s_LEN %i\n",fontname,array_length);
//	fprintf(fp,"const byte %s[%s_LEN]=\n",fontname,fontname);	// for non-AVR uCs
	fprintf(fp,"const byte %s[%s_LEN] __attribute__((section(\".progmem.data\"))) = \n",fontname,fontname);	// for AVR uCs
	fprintf(fp,"{\n");
	fprintf(fp,"70,86,0x%02X,0x%02X,%i,%i,%i,%i,\n",start_char,stop_char,width_max,heightbyte*8,heightbyte,width_max*heightbyte);

	for(char z=start_char; z<=stop_char; z++)
	{
		for(i=0; i<40; i++) buffer[i]=0;
		draw_char(z,baseline, debug);
		for(j=0;j<heightbyte;j++)
		{
			for(i=0; i<(signed)width_max; i++) 
			{
				fprintf(fp,"0x%02X",(unsigned char) (((buffer[i])>>(j*8))&(0x000000FF)));
				if(i<((signed)width_max-1)) fprintf(fp,",");
			}
			if(j<(heightbyte-1)||z!=(stop_char)) fprintf(fp,",");
		}
		fprintf(fp,"\n");
	}

	fprintf(fp,"};\n");
	return 0;
}
