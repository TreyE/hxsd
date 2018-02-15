#ifndef HSLIBXML_SHIM_H
#define HSLIBXML_SHIM_H
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <libxml/xmlschemas.h>

typedef struct {
  int error_size;
  char **errors;
} SValidationErrors;

typedef struct {
  xmlSchemaPtr schema;
} SValidationContext;

xmlDocPtr parseDocString(char* doc_data, int doc_len);

SValidationContext* loadSchemaFromFile(char* file_location);
void freeSValidationContext(SValidationContext* ctx);

SValidationErrors* new_schema_validation_errors();
void free_schema_validation_errors(SValidationErrors* val_struct);

char* hs_get_error_message(SValidationErrors* val_struct, int idx);

int hs_get_error_count(SValidationErrors* val_struct);
int runValidationsAgainstDoc(SValidationContext* v_ctx, SValidationErrors* errs, xmlDocPtr doc);
#endif
