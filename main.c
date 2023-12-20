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

static int player_nr; // player ���

//player�� ����üȭ 
typedef struct player{
        int energy;
        int position; //current position
        char name[MAX_CHARNAME];
        int accumCredit; //������ ���� �� 
        int flag_graduate; //�������� ���� ���� ���� ����  
} player_t;

static player_t *cur_player;
//instance ����  
//static player_t cur_player[MAX_PLAYER];


//player ������, ��ġ, �̸� ����  
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];


 
//function prototypes
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy); //generate a new player
void printGrades(int player); //print grade history of the player
//-> �ش� �÷��̾ ������ ���ǿ� �ش� ������ ���� ǥ�� 
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
            return 1;  // ������ �÷��̾ ������
        }
    }

    return 0;  // �ƹ��� �������� ����
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

//Player ���� �Լ� 
void generatePlayers(int n, int initEnergy)
{
     int i;
     //n time loop (player�� n�� �ִٰ� ����)
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

//�ֻ���  
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
             //���� ���� ���� Ȯ�� 
             if(cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr))
             {
                //���� ���� ����
                printf("%s is listening lecture in %s.\n", cur_player[player].name, name);
                
                // ������ �������� �����ϰ� �÷��̾��� �̷¿� �߰�
                smmObjGrade_e grade = rand() % MAX_GRADE;
                gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, grade);
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                
                //grade generation
                gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit(boardPtr), 0, smmObj_getNodeEnergy(boardPtr));
                smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            
                //Update players' energy and grade  
                cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
                cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
                
                printf("%s���� %s���� %i������ ������ϴ�. (����: %s)\n", cur_player[player].name, name, smmObj_getNodeCredit(boardPtr), smmObj_getGradeName(grade));
             }
             else
             {
                 printf("%s���� %s���� ���Ǹ� ��⿡�� �������� �����մϴ�.\n", cur_player[player].name, name);
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

// �־��� �÷��̾��� ������ ������ ��� ���� ���
float calcAverageGrade(int player) {
    int i;
    int totalGrade = 0;
    int numCourses = smmdb_len(LISTNO_OFFSET_GRADE + player);

    if (numCourses == 0) {
        // ������ ���ǰ� ������ ��� ������ 0
        return 0;
    }

    for (i = 0; i < numCourses; i++) {
        void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        totalGrade += smmObj_getNodeGrade(gradePtr);
    }

    // ��� ���
    return (float)totalGrade / numCourses;
}

// ���� ���� �Լ�
smmObjGrade_e takeLecture(int player, char *lectureName, int credit) {
    smmObjGrade_e grade = rand() % MAX_GRADE;  // �������� ���� ����
    void *gradePtr = smmObj_genObject(lectureName, smmObjType_grade, 0, credit, 0, grade);
    
    // �÷��̾��� ���� �̷¿� �߰�
    smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

    // �÷��̾��� ���� ���� �� ������ ������Ʈ
    cur_player[player].accumCredit += credit;
    cur_player[player].energy -= smmObj_getNodeEnergy(smmdb_getData(LISTNO_NODE, cur_player[player].position));

    printf("%s���� %s ������ ��� %i������ ������ϴ�. (����: %s)\n", cur_player[player].name, lectureName,
           credit, smmObj_getGradeName(grade));

    return grade;
}

// Ư�� ���Ǹ��� ���� ã�� �Լ�
void* findGrade(int player, char *lectureName) {
    // �÷��̾��� ���� �̷��� ��ȸ�ϸ鼭 �ش� ���Ǹ��� ������ ã��
    int nodeIndex = LISTNO_OFFSET_GRADE + player;
    void* gradePtr = smmdb_getData(nodeIndex, 0);

    while (gradePtr != NULL) {
        char* gradeName = smmObj_getNodeName(gradePtr);

        if (strcmp(gradeName, lectureName) == 0) {
            // �ش� ���Ǹ�� ��ġ�ϴ� ������ ã���� ��� �ش� ���� ��ȯ
            return gradePtr;
        }

        // ���� �������� �̵�
        int nextIndex = smmdb_addTail(nodeIndex, gradePtr);
        if (nextIndex >= 0) {
            gradePtr = smmdb_getData(nodeIndex, nextIndex);
        } else {
            break;  // ���� ������ ������ ����
        }
    }

    // �ش� ���Ǹ��� ������ ���� ��� NULL ��ȯ
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
        void *boardObj = smmObj_genObject(name, type, type, credit, energy, 0); //�Լ� ȣ��  
        smmdb_addTail(LISTNO_NODE, boardObj);
    
        if (type == SMMNODE_TYPE_HOME)
            initEnergy = energy;
        board_nr++; //while�� �������� ���� �ϳ��� ����  
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
        void *boardObj = smmObj_genObject (name, smmObjType_board, type, credit, energy, 0); //�Լ� ȣ��  
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy; 
        board_nr++; //while�� �������� ���� �ϳ��� ����  
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
       food_nr++; //while�� �������� ���� �ϳ��� ����  
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
        
        fgetc(fp); //�� ���ھ� �о���̱� ����  
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
        fflush(stdin); //delete ����  
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
        //���� �غ��� 
        //rolldie �Լ��� �����Ͽ� �÷��̾� �������� ������Ŵ -> ��ġ�� ����� ���� ����  
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        // ���� ��û, ���� ��ȸ, ��� ���� ���
        char lectureName[MAX_CHARNAME];
        int credit;

        printf("Enter lecture name: ");
        scanf("%s", lectureName);
        printf("Enter credit: ");
        scanf("%d", &credit);

        // ���� ����
        smmObjGrade_e grade = takeLecture(turn, lectureName, credit);

        // ���� ��ȸ
        void* foundGrade = findGrade(turn, lectureName);
        if (foundGrade != NULL) {
            printf("%s���� %s ���� ����: %s\n", cur_player[turn].name, lectureName, smmObj_getTypeName(smmObj_getNodeType(foundGrade)));
        } else {
            printf("%s���� %s ���Ǹ� �������� �ʾҽ��ϴ�.\n", cur_player[turn].name, lectureName);
        }

        // ��� ���� ��� �� ���
        printf("%s���� ��� ����: %.2f\n", cur_player[turn].name, calcAverageGrade(turn)); 

        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    free(cur_player);
    
    getchar();
    
    return 0;
}
