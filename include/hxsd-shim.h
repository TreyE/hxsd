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
  xmlSchemaParserCtxtPtr schema_parser_context;
  xmlSchemaValidCtxtPtr schema_validation_context;
  xmlSchemaPtr schema;
} SValidationContext;

xmlDocPtr parseDocFile(char* file_uri);

#endif
