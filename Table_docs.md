В файле `Table.h` описаны следующие методы для работы с базой данных:

1. `enum Errors createTable(char *tableName, struct TableStruct *tableStruct)`: Создает таблицу с указанным именем и определением полей.

2. `enum Errors deleteTable(char *tableName)`: Удаляет таблицу с указанным именем.

3. `enum Errors openTable(char *tableName, THandle *tableHandle)`: Открывает таблицу с указанным именем и возвращает дескриптор таблицы.

4. `enum Errors closeTable(THandle tableHandle)`: Закрывает таблицу, освобождая ресурсы, связанные с ней.

5. `enum Errors moveFirst(THandle tableHandle)`: Перемещает указатель на первую запись в таблице.

6. `enum Errors moveLast(THandle tableHandle)`: Перемещает указатель на последнюю запись в таблице.

7. `enum Errors moveNext(THandle tableHandle)`: Перемещает указатель на следующую запись в таблице.

8. `enum Errors movePrevious(THandle tableHandle)`: Перемещает указатель на предыдущую запись в таблице.

9. `Bool beforeFirst(THandle tableHandle)`: Проверяет, находится ли указатель перед первой записью в таблице.

10. `Bool afterLast(THandle tableHandle)`: Проверяет, находится ли указатель после последней записи в таблице.

11. `enum Errors getText(THandle tableHandle, char *fieldName, char **pvalue)`: Получает текстовое значение указанного поля текущей записи.

12. `enum Errors getLong(THandle tableHandle, char *fieldName, long *pvalue)`: Получает целочисленное значение указанного поля текущей записи.

13. `enum Errors startEdit(THandle tableHandle)`: Начинает редактирование текущей записи.

14. `enum Errors putText(THandle tableHandle, char *fieldName, char *value)`: Устанавливает текстовое значение для указанного поля текущей записи.

15. `enum Errors putLong(THandle tableHandle, char *fieldName, long value)`: Устанавливает целочисленное значение для указанного поля текущей записи.

16. `enum Errors finishEdit(THandle tableHandle)`: Завершает редактирование текущей записи.

17. `enum Errors createNew(THandle tableHandle)`: Создает новую запись в таблице.

18. `enum Errors putTextNew(THandle tableHandle, char *fieldName, char *value)`: Устанавливает текстовое значение для указанного поля в новой записи.

19. `enum Errors putLongNew(THandle tableHandle, char *fieldName, long value)`: Устанавливает целочисленное значение для указанного поля в новой записи.

20. `enum Errors insertNew(THandle tableHandle)`: Вставляет новую запись в таблицу.

21. `enum Errors deleteRec(THandle tableHandle)`: Удаляет текущую запись из таблицы.

22. `char *getErrorString(enum Errors code)`: Возвращает строковое представление кода ошибки.

23. `enum Errors getFieldLen(THandle tableHandle, char *fieldName, unsigned *plen)`: Получает длину поля в таблице.

24. `enum Errors getFieldType(THandle tableHandle, char *fieldName, enum FieldType *ptype)`: Получает тип поля в таблице.

25. `enum Errors getFieldsNum(THandle tableHandle, unsigned *pNum)`: Получает количество полей в таблице.

26. `enum Errors getFieldName(THandle tableHandle, unsigned index, char **pFieldName)`: Получает имя поля по индексу.