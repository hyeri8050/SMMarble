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

//������ ���� 2���� �迭 ���� 
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

//1. ����ü ���� ���� 
typedef struct smmObject{
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
} smmObject_t;

//2. ����ü �迭 ���� ���� 
smmObject_t smm_node[MAX_NODE];

static int smmObj_noNode = 0;

#if 0
//smmObj�ȿ� �ִ� ���� ���� ���� �� �迭ȭ => 11.30 ����üȭ�� �����ϱ�  
static char smmObj_name [MAX_NODE][MAX_CHARNAME]; 
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode = 0;
#endif

//object generation (Node �����, �� main.c�� ����) 
void smmObj_genNode(char* name, int type, int credit, int energy)
{
    #if 0
    //strcpy (���, ����)  
    strcpy (smmObj_type[smmObj_noNode], name); //���ڿ�(�迭 ����) -> �ϳ��� ����  
    smmObj_type[smmObj_noNode] = type; //���� �ִ� �迭�� �Էµ� type���� ���� 
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
