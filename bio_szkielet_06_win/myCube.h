﻿#ifndef MYCUBE_H_INCLUDED
#define MYCUBE_H_INCLUDED

int myCubeVertexCount=36;
int myCubeTexturedVertexCount = 36;

float myCubeVertices[]={
                //ściana 1
				1.0f,-1.0f,-1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,
				-1.0f,-1.0f,-1.0f,1.0f,

				1.0f,-1.0f,-1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,

                //ściana 2
				-1.0f,-1.0f, 1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,

				-1.0f,-1.0f, 1.0f,1.0f,
				-1.0f, 1.0f, 1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,


				//ściana 3
				-1.0f,-1.0f,-1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,
				1.0f,-1.0f,-1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f,-1.0f, 1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,

				//ściana 4
				-1.0f, 1.0f, 1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,

				-1.0f, 1.0f, 1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,

				//ściana 5
				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f, 1.0f, 1.0f,1.0f,
				-1.0f,-1.0f, 1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,
				-1.0f, 1.0f, 1.0f,1.0f,

                //ściana 6
				1.0f,-1.0f, 1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,
				1.0f,-1.0f,-1.0f,1.0f,

				1.0f,-1.0f, 1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,




			};

float myCubeMergedVerts[] = {
  1.0f,-1.0f,0.0f,1.0f, //A
 -1.0f, 1.0f,0.0f,1.0f, //B
 -1.0f,-1.0f,0.0f,1.0f, //C

  1.0f,-1.0f,0.0f,1.0f, //A
  1.0f, 1.0f,0.0f,1.0f, //D
 -1.0f, 1.0f,0.0f,1.0f, //B
};

float myCubeTexCoords[] = {
  2.0f, 0.0f,   //A			1
  0.0f, 2.0f,    //B
  0.0f, 0.0f,    //C

  2.0f, 0.0f,    //A		
  2.0f, 2.0f,    //D
  0.0f, 2.0f,    //B

	2.0f, 0.0f,   //A		2
  0.0f, 2.0f,    //B
  0.0f, 0.0f,    //C
	
  2.0f, 0.0f,    //A		
  2.0f, 2.0f,    //D
  0.0f, 2.0f,    //B

	2.0f, 0.0f,   //A		3
  0.0f, 2.0f,    //B
  0.0f, 0.0f,    //C

  2.0f, 0.0f,    //A		
  2.0f, 2.0f,    //D
  0.0f, 2.0f,    //B

	2.0f, 0.0f,   //A		4
  0.0f, 2.0f,    //B
  0.0f, 0.0f,    //C

  2.0f, 0.0f,    //A
  2.0f, 2.0f,    //D
  0.0f, 2.0f,    //B

	2.0f, 0.0f,   //A		5
  0.0f, 2.0f,    //B
  0.0f, 0.0f,    //C

  2.0f, 0.0f,    //A
  2.0f, 2.0f,    //D
  0.0f, 2.0f,    //B

	2.0f, 0.0f,   //A		6
  0.0f, 2.0f,    //B
  0.0f, 0.0f,    //C

  2.0f, 0.0f,    //A
  2.0f, 2.0f,    //D
  0.0f, 2.0f,    //B
};

float myCubeNormals[] = {
	0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f,

	0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 0.0f, -1.0f, 0.0f,

	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,

	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 0.0f,

	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,

};


float myCubeColors[]={
                //ściana 1
				1.0f,0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,1.0f,

				1.0f,0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,1.0f,
				1.0f,0.0f,0.0f,1.0f,

				//ściana 2
				0.0f,1.0f,0.0f,1.0f,
				0.0f,1.0f,0.0f,1.0f,
				0.0f,1.0f,0.0f,1.0f,

				0.0f,1.0f,0.0f,1.0f,
				0.0f,1.0f,0.0f,1.0f,
				0.0f,1.0f,0.0f,1.0f,

				//ściana 3
				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,

				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,
				0.0f,0.0f,1.0f,1.0f,

				//ściana 4
				1.0f,1.0f,0.0f,1.0f,
				1.0f,1.0f,0.0f,1.0f,
				1.0f,1.0f,0.0f,1.0f,

				1.0f,1.0f,0.0f,1.0f,
				1.0f,1.0f,0.0f,1.0f,
				1.0f,1.0f,0.0f,1.0f,

				//ściana 5
				0.0f,1.0f,1.0f,1.0f,
				0.0f,1.0f,1.0f,1.0f,
				0.0f,1.0f,1.0f,1.0f,

				0.0f,1.0f,1.0f,1.0f,
				0.0f,1.0f,1.0f,1.0f,
				0.0f,1.0f,1.0f,1.0f,

				//ściana 6
				1.0f,1.0f,1.0f,1.0f,
				1.0f,1.0f,1.0f,1.0f,
				1.0f,1.0f,1.0f,1.0f,

				1.0f,1.0f,1.0f,1.0f,
				1.0f,1.0f,1.0f,1.0f,
				1.0f,1.0f,1.0f,1.0f,
			};





#endif // MYCUBE_H_INCLUDED