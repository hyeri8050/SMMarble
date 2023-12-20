//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

//#define MAX_PLAYER                    100

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr; // player 명수

//player의 구조체화 
typedef struct player{
        int energy;
        int position; //current position
        char name[MAX_CHARNAME];
        int accumCredit; //학점의 누적 합 
        int flag_graduate; //졸업가능 여부 상태 저장 변수  
} player_t;

static player_t *cur_player;
//instance 선언  
//static player_t cur_player[MAX_PLAYER];


//player 에너지, 위치, 이름 정의  
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];


 
//function prototypes
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy); //generate a new player
void printGrades(int player); //print grade history of the player
//-> 해당 플레이어가 수강한 강의와 해당 강의의 학점 표시 
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
void actionNode(int player);
float calcAverageGrade(int player); //calculate average grade of the player
smmObjGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history

int isGraduated(void) {
    int i;

    for (i = 0; i < player_nr; i++) {
        if (cur_player[i].flag_graduate) {
            return 1;  // 졸업한 플레이어가 존재함
        }
    }

    return 0;  // 아무도 졸업하지 않음
}


void printGrades(int player)
{
     int i;
     void *gradePtr;
     for (i=0; i<smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
     }
}

//printPlayerStatus
void printPlayerStatus(void)
{
     int i;
     
     for (i=0; i<player_nr; i++)
     {
         printf("%s: credit %i, energy %i, position %i\n",
                     cur_player[i].name,
                     cur_player[i].position,
                     cur_player[i].energy,
                     cur_player[i].accumCredit);
     }
}

//Player 생성 함수 
void generatePlayers(int n, int initEnergy)
{
     int i;
     //n time loop (player가 n명 있다고 가정)
     for (i=0; i<n; i++)
     {
         //input name
         printf("Input player %i's name: ", i);
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
     } 
}

//주사위  
int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    

    if (c == 'g')
        printGrades(player);

    
    return (rand()%MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
    void* boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType (boardPtr); 
    //int type = smmObj_getNodeType(cur_player[player].position);
    char* name = smmObj_getNodeName (boardPtr);
    void* gradePtr; 
    
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
             //강의 수강 조건 확인 
             if(cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr))
             {
                //강의 수강 진행
                printf("%s is listening lecture in %s.\n", cur_player[player].name, name);
                
                // 학점을 랜덤으로 생성하고 플레이어의 이력에 추가
                smmObjGrade_e grade = rand() % MAX_GRADE;
                gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, grade);
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                
                //grade generation
                gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, smmObj_getNodeEnergy(boardPtr));
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            
                //Update players' energy and grade  
                cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
                cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
                
                printf("%s님이 %s에서 %i학점을 얻었습니다. (학점: %s)\n", cur_player[player].name, name, smmObj_getNodeCredit(boardPtr), smmObj_getGradeName(grade));
             }
             else
             {
                 printf("%s님이 %s에서 강의를 듣기에는 에너지가 부족합니다.\n", cur_player[player].name, name);
             }
             break;
        
        default:
            break;
    }
}

void goForward (int player, int step)
{
     void *boardPtr;
     cur_player[player].position += step;
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
     
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));
}

// 주어진 플레이어의 수강한 강의의 평균 학점 계산
float calcAverageGrade(int player) {
    int i;
    int totalGrade = 0;
    int numCourses = smmdb_len(LISTNO_OFFSET_GRADE + player);

    if (numCourses == 0) {
        // 수강한 강의가 없으면 평균 학점은 0
        return 0;
    }

    for (i = 0; i < numCourses; i++) {
        void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        totalGrade += smmObj_getNodeGrade(gradePtr);
    }

    // 평균 계산
    return (float)totalGrade / numCourses;
}

// 강의 수강 함수
smmObjGrade_e takeLecture(int player, char *lectureName, int credit) {
    smmObjGrade_e grade = rand() % MAX_GRADE;  // 랜덤으로 학점 생성
    void *gradePtr = smmObj_genObject(lectureName, smmObjType_grade, 0, credit, 0, grade);
    
    // 플레이어의 학점 이력에 추가
    smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

    // 플레이어의 누적 학점 및 에너지 업데이트
    cur_player[player].accumCredit += credit;
    cur_player[player].energy -= smmObj_getNodeEnergy(smmdb_getData(LISTNO_NODE, cur_player[player].position));

    printf("%s님이 %s 수업을 듣고 %i학점을 얻었습니다. (학점: %s)\n", cur_player[player].name, lectureName,
           credit, smmObj_getGradeName(grade));

    return grade;
}

// 특정 강의명의 학점 찾기 함수
void* findGrade(int player, char *lectureName) {
    // 플레이어의 강의 이력을 순회하면서 해당 강의명의 학점을 찾기
    int nodeIndex = LISTNO_OFFSET_GRADE + player;
    void* gradePtr = smmdb_getData(nodeIndex, 0);

    while (gradePtr != NULL) {
        char* gradeName = smmObj_getNodeName(gradePtr);

        if (strcmp(gradeName, lectureName) == 0) {
            // 해당 강의명과 일치하는 학점을 찾았을 경우 해당 학점 반환
            return gradePtr;
        }

        // 다음 학점으로 이동
        int nextIndex = smmdb_addTail(nodeIndex, gradePtr);
        if (nextIndex >= 0) {
            gradePtr = smmdb_getData(nodeIndex, nextIndex);
        } else {
            break;  // 다음 학점이 없으면 종료
        }
    }

    // 해당 강의명의 학점이 없을 경우 NULL 반환
    return NULL;
}

int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type; //node type
    int credit; //grade
    int energy;
    int i; 
    int initEnergy;
    int turn = 0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    //error handling code
    if ((fp = fopen(BOARDFILEPATH, "r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%*d. %[^,], type:%d, credit:%d, energy:%d", name, &type, &credit, &energy) == 4) //read a node parameter set
    {
        //store the parameter set 
        void *boardObj = smmObj_genObject(name, type, type, credit, energy, 0); //함수 호출  
        smmdb_addTail(LISTNO_NODE, boardObj);
    
        if (type == SMMNODE_TYPE_HOME)
            initEnergy = energy;
        board_nr++; //while문 돌때마다 변수 하나씩 증가  
    }
    fclose(fp);
    
    for (i = 0; i < board_nr; i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
    
        printf("=> %i: %s, type: %i, credit %i, energy %i\n",
               i, smmObj_getNodeName(boardObj),
               type, smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    
    printf("Total number of board nodes : %i\n", board_nr);
    
    /*if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set 
        void *boardObj = smmObj_genObject (name, smmObjType_board, type, credit, energy, 0); //함수 호출  
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy; 
        board_nr++; //while문 돌때마다 변수 하나씩 증가  
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);

    
    for (i=0; i<board_nr; i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i); 
        
        printf("node %i: %s, %i(%s), credit %i, energy %i\n", 
            i, smmObj_getNodeName(boardObj),  
            smmObj_getNodeType(boardObj), smmObj_getNodeType(smmObj_getNodeName(boardObj)),
            smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj)
        );
        
    }; */
    
    
    //2. food card config 
    //error handling code
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1; 
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%[^,], charge:%d", name, &credit) == 2) //read a food parameter set
    {
       //store the parameter set
       void *foodObj = smmObj_genObject(name, LISTNO_FOODCARD, type, credit, energy, 0);
       smmdb_addTail(LISTNO_NODE, foodObj);

       printf("%s, charge: %d", name, credit); // print food card information
       food_nr++; //while문 돌때마다 변수 하나씩 증가  
    }
    fclose(fp);
    printf("\nTotal number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%[^\n]", name) == 1) //read a festival card string
    {
        //store the parameter set
        void *festivalObj = smmObj_genObject(name, LISTNO_FESTCARD, 0, 0, 0, 0);
        smmdb_addTail(LISTNO_NODE, festivalObj);
        
        printf("%s\n", name); 
        festival_nr++;
        
        fgetc(fp); //한 문자씩 읽어들이기 위해  
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n\n\n", festival_nr);
    
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    /*
    do
    {
        //input player number to player_nr
    }
    while ();
    */
    
    printf("=======================================================================\n");
    printf("-----------------------------------------------------------------------\n");
    printf("       Sookmyung Marble !! Let's Graduate (total credit : 30)!!         \n");
    printf("-----------------------------------------------------------------------\n");
    printf("=======================================================================\n\n");

    do
    {
        //input player number to player_nr
        printf("Input the number of players (1 to %d): ", MAX_PLAYER);
        scanf("%d", &player_nr);
        fflush(stdin); //delete 잔재  
    }
    while(player_nr < 0 || player_nr > MAX_PLAYER);
    
    cur_player  = (player_t*)malloc(player_nr*sizeof(player_t)); 
    generatePlayers(player_nr, initEnergy);
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (!isGraduated()) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment) 
        //직접 해보기 
        //rolldie 함수를 실행하여 플레이어 포지션을 증가시킴 -> 위치가 어딘지 검증 가능  
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        // 수강 신청, 학점 조회, 평균 학점 계산
        char lectureName[MAX_CHARNAME];
        int credit;

        printf("Enter lecture name: ");
        scanf("%s", lectureName);
        printf("Enter credit: ");
        scanf("%d", &credit);

        // 강의 수강
        smmObjGrade_e grade = takeLecture(turn, lectureName, credit);

        // 학점 조회
        void* foundGrade = findGrade(turn, lectureName);
        if (foundGrade != NULL) {
            printf("%s님의 %s 강의 학점: %s\n", cur_player[turn].name, lectureName, smmObj_getTypeName(smmObj_getNodeType(foundGrade)));
        } else {
            printf("%s님은 %s 강의를 수강하지 않았습니다.\n", cur_player[turn].name, lectureName);
        }

        // 평균 학점 계산 및 출력
        printf("%s님의 평균 학점: %.2f\n", cur_player[turn].name, calcAverageGrade(turn)); 

        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    free(cur_player);
    
    getchar();
    
    return 0;
}
