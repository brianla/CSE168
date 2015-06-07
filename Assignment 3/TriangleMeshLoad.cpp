#include "TriangleMesh.h"
#include "Console.h"
#include <algorithm>

#include "LodePNG.h"

#ifdef WIN32
// disable useless warnings
#pragma warning(disable:4996)
#endif


void
TriangleMesh::createSingleTriangle()
{
    m_normals = new Vector3[3];
    m_vertices = new Vector3[3];
    m_texCoords = new VectorR2[3];

    m_texCoords[0].x = 0.0f;
    m_texCoords[0].y = 0.0f;
    m_texCoords[1].x = 1.0f;
    m_texCoords[1].y = 0.0f;
    m_texCoords[2].x = 0.0f;
    m_texCoords[2].y = 1.0f;

    m_normalIndices = new TupleI3[1];
    m_vertexIndices = new TupleI3[1];
    m_texCoordIndices = new TupleI3[1];

    m_vertexIndices[0].x = 0;
    m_vertexIndices[0].y = 1;
    m_vertexIndices[0].z = 2;

    m_normalIndices[0].x = 0;
    m_normalIndices[0].y = 1;
    m_normalIndices[0].z = 2;

    m_texCoordIndices[0].x = 0;
    m_texCoordIndices[0].y = 1;
    m_texCoordIndices[0].z = 2;

    m_numTris = 1;
}

//************************************************************************
// You probably don't want to modify the following functions
// They are for loading .obj files
//************************************************************************

bool
TriangleMesh::load(char* file, const Matrix4x4& ctm)
{
    FILE *fp = fopen(file, "rb");
    if (!fp)
    {
        error("Cannot open \"%s\" for reading\n",file);
        return false;
    }
    debug("Loading \"%s\"...\n", file);

    loadObj(fp, ctm);
    debug("Loaded \"%s\" with %d triangles\n",file,m_numTris);
    fclose(fp);

    return true;
}

void
getIndices(char *word, int *vindex, int *tindex, int *nindex)
{
    char *null = " ";
    char *ptr;
    char *tp;
    char *np;

    // by default, the texture and normal pointers are set to the null string
    tp = null;
    np = null;

    // replace slashes with null characters and cause tp and np to point
    // to character immediately following the first or second slash
    for (ptr = word; *ptr != '\0'; ptr++)
    {
        if (*ptr == '/')
        {
            if (tp == null)
                tp = ptr + 1;
            else
                np = ptr + 1;

            *ptr = '\0';
        }
    }

    *vindex = atoi (word);
    *tindex = atoi (tp);
    *nindex = atoi (np);
}


void
TriangleMesh::loadObj(FILE* fp, const Matrix4x4& ctm)
{
    int nv=0, nt=0, nn=0, nf=0, nu=0;
    char line[81];
    while (fgets(line, 80, fp) != 0)
    {
        if (line[0] == 'v')
        {
            if (line[1] == 'n')
                nn++;
            else if (line[1] == 't')
                nt++;
            else
                nv++;
        }
        else if (line[0] == 'f')
        {
            nf++;
        }
		else if (line[0] == 'u')
		{
			nu++;
		}
    }
    fseek(fp, 0, 0);


    m_normals = new Vector3[std::max(nv,nf)];
    m_vertices = new Vector3[nv];

    if (nt)
    {   // got texture coordinates
        m_texCoords = new VectorR2[nt];
        m_texCoordIndices = new TupleI3[nf];
    }
    m_normalIndices = new TupleI3[nf]; // always make normals
    m_vertexIndices = new TupleI3[nf]; // always have vertices

    m_numTris = 0;
    int nvertices = 0;
    int nnormals = 0;
    int ntextures = 0;
	
	int currTexIndex = 0;

    Matrix4x4 nctm = ctm;
    nctm.invert();
    nctm.transpose();
    nctm.invert();

    while (fgets(line, 80, fp) != 0)
    {
        if (line[0] == 'v')
        {
            if (line[1] == 'n')
            {
                float x, y, z;
                sscanf(&line[2], "%f %f %f\n", &x, &y, &z);
                Vector3 n(x, y, z);
                m_normals[nnormals] = nctm*n;
                m_normals[nnormals].normalize();
                nnormals++;
            }
            else if (line[1] == 't')
            {
                float x, y;
                sscanf(&line[2], "%f %f\n", &x, &y);
                m_texCoords[ntextures].x = x;
                m_texCoords[ntextures].y = y;
                ntextures++;
            }
            else
            {
                float x, y, z;
                sscanf(&line[1], "%f %f %f\n", &x, &y, &z);
                Vector3 v(x, y, z);
                m_vertices[nvertices] = ctm*v;
                nvertices++;
            }
        }
		else if (line[0] == 'f')
		{
			m_triToTexture.push_back(currTexIndex);

			char s1[32], s2[32], s3[32];
			int v, t, n;
			sscanf(&line[1], "%s %s %s\n", s1, s2, s3);

			getIndices(s1, &v, &t, &n);
			m_vertexIndices[m_numTris].x = v - 1;
			if (n)
				m_normalIndices[m_numTris].x = n - 1;
			if (t)
				m_texCoordIndices[m_numTris].x = t - 1;
			getIndices(s2, &v, &t, &n);
			m_vertexIndices[m_numTris].y = v - 1;
			if (n)
				m_normalIndices[m_numTris].y = n - 1;
			if (t)
				m_texCoordIndices[m_numTris].y = t - 1;
			getIndices(s3, &v, &t, &n);
			m_vertexIndices[m_numTris].z = v - 1;
			if (n)
				m_normalIndices[m_numTris].z = n - 1;
			if (t)
				m_texCoordIndices[m_numTris].z = t - 1;

			if (!n)
			{   // if no normal was supplied
				Vector3 e1 = m_vertices[m_vertexIndices[m_numTris].y] -
					m_vertices[m_vertexIndices[m_numTris].x];
				Vector3 e2 = m_vertices[m_vertexIndices[m_numTris].z] -
					m_vertices[m_vertexIndices[m_numTris].x];

				m_normals[nn] = cross(e1, e2);
				m_normalIndices[nn].x = nn;
				m_normalIndices[nn].y = nn;
				m_normalIndices[nn].z = nn;
				nn++;
			}

			m_numTris++;
		}
		else if (line[0] == 'u') {
			char s[100];
			sscanf(&line[6], "%s\n", s);
			std::string filename = std::string(s);

			std::unordered_map<std::string, int>::const_iterator got = m_textureMap.find(filename);

			if (got == m_textureMap.end()) {
				//std::cout << "not found";

				TexImage* image = new TexImage();
				m_texImages.push_back(image);

				decodeTwoSteps(filename.c_str(), image->width, image->height, image->image);
				m_textureMap.insert(std::make_pair<std::string, int>(std::string(s), m_texImages.size() - 1));

				currTexIndex = m_textureMap.size() - 1;
			}
			else {
				//std::cout << got->first << " is " << got->second;
				currTexIndex = got->second;
			}
		
		} //  else ignore line
    }
}


//Example 2
//Load PNG file from disk to memory first, then decode to raw pixels in memory.
void TriangleMesh::decodeTwoSteps(const char* filename, int& width, 
	int& height, std::vector<Vector3*>& image)
{
	std::vector<unsigned char> png;
	std::vector<unsigned char> decoded;
	unsigned w, h;

	//load and decode
	lodepng::load_file(png, filename);
	unsigned error = lodepng::decode(decoded, w, h, png);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
	width = (int)w;
	height = (int)h;

	// 4 bytes per pixel
	for (int i = 0; i < decoded.size(); i += 4) {
		unsigned char r, g, b, a;

		r = (int) decoded[i];
		g = (int) decoded[i + 1];
		b = (int) decoded[i + 2];

		Vector3 *pixel = new Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
		image.push_back(pixel);
	}
}

