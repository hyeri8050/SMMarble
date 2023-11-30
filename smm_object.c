//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100

//유형에 대한 2차원 배열 정의 
static char smmNodeName[SMMNODE_TYPE_MAX ][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};

//enum to string
char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}

//1. 구조체 형식 정의 
typedef struct smmObject{
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
} smmObject_t;

//2. 구조체 배열 변수 정의 
smmObject_t smm_node[MAX_NODE];

static int smmObj_noNode = 0;

#if 0
//smmObj안에 있는 전역 변수 선언 및 배열화 => 11.30 구조체화로 변경하기  
static char smmObj_name [MAX_NODE][MAX_CHARNAME]; 
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode = 0;
#endif

//object generation (Node 만들기, 즉 main.c에 전달) 
void smmObj_genNode(char* name, int type, int credit, int energy)
{
    #if 0
    //strcpy (대상, 원본)  
    strcpy (smmObj_type[smmObj_noNode], name); //문자열(배열 복사) -> 하나씩 복사  
    smmObj_type[smmObj_noNode] = type; //갖고 있는 배열에 입력된 type값을 넣음 
    smmObj_credit[smmObj_noNode] = credit;
    smmObj_energy[smmObj_noNode] = energy;
    #endif
    strcpy(smm_node[smmObj_noNode].name, name);
    smm_node[smmObj_noNode].type, type);
    smm_node[smmObj_noNode].credit, credit);
    smm_node[smmObj_noNode].energy, );
    
    //static int smmObj_noNode++; 
    smmObj_noNode++;
}
//name
char* smmObj_getNodeName(int node_nr)
{
      return smm_node[node_nr].name;
      //smmObj_noNode];
}
 
//type
int smmObj_getNodeType(int node_nr)
{
    //return smmObj_type[node_nr];
    return smm_node[node_nr].type;
}

//credit
int smmObj_getNodeCredit(int node_nr)
{
    //return smmObj_credit[node_nr];
    return smm_node[node_nr].credit;
}

//energy
int smmObj_getNodeEnergy(int node_nr)
{
    //return smmObj_energy[node_nr];
    return smm_node[node_nr].energy;
    
}

//member retrieving



//element to string
char* smmObj_getNodeName(smmNode_e type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif
