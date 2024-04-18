     #include <stdio.h>
     #include <stdlib.h>
     #include <string.h>
     #include <libpq-fe.h>
    
        /*
     	Функция выхода из программы с сообщением об ошибке.
     */
     void err_exit(PGconn *conn) {
         PQfinish(conn); //Закрытие соединения с сервером базы данных 
         exit(1);        //Функция возвращает значение 1 -> программа завершилась аварийно
      }
    
     /*
        Функция печати результата запроса на экран
             *conn -> дескриптор подключения к серверу
             query -> текст запроса 
    */
     void print_query(PGconn *conn, char* query){
         PGresult *res = PQexec(conn, query); //Передача запроса на сервер
         int rows = PQntuples(res);          //Получение числа строк в результате запроса
         int cols = PQnfields(res);          //Получение числа столбцов в результате запроса
    
    
         // Проверка наличия возвращенных запросом строк. 
         // В случае их отсутствие - аварийное завершение программы
         if (PQresultStatus(res) != PGRES_TUPLES_OK) {
             printf("No data retrieved\n");        
               PQclear(res);
               err_exit(conn);
           } 
           //Вывод результата запроса на экран
           for(int i=0; i<rows; i++) {
               for(int j=0; j<cols; j++) {
                   printf("%s ", PQgetvalue(res, i, j));
               }
               printf("\n");
           }     
           PQclear(res);
       }
       int main() {
         //Подключение к серверу базы данных
         PGconn *conn = PQconnectdb("user=SAB password=123456 dbname=students");
    
     //Проверка статуса подключения. В случае ошибки - аварийное завершение программы
         if (PQstatus(conn) == CONNECTION_BAD) {        
             fprintf(stderr, "Connection to database failed: %s\n",PQerrorMessage(conn));
             err_exit(conn);
         }
         print_query(conn, "SELECT * FROM students LIMIT 5;");
         PQfinish(conn);
         return 0;
     }
     