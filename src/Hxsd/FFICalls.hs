{-# LANGUAGE ForeignFunctionInterface,CPP #-}
module Hxsd.FFICalls where

import Hxsd.Types
import Foreign.Ptr
import Foreign.ForeignPtr
import Foreign.C.String
import Foreign.C.Types

data SchemaValidContext = SchemaValidContext
type SchemaValidContextPtr = Ptr SchemaValidContext
type SchemaValidContextFPtr = ForeignPtr SchemaValidContext
data HXmlDoc = HXmlDoc
type HXmlDocPtr = Ptr HXmlDoc
type HXmlDocFPtr = ForeignPtr HXmlDoc

data SValidationErrors = SValidationErrors
type SValidationErrorsPtr = Ptr SValidationErrors

foreign import ccall "hxsd-shim.h &xmlFreeDoc" xmlFreeDoc :: FinalizerPtr HXmlDoc
foreign import ccall "hxsd-shim.h &freeSValidationContext" freeSValidationContext:: FinalizerPtr SchemaValidContext
foreign import ccall "hxsd-shim.h parseDocString" parseDocString :: CString -> Int -> IO HXmlDocPtr
foreign import ccall "hxsd-shim.h new_schema_validation_errors" new_schema_validation_errors :: IO SValidationErrorsPtr
foreign import ccall "hxsd-shim.h &free_schema_validation_errors" free_schema_validation_errors :: FinalizerPtr SValidationErrors
foreign import ccall "hxsd-shim.h hs_get_error_count" hs_get_error_count :: SValidationErrorsPtr -> IO Int
foreign import ccall "hxsd-shim.h hs_get_error_message" hs_get_error_message :: SValidationErrorsPtr -> Int -> IO CString
foreign import ccall "hxsd-shim.h loadSchemaFromFile" loadSchemaFromFile :: CString -> IO SchemaValidContextPtr
foreign import ccall "hxsd-shim.h runValidationsAgainstDoc" runValidationsAgainstDoc :: SchemaValidContextPtr -> SValidationErrorsPtr -> HXmlDocPtr -> IO Int

copyErrorsToList :: Int -> SValidationErrorsPtr -> IO [String]
copyErrorsToList i svep = case i of
                            0 -> return []
                            errs -> mapM (\x -> ((hs_get_error_message svep x) >>= peekCString)) ([0..(errs - 1)])

extractSchemaErrors :: SValidationErrorsPtr -> IO [String]
extractSchemaErrors svep = do
                             ec <- hs_get_error_count svep
                             rv <- copyErrorsToList ec svep
                             finalPtr <- (newForeignPtr (free_schema_validation_errors) svep)
                             finalizeForeignPtr finalPtr
                             return rv

throwAwayValidationErrors :: SValidationErrorsPtr -> IO ()
throwAwayValidationErrors ve = do
                                 finalPtr <- (newForeignPtr (free_schema_validation_errors) ve)
                                 finalizeForeignPtr finalPtr

parseXmlString :: String -> IO (XmlParsingResult HXmlDocFPtr)
parseXmlString s = do
                    (cs,l) <- newCStringLen s
                    dfp <- parseDocString cs l
                    if (dfp == nullPtr) then 
                       return (Left (XmlParsingFailure))
                    else
                       (newForeignPtr (xmlFreeDoc) dfp) >>= (\x -> return (Right x))

parseSchemaFile :: String -> IO (SchemaLoadResult SchemaValidContextFPtr)
parseSchemaFile s = do
                      cs <- newCString s
                      dfp <- loadSchemaFromFile cs
                      if (dfp == nullPtr) then 
                         return (Left SchemaLoadFailure)
                      else
                         (newForeignPtr (freeSValidationContext) dfp) >>= (\x -> return (Right x))

validateXmlAgainstSchema :: SchemaValidContextFPtr -> HXmlDocFPtr -> IO XmlSchemaValidationResult
validateXmlAgainstSchema sc xdoc = do
                                     errs_context <- new_schema_validation_errors
                                     validate_result <- withForeignPtr sc (\s -> withForeignPtr xdoc (\d -> runValidationsAgainstDoc s errs_context d))
                                     case validate_result of
                                       0 -> (throwAwayValidationErrors errs_context) >>= (\x -> return XmlIsSchemaValid)
                                       _ -> (extractSchemaErrors errs_context) >>= (\x -> return (XmlFailsSchemaValidation x))
