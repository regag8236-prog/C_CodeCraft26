#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX_REQUESTS 200
#define MAX_TEAMS 20

typedef struct {
    char id[20];
    char location[100];
    int people;
    int priority;
    char emergency[20];
    char resources[100];
    char team[50];
    char status[30];
} RescueRequest;

typedef struct {
    char name[50];
    char resource[100];
    int available;
} RescueTeam;

void registerRequest(void);
void displayRequests(void);
void assignTeam(void);
void updateStatus(void);
void searchRequest(void);
void emergencySummary(void);
void saveRecords(void);
void loadRecords(void);
void manageTeams(void);

#endif