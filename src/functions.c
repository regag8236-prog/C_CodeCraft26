#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

static RescueRequest requests[MAX_REQUESTS];
static RescueTeam teams[MAX_TEAMS];
static int count = 0, teamCount = 0;

static void readLine(const char *prompt, char *out, int size)
{
    printf("%s", prompt);
    if (fgets(out, size, stdin)) {
        out[strcspn(out, "\n")] = '\0';
    } else {
        out[0] = '\0';
    }
}

static int readInt(const char *prompt, int min, int max)
{
    char line[80], extra;
    int n;

    while (1) {
        readLine(prompt, line, sizeof(line));
        if (sscanf(line, " %d %c", &n, &extra) == 1 &&
            n >= min && n <= max)
            return n;
        printf("Invalid input. Enter %d to %d.\n", min, max);
    }
}

static int getPriority(const char *level)
{
    if (strcmp(level, "Critical") == 0) return 1;
    if (strcmp(level, "High") == 0) return 2;
    if (strcmp(level, "Medium") == 0) return 3;
    return 4;
}

static int findRequest(const char *id)
{
    for (int i = 0; i < count; i++)
        if (strcmp(requests[i].id, id) == 0)
            return i;
    return -1;
}

void saveRecords(void)
{
    FILE *fp = fopen("rescue_data.txt", "w");
    if (!fp) {
        printf("Could not save records.\n");
        return;
    }

    fprintf(fp, "%d\n", count);
    for (int i = 0; i < count; i++) {
        RescueRequest r = requests[i];
        fprintf(fp, "%s\n%s\n%d\n%d\n%s\n%s\n%s\n%s\n",
                r.id, r.location, r.people, r.priority,
                r.emergency, r.resources, r.team, r.status);
    }

    fprintf(fp, "%d\n", teamCount);
    for (int i = 0; i < teamCount; i++) {
        fprintf(fp, "%s\n%s\n%d\n",
                teams[i].name, teams[i].resource,
                teams[i].available);
    }

    fclose(fp);
}

void loadRecords(void)
{
    FILE *fp = fopen("rescue_data.txt", "r");
    if (!fp) return;

    int n;
    if (fscanf(fp, "%d\n", &n) != 1 ||
        n < 0 || n > MAX_REQUESTS) {
        fclose(fp);
        return;
    }

    count = 0;
    for (int i = 0; i < n; i++) {
        RescueRequest r = {0};
        if (!fgets(r.id, sizeof(r.id), fp) ||
            !fgets(r.location, sizeof(r.location), fp))
            break;

        r.id[strcspn(r.id, "\n")] = '\0';
        r.location[strcspn(r.location, "\n")] = '\0';

        if (fscanf(fp, "%d\n%d\n", &r.people, &r.priority) != 2)
            break;

        if (!fgets(r.emergency, sizeof(r.emergency), fp) ||
            !fgets(r.resources, sizeof(r.resources), fp) ||
            !fgets(r.team, sizeof(r.team), fp) ||
            !fgets(r.status, sizeof(r.status), fp))
            break;

        r.emergency[strcspn(r.emergency, "\n")] = '\0';
        r.resources[strcspn(r.resources, "\n")] = '\0';
        r.team[strcspn(r.team, "\n")] = '\0';
        r.status[strcspn(r.status, "\n")] = '\0';
        requests[count++] = r;
    }

    if (fscanf(fp, "%d\n", &n) == 1 &&
        n >= 0 && n <= MAX_TEAMS) {
        teamCount = 0;
        for (int i = 0; i < n; i++) {
            RescueTeam t = {0};
            if (!fgets(t.name, sizeof(t.name), fp) ||
                !fgets(t.resource, sizeof(t.resource), fp) ||
                fscanf(fp, "%d\n", &t.available) != 1)
                break;

            t.name[strcspn(t.name, "\n")] = '\0';
            t.resource[strcspn(t.resource, "\n")] = '\0';
            teams[teamCount++] = t;
        }
    }

    fclose(fp);
}

void registerRequest(void)
{
    if (count >= MAX_REQUESTS) {
        printf("Request limit reached.\n");
        return;
    }

    RescueRequest r = {0};
    readLine("Request ID: ", r.id, sizeof(r.id));

    if (strlen(r.id) == 0 || findRequest(r.id) != -1) {
        printf("ID is empty or already exists.\n");
        return;
    }

    readLine("Location: ", r.location, sizeof(r.location));
    r.people = readInt("People affected: ", 0, 1000000);

    printf("Emergency level: 1.Critical 2.High 3.Medium 4.Low\n");
    int level = readInt("Choose level: ", 1, 4);

    const char *levels[] = {
        "", "Critical", "High", "Medium", "Low"
    };
    strcpy(r.emergency, levels[level]);
    r.priority = getPriority(r.emergency);

    readLine("Required resources: ",
             r.resources, sizeof(r.resources));

    strcpy(r.team, "Not Assigned");
    strcpy(r.status, "Pending");

    requests[count++] = r;
    saveRecords();
    printf("Request registered and saved!\n");
}

void displayRequests(void)
{
    if (!count) {
        printf("No requests registered.\n");
        return;
    }

    /* Sort by priority: 1 is the highest priority */
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (requests[j].priority < requests[i].priority) {
                RescueRequest temp = requests[i];
                requests[i] = requests[j];
                requests[j] = temp;
            }
        }
    }

    printf("\n--- RESCUE REQUESTS (HIGH PRIORITY FIRST) ---\n");
    for (int i = 0; i < count; i++) {
        printf("\nID: %s | Emergency: %s\n",
               requests[i].id, requests[i].emergency);
        printf("Location: %s | People: %d\n",
               requests[i].location, requests[i].people);
        printf("Resources: %s\n", requests[i].resources);
        printf("Team: %s | Status: %s\n",
               requests[i].team, requests[i].status);
    }
}

void manageTeams(void)
{
    if (teamCount >= MAX_TEAMS) {
        printf("Team limit reached.\n");
        return;
    }

    RescueTeam t = {0};
    readLine("Team name: ", t.name, sizeof(t.name));
    readLine("Available resources: ",
             t.resource, sizeof(t.resource));

    if (!strlen(t.name)) {
        printf("Team name cannot be empty.\n");
        return;
    }

    for (int i = 0; i < teamCount; i++) {
        if (strcmp(teams[i].name, t.name) == 0) {
            printf("Team already exists.\n");
            return;
        }
    }

    t.available = 1;
    teams[teamCount++] = t;
    saveRecords();
    printf("Rescue team added and marked available.\n");
}

void assignTeam(void)
{
    char id[20];
    readLine("Request ID: ", id, sizeof(id));
    int index = findRequest(id);

    if (index == -1) {
        printf("Request not found.\n");
        return;
    }

    if (strcmp(requests[index].status, "Resolved") == 0) {
        printf("Resolved requests cannot be assigned.\n");
        return;
    }

    printf("\nAvailable teams:\n");
    int found = 0;

    for (int i = 0; i < teamCount; i++) {
        if (teams[i].available) {
            printf("%d. %s (Resources: %s)\n",
                   i + 1, teams[i].name, teams[i].resource);
            found = 1;
        }
    }

    if (!found) {
        printf("No available teams. Use menu option 7 to add a team.\n");
        return;
    }

    int choice = readInt("Select team number: ", 1, teamCount);
    int t = choice - 1;

    if (!teams[t].available) {
        printf("That team is not available. Try again.\n");
        return;
    }

    strcpy(requests[index].team, teams[t].name);
    teams[t].available = 0;

    if (strcmp(requests[index].status, "Pending") == 0)
        strcpy(requests[index].status, "In Progress");

    saveRecords();
    printf("Team assigned successfully!\n");
}

void updateStatus(void)
{
    char id[20];
    readLine("Request ID: ", id, sizeof(id));
    int index = findRequest(id);

    if (index == -1) {
        printf("Request not found.\n");
        return;
    }

    printf("1. Pending  2. In Progress  3. Resolved\n");
    int choice = readInt("New status: ", 1, 3);

    const char *statuses[] = {
        "", "Pending", "In Progress", "Resolved"
    };

    if (choice == 3 &&
        strcmp(requests[index].team, "Not Assigned") == 0) {
        printf("Assign a rescue team before resolving the request.\n");
        return;
    }

    if (choice == 3) {
        for (int i = 0; i < teamCount; i++) {
            if (strcmp(teams[i].name, requests[index].team) == 0)
                teams[i].available = 1;
        }
    }

    strcpy(requests[index].status, statuses[choice]);
    saveRecords();
    printf("Status updated successfully!\n");
}

void searchRequest(void)
{
    char id[20];
    readLine("Enter request ID: ", id, sizeof(id));
    int i = findRequest(id);

    if (i == -1) {
        printf("Request not found.\n");
        return;
    }

    printf("\nID: %s\nLocation: %s\nPeople: %d\n",
           requests[i].id, requests[i].location,
           requests[i].people);
    printf("Emergency: %s\nResources: %s\n",
           requests[i].emergency, requests[i].resources);
    printf("Team: %s\nStatus: %s\n",
           requests[i].team, requests[i].status);
}

void emergencySummary(void)
{
    int levels[5] = {0};
    int pending = 0, progress = 0, resolved = 0, people = 0;

    for (int i = 0; i < count; i++) {
        levels[requests[i].priority]++;
        people += requests[i].people;

        if (strcmp(requests[i].status, "Pending") == 0)
            pending++;
        else if (strcmp(requests[i].status, "In Progress") == 0)
            progress++;
        else if (strcmp(requests[i].status, "Resolved") == 0)
            resolved++;
    }

    printf("\n--- EMERGENCY SUMMARY ---\n");
    printf("Total requests: %d\n", count);
    printf("Critical: %d | High: %d\n", levels[1], levels[2]);
    printf("Medium: %d | Low: %d\n", levels[3], levels[4]);
    printf("People affected: %d\n", people);
    printf("Pending: %d | In Progress: %d | Resolved: %d\n",
           pending, progress, resolved);

    int available = 0;
    for (int i = 0; i < teamCount; i++)
        if (teams[i].available) available++;

    printf("Rescue teams: %d total, %d available\n",
           teamCount, available);
}