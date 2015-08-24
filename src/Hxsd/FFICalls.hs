{-# LANGUAGE ForeignFunctionInterface,CPP #-}
module Hxsd.FFICalls where

import Hxsd.Types
import Foreign.Ptr
import Foreign.ForeignPtr
import Foreign.C.String
import Foreign.C.Types

data SchemaValidContext = SchemaValidContext
type SchemaValidContextPtr = Ptr SchemaValidContext
data HXmlDoc = HXmlDoc
type HXmlDocPtr = Ptr HXmlDoc
type HXmlDocFPtr = ForeignPtr HXmlDoc

data SValidationErrors = SValidationErrors
type SValidationErrorsPtr = Ptr SValidationErrors

foreign import ccall "hxsd-shim.h &xmlFreeDoc" xmlFreeDoc :: FinalizerPtr HXmlDoc
foreign import ccall "hxsd-shim.h parseDocString" parseDocString :: CString -> Int -> IO HXmlDocPtr
foreign import ccall "hxsd-shim.h hs_get_error_count" hs_get_error_count :: SValidationErrorsPtr -> IO CInt
foreign import ccall "hxsd-shim.h hs_get_error_message" hs_get_error_message :: SValidationErrorsPtr -> CInt -> IO CString

parseXmlString :: String -> IO (Maybe HXmlDocFPtr)
parseXmlString s = do
                    (cs,l) <- newCStringLen s
                    dfp <- parseDocString cs l
                    if (dfp == nullPtr) then 
                       return (Nothing)
                    else
                       (newForeignPtr (xmlFreeDoc) dfp) >>= (\x -> return (Just x))
