#ifndef GLOBAL_H
#define GLOBAL_H

#include "color.h"
#include "material.h"

const Color COLOR_NONE = Color(0,0,0);
const Color COLOR_BLACK = Color(EPS,EPS,EPS);
const Color COLOR_WHITE = Color(1,1,1);
const Color COLOR_GRAY = Color(0.5,0.5,0.5);
const Color COLOR_RED = Color(1,0.4,EPS);
const Color COLOR_GREEN = Color(0.3244,0.9844,EPS);
const Color COLOR_BLUE = Color(EPS,EPS,1);
const Color COLOR_YELLOW = Color(1,0.9,0.3);
const Color COLOR_CYAN = Color(EPS,1,1);
const Color COLOR_PINK = Color(1,EPS,1);
const Color COLOR_ORANGE=Color(1,0.65,EPS);
const Color COLOR_CORNSILK=Color(1,0.973,0.863);
const Color COLOR_LIGHTRED=Color(219.0/255,112.0/255,147.0/255);

//color shininess ambient diffuse specular transparency;
Material* const MATERIAL_LIGHT_RED=new Material(COLOR_LIGHTRED,40,0.5,0.9,0.05,0);
Material* const MATERIAL_GREEN=new Material(COLOR_GREEN,40,0.5,0.9,0.0,0);
Material* const MATERIAL_YELLOW=new Material(COLOR_YELLOW,40,0.5,0.9,0.0,0);
Material* const MATERIAL_PINK=new Material(COLOR_PINK,40,0.5,0.9,0.0,0);
Material* const MATERIAL_GRAY=new Material(COLOR_GRAY,40,0.5,0.9,0.05,0);
Material* const MATERIAL_CORNSILK=new Material(COLOR_CORNSILK,40,0.5,0.9,0.05,0);
Material* const MATERIAL_CYAN=new Material(COLOR_CYAN,40,0.5,0.9,0.1,0);
Material* const MATERIAL_RED=new Material(COLOR_RED,40,0.5,0.9,0.0,0);
Material* const MATERIAL_ORANGE=new Material(COLOR_ORANGE,40,0.5,0.9,0.05,0);
Material* const MATERIAL_WHITE=new Material(COLOR_WHITE,40,0.5,0.9,0.05,0);
Material* const MATERIAL_LIGHT_WHITE=new Material(COLOR_WHITE*0.9,40,0.5,0.9,0.05,0);
Material* const MATERIAL_CRYSTAL=new Material(COLOR_WHITE,40,0.1,1,1,30); //1 1 15
Material* const MATERIAL_CRYSTAL_BLUE=new Material(COLOR_BLUE*5,40,0.1,0.1,0.1,1.5);
//TextureMaterial* const TM_BLACK_WHITE=new TextureMaterial(40,0.1,0.9,0.05,0,"grid.bmp",4);
//TextureMaterial* const TM_SNOW_TABLE=new TextureMaterial(40,0.1,0.9,0.05,0,"snowtable.jpg",60);
//TextureMaterial* const TM_SEA=new TextureMaterial(40,0,1,0,0,"sea.jpg",45);
//TextureMaterial* const TM_GROUND=new TextureMaterial(40,0,1,0,0,"ground.jpg",6);
//Material* const MATERIAL_WHITE=new Material(0,COLOR_WHITE,0,40,0.95); //
//Material* const MATERIAL_BLUE=new Material(0.5,COLOR_BLUE,0.2,10,0.8);
//Material* const MATERIAL_GREEN=new Material(0.2,COLOR_GREEN,0.3,20,0.2);
//Material* const MATERIAL_YELLOW=new Material(0.2,COLOR_YELLOW,0.3,20,0.6);
//Material* const MATERIAL_CYAN=new Material(0.2,COLOR_CYAN,0.3,20);
//Material* const MATERIAL_PINK=new Material(0.2,COLOR_PINK,0.5,20);
//Material* const MATERIAL_ORANGE=new Material(0.2,COLOR_ORANGE,0.5,20);
//Material* const MATERIAL_CORNSILK=new Material(0.2,COLOR_CORNSILK,0.2,20);
//Material* const MATERIAL_GRAY=new Material(0.2,COLOR_GRAY,0.5,5);
//Material* const MATERIAL_RED=new Material(0.2,COLOR_RED,1.5,10);
//Material* const MATERIAL_LIGHTRED=new Material(0.2,COLOR_LIGHTRED,0.4,10);
//Material* const MATERIAL_MIRROR=new Material(0,COLOR_WHITE,4,10000); //failed :(

#endif // GLOBAL_H
