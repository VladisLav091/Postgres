#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
void err_exit(PGconn *conn) {
         PQfinish(conn); //Закрытие соединения с сервером базы данных 
         exit(1);        //Функция возвращает значение 1 -> программа завершилась аварийно
      }
// Функция выполнения SQL запроса
void execute_query(PGconn *conn, const char *query) {
    PGresult *res = PQexec(conn, query); // Выполнение запроса

    if (PQresultStatus(res) != PGRES_TUPLES_OK) { // Проверка статуса выполнения запроса
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    int rows = PQntuples(res); // Получение числа строк результата
    int cols = PQnfields(res); // Получение числа столбцов результата

    // Вывод результата запроса на экран
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%s ", PQgetvalue(res, i, j));
        }
        printf("\n");
    }

    PQclear(res); // Освобождение результата запроса
}
void execute_safe_query(PGconn *conn, const char *student_id) {
    const char *paramValues[1]; // Массив параметров запроса
    char query[512]; // Буфер для формирования запроса

    // Формирование SQL запроса с использованием параметров
    snprintf(query, sizeof(query), "SELECT * FROM students_field_mark WHERE student_id = '%s'", student_id);

    paramValues[0] = student_id; // Параметр идентификатора студента

    // Выполнение SQL запроса
    execute_query(conn, query);
}
void search_mark(PGconn *conn, const char *last_name, const char *first_name, const char *group_number) {
    // SQL запрос для поиска оценки по параметрам
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM field_comprehensions WHERE student_id IN (SELECT student_id FROM students WHERE last_name = '%s' AND first_name = '%s' AND students_group_number = '%s')", last_name, first_name, group_number);
    
    // Выполнение SQL запроса
    execute_query(conn, query);
}
// Функция выполнения безопасного SQL запроса на обновление данных
void execute_safe_update_query(PGconn *conn, const char *student_id, const char *mark, char* f_name) {
    const char *paramValues[3]; // Массив параметров запроса
    char query[512]; // Буфер для формирования запроса
         
    // Формирование SQL запроса с использованием параметров для обновления оценки
    snprintf(query, sizeof(query), "UPDATE field_comprehensions f_c "
                                    "SET mark = $1 "
                                    "FROM fields f "
                                    "WHERE f.field_id = f_c.field "
                                    "AND student_id = $2 "
                                    "AND field_name = $3");

    paramValues[0] = mark; // Параметр новой оценки
    paramValues[1] = student_id; // Параметр идентификатора студента
    paramValues[2] = f_name; // Параметр названия дисциплины

    // Выполнение безопасного SQL запроса с параметрами для обновления данных
    PGresult *res = PQexecParams(conn, query, 3, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) { // Проверка статуса выполнения запроса
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    printf("Update successful\n"); // Вывод сообщения об успешном обновлении

    PQclear(res); // Освобождение результата запроса
}
void execute_safe_insert_query(PGconn *conn, const char *student_id, const char *mark, char* f_name) {
    const char *paramValues[3]; // Массив параметров запроса
    char query[512]; // Буфер для формирования запроса
    
    // Формирование SQL запроса с использованием параметров для обновления оценки
    snprintf(query, sizeof(query), "INSERT INTO field_comprehensions (student_id, field ,mark)"
                                    "SELECT $2, f.field_id, $1"
                                    "FROM fields f "
                                    "WHERE f.field_name = $3");

    paramValues[0] = mark; // Параметр новой оценки
    paramValues[1] = student_id; // Параметр идентификатора студента
    paramValues[2] = f_name; // Параметр названия дисциплины

    // Выполнение безопасного SQL запроса с параметрами для обновления данных
    PGresult *res = PQexecParams(conn, query, 3, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) { // Проверка статуса выполнения запроса
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    printf("Update successful\n"); // Вывод сообщения об успешном обновлении

    PQclear(res); // Освобождение результата запроса
}
void execute_safe_delete_query(PGconn *conn, const char *student_id, char* f_name) {
    const char *paramValues[2]; // Массив параметров запроса
    char query[512]; // Буфер для формирования запроса
         
    // Формирование SQL запроса с использованием параметров для обновления оценки
    snprintf(query, sizeof(query), "DELETE FROM field_comprehensions "
                                    " WHERE student_id = $1 AND field IN (SELECT field_id FROM fields WHERE field_name = $2)");

     // Параметр новой оценки
    paramValues[0] = student_id; // Параметр идентификатора студента
    paramValues[1] = f_name; // Параметр названия дисциплины

    // Выполнение безопасного SQL запроса с параметрами для обновления данных
    PGresult *res = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) { // Проверка статуса выполнения запроса
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    printf("Update successful\n"); // Вывод сообщения об успешном обновлении

    PQclear(res); // Освобождение результата запроса
}
int main() {
    // Переменные для хранения роли, логина и пароля
    char role[50];
    char username[50];
    char password[50];
    char student_id[50];
    char mark[50]; 
    char field_name[256];   
    // Ввод роли, логина и пароля
    printf("Введите роль (admin/junior): ");
    scanf("%49s", role);
    printf("Введите логин: ");
    scanf("%49s", username);
    printf("Введите пароль: ");
    scanf("%49s", password);
    // Подключение к серверу базы данных в зависимости от роли пользователя
    PGconn *conn;
    if (strcmp(role, "admin") == 0) {
        conn = PQsetdbLogin("localhost", "5432", NULL, NULL, "students", username, password);
    } else if (strcmp(role, "junior") == 0) {
        conn = PQsetdbLogin("localhost", "5432", NULL, NULL, "students", username, password);
    } else {
        printf("Неизвестная роль пользователя\n");
        return 1;
    }

    // Проверка статуса подключения
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    while (strcmp(role, "junior") == 0) {
        // Если пользователь - junior, предоставить доступ к просмотру оценок и поиску
        int action;
        printf("Выберите действие:\n");
        printf("1. Просмотр всех оценок\n");
        printf("2. Поиск оценок по параметрам\n");
        printf("3. Exit\n");
        scanf("%d", &action);

        if (action == 1) {
            printf("Введите Номер студенческого: ");
            scanf("%49s", student_id);
            execute_safe_query(conn, student_id);
        } else if (action == 2) {
            // Запрос параметров для поиска
            char last_name[50];
            char first_name[50];
            char group_number[50];
            printf("Введите фамилию: ");
            scanf("%49s", last_name);
            printf("Введите имя: ");
            scanf("%49s", first_name);
            printf("Введите номер группы: ");
            scanf("%49s", group_number);

            search_mark(conn, last_name, first_name, group_number);
        } else if (action ==3){
            break;
        } else {
            printf("Неверное действие\n");
        }
    }
    while (strcmp(role, "admin") == 0) {
        int action;
        printf("Выберите действие:\n");
        printf("1. Добавить оценку\n");
        printf("2. Изменить оценку\n");
        printf("3. Удалить оценку\n");
        printf("4. Exit\n");
        scanf("%d", &action);

        if (action == 1) {
        printf("Введите оценку:\n");
            scanf("%49s", mark);
        getchar();
        printf("Введите студенческий:\n");
            scanf("%49s", student_id);
        getchar();
        printf("Введите название дисциплины:\n");
        //scanf("%s", field_name);
        fgets(field_name, 256 , stdin);
        size_t len = strlen(field_name);
        if(len > 0 && field_name[len -1] == '\n'){
            field_name[len - 1] = '\0';
        }
        execute_safe_insert_query(conn,student_id,mark,field_name);
        PQfinish(conn);
        } else if (action == 2) {
        printf("Введите оценку:\n");
            scanf("%49s", mark);
        getchar();
        printf("Введите студенческий:\n");
            scanf("%49s", student_id);
        getchar();
        printf("Введите название дисциплины:\n");
        //scanf("%s", field_name);
        fgets(field_name, 256 , stdin);
        size_t len = strlen(field_name);
        if(len > 0 && field_name[len -1] == '\n'){
            field_name[len - 1] = '\0';
        }
        execute_safe_update_query(conn,student_id,mark,field_name);
        PQfinish(conn);
        } else if (action == 3) {
        printf("Введите студенческий:\n");
            scanf("%49s", student_id);
        getchar();
        printf("Введите название дисциплины:\n");
        //scanf("%s", field_name);
        fgets(field_name, 256 , stdin);
        size_t len = strlen(field_name);
        if(len > 0 && field_name[len -1] == '\n'){
            field_name[len - 1] = '\0';
        }
        execute_safe_delete_query(conn,student_id,field_name);
        } else if (action == 4) {
            break;   
        } else {
            printf("Неверное действие\n");
        }
    }

    PQfinish(conn); // Закрытие соединения с сервером базы данных

    return 0;
}