//
//  main.c
//  Projet
//
//  Created by CHEN KUO on 2020/5/8.
//  Copyright © 2020 CHEN KUO. All rights reserved.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Family{
    int size;
    int preference[5];
    int day;  //Reception day, initially - 1
    int choose; //Record the reception day as the customer's option, initially - 1
}Fam;

Fam family[449] = {
    [0 ... 448] = {
        .day = -1,
        .choose = -1
    }
};

void RemoveBlank(char *ch)  //Remove spaces between data
{
    char *c;
    while (*ch != ' ') {
        if (*ch == '\0') {
            return;
        }
        ch++;
    }
    
    c = ch + 1;
    while (c == ' ') {
        c++;
    }
    
    while (*c != '\0') {
        if (*c != ' ') {
            *ch = *c;
            *c = ' ';
            ch++;
        }
        c++;
    }
    *ch = '\0';
    return;
}

void DealData(int (*data)[6]) { //Enter data into the structure
    int line, col;
    for(line = 0; line < 449; line++){
        for(col = 0; col < 6; col++){
            if (col == 0)
                family[line].size = data[line][col];
            else
                family[line].preference[col - 1] = data[line][col];
        }
    }
}

int ReadCsv() {
    FILE *fp = NULL;
    char *fg, *line;
    char buffer[4096];
    int data[449][6];
    int l = 0, c = 0;
    if ((fp = fopen("pb0.csv", "at+")) != NULL)
    {
        fseek(fp, 72L, SEEK_SET);  //Go to second line
        char delims[] = ",";
        char *element = NULL;
        while ((fg = fgets(buffer, sizeof(buffer), fp))!=NULL) {
            line = strtok(fg, ",");
            while (line != NULL) {    //Read each line of data
                element = strtok(line, ";");    ////Divide a line of data into cells
                RemoveBlank(element);
                data[l][c] = (int)(*(element + 0)-'0');
                // printf("%d\n", data[l][c]);
                c++;
                if (c == 6) {
                    c = 0;
                    l++;
                    if (l == 448)
                        break;
                }
                line = strtok(NULL, ",");
            }
        }
        fclose(fp);
        fp = NULL;
    }

    DealData(data);
    
    return 0;
}

void Des_Order() {  //Descending family by size
    int m, n;
    Fam temp;
    
    for(m = 0; m < 449; m++){
        for(n = m - 1; n >= 0; n--){
            if (family[n + 1].size > family[n].size){
                temp = family[n + 1];
                family[n + 1] = family[n];
                family[n] = temp;
            }
            else
                break;
        }
    }
}

//Priority should be given to the first and second volunteers for families with a large number of people, and the rest of the families should fill the vacancy. When the total number of people in a day's service reaches saturation, the date will be postponed one day, and the distribution will be based on the principle of voluntary priority from the new large number of families. After traversing the first and second volunteers of all families, follow-up volunteers are considered according to the same principle.

void Plan(int *week, int *pop) {
    int f, d = 0, c = 0;
    Des_Order();
    
    while(c < 5) {
        f = 0;
        while(f < 449) {
            if(pop[d] + family[f].size >= 300) {
                   d++;
                   f = 0;
            }
            else {
                if(family[f].day = -1) {
                    if(c < 2) {
                        if(family[f].preference[0] == d) {
                            family[f].day = d;
                            family[f].choose = 0;
                            pop[d] += family[f].size;
                            week[d]++;
                        }
                        else if(family[f].preference[1] == d) {
                            family[f].day = d;
                            family[f].choose = 1;
                            pop[d] += family[f].size;
                            week[d]++;
                        }
                    }
                    else {
                        if(family[f].preference[c] == d) {
                            family[f].day = d;
                            family[f].choose = c;
                            pop[d] += family[f].size;
                            week[d]++;
                        }
                    }
                }
                f++;
            }
        }
        d++;
        c++;
    }
}

int Pay(int *week, int *pop) {
    int f, d;
    int cost, punish, pay;
    
    cost = 0;
    for(f = 0; f < 449; f++){   //Calculate cost
        switch (family[f].choose) {
            case 0:
                break;
            case 1:
                cost += 50;
                break;
            case 2:
                cost += 50 + 9 * family[f].size;
                break;
            case 3:
                cost += 100 + 9 * family[f].size;
                break;
            case 4:
                cost += 200 + 9 * family[f].size;
                break;
        }
    }
    
    punish = 0;
    for(d = 0; d < 6; d++)  //Calculate pushish
        punish += ((pop[d] - 125) / 400) * pow(pop[d], abs(pop[d]-pop[d + 1]) * 0.5);
    punish += ((pop[d] - 125) / 400) * pow(pop[6], abs(pop[6]-pop[6]) * 0.5);
    
    pay = cost + punish;    //Calculate pay
    
    return pay;
}

void Play(int *week, int *pop, int pay){
    int n;
    char *date[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for(n = 0; n < 6; n++)
        printf("The number of reception families on %s is %d, and the total population of this day is %d\n", date[n], week[n], pop[n]);
    
    printf("The total pay is %d\n", pay);
}

int main() {
    float pay;
    int week[] = {0, 0, 0, 0, 0, 0, 0};
    int pop[] = {0, 0, 0, 0, 0, 0, 0};
    
    if(ReadCsv() == 0) {
        Plan(week, pop);
        pay = Pay(week, pop);
        Play(week, pop, pay);
    }
    else
        printf("error");
        return -1;
    
    return 0;
}
