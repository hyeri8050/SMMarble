//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//
//보드의 노드, 음식카드, 축제카드, 각 플레이어의 수강 강의 이력 저장 DB 

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100

//유형에 대한 2차원 배열 정의 
static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
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
    smmObjType_e objType;
    int type;
    int credit;
    int energy;
    smmObjGrade_e grade;
} smmObject_t;

//2. 구조체 변수 정의 
//smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;


#if 0
//smmObj안에 있는 전역 변수 선언 및 배열화 => 11.30 구조체화로 변경하기  
static char smmObj_name [MAX_NODE][MAX_CHARNAME]; 
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode = 0;
#endif

//3.관련 함수 변경 
//object generation (Node 만들기, 즉 main.c에 전달) 
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
     
    strcpy (ptr->name, name);  
    ptr->objType = objType; 
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;

}

//Node 생성 
void smmObj_genNode(char *name, int type, int credit, int energy) 
{
    // Create a new node and add it to the database
    void *nodePtr = smmObj_genObject(name, smmObjType_board, type, credit, energy, smmObjGrade_Ap);
    smmdb_addTail(0, nodePtr);
}


//name
char* smmObj_getNodeName(void *obj)
{
      smmObject_t* ptr = (smmObject_t*)obj;
      
      return ptr->name; //외부에 구조체 정의를 유출하지도 않을 수 O 
      //smmObj_noNode];
}
 
//type
int smmObj_getNodeType(void *obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    //return smmObj_type[node_nr];
    return ptr->type;
}

//credit
int smmObj_getNodeCredit(void *obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    //return smmObj_credit[node_nr];
    return ptr->credit;
}

//energy
int smmObj_getNodeEnergy(void *obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    //return smmObj_energy[node_nr];
    return ptr->energy;
    
}

//grade
int smmObj_getNodeGrade(void *obj) {
    smmObject_t* ptr = (smmObject_t*)obj;
    return ptr->grade;
}

const char* smmObj_getGradeName(smmObjGrade_e grade) {
    // 여기에 해당 학점에 대한 문자열 반환
    switch (grade) {
        case smmObjGrade_Ap:
            return "A+";
        case smmObjGrade_A0:
            return "A0";
        case smmObjGrade_Am:
            return "A-";
        case smmObjGrade_Bp:
            return "B+";
        case smmObjGrade_B0:
            return "B0";
        case smmObjGrade_Bm:
            return "B-";
        case smmObjGrade_Cp:
            return "C+";
        case smmObjGrade_C0:
            return "C0";
        case smmObjGrade_Cm:
            return "C-";
        default:
            return "Unknown Grade";
    } 
}


//member retrieving



//element to string





