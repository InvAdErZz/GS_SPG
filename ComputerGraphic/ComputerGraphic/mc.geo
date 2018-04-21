#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 15) out;
uniform sampler3D densityTex;

struct CellTriangles
{
	int tris[16];
};

layout (shared, std140) uniform mc_edgeTable
{
	int edgeTable[256];
};

layout (shared, std140) uniform mc_triTable
{
	CellTriangles triTable[256];
};


in VS_OUT
{
	uint mc_case;
   vec3 samplePositions[8];
   float sampleDensity[8];
} geo_in[];


out vec3 outValue;

vec3 VertexInterp(float isolevel,vec3 p1,vec3 p2, float density1, float density2)
{
   if (abs(isolevel-density1) < 0.00001)
      return(p1);
   if (abs(isolevel-density2) < 0.00001)
      return(p2);
   if (abs(density1-density2) < 0.00001)
      return(p1);
   float mu = (isolevel - density1) / (density2 - density1);
   
   vec3 p;
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return(p);
}
void main()
{

	float mc_case = geo_in[0].mc_case;
	vec3 mcvec = vec3(mc_case,mc_case,mc_case);
	outValue = mcvec;
	outValue = vec3(geo_in[0].sampleDensity[0],geo_in[0].sampleDensity[1],geo_in[0].sampleDensity[2]);

	EmitVertex();
	
	outValue =  mcvec;
	outValue = vec3(geo_in[0].sampleDensity[3],geo_in[0].sampleDensity[4],geo_in[0].sampleDensity[5]);
	EmitVertex();
	
	outValue =mcvec;
	outValue = vec3(geo_in[0].sampleDensity[6],geo_in[0].sampleDensity[7],-1.f);
	EmitVertex();
	EndPrimitive();

return;
	vec3 verticesOnEdge[12];
	int isoLevel = 0;
	
	// edgeTable contains 12 bit key that determines whether an edge is cut by the surface or not.
	int edgeflags = edgeTable[geo_in[0].mc_case];
	if ((edgeflags & 1) != 0)
		verticesOnEdge[ 0] = VertexInterp(isoLevel, geo_in[0].samplePositions[0], geo_in[0].samplePositions[1], geo_in[0].sampleDensity[0], geo_in[0].sampleDensity[1]);
	if ((edgeflags & 2) != 0)
	    verticesOnEdge[ 1] = VertexInterp(isoLevel, geo_in[0].samplePositions[1], geo_in[0].samplePositions[2], geo_in[0].sampleDensity[1], geo_in[0].sampleDensity[2]);
	if ((edgeflags & 4) != 0)
	    verticesOnEdge[ 2] = VertexInterp(isoLevel, geo_in[0].samplePositions[2], geo_in[0].samplePositions[3], geo_in[0].sampleDensity[2], geo_in[0].sampleDensity[3]);
	if ((edgeflags & 8) != 0)
	    verticesOnEdge[ 3] = VertexInterp(isoLevel, geo_in[0].samplePositions[3], geo_in[0].samplePositions[0], geo_in[0].sampleDensity[3], geo_in[0].sampleDensity[0]);
	if ((edgeflags & 16) != 0)
	    verticesOnEdge[ 4] = VertexInterp(isoLevel, geo_in[0].samplePositions[4], geo_in[0].samplePositions[5], geo_in[0].sampleDensity[4], geo_in[0].sampleDensity[5]);
	if ((edgeflags & 32) != 0)
	    verticesOnEdge[ 5] = VertexInterp(isoLevel, geo_in[0].samplePositions[5], geo_in[0].samplePositions[6], geo_in[0].sampleDensity[5], geo_in[0].sampleDensity[6]);
	if ((edgeflags & 64) != 0)
	    verticesOnEdge[ 6] = VertexInterp(isoLevel, geo_in[0].samplePositions[6], geo_in[0].samplePositions[7], geo_in[0].sampleDensity[6], geo_in[0].sampleDensity[7]);
	if ((edgeflags & 128) != 0)
	    verticesOnEdge[ 7] = VertexInterp(isoLevel, geo_in[0].samplePositions[7], geo_in[0].samplePositions[4], geo_in[0].sampleDensity[7], geo_in[0].sampleDensity[4]);
	if ((edgeflags & 256) != 0)
	    verticesOnEdge[ 8] = VertexInterp(isoLevel, geo_in[0].samplePositions[0], geo_in[0].samplePositions[4], geo_in[0].sampleDensity[0], geo_in[0].sampleDensity[4]);
	if ((edgeflags & 512) != 0)
	    verticesOnEdge[ 9] = VertexInterp(isoLevel, geo_in[0].samplePositions[1], geo_in[0].samplePositions[5], geo_in[0].sampleDensity[1], geo_in[0].sampleDensity[5]);
	if ((edgeflags & 1024) != 0)
	    verticesOnEdge[10] = VertexInterp(isoLevel, geo_in[0].samplePositions[2], geo_in[0].samplePositions[6], geo_in[0].sampleDensity[2], geo_in[0].sampleDensity[6]);
	if ((edgeflags & 2048) != 0)
		verticesOnEdge[11] = VertexInterp(isoLevel, geo_in[0].samplePositions[3], geo_in[0].samplePositions[7], geo_in[0].sampleDensity[3], geo_in[0].sampleDensity[7]);

	
	for (int i = 0; triTable[geo_in[0].mc_case].tris[i] != -1; i += 3)
	{
		vec3 pos1 = verticesOnEdge[triTable[geo_in[0].mc_case].tris[i  ]];
		vec3 pos2 = verticesOnEdge[triTable[geo_in[0].mc_case].tris[i+1]];
		vec3 pos3 = verticesOnEdge[triTable[geo_in[0].mc_case].tris[i+2]];
		
		outValue = pos1;
		EmitVertex();
		
		outValue = pos2;
		EmitVertex();
		
		outValue = pos3;
		EmitVertex();
		
		EndPrimitive();		
	}
}