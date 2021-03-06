#ifndef HSLIBXML_SHIM_H
#define HSLIBXML_SHIM_H
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <libxml/xmlschemas.h>

typedef struct {
  char *message;
  int col;
  int line;
} SValError;

typedef struct {
  int error_size;
  SValError** errors;
} SValidationErrors;

typedef struct {
  xmlSchemaPtr schema;
} SValidationContext;

typedef struct {
  xmlParserInputBuffer* buff;
  xmlCharEncoding enc;
  char notString;
} XMLParseBuffer;

xmlDocPtr parseDocString(char* doc_data, int doc_len);

SValidationContext* loadSchemaFromFile(char* file_location);
void freeSValidationContext(SValidationContext* ctx);

SValidationErrors* new_schema_validation_errors();
void free_schema_validation_errors(SValidationErrors* val_struct);

char* hs_get_error_message(int idx, SValidationErrors* val_struct);

int hs_get_error_count(SValidationErrors* val_struct);
int hs_get_error_line(int idx, SValidationErrors* val_struct);
int hs_get_error_col(int idx, SValidationErrors* val_struct);
int runValidationsAgainstDoc(SValidationContext* v_ctx, SValidationErrors* errs, xmlDocPtr doc);

XMLParseBuffer * newXMLParseBufferFromHaskellMem(const char * mem, int size);
XMLParseBuffer * newXMLParseBufferFromFilePath(const char * path);
void freeXMLParseBuffer(XMLParseBuffer* buf);
int runValidationsAgainstSAX(SValidationContext* v_ctx, SValidationErrors* errs, XMLParseBuffer* buff);
#endif
