module Hxsd.Types where

data XmlParsingFailure = XmlParsingFailure deriving Eq
data SchemaLoadFailure = SchemaLoadFailure deriving Eq

data XmlSchemaValidationError = XmlSchemaValidationError
                                   String -- Message
                                   Int -- Line
                                   Int -- Column
                                   deriving(Eq, Show)

data XmlSchemaValidationResult = XmlIsSchemaValid
                                 | XmlFailsSchemaValidation [XmlSchemaValidationError]

data XmlAgainstSchemaFileResult = XmlSchemaValidationCompleted XmlSchemaValidationResult
                                  | XmlDataParseFailure XmlParsingFailure
                                  | SchemaFileLoadFailure SchemaLoadFailure

type XmlParsingResult a = Either XmlParsingFailure a
type SchemaLoadResult a = Either SchemaLoadFailure a
