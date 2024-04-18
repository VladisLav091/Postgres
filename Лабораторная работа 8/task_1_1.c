#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

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

// Функция для выполнения безопасного SQL запроса с параметрами
void execute_safe_query(PGconn *conn, const char *student_id) {
    const char *paramValues[1]; // Массив параметров запроса
    char query[512]; // Буфер для формирования запроса

    // Формирование SQL запроса с использованием параметров
    snprintf(query, sizeof(query), "SELECT s.first_name, s.last_name, s.patronymic, fc.mark, f.field_name "
                                    "FROM students s "
                                    "INNER JOIN field_comprehensions fc ON s.student_id = fc.student_id "
                                    "INNER JOIN fields f ON f.field_id = fc.field "
                                    "WHERE s.student_id IN ($1)");

    paramValues[0] = student_id; // Параметр идентификатора студента

    // Выполнение безопасного SQL запроса с параметрами
    PGresult *res = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

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

// Функция обработки попытки SQL инъекции
void handle_injection_attempt() {
    printf("Предупреждение: Обнаружена попытка внедрения SQL инъекции.\n");
    // Дополнительные действия при обнаружении инъекции могут быть добавлены здесь
}

int main() {
    // Подключение к серверу базы данных
    PGconn *conn = PQconnectdb("user=SAB password=123456 dbname=students");

    // Проверка статуса подключения
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    // Выполнение безопасного SQL запроса с параметрами
    char student_id[50];
    printf("Введите идентификатор студента: ");
    scanf("%49s", student_id); // Получение идентификатора студента

    execute_safe_query(conn, student_id);

    PQfinish(conn); // Закрытие соединения с сервером базы данных

    return 0;
}