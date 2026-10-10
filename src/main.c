#include <stdio.h>
#include "functions.h"

int main(void)
{
    int choice;

    loadRecords();

    do {
        printf("\n=================================\n");
        printf(" EMERGENCY RESCUE PRIORITY SYSTEM\n");
        printf("=================================\n");
        printf("1. Register Emergency Request\n");
        printf("2. Display Requests by Priority\n");
        printf("3. Assign Rescue Team\n");
        printf("4. Update Request Status\n");
        printf("5. Search Request\n");
        printf("6. Emergency Summary\n");
        printf("7. Add Rescue Team\n");
        printf("0. Exit\n");

        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            return 1;
        }

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}

        switch (choice) {
            case 1: registerRequest(); break;
            case 2: displayRequests(); break;
            case 3: assignTeam(); break;
            case 4: updateStatus(); break;
            case 5: searchRequest(); break;
            case 6: emergencySummary(); break;
            case 7: manageTeams(); break;
            case 0: printf("Thank you!\n"); break;
            default: printf("Invalid choice!\n");
        }

    } while (choice != 0);

    return 0;
}