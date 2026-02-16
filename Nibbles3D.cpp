//helloNibbles.cpp

//created by Dave Ruth
//		Niles North High School
//modified: April 29, 2002

//.. changed delayTimei to 100 from 75 in Feb 2026

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include<windows.h>
#include "glut.h"

#define _pi 3.14159265358979323846
#define _2pi 6.28318530717958647692

#define rows			25
#define cols			25
#define levs    25
#define rowsDiv2	12
#define colsDiv2	12
#define colsDiv2	12
#define levsDiv2	12


GLfloat step = 1.0, delayTime = 0.5;

char grid[rows][cols][levs];//game grid
//char grid[rows][cols];
const char head = 'h', body = 'b', nibble = '$', blank = ' ';//space
//const double nibble_time = 10.0;

const GLfloat cps = CLOCKS_PER_SEC;
const GLint cpsi = CLOCKS_PER_SEC;
clock_t oldTimei=0, curTimei=0, delayTimei = 100;

int i=0,j=0,k=0;//indexes

enum Direction  
{LEFT = '4', RIGHT = '6', BACK = '8', FORWARD = '5',
	UP = 'A', DOWN = 'Z' };
GLint score = 0,snakeAdd = 4,bodyAdd = 0,nibScore = 5,
	snakeLen = 4,
	headRow = 12,headCol = 12,headLev = 0,
	tailRow = 15,tailCol = 12,tailLev = 0,
	curRow = 0, curCol=0, curLev=0;
GLint nibRow = 5, nibCol = 5, nibLev = 0;

GLint jump = 0;
GLfloat oldTime=0.0, tme=0.0, curTime=0.0;
char ky = FORWARD,	dir = FORWARD;
Direction headDir = FORWARD, tailDir = FORWARD;
Direction snake[300];//arbitrarily large snake vector...

GLfloat nibAngle = 0.0;

GLfloat size = 12.5, size2 = 25.0, size2_1 = 24.0, size2_5 = 25.5;
GLuint snakeList,floorList,nibList;
GLfloat scenex= 0.0, sceney = 0.0;
GLfloat xpos=0.0, ypos=0.0, zpos=0.0, xsize=0.8;

GLint startx,starty, guideLinesX=0, guideLinesC=0;
GLint moving = 0, goThru = 1, isNib = 0;
GLint win, subwin1, subwin2;

void newNibble();

	GLfloat mat_spec[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_ambSnake[] = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat mat_ambNib[] = { 1.0, 0.0, 0.0, 0.0 };
	//GLfloat mat_specNib[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_shine[] = { 120.0 };
	GLfloat position[] = {1.0, 1.0, 2.0, 0.0 };//light position

static void init(void)
{
	glClearColor(0.0,0.0,0.0,0.0);//black
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambSnake);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	
	snakeList = glGenLists(1);
	glNewList(snakeList, GL_COMPILE);
	  //glTranslatef (0.0,1.0,0.0);
	  glutSolidSphere (0.8, 16, 12);
	 
	glEndList();
	
	nibList = glGenLists(1);
	glNewList(nibList, GL_COMPILE);
	  //glutSolidTorus (0.3,0.7,20,16);
	  glutSolidTeapot(1);
	glEndList();
   
	floorList = glGenLists(1);
	glNewList(floorList, GL_COMPILE);
		glTranslatef(0.0,-1.0,0.0);
		glColor3f(0.0,1.0,0.0);
		glBegin(GL_QUADS);//floor
			glVertex3f(size,0.0,size);
			glVertex3f(size,0.0,-size);
			glVertex3f(-size,0.0,-size);
			glVertex3f(-size,0.0,size);
		glEnd();
		
		glColor3f(1.0,1.0,1.0);//white
		glLineWidth(1.0);
		glBegin(GL_LINES);
		GLfloat m;
		for (m=-size; m<=size; m+=5.0)
		{
			glVertex3f(m,0.05,-size);//floor
			glVertex3f(m,0.05,size);
			glVertex3f(-size,0.05,m);
			glVertex3f(size,0.05,m);
			glVertex3f(m,size2_5,-size);//ceiling
			glVertex3f(m,size2_5,size);
			glVertex3f(-size,size2_5,m);
			glVertex3f(size,size2_5,m);
			glVertex3f(-size,0.05,m);//left wall
			glVertex3f(-size,size2_5,m);
			glVertex3f(size,0.05,m);//right wall
			glVertex3f(size,size2_5,m);
			//glVertex3f(k,0.05,size);//front wall
			//glVertex3f(k,size2,size);	
			glVertex3f(m,0.05,-size);//back wall
			glVertex3f(m,size2_5,-size);
			glVertex3f(-size,m+size,-size);
			glVertex3f(size,m+size,-size);									
		}
		glEnd();
		glTranslatef(0.0,1.0,0.0);
	glEndList();//end floor list
	
	srand(clock()%59);
	
	//init grid to blanks
	for (i=0; i<=24; i++)
		for (j=0; j<=24; j++)
			for (k=0; k<=24; k++)
				for (k=0; k<=24; k++)
					grid[i][j][k] = blank;
	newNibble();
	
	snakeLen = 4; bodyAdd = 0; snakeAdd = 4; score = 0;
	headRow = 12; headCol = 12; headLev = 0;
	tailRow = 15; tailCol = 12; tailLev = 0;
	jump = 0;
	oldTime=0.0; tme=0.0;
	oldTimei = clock();
	for (i=0; i<=299; i++)
		snake[i]=FORWARD;
}

void display(void)
{
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//printf("scenex %.2f\nsceney %.2f\n",scenex,sceney);
	
	glDisable(GL_LIGHTING);
	//output text...
	glPushMatrix();
	glColor3f(1.0,1.0,1.0);//white
	glLineWidth(1.5);
	glTranslatef (-22.0,-15.0, 0.0);//push left and down
	glScalef(0.01, 0.01, 0.01);
	char message[] = "SCORE";
	//int len = (int) strlen(message);
  for (int i = 0; i < 5; i++) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, message[i]);
  }
	glTranslatef (-400.0,-200.0, 0.0);
  char digit[5];
  digit[0] = (char)score/100 + '0';
  digit[1] = ' ';
  digit[2] = (char)score%100/10 + '0';
  digit[3] = ' ';
  digit[4] = (char)score%10 + '0';
	for (int i = 0; i < 5; i++) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, digit[i]);
  }  
	glPopMatrix();
	
	//rotate scene based on mouse input
	glPushMatrix();
		glRotatef(sceney, 1.0, 0.0, 0.0);
		glRotatef(scenex, 0.0, 1.0, 0.0);
		glTranslatef(0.0,-10.0,0.0);
		
		//draw floor
		glCallList (floorList);
		
		/*
		if (guideLinesX)
		{//draw lines for head
			glPushMatrix();
			GLfloat xpos, ypos, zpos;
			switch(headDir)
				{	case LEFT	:
						glBegin(GL_LINES);
							xpos = headCol-colsDiv2-0.8;
							glVertex3f(xpos,headLev,-size);//horizontal line
							glVertex3f(xpos,headLev,size);
							zpos = headRow-rowsDiv2;
							glVertex3f(xpos,-1.0,zpos);//vertical line
							glVertex3f(xpos,size2-1,zpos);
						glEnd();					break;
				}
			glPopMatrix();
		}
		*/
		
		xpos = headCol - colsDiv2;
		zpos = headRow - rowsDiv2;
		ypos = headLev;
		
		if (guideLinesC)
		{
			//draw lines for head
			glPushMatrix();
			
			switch(headDir)
			{	case LEFT	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(-size,ypos,zpos);
						glVertex3f(-size,ypos-xsize,zpos-xsize);//X
						glVertex3f(-size,ypos+xsize,zpos+xsize);
						glVertex3f(-size,ypos+xsize,zpos-xsize);//X
						glVertex3f(-size,ypos-xsize,zpos+xsize);
					glEnd();					
					break;
				case RIGHT	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(size,ypos,zpos);
						glVertex3f(size,ypos-xsize,zpos-xsize);//X
						glVertex3f(size,ypos+xsize,zpos+xsize);
						glVertex3f(size,ypos+xsize,zpos-xsize);//X
						glVertex3f(size,ypos-xsize,zpos+xsize);
					glEnd();					
					break;
				case UP	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,size2-1,zpos);
						glVertex3f(xpos-xsize,size2-1,zpos-xsize);//X
						glVertex3f(xpos+xsize,size2-1,zpos+xsize);
						glVertex3f(xpos+xsize,size2-1,zpos-xsize);//X
						glVertex3f(xpos-xsize,size2-1,zpos+xsize);
					glEnd();					
					break;
				case DOWN	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,0.0,zpos);
						glVertex3f(xpos-xsize,0.0,zpos-xsize);//X
						glVertex3f(xpos+xsize,0.0,zpos+xsize);
						glVertex3f(xpos+xsize,0.0,zpos-xsize);//X
						glVertex3f(xpos-xsize,0.0,zpos+xsize);
					glEnd();					
					break;
				case FORWARD :
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,ypos,-size);
						glVertex3f(xpos-xsize,ypos-xsize,-size);//X
						glVertex3f(xpos+xsize,ypos+xsize,-size);
						glVertex3f(xpos+xsize,ypos-xsize,-size);//X
						glVertex3f(xpos-xsize,ypos+xsize,-size);
					glEnd();					
					break;
				case BACK :
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,ypos,size);
						glVertex3f(xpos-xsize,ypos-xsize,size);//X
						glVertex3f(xpos+xsize,ypos+xsize,size);
						glVertex3f(xpos+xsize,ypos-xsize,size);//X
						glVertex3f(xpos-xsize,ypos+xsize,size);
					glEnd();					
					break;
			}
			glPopMatrix();
		}
	
		//draw nibble
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambNib);
		glPushMatrix();
			glTranslatef( (nibCol-colsDiv2),(GLfloat)nibLev,(nibRow-rowsDiv2) );
			glRotatef(nibAngle,0.0,1.0,0.0);
			glCallList(nibList);
		glPopMatrix();
		nibAngle += 10.0;
		
		glPushMatrix();
	  //calculate headposition
	  glTranslatef( (headCol-colsDiv2),(GLfloat)headLev,(headRow-rowsDiv2));
	  
	  //draw head...
	  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambSnake);
	  if (isNib)
	  {	glPushMatrix();
		  	glScalef(2.0,2.0,2.0);
		  	glCallList(snakeList);
	  	glPopMatrix();
	  	isNib = 0;
	  }
	  else
	  	glCallList(snakeList);
	  //draw body
	  for(i=1;i<snakeLen;i++)
		{	switch(snake[i])
			{	case LEFT	: 	
					if(snakeLen - jump == i)
						glTranslatef(-size2_1,0.0,0.0);
					else
						glTranslatef(step,0.0,0.0);			break;
				case RIGHT:		
					if(snakeLen - jump == i)
						glTranslatef(size2_1,0.0,0.0);
					else
						glTranslatef(-step,0.0,0.0);		break;
				case FORWARD:	
					if(snakeLen - jump == i)
						glTranslatef(0.0,0.0,-size2_1);
					else
						glTranslatef(0.0,0.0,step);			break;
				case BACK	:
					if(snakeLen - jump == i)
						glTranslatef(0.0,0.0,size2_1);
					else
						glTranslatef(0.0,0.0,-step);		break;
				case UP:	
					if(snakeLen - jump == i)
						glTranslatef(0.0,size2_1,0.0);
					else
						glTranslatef(0.0,-step,0.0);		break;
				case DOWN	:		
					if(snakeLen - jump == i)
						glTranslatef(0.0,-size2_1,0.0);
					else
						glTranslatef(0.0,step,0.0);			break;
			}//convert these to normal vectors to speed up drawing...
		glCallList(snakeList);
		}
		//if (jump > 0) jump--;
		glPopMatrix();

	glPopMatrix();//end rotate scene
	
	glutSwapBuffers();
}
void display2(void)
{
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	
	//printf("xpos %.2f\nypos %.2f\nzpos %.2f\n",xpos, ypos, zpos);
	
	glPushMatrix();
		switch(headDir)
		{	case LEFT:
				gluLookAt(xpos,ypos,zpos,
						  		-size2,ypos,zpos, 0, 1, 0);	break;
			case RIGHT:
				gluLookAt(xpos,ypos,zpos,
						  		size2,ypos,zpos, 0, 1, 0);	break;
			case UP:
				gluLookAt(xpos,ypos,zpos,
						  		xpos,size2+3,zpos, 0, 0, 1);	break;
			case DOWN:
				gluLookAt(xpos,ypos,zpos,
						  		xpos,-size2-3,zpos, 0, 0, -1);	break;
			case FORWARD:
				gluLookAt(xpos,ypos,zpos,
						  		xpos,ypos,-size2, 0, 1, 0);	break;
			case BACK:
				gluLookAt(xpos,ypos,zpos,
						  		xpos,ypos,size2, 0, 1, 0);	break;
		}	
		//draw floor
		glCallList (floorList);
		
		if (guideLinesC)
		{
			//draw lines for head
			glPushMatrix();
			
			switch(headDir)
			{	case LEFT	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(-size,ypos,zpos);
						glVertex3f(-size,ypos-xsize,zpos-xsize);//X
						glVertex3f(-size,ypos+xsize,zpos+xsize);
						glVertex3f(-size,ypos+xsize,zpos-xsize);//X
						glVertex3f(-size,ypos-xsize,zpos+xsize);
					glEnd();					
					break;
				case RIGHT	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(size,ypos,zpos);
						glVertex3f(size,ypos-xsize,zpos-xsize);//X
						glVertex3f(size,ypos+xsize,zpos+xsize);
						glVertex3f(size,ypos+xsize,zpos-xsize);//X
						glVertex3f(size,ypos-xsize,zpos+xsize);
					glEnd();					
					break;
				case UP	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,size2-1,zpos);
						glVertex3f(xpos-xsize,size2-1,zpos-xsize);//X
						glVertex3f(xpos+xsize,size2-1,zpos+xsize);
						glVertex3f(xpos+xsize,size2-1,zpos-xsize);//X
						glVertex3f(xpos-xsize,size2-1,zpos+xsize);
					glEnd();					
					break;
				case DOWN	:
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,0.0,zpos);
						glVertex3f(xpos-xsize,0.0,zpos-xsize);//X
						glVertex3f(xpos+xsize,0.0,zpos+xsize);
						glVertex3f(xpos+xsize,0.0,zpos-xsize);//X
						glVertex3f(xpos-xsize,0.0,zpos+xsize);
					glEnd();					
					break;
				case FORWARD :
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,ypos,-size);
						glVertex3f(xpos-xsize,ypos-xsize,-size);//X
						glVertex3f(xpos+xsize,ypos+xsize,-size);
						glVertex3f(xpos+xsize,ypos-xsize,-size);//X
						glVertex3f(xpos-xsize,ypos+xsize,-size);
					glEnd();					
					break;
				case BACK :
					glBegin(GL_LINES);
						glVertex3f(xpos,ypos,zpos);//horizontal line
						glVertex3f(xpos,ypos,size);
						glVertex3f(xpos-xsize,ypos-xsize,size);//X
						glVertex3f(xpos+xsize,ypos+xsize,size);
						glVertex3f(xpos+xsize,ypos-xsize,size);//X
						glVertex3f(xpos-xsize,ypos+xsize,size);
					glEnd();					
					break;
			}
			glPopMatrix();
		}
	
		//draw nibble
		glEnable(GL_LIGHTING);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambNib);
		glPushMatrix();
			glTranslatef( (nibCol-colsDiv2),(GLfloat)nibLev,(nibRow-rowsDiv2) );
			glRotatef(nibAngle,0.0,1.0,0.0);
			glCallList(nibList);
		glPopMatrix();
		nibAngle += 10.0;
		
		glPushMatrix();
	  //calculate headposition
	  glTranslatef( (headCol-colsDiv2),(GLfloat)headLev,(headRow-rowsDiv2));
	  
	  //draw head...
	  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambSnake);
	  glCallList(snakeList);
	  //draw body
	  for(i=1;i<snakeLen;i++)
		{	switch(snake[i])
			{	case LEFT	: 	
					if(snakeLen - jump == i)
						glTranslatef(-size2_1,0.0,0.0);
					else
						glTranslatef(step,0.0,0.0);			break;
				case RIGHT:		
					if(snakeLen - jump == i)
						glTranslatef(size2_1,0.0,0.0);
					else
						glTranslatef(-step,0.0,0.0);		break;
				case FORWARD:	
					if(snakeLen - jump == i)
						glTranslatef(0.0,0.0,-size2_1);
					else
						glTranslatef(0.0,0.0,step);			break;
				case BACK	:
					if(snakeLen - jump == i)
						glTranslatef(0.0,0.0,size2_1);
					else
						glTranslatef(0.0,0.0,-step);		break;
				case UP:	
					if(snakeLen - jump == i)
						glTranslatef(0.0,size2_1,0.0);
					else
						glTranslatef(0.0,-step,0.0);		break;
				case DOWN	:		
					if(snakeLen - jump == i)
						glTranslatef(0.0,-size2_1,0.0);
					else
						glTranslatef(0.0,step,0.0);			break;
			}//convert these to normal vectors to speed up drawing...
		glCallList(snakeList);
		}
		//if (jump > 0) jump--;
		glPopMatrix();

	glPopMatrix();//end translate scene -10 on y
	glutSwapBuffers();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLint) w, (GLint) h);
   glutSetWindow(subwin1);
   glutPositionWindow(0,0);
   glutReshapeWindow(550,500);   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45, ( (GLfloat) w )/(GLfloat) h, 1.0, 100.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0, 0, 50, 0, 0, 0, 0, 1, 0);
   glClearColor(0.0, 0.0, 0.0, 0.0);

   
}

void reshape2(int w, int h)
{
  glViewport(0, 0, (GLint) w, (GLint) h);
	glutSetWindow(subwin2);
	glutPositionWindow(550,150);
	glutReshapeWindow(200,200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, ( (GLfloat) w )/(GLfloat) h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(xpos,ypos,zpos,
	//				  xpos,ypos,-size-2.0, 0, 1, 0);
  glClearColor(0.2, 0.2, 0.2, 0.0);
}

void newNibble()
{
	i = int( rand()%25 );
	j = int( rand()%25 );
	k = int( rand()%25 );
	while(grid[i][j][k]!=blank)
	{	i = int( rand()%25 );
		j = int( rand()%25 );
		k = int( rand()%25 );
	}
	grid[i][j][k]=nibble;
	nibRow = i;
	nibCol = j;
	nibLev = k;
	nibAngle = 0.0;
}

void delay(int m)
//pause the program for m milliseconds
{
	double tim = clock();
	while( true )
	 if ( (clock() - tim)/cps > m / 1000.0 )
	 	return;
}

void animate(void)
{
  grid[headRow][headCol][headLev]=body;
	switch ( dir )
	{//update headDir and headRow/headCol
		case LEFT 	: headDir = LEFT;   headCol--;  break;
		case RIGHT	: headDir = RIGHT;  headCol++;  break;
		case FORWARD: headDir = FORWARD;   headRow--;  break;
		case BACK  	: headDir = BACK;     headRow++;  break;
		case UP			: headDir = UP;   headLev++;  break;
		case DOWN 	: headDir = DOWN;     headLev--;  break;						
		default		: ;
	}
	
  if (goThru) //not hitWall
  {	if (headRow < 0)
		{	headRow = 24;
			jump = snakeLen;
		}
		if (headRow > 24)
		{	headRow = 0;
			jump = snakeLen;
		}
		if (headCol < 0) 
		{	headCol = 24;
			jump = snakeLen;
		}
		if (headCol > 24)
		{	headCol = 0;
			jump = snakeLen;
		}
		if (headLev < 0)
		{	headLev = 24;
			jump = snakeLen;
		}
		if (headLev > 24)
		{	headLev = 0;
			jump = snakeLen;
		}
		if (jump > 0) jump--;
	}
	else
	{	if (headCol < 0 || headCol > 24 ||
      	headRow < 0 || headRow > 24 ||
      	headLev < 0 || headLev > 24     )
  	{	//hit wall...
  		glutIdleFunc(NULL);
  		return;
  	}
  }
  if (grid[headRow][headCol][headLev]==body)
  {	//hit body... 
  	glutIdleFunc(NULL);
  	return;
  }
	if (grid[headRow][headCol][headLev]==nibble)
  {	score += nibScore;
  	bodyAdd += snakeAdd;
  	isNib = 1;
  	newNibble();
  }
	grid[headRow][headCol][headLev]=head;
	if (bodyAdd > 0)
	{
	  bodyAdd--;
	  snake[snakeLen]=snake[snakeLen-1];
	  snakeLen++;
	}
	else
	{
		//replace current tail with blank
		grid[tailRow][tailCol][tailLev]=blank;
		//update tailRow,tailCol,tailLev
		tailDir = snake[snakeLen-1];
		switch (tailDir)//tail Direction
		{	case LEFT	: 	tailCol--;	break;
			case RIGHT: 	tailCol++;	break;
			case FORWARD: tailRow--;	break;
			case BACK	: 	tailRow++;	break;
			case UP		:		tailLev++;	break;
			case DOWN	: 	tailLev--;	break;
		}
		//update snake direction vector
		if (tailRow < 0) tailRow = 24;
		if (tailRow > 24) tailRow = 0;
		if (tailCol < 0) tailCol = 24;
		if (tailCol > 24) tailCol = 0;
		if (tailLev < 0) tailLev = 24;
		if (tailLev > 24) tailLev = 0;
	}	
	snake[0] = headDir;
	for (i = snakeLen - 1; i >= 1; i--)
		snake[i] = snake[i-1];

	//delayTimei = 75;
	curTimei = clock();
	printf("%ul\t%ul\t%ul\t%f\n",curTimei, oldTimei, curTimei - oldTimei,
					curTimei - oldTimei < delayTimei);
	i = 0;
	while (curTimei - oldTimei < delayTimei)
	{
		curTimei = clock();
		i++;
		//printf("%ul\t%ul\t%ul\t%f\n",curTimei, oldTimei, curTimei - oldTimei,
		//			curTimei - oldTimei <= delayTimei);
	}
	//printf("num iter %i\n",i);
	oldTimei = curTimei;
	//delay(2000);
  glutSetWindow(subwin1); 
	glutPostRedisplay();
	glutSetWindow(subwin2);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
  { if (state == GLUT_DOWN)
    { moving = 1;
      startx = x;
      starty = y;
    }
    if (state == GLUT_UP)
    	moving = 0;
	}
}

void motion(int x, int y)
{
	scenex = scenex + (x - startx);
	sceney = sceney + (y - starty);
	startx = x;
	starty = y;
	glutSetWindow(subwin1); 
	glutPostRedisplay();
	glutSetWindow(subwin2); 
	glutPostRedisplay();}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case ' ':
			glutIdleFunc(NULL);
			glutPostRedisplay();
			break;
		case 'a':
		case 'A':
			dir = UP;
			glutIdleFunc(animate);
			break;
		case 'z':
		case 'Z':
			dir = DOWN;
			break;
		case 'p':
		case 'P':
			glutIdleFunc(NULL);
			init();
			break;

		//case 'w':
		//	delayTimei -= 5;
		//	break;
		//case 's':
		//	delayTimei += 5;
		//	break;
		case 27:
		  exit(0);
    }
  if (dir == UP && headDir == DOWN ||
	  	dir == DOWN && headDir == UP 		 )
  	dir = headDir;
  
  animate();
  
  glutSetWindow(subwin1); 
	glutPostRedisplay();
	glutSetWindow(subwin2); 
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch (key) 
	{ case GLUT_KEY_UP:
	    dir = FORWARD;
	    glutIdleFunc(animate);
	    break;
	  case GLUT_KEY_DOWN:
	    dir = BACK;
	    break;
	  case GLUT_KEY_LEFT:
	    dir = LEFT;
	    break;
	  case GLUT_KEY_RIGHT:
	    dir = RIGHT;
	    break;
	}
	  if (dir == LEFT && headDir == RIGHT ||
	  		dir == RIGHT && headDir == LEFT ||
	  		dir == BACK && headDir == FORWARD ||
  			dir == FORWARD && headDir == BACK	   )
	  	dir = headDir;
	
	//glutIdleFunc(animate);
	//animate();
	
	glutSetWindow(subwin1); 
	glutPostRedisplay();
	glutSetWindow(subwin2); 
	glutPostRedisplay();
}

void menu(int item)
{
  switch (item) {
  case 1://view guide lines
  	guideLinesC = 1;
    glutChangeToMenuEntry(1, "hide guide lines", 2);    break;
  case 2:
   	guideLinesC = 0;
    glutChangeToMenuEntry(1, "view guide lines", 1);    break;
  case 3:
  	goThru = 0;
  	glutChangeToMenuEntry(2, "go thru walls", 4);		break;
	case 4:
		goThru = 1;
  	glutChangeToMenuEntry(2, "die hitting walls", 3);		break;
  
  case 5:
    exit(0);
    break;
  }
	glutSetWindow(subwin1); 
	glutPostRedisplay();
	glutSetWindow(subwin2); 
	glutPostRedisplay();
}

void instructions()
{
	printf("Nibbles 3D\n\ncreated by Dave Ruth\nNiles North High School\n\n");
	printf("   *** INSTRUCTIONS ***\n\n");
	printf("the object of the game is to maneuver the snake around the\n");
	printf("three dimensional grid to eat as many apples (teapots) as possible\n");
	printf("without running into yourself (or walls if challenging play is selected\n\n");
	printf("arrow keys move left, right, back and forward\n");

	printf("arrow keys move left, right, back and forward\n");

	
	printf("arrow keys move left, right, back and forward\n");
	printf("a and z keys move up and down\n");
	printf("s key stops the animation\n");
	printf("p key starts another game\n");
	printf("esc key closes both windows\n");
	printf("menu options: (right click) guide lines, default if off, \n");
	printf("and for more challenging play, opt for snake to die\n");
	printf("if it runs into a wall(or floor/ceiling), default is to pass thru walls\n");
	printf("drag the mouse on game grid to change viewing angle\n%f\n",cps);
	printf("hit an arrow key to begin\n");
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(750, 500);
	win=glutCreateWindow("Nibbles 3D");	
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	
	subwin1 = glutCreateSubWindow(win,0,0,550,500);
	glutCreateMenu(menu);
	glutAddMenuEntry("view guide lines", 1);
	glutAddMenuEntry("die hitting walls", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	subwin2 = glutCreateSubWindow(win,550,100,200,200);
	init();
	glutDisplayFunc(display2);
	glutReshapeFunc(reshape2);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	//glutMouseFunc(mouse);
	//glutMotionFunc(motion);
	instructions();
	
	
	glutMainLoop();
	return 0;
}

