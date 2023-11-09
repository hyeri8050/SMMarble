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

//smmObj안에 있는 전역 변수 선언 및 배열화  
static char smmObj_name [MAX_NODE][MAX_CHARNAME]; 
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode = 0;

//object generation (Node 만들기, 즉 main.c에 전달) 
void smmObj_genNode(char* name, int type, int credit, int energy)
{
    //strcpy (대상, 원본)  
    strcpy (smmObj_type[smmObj_noNode], name); //문자열(배열 복사) -> 하나씩 복사  
    smmObj_type[smmObj_noNode] = type; //갖고 있는 배열에 입력된 type값을 넣음 
    smmObj_credit[smmObj_noNode] = credit;
    smmObj_energy[smmObj_noNode] = energy;  
    
    static int smmObj_noNode++; 
}
//name
char* smmObj_getNodeName(int node_nr)
{
      return smmObj_type[smmObj_noNode];
}
 
//type
int smmObj_getNodeType(int node_nr)
{
    return smmObj_type[node_nr];
}

//credit
int smmObj_getNodeCredit(int node_nr)
{
    return smmObj_credit[node_nr];
}

//energy
int smmObj_getNodeEnergy(int node_nr)
{
    return smmObj_energy[node_nr];
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
