#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COLUMNS 10
#define MAX_ROWS 100
#define MAX_NAME_LEN 50

typedef struct {
    char name[MAX_NAME_LEN];
    int num_columns;
    char column_names[MAX_COLUMNS][MAX_NAME_LEN];
    int num_rows;
    char data[MAX_ROWS][MAX_COLUMNS][MAX_NAME_LEN];
} Table;

Table db_table;

void init_table(char *table_name) {
    strcpy(db_table.name, table_name);
    db_table.num_columns=0;
    db_table.num_rows=0;
    printf("Table %s created\n", table_name);
}


void add_column(char *col_name) {
    if(db_table.num_columns >= MAX_COLUMNS) {
        printf("Max columns reached\n");
        return;
    }

    strcpy(db_table.column_names[db_table.num_columns], col_name);
    db_table.num_columns++;
    printf("Column %s added\n", col_name);
}

void insert_row(char values[][MAX_NAME_LEN], int num_values) {
    if(db_table.num_rows >= MAX_ROWS) {
        printf("max rows reached");
        return;
    }

    if(num_values !=db_table.num_columns){
        printf("error: expected %d values, got %d\n", db_table.num_columns, num_values);
        return;
    }

    for(int i=0;i<num_values;i++){
        strcpy(db_table.data[db_table.num_rows][i],values[i]);
    }

    db_table.num_rows++;
    printf("row inserted");
}

void select_all(){
    printf("\n");

    for (int i=0; i< db_table.num_columns;i++){
        printf("%-20s", db_table.column_names[i]);
    }
    printf("\n");

    for (int i=0; i < db_table.num_columns*20;i++)
        printf("-");
    printf("\n");

    for (int i=0;i<db_table.num_rows;i++){
        for(int j=0;j<db_table.num_columns;j++){
            printf("%-20s", db_table.data[i][j]);
        }
        printf("\n");
    }
}

void select_where(char*column, char*value) {
    int col_idx=-1;
    for (int i=0;i<db_table.num_columns;i++){
        if(strcmp(db_table.column_names[i], column) == 0){
            col_idx= i;
            break;
        }
    }

    if(col_idx==-1){
        printf("column %s not found", column);
        return;
    }

    printf("\n");

    // print headers
    for(int i=0;i<db_table.num_columns;i++){
        printf("%-20s", db_table.column_names[i]);

    }

    printf("\n");

    for(int i=0;i<db_table.num_columns*20;i++) printf("-");
    printf("\n");

    int found =0;
    for (int i=0;i<db_table.num_rows;i++){
        if(strcmp(db_table.data[i][col_idx], value)==0) {
            for (int j=0; j < db_table.num_columns;j++){
                printf("%-20s", db_table.data[i][j]);
            }
            printf("\n");
            found++;
        }
    }

    printf("%d rows found\n", found);
}

void delete_where(char*column, char*value) {
    int col_idx=-1;
    for(int i=0;i<db_table.num_columns;i++){
        if(strcmp(db_table.column_names[i], column)==0){
            col_idx=i;
            break;
        }
    }


    if (col_idx==-1){
        printf("column %s not found", column);
        return;
    }

    int deleted=0;
    int i=0;

    while (i < db_table.num_rows){
        if(strcmp(db_table.data[i][col_idx],value)==0){
            for(int j=i;j<db_table.num_rows-1;j++){
                for (int k=0;k <db_table.num_columns;k++)
                    strcpy(db_table.data[j][k], db_table.data[j+1][k]);
            }
            db_table.num_rows--;
            deleted++;
        } else {
            i++;
        }

    }

    printf("%d rows deleted", deleted);
}


void update_where(char *where_col, char *where_val, char*set_col, char *set_val) {
    int where_idx = -1, set_idx = -1;

    for (int i=0;i<db_table.num_columns;i++){
        if(strcmp(db_table.column_names[i], where_col)==0)
            where_idx=i;
        if(strcmp(db_table.column_names[i], set_col)==0)
            set_idx=i;
    }

    if(where_idx==-1){
        printf("column %s not found", where_col);
        return;
    }

    if(set_idx==-1){
        printf("column %s not found", set_col);
        return;
    }

    int updated=0;

    for (int i=0; i<db_table.num_rows;i++){
        printf("debug: %s %s %d\n", db_table.data[i][where_idx], where_val, strcmp(db_table.data[i][where_idx], where_val));
        if(strcmp(db_table.data[i][where_idx], where_val)==0){
            strcpy(db_table.data[i][set_idx], set_val);
            updated++;
        }
    }
    printf("%d rows updated\n", updated);
}


void save_to_file(char*filename) {
    FILE *fp=fopen(filename,"w");
    if(fp==NULL){
        printf("error opening file for writing, fopen failed");
        return;
    }

    for (int i=0;i< db_table.num_columns;i++){
        fprintf(fp, "%s", db_table.column_names[i]);
        if(i<db_table.num_columns-1){
            fprintf(fp, ",");
        }
    }

    fprintf(fp, "\n");

    for (int i=0;i<db_table.num_rows;i++){
        for(int j=0;j<db_table.num_columns;j++){
            fprintf(fp,"%s", db_table.data[i][j]);
            if(j < db_table.num_columns-1){
                fprintf(fp, ",");
            }
        }

        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("table saved to %s\n", filename);
}

void load_from_file(char*filename){
    printf("debug: %s\n", filename);
    FILE *fp=fopen(filename, "r");
    printf("debug: %p\n", fp);
    
    if(fp==NULL){
        perror("fopen failed\n");
        return;
    }

    char line[1024];

    if(fgets(line, sizeof(line), fp)!=NULL){
        line[strcspn(line, "\n")]=0;

        char *token=strtok(line, ",");
        db_table.num_columns=0;
        while(token && db_table.num_columns<MAX_COLUMNS) {
            strcpy(db_table.column_names[db_table.num_columns], token);
            db_table.num_columns++;
            token=strtok(NULL, ",");
        }
    }

    db_table.num_rows=0;
    while(fgets(line,sizeof(line),fp)&&db_table.num_rows<MAX_ROWS){
        line[strcspn(line, "\n")]=0;

        char *token=strtok(line, ",");
        int col=0;
        while(token&&col<db_table.num_columns) {
            strcpy(db_table.data[db_table.num_rows][col], token);
            col++;
            token=strtok(NULL, ",");
        }
        db_table.num_rows++;
    }
    fclose(fp);
    printf("table loaded from %s\n", filename);
}

void run_repl(){
    char command[20];
    char arg1[MAX_NAME_LEN], arg2[MAX_NAME_LEN], arg3[MAX_NAME_LEN];

    printf("Mini SQL - Commands: INSERT, SELECT, DELETE, UPDATE, SAVE, LOAD, EXIT\n");

    while (1){
        printf("SQL> ");
        fflush(stdout);

        if(scanf("%s", command) != 1) break;

        if(strcmp(command, "EXIT") == 0) break;

        else if (strcmp(command, "INSERT") == 0) {
            char values[MAX_COLUMNS][MAX_NAME_LEN];
            for (int i=0;i< db_table.num_columns;i++)
                scanf("%s", values[i]);

            insert_row(values, db_table.num_columns);

        }
        else if(strcmp(command, "SELECT") == 0) select_all();
        else if (strcmp(command, "DELETE")==0) {
            scanf("%s %s", arg1, arg2);
            delete_where(arg1, arg2);
        }
        else if(strcmp(command, "UPDATE") == 0){
            char arg4[MAX_NAME_LEN];
            scanf("%s %s %s %s", arg1, arg2, arg3, arg4);
            update_where(arg1, arg2, arg3, arg4);
        }

        else if (strcmp(command, "SAVE") == 0) {
            scanf("%s", arg1);
            save_to_file(arg1);
        } else if(strcmp(command, "LOAD")==0){
            scanf("%s", arg1);
            load_from_file(arg1);
        }else {
            printf("unknown command\n");
        }
    }
}

int main() {
    init_table("users");
    add_column("id");
    add_column("name");
    add_column("email");

    
    run_repl();

    // char row1[3][MAX_NAME_LEN]={"1", "Alice", "alice@email.com"};
    // char row2[3][MAX_NAME_LEN] ={"2","Bob", "bob@email.com"};
    // char row3[3][MAX_NAME_LEN]={"3", "Charlie", "charlie@email.com"};

    // insert_row(row1, 3);
    // insert_row(row2,3);
    // insert_row(row3, 3);

    // select_all();


    // printf("\nWHERE name = Alice");
    // select_where("name", "Alice");
    

    // delete_where("name", "Alice");
    // select_all();

    // update_where("name", "Bob", "email", "newemail@test.com");
    // select_all();

    // printf("\nsaving to file\n");
    // save_to_file("users.csv");


    // // clear table and reload
    // db_table.num_rows=0;
    // db_table.num_columns=0;

    // printf("table empty, rows: %d", db_table.num_rows);

    // load_from_file("users.csv");
    // select_all();
    return 0;
}