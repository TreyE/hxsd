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

mConsTransform2 :: Monad m => ((inType -> m a),(inType -> m b)) -> (a -> b -> builtType) -> inType -> m builtType
mConsTransform2 (f1, f2) c x = do
                                 a_val <- f1 x
                                 b_val <- f2 x
                                 return $ c a_val b_val

mConsTransform3 :: Monad m => ((inType -> m a),(inType -> m b), (inType -> m c)) -> (a -> b -> c -> builtType) -> inType -> m builtType
mConsTransform3 (f1, f2, f3) c x = do
                                 a_val <- f1 x
                                 b_val <- f2 x
                                 c_val <- f3 x
                                 return $ c a_val b_val c_val