#include <hxsd-shim.h>
#include <stdarg.h>

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
  if (val_struct->error_size > 0) {
    for (int i = 0; i <= val_struct->error_size - 1; i++) {
		  free((void *)val_struct->errors[i]);
	  }
  }
	free(val_struct);
}

/*
 * We need to make a copy of the string pointer -
 * it will go away when we free the context
 */
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
  xmlSchemaParserCtxtPtr s_parse_result;
  xmlSchemaValidCtxtPtr valid_ctxt;
  xmlSchemaPtr s_ptr;
  SValidationContext* result = NULL;
  s_parse_result = xmlSchemaNewParserCtxt(file_location);
  if (s_parse_result == NULL) {
    xmlSchemaFreeParserCtxt(s_parse_result);
	  return NULL;
  }
  result = (SValidationContext*)malloc(sizeof(SValidationContext));
  s_ptr = xmlSchemaParse(s_parse_result);
  xmlSchemaFreeParserCtxt(s_parse_result);
  if (s_ptr == NULL) {
	  free(result);
	  return NULL;
  }
  result->schema = s_ptr;
  return result;
}

void freeSValidationContext(SValidationContext* ctx) {
  xmlSchemaFree(ctx->schema);
  free(ctx);
}

int runValidationsAgainstDoc(SValidationContext* v_ctx, SValidationErrors* errs, xmlDocPtr doc) {
  int res;
  xmlSchemaValidCtxtPtr schema_validation_context;
  schema_validation_context = xmlSchemaNewValidCtxt(v_ctx->schema);
  xmlSchemaSetValidStructuredErrors(schema_validation_context, &validityErrorCallback, (void *)errs);
  res = xmlSchemaValidateDoc(schema_validation_context,doc);
  xmlSchemaFreeValidCtxt(schema_validation_context);
  return res;
}

void freeXMLParseBuffer(XMLParseBuffer* buf) {
  if (buf->not_read) {
    xmlFreeParserInputBuffer(buf->buff);
  }
  free(buf);
}

XMLParseBuffer * newXMLParseBuffer() {
	XMLParseBuffer *xml_parse_buffer;
	xml_parse_buffer = (XMLParseBuffer*)malloc(sizeof(XMLParseBuffer));
  xml_parse_buffer->enc = XML_CHAR_ENCODING_UTF8;
  xml_parse_buffer->not_read = 1;
	return xml_parse_buffer;
}

XMLParseBuffer * newXMLParseBufferFromHaskellMem(const char * mem, int size) {
	XMLParseBuffer *xml_parse_buffer;
	xmlParserInputBuffer* buff;
	xml_parse_buffer = newXMLParseBuffer();
	buff = xmlParserInputBufferCreateMem(mem, size, xml_parse_buffer->enc);
  if (buff == NULL) {
		free(xml_parse_buffer);
		return NULL;
	}
	xml_parse_buffer->buff = buff;
	return xml_parse_buffer;
}

XMLParseBuffer * newXMLParseBufferFromFilePath(const char * path) {
	XMLParseBuffer *xml_parse_buffer;
	xmlParserInputBuffer* buff;
	xml_parse_buffer = newXMLParseBuffer();
	buff = xmlParserInputBufferCreateFilename(path, xml_parse_buffer->enc);
        if (buff == NULL) {
		free(xml_parse_buffer);
		return NULL;
	}
	xml_parse_buffer->buff = buff;
	return xml_parse_buffer;
}

xmlSAXHandler emptySAXHandlerStruct = {
    NULL, /* internalSubset */
    NULL, /* isStandalone */
    NULL, /* hasInternalSubset */
    NULL, /* hasExternalSubset */
    NULL, /* resolveEntity */
    NULL, /* getEntity */
    NULL, /* entityDecl */
    NULL, /* notationDecl */
    NULL, /* attributeDecl */
    NULL, /* elementDecl */
    NULL, /* unparsedEntityDecl */
    NULL, /* setDocumentLocator */
    NULL, /* startDocument */
    NULL, /* endDocument */
    NULL, /* startElement */
    NULL, /* endElement */
    NULL, /* reference */
    NULL, /* characters */
    NULL, /* ignorableWhitespace */
    NULL, /* processingInstruction */
    NULL, /* comment */
    NULL, /* xmlParserWarning */
    NULL, /* xmlParserError */
    NULL, /* xmlParserError */
    NULL, /* getParameterEntity */
    NULL, /* cdataBlock; */
    NULL, /* externalSubset; */
    XML_SAX2_MAGIC,
    NULL,
    NULL, /* startElementNs */
    NULL, /* endElementNs */
    NULL  /* xmlStructuredErrorFunc */
};

int runValidationsAgainstSAX(SValidationContext* v_ctx, SValidationErrors* errs, XMLParseBuffer* buff) {
  int res;
  xmlSAXHandlerPtr saxHandler;
  xmlSchemaValidCtxtPtr schema_validation_context;
  schema_validation_context = xmlSchemaNewValidCtxt(v_ctx->schema);
  saxHandler = &emptySAXHandlerStruct;
  xmlSchemaSetValidStructuredErrors(schema_validation_context, &validityErrorCallback, (void *)errs);
  // Consumes the buffer, so we don't need to free it.
  res = xmlSchemaValidateStream(schema_validation_context, buff->buff, buff->enc, saxHandler, (void *)errs);
  buff->not_read = 0;
  xmlSchemaFreeValidCtxt(schema_validation_context);
  return res;
}