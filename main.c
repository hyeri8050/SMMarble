//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//  Completed by Hyeri Park on 2023/12/21.

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

#define NUM_GRADES                      9

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

//instance 선언 
static player_t *cur_player;

//player 에너지, 위치, 이름 정의  
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];

char passedNodes[GRADUATE_CREDIT][MAX_CHARNAME];
int passedNodesCount = 0;
 
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
int FoodChargeConfig(int player); //Food Card Configuration
void checkGraduationStatus(int player); //check graduation condition 
  
int isGraduated(void) {
    int i;

    for (i = 0; i < player_nr; i++) {
        if (cur_player[i].flag_graduate) {
            return 1;  // 졸업한 플레이어가 존재함
        }
    }

    return 0;  // 아무도 졸업하지 않음
}

// 각 플레이어의 현재 누적 학점을 체크하여 졸업 가능 여부를 설정하는 함수
void checkGraduationStatus(int player) {
    if (cur_player[player].accumCredit >= 30) {
        cur_player[player].flag_graduate = 1;
    }
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
         printf("\n===============PLAYER STATUS================\n");
         printf("%s :  credit: %i, energy: %i, position: %i\n",
                     cur_player[i].name,
                     cur_player[i].accumCredit,
                     cur_player[i].energy,
                     cur_player[i].position);
         printf("===============PLAYER STATUS================\n");
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
         
         cur_player[i].position = 0;
         
         //set energy
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
     } 
}

//주사위  
int rolldie(int player)
{
    char c;
    printf("\nThis is %s's turn ::::  Press any key to roll a die (press g to see grade): ", cur_player[player].name);
    c = getchar();
    fflush(stdin);
    

    if (c == 'g') {
        printGrades(player);
        // Calculate and print the average grade
        float averageGrade = calcAverageGrade(player);
        printf("--> Printing player %s's grade (average %.6f) ::::\n", cur_player[player].name, averageGrade);
        
        int die_result = rand() % 6 + 1;
        printf("--> result : %d\n", die_result);
        
        // Jump to the next node
        int newPosition = cur_player[0].position;
        int j;
        for (j = 0; j < die_result; j++) {
            newPosition++;
            if (newPosition >= smmdb_len(LISTNO_NODE)) {
                newPosition = 0; // 맨 처음 노드로 순환
            }
    
            // 현재 위치를 지나간 노드 배열에 저장
            strncpy(passedNodes[passedNodesCount], smmObj_getNodeName(smmdb_getData(LISTNO_NODE, newPosition)), MAX_CHARNAME);
            passedNodesCount++;
        }
        
        int i;
        // Print the result and jumped nodes
        for (i = 0; i < die_result; i++) {
            printf("  => Jump to %s\n", passedNodes[passedNodesCount - die_result + i]);
        }

        actionNode(player);
    } else {
        int die_result = rand() % 6 + 1;
        printf("--> result : %d\n", die_result);
        
        // Jump to the next node
        int newPosition = cur_player[0].position;
        int j;
        for (j = 0; j < die_result; j++) {
            newPosition++;
            if (newPosition >= smmdb_len(LISTNO_NODE)) {
                newPosition = 0; // 맨 처음 노드로 순환
            }
    
            // 현재 위치를 지나간 노드 배열에 저장
            strncpy(passedNodes[passedNodesCount], smmObj_getNodeName(smmdb_getData(LISTNO_NODE, newPosition)), MAX_CHARNAME);
            passedNodesCount++;
        }
        
        int i;
        // Print the result and jumped nodes
        for (i = 0; i < die_result; i++) {
            printf("  => Jump to %s\n", passedNodes[passedNodesCount - die_result + i]);
        }

        actionNode(player);
    }
    
    return (rand()%MAX_DIE + 1);
}

// 보충 찬스 카드 함수  
int FoodChargeConfig(int player) {

        // 랜덤으로 카드 선택
        int missionCard = rand() % 13;
        char mission[100];
        int charge;

        // 음식 카드에서 랜덤으로 보충 에너지를 선택하여 반환
        FILE* foodConfigFile = fopen("marbleFoodConfig.txt", "r");
        if (foodConfigFile == NULL) {
            perror("Error opening marbleFoodConfig.txt");
            exit(EXIT_FAILURE);
        }

        // 랜덤으로 선택한 미션 카드까지 읽기
        int i;

        for (i = 0; i <= missionCard; i++) {
            if (fscanf(foodConfigFile, "%[^,], charge:%d", mission, &charge) != 2) {
                perror("Error reading from marbleFoodConfig.txt");
                exit(EXIT_FAILURE);
            }
        }
    
        fclose(foodConfigFile);
    
        // 출력 및 미션 수행
        printf("%s picks %s! Charged Energy: %d (remained energy: %d)\n", cur_player[player].name, mission, charge, cur_player[player].energy);
        
        // 플레이어 에너지 업데이트
        cur_player[player].energy += charge;
    
        return charge;
        
}


//Festival mission function (축제 미션 수행)
void FestivalMission(int player){
     
    //랜덤으로 미션 선택
    int missionIndex = rand() %5;
    char mission[100];
     
    //file에서 미션 불러오기
    FILE* festivalConfigFile = fopen("marbleFestivalConfig.txt", "r");
    if (festivalConfigFile == NULL) {
        perror("Error opening marbleFestivalConfig.txt");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i <= missionIndex; i++) {
        if (fscanf(festivalConfigFile, "%[^\n]\n", mission) != 1) {
            perror("Error reading from marbleFestivalConfig.txt");
            exit(EXIT_FAILURE);
        }
    }

    fclose(festivalConfigFile);

    // 출력 및 미션 수행
    printf("Your mission: %s\n", mission);

    printf("Wow Mission End!\n");
    getchar();
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

// 강의 수강 함수
smmObjGrade_e takeLecture(int player, char *lectureName, int credit) {
    //반복문으로 y 또는 n키를 누르지 않았을 경우 다시 입력받을 수 있도록 함.  
    while (1) {
        printf("--> Lecture %s (credit:%d, energy:%d) starts! Are you going to join? (y/n): ", lectureName, credit, smmObj_getNodeEnergy(smmdb_getData(LISTNO_NODE, cur_player[player].position)));

        char join_input;
        scanf(" %c", &join_input); // 공백을 포함하여 문자 하나만 받음

        if (join_input == 'y') {
            // 각 학점에 따라 할당된 값 사용 
            smmObjGrade_e grade = (smmObjGrade_e)(rand() % NUM_GRADES);

            // 학점을 플레이어의 이력에 추가
            void *gradePtr = smmObj_genObject(lectureName, smmObjType_grade, 0, credit, 0, grade);
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

            // 에너지 소비 및 누적 학점 업데이트
            cur_player[player].accumCredit += credit;
            cur_player[player].energy -= smmObj_getNodeEnergy(smmdb_getData(LISTNO_NODE, cur_player[player].position));

            // 결과 출력
            printf("-> %s successfully takes the lecture %s with grade %s (average: %.6f), remained energy: %d)\n",
                   cur_player[player].name, lectureName, smmObj_getGradeName(grade), calcAverageGrade(player), cur_player[player].energy);

            // 강의 수강 결과 반환
            return grade;
        } else if (join_input == 'n') {
            printf("Player %s drops the lecture %s!\n", cur_player[player].name, lectureName);
            break;  // 반복문 탈출
        } else {
            printf("Invalid! Please press y key or n key!\n");
        }
    }
}

// 학점 종류에 따른 값 반환 함수 => 누적 학점의 평균 학점 출력을 위해 필요  
float smmObj_getGradeValue(smmObjGrade_e grade) {
    switch (grade) {
        case smmObjGrade_Ap:
            return 4.3;
        case smmObjGrade_A0:
            return 4.0;
        case smmObjGrade_Am:
            return 3.7;
        case smmObjGrade_Bp:
            return 3.4;
        case smmObjGrade_B0:
            return 3.1;
        case smmObjGrade_Bm:
            return 2.8;
        case smmObjGrade_Cp:
            return 2.5;
        case smmObjGrade_C0:
            return 2.2;
        case smmObjGrade_Cm:
            return 1.9;
        default:
            return 0; // 예외 처리: 알 수 없는 학점에 대한 값
    }
}


// 주어진 플레이어의 수강한 강의의 평균 학점 계산
float calcAverageGrade(int player) {
    int i;
    float totalGrade = 0;
    int numCourses = smmdb_len(LISTNO_OFFSET_GRADE + player);

    if (numCourses == 0) {
        // 수강한 강의가 없으면 평균 학점은 0
        return 0;
    }

    for (i = 0; i < numCourses; i++) {
        void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        smmObjGrade_e grade = smmObj_getNodeGrade(gradePtr);
        float gradeValue = smmObj_getGradeValue(grade);
        //학점을 누적하여 더함 
        totalGrade += gradeValue;
    }

    // 평균 계산
    return totalGrade / numCourses;
}

//action code when a player stays at a node
void actionNode(int player) {
    void* boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);
    char* name = smmObj_getNodeName(boardPtr);

    switch (type) {
        case SMMNODE_TYPE_LECTURE:
            // 강의 수강 함수 호출
            takeLecture(player, name, smmObj_getNodeCredit(boardPtr));
            break;

        case SMMNODE_TYPE_RESTAURANT:
            // 식당 노드일 경우, 보충 에너지만큼 플레이어의 현재 에너지에 더함
            cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
            printf("%s : Let's eat/drink in %s and charge %d energies. (remained energy :%d)\n",
                   cur_player[player].name, smmObj_getNodeName(boardPtr),
                   smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
            break;

        case SMMNODE_TYPE_LABORATORY:
            // 실험실 노드일 경우
            // 실험 중 상태인지 확인
            if (cur_player[player].position == SMMNODE_TYPE_GOTOLAB) {
                // 실험 중 상태인 경우 주사위를 굴려서 사전에 지정된 실험 성공 기준값 이상이 나오면 실험이 종료되고, 그렇지 않으면 이동하지 못하고 실험중 상태로 머무름
                int dice_result = rolldie(player);
                int experiment_threshold = rand()%6 + 1; // 실험 성공 기준값 (주사위 범위 내에서 임의로 설정) 

            // 실험 결과 출력
            printf("   -> Experiment time! Let's see if you can satisfy the professor (threshold: %d)\n", experiment_threshold);
            printf(" Press any key to roll a die");
            getchar(); // Enter 키 입력 대기
            
            if (dice_result >= experiment_threshold) {
                    // 실험 성공
                    printf("   -> Experiment result : %d, %s successfully completed the experiment in the laboratory! Energy: %d\n", dice_result, cur_player[player].name, cur_player[player].energy);
            } else {
                    // 실험 실패, 이동하지 못하고 실험중 상태 유지
                    printf("   -> Experiment result : %d, fail T_T. %s needs more experiment", dice_result, cur_player[player].name, cur_player[player].energy);
                }
            }
            break;

        case SMMNODE_TYPE_HOME:
            // 집 노드일 경우, 보충 에너지만큼 현재 에너지에 더함
            cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
            printf("%s returned to HOME! Energy charged by %d (total : %d)\n",
                   cur_player[player].name, smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);
            break;

        case SMMNODE_TYPE_GOTOLAB:
            // 실험 노드일 경우 실험 중 상태로 전환되면서 실험실로 이동
            cur_player[player].position = 8;
            printf("OMG! This is experiment time!! Player %s goes to the lab.\n", cur_player[player].name);
            break;

        case SMMNODE_TYPE_FOODCHANCE: 
             
            // 식사 기회 노드일 경우
            printf("-> %s gets a food chance! Press any key to pick a food card: ", cur_player[player].name);
            getchar();
            
            FoodChargeConfig(player);
                        
            // 출력 및 미션 수행
            printf(" Charged Energy : %d (remained energy: %d)\n", smmObj_getNodeEnergy(boardPtr), cur_player[player].energy);

            break;

        case SMMNODE_TYPE_FESTIVAL:
            // 축제 노드일 경우
            printf("%s participates in Snow Festival! Press any key to pick a festival card :", cur_player[player].name);
            getchar();

            // 미션 수행 함수 호출
            FestivalMission(player);

            break;

        default:
            break;
    }
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
  
   
    //After showing txt file
    //Start 
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
        //rolldie 함수를 실행하여 플레이어 포지션을 증가시킴 -> 위치가 어딘지 검증 가능  
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn); 

        //End condition
        // check if any player has graduated
        checkGraduationStatus(turn);
        
        if (isGraduated()) {
            // find the graduating player and print course details
            int i;
            for (i = 0; i < player_nr; i++) {
                if (cur_player[i].flag_graduate) {
                    printf("\nCongratulations! %s has graduated!\n", cur_player[i].name);
                    printf("Courses taken:\n");
                    printGrades(i);
                    break;
                }
            }
            break;  // end the game
        }
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    free(cur_player);
    
    getchar();
    
    return 0;
}
