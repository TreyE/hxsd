#include <hxsd-shim.h>

char* hs_get_error_message(SValidationErrors* val_struct, int idx) {
  return(val_struct->errors[idx]);
}

int hs_get_error_count(SValidationErrors* val_struct) {
  return(val_struct->error_size);
}

xmlDocPtr parseDocString(char* doc_data, int doc_len) {
  xmlDocPtr docPtr;
  docPtr = xmlReadMemory(doc_data, doc_len, NULL, NULL, 0);
  if (docPtr == NULL) {
          xmlFreeDoc(docPtr);
	  return NULL;
  }
  return docPtr;
}

SValidationErrors* new_schema_validation_errors() {
  SValidationErrors* new_struct = (SValidationErrors*)malloc(sizeof(SValidationErrors));
  new_struct->errors = NULL;
  new_struct->error_size = 0;
  return new_struct;
}

void free_schema_validation_errors(SValidationErrors* val_struct) {
        for (int i = 0; i <= val_struct->error_size- 1; i++) {
		free((void *)val_struct->errors[i]);
	}
	free(val_struct);
}

void add_error_to_context(SValidationErrors* valErrors, const char* err) {
  int error_count = valErrors->error_size;
  char** all_errors = valErrors->errors;
  int new_error_size = error_count + 1;
  valErrors->error_size = new_error_size;
  char* new_error = strdup(err);
  if (error_count == 0) {
	  valErrors->errors = (char**)calloc(1,sizeof(char*));
	  valErrors->errors[0] = new_error;
  } else {
	  valErrors->error_size = new_error_size;
	  char **new_ptr = (char**)calloc(new_error_size, sizeof(char*));
	  int copy_size = error_count * sizeof(char*);
	  valErrors->errors = new_ptr;
	  memcpy(new_ptr,all_errors,copy_size);
	  free(all_errors);
	  valErrors->errors[error_count] = new_error;
  }
}

void validityErrorCallback(void * ctx, xmlErrorPtr err) {
	add_error_to_context((SValidationErrors*)ctx, err->message);
}

SValidationContext* loadSchemaFromFile(char* file_location) {
  xmlSchemaParserCtxtPtr s_parse_result = NULL;
  xmlSchemaValidCtxtPtr valid_ctxt = NULL;
  xmlSchemaPtr s_ptr = NULL;
  SValidationContext* result = NULL;
  s_parse_result = xmlSchemaNewParserCtxt(file_location);
  if (s_parse_result == NULL) {
	  return NULL;
  }
  result = (SValidationContext*)malloc(sizeof(SValidationContext));
  s_ptr = xmlSchemaParse(s_parse_result);
  valid_ctxt = xmlSchemaNewValidCtxt(s_ptr);
  result->schema = s_ptr;
  result->schema_validation_context = valid_ctxt;
  result->schema_parser_context = s_parse_result;
  return result;
}

void freeSValidationContext(SValidationContext* ctx) {
  xmlSchemaFreeValidCtxt(ctx->schema_validation_context);
  xmlSchemaFree(ctx->schema);
  xmlSchemaFreeParserCtxt(ctx->schema_parser_context);
  free(ctx);
}

int runValidationsAgainstDoc(SValidationContext* v_ctx, SValidationErrors* errs, xmlDocPtr doc) {
  int res;
  xmlSchemaSetValidStructuredErrors(v_ctx->schema_validation_context, &validityErrorCallback, (void *)errs);
  res = xmlSchemaValidateDoc(v_ctx->schema_validation_context,doc);
  return res;
}

/*
int main(int arc, char ** argc) {
  char* schema_url = "/Users/tevans/proj/cv/vocabulary.xsd";
  SValidationContext* s_context = loadSchemaFromFile(schema_url);
  SValidationErrors* vals = new_schema_validation_errors();
  xmlDocPtr xd_ptr = NULL;
  xd_ptr = parseDocFile("example.xml");
  if (xd_ptr == NULL) {
    printf("FAILED TO LOAD\n");
    return -1;
  }
  runValidationsAgainstDoc(s_context, vals, xd_ptr);
  for (int i = 0; i <= (vals->error_size - 1); i++) {
    printf("%s\n", vals->errors[i]);
  }
  free_schema_validation_errors(vals);
  freeSValidationContext(s_context);
  return 0;
}
*/
