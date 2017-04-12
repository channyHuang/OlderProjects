/*    
 *  GLM library.  Wavefront .obj file format reader/writer/manipulator.
 *
 *  Written by Nate Robins, 1997.
 *  email: ndr@pobox.com
 *  www: http://www.pobox.com/~ndr
 */
#ifndef GLM_H_H

#define GLM_H_H

#include <GL/glut.h>
#include <string>
#include <cmath>

using std::string;


/* GLMmaterial: Structure that defines a material in a model. 
 */
struct GLMmaterial
{
   char* name;				    /* name of material */
   GLfloat diffuse[4];			/* diffuse component */
   GLfloat ambient[4];			/* ambient component */
   GLfloat specular[4];			/* specular component */
   GLfloat emmissive[4];		/* emmissive component */
   GLfloat shininess;			/* specular exponent */
   float  reflection;
   float  refraction;
   float  g;
};


/**
 * Structure that defines a model.
 */
class GLMmodel
{
  public: 
    enum RenderMode
    {
      GLM_NONE =     0,		// render with only vertices
      GLM_FLAT =     1 << 0,	// render with facet normals
      GLM_SMOOTH =   1 << 1,	// render with vertex normals
      GLM_TEXTURE =  1 << 2,	// render with texture coords
      GLM_COLOR =    1 << 3,	// render with colors
      GLM_MATERIAL = 1 << 4	// render with materials
    };


  public:
    GLMmodel (const string & filename);
    ~GLMmodel ();

    /**
     * "Unitize" a model by translating it to the origin and
     * scaling it to fit in a unit cube around the origin.
     * @return the scalefactor used.
     */
    GLfloat unitize ();


    /**
     * Calculates the dimensions (width, height, depth) of model
     * dimensions - array of 3 GLfloats (GLfloat dimensions[3])
     */
    GLvoid getDimensions (GLfloat* dimensions);

    /**
     * Reverse the polygon winding for all polygons in this model.
     * Default winding is counter-clockwise.  Also changes the
     * direction of the normals.
     */
    GLvoid reverseWinding ();

    /**
     * Generates facet normals for a model (by taking the cross
     * product of the two vectors derived from the sides of each
     * triangle).  Assumes a counter-clockwise winding.
     */
    GLvoid generateFacetNormals ();

    /**
     * Generates smooth vertex normals for a model.  First builds a
     * list of all the triangles each vertex is in.  Then loops
     * through each vertex in the the list averaging all the facet
     * normals of the triangles each vertex is in.  Finally, sets the
     * normal index in the triangle for the vertex to the generated
     * smooth normal.  If the dot product of a facet normal and the
     * facet normal associated with the first triangle in the list of
     * triangles the current vertex is in is greater than the cosine
     * of the angle parameter to the function, that facet normal is
     * not added into the average normal calculation and the
     * corresponding vertex is given the facet normal.  This tends to
     * preserve hard edges.  The angle to use depends on the model,
     * but 90 degrees is usually a good start.
     *   angle - maximum angle (in degrees) to smooth across
     */
    GLvoid generateVertexNormals (GLfloat angle = 90);

    /**
     * Generates texture coordinates according to a linear projection
     * of the texture map.  It generates these by linearly mapping the
     * vertices onto a square.
     */
    GLvoid generateLinearTextureCoordinates ();

    /**
     * Generates texture coordinates according to a spherical
     * projection of the texture map.  Sometimes referred to as
     * spheremap, or reflection map texture coordinates.  It generates
     * these by using the normal to calculate where that vertex would
     * map onto a sphere.  Since it is impossible to map something
     * flat perfectly onto something spherical, there is distortion at
     * the poles.  This particular implementation causes the poles
     * along the X axis to be distorted.
     */
    GLvoid generateSpheremapTextureCoordinates ();

    /**
     * Writes a model description in Wavefront .OBJ format to file.
     * filename - name of the file to write the Wavefront .OBJ format data to
     * mode     - a bitwise or of values describing what is written to the file
     *            GLM_NONE    -  write only vertices
     *            GLM_FLAT    -  write facet normals
     *            GLM_SMOOTH  -  write vertex normals
     *            GLM_TEXTURE -  write texture coords
     *            GLM_FLAT and GLM_SMOOTH should not both be specified.
     */
    GLvoid writeOBJ (const string & filename, GLuint mode);

    /**
     * Renders the model to the current OpenGL context using specified mode.
     * mode     - a bitwise OR of values describing what is to be rendered.
     *            GLM_NONE     -  render with only vertices
     *            GLM_FLAT     -  render with facet normals
     *            GLM_SMOOTH   -  render with vertex normals
     *            GLM_TEXTURE  -  render with texture coords
     *            GLM_COLOR    -  render with colors (color material)
     *            GLM_MATERIAL -  render with materials
     *            GLM_COLOR and GLM_MATERIAL should not both be specified.  
     *            GLM_FLAT and GLM_SMOOTH should not both be specified.  
     */
    GLvoid draw (GLuint mode = GLM_SMOOTH | GLM_MATERIAL);

    /* Generates and returns a display list for the model using specified mode.
     * mode     - a bitwise OR of values describing what is to be rendered.
     *            GLM_NONE     -  render with only vertices
     *            GLM_FLAT     -  render with facet normals
     *            GLM_SMOOTH   -  render with vertex normals
     *            GLM_TEXTURE  -  render with texture coords
     *            GLM_COLOR    -  render with colors (color material)
     *            GLM_MATERIAL -  render with materials
     *            GLM_COLOR and GLM_MATERIAL should not both be specified.  
     *            GLM_FLAT and GLM_SMOOTH should not both be specified.  
     */
    GLuint compile (GLuint mode = GLM_SMOOTH | GLM_MATERIAL);

    /**
     * Eliminate (weld) vectors that are within an epsilon of each other.
     * epsilon    - maximum difference between vertices
     */
    GLvoid weld (GLfloat epsilon = 0.00001);

  private:
    /**
     * Reads a model description from a Wavefront .OBJ file.
     * filename - name of the file containing the Wavefront .OBJ format data.  
     */
    void readOBJ (const char* filename);

  public: //YUCK :(
    /**
     * Structure that defines a triangle in a model.
     */
    struct GLMtriangle
    {
      GLuint vindices[3];		// array of triangle vertex indices
      GLuint nindices[3];		// array of triangle normal indices
      GLuint tindices[3];		// array of triangle texcoord indices
      GLuint findex;			// index of triangle facet normal
    };

    /**
     * Structure that defines a group in a model.
     */
    struct GLMgroup
    {
      char*     name;		        // name of this group
      GLuint    numtriangles;	        // number of triangles in this group
      GLuint*   triangles;		// array of triangle indices
      GLuint    material;               // index to material for group
      GLMgroup* next;		        // pointer to next group in model
    };

    char*    pathname;			// path to this model
    char*    mtllibname;		// name of the material library

    GLuint   numvertices;		// number of vertices in model
    GLfloat* vertices;			// array of vertices 

    GLuint   numnormals;		// number of normals in model
    GLfloat* normals;			// array of normals

    GLuint   numtexcoords;		// number of texcoords in model
    GLfloat* texcoords;			// array of texture coordinates

    GLuint   numfacetnorms;		// number of facetnorms in model
    GLfloat* facetnorms;		// array of facetnorms

    GLuint       numtriangles;		// number of triangles in model
    GLMtriangle* triangles;		// array of triangles

    GLuint       nummaterials;		// number of materials in model
    GLMmaterial* materials;		// array of materials

    GLuint       numgroups;		// number of groups in model
    GLMgroup*    groups;		// linked list of groups

    GLfloat position[3];		// position of the model

};
#endif