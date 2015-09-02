module Hxsd.Types where

data XmlParsingFailure = XmlParsingFailure deriving Eq
data SchemaLoadFailure = SchemaLoadFailure deriving Eq

data XmlSchemaValidationResult = XmlIsSchemaValid
                                 | XmlFailsSchemaValidation [String]

data XmlAgainstSchemaFileResult = XmlSchemaValidationCompleted XmlSchemaValidationResult
                                  | XmlDataParseFailure XmlParsingFailure
                                  | SchemaFileLoadFailure SchemaLoadFailure

type XmlParsingResult a = Either XmlParsingFailure a
type SchemaLoadResult a = Either SchemaLoadFailure a
