#pragma once

#include <fstream>
/* Write point files to a ply file */
/************** ply header *********************************/
static char ply_header[] = 
"ply\n"
"format ascii 1.0\n"
"element face 0\n"
"property list uchar int vertex_indices\n"
"element vertex %d\n"
"property float x\n"
"property float y\n"
"property float z\n"
"property uchar diffuse_red\n"
"property uchar diffuse_green\n"
"property uchar diffuse_blue\n"
"end_header\n";

static char ply_header_normal[] = 
"ply\n"
"format ascii 1.0\n"
"element face 0\n"
"property list uchar int vertex_indices\n"
"element vertex %d\n"
"property float x\n"
"property float y\n"
"property float z\n"
"property float nx\n"
"property float ny\n"
"property float nz\n"
"end_header\n";

/* Write point files to a ply file */
static void DumpPointsToPly(const char *output_directory, const char *filename, std::vector<CvPoint3D32f> g_points, std::vector<CvPoint3D32f> g_colors)
{
	char ply_out[256];
    sprintf(ply_out, "%s/%s", output_directory, filename);

	printf("write out %s\n", ply_out);
    FILE *f = fopen(ply_out, "w");

    if (f == NULL) {
	printf("Error opening file %s for writing\n", ply_out);
	return;
    }

	
    /* Print the ply header */
	fprintf(f, ply_header,g_points.size());

	for(int i=0; i<g_points.size(); i++)
	{
		/* Output the vertex */
		fprintf(f, "%.5f %.5f %.5f ", g_points[i].x, g_points[i].y, g_points[i].z );
		fprintf(f, "%.0f %.0f %.0f \n", g_colors[i].x, g_colors[i].y, g_colors[i].z );
	}
     

 fclose(f);
}

/* Write point files to a ply file */
static void DumpPointsToPly(const char *output_directory, const char *filename, std::vector<CvPoint3D32f> g_points )
{
	char ply_out[256];
    sprintf(ply_out, "%s/%s", output_directory, filename);

	printf("write out %s\n", ply_out);
    FILE *f = fopen(ply_out, "w");

    if (f == NULL) {
	printf("Error opening file %s for writing\n", ply_out);
	return;
    }

	
    /* Print the ply header */
	fprintf(f, ply_header_normal,g_points.size()/2);

	for(long i=0; i<g_points.size(); i+=2)
	{
		/* Output the vertex */
		fprintf(f, "%.3f %.3f %.3f ", g_points[i].x, g_points[i].y, g_points[i].z );//pos
		fprintf(f, "%.3f %.3f %.3f \n", g_points[i+1].x, g_points[i+1].y, g_points[i+1].z );//normal
	}
     

 fclose(f);
}