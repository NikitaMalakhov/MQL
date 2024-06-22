#if !defined(_Table_h_)
#define _Table_h_

#define MaxFieldNameLen 30

enum Errors
{
  OK,
  CantCreateTable,
  CantOpenTable,
  FieldNotFound,
  BadHandle,
  BadArgs,
  CantMoveToPos,
  CantWriteData,
  CantReadData,
  CorruptedData,
  CantCreateHandle,
  ReadOnlyFile,
  BadFileName,
  CantDeleteTable,
  CorruptedFile,
  BadPos,
  BadFieldType,
  BadFieldLen,
  NoEditing,
  BadPosition,
};

extern char *ErrorText[];

typedef struct Table *THandle;

typedef unsigned Bool;

#define TRUE 1
#define FALSE 0

enum FieldType
{
  Text,
  Long,
  Logic
};

struct FieldDef
{
  char name[MaxFieldNameLen];
  enum FieldType type;
  unsigned len;
};

struct TableStruct
{
  unsigned numOfFields;
  struct FieldDef *fieldsDef;
};

enum Errors createTable(char *tableName,
                        struct TableStruct *tableStruct);

enum Errors deleteTable(char *tableName);

enum Errors openTable(char *tableName, THandle *tableHandle);

enum Errors closeTable(THandle tableHandle);

enum Errors moveFirst(THandle tableHandle);

enum Errors moveLast(THandle tableHandle);

enum Errors moveNext(THandle tableHandle);

enum Errors movePrevios(THandle tableHandle);

Bool beforeFirst(THandle tableHandle);

Bool afterLast(THandle tableHandle);

enum Errors getText(THandle tableHandle, char *fieldName, char **pvalue);

enum Errors getLong(THandle tableHandle, char *fieldName,
                    long *pvalue);

enum Errors startEdit(THandle tableHandle);

enum Errors putText(THandle tableHandle, char *fieldName,
                    char *value);

enum Errors putLong(THandle tableHandle, char *fieldName,
                    long value);

enum Errors finishEdit(THandle tableHandle);

enum Errors createNew(THandle tableHandle);

enum Errors putTextNew(THandle tableHandle, char *fieldName, char *value);
enum Errors putLongNew(THandle tableHandle,
                       char *fieldName, long value);

enum Errors insertNew(THandle tableHandle);

enum Errors insertaNew(THandle tableHandle);

enum Errors insertzNew(THandle tableHandle);

enum Errors deleteRec(THandle tableHandle);

char *getErrorString(enum Errors code);

enum Errors getFieldLen(THandle tableHandle, char *fieldName, unsigned *plen);

enum Errors getFieldType(THandle tableHandle,
                         char *fieldName, enum FieldType *ptype);

enum Errors getFieldsNum(THandle tableHandle,
                         unsigned *pNum);

enum Errors getFieldName(THandle tableHandle,
                         unsigned index, char **pFieldName);
#endif
