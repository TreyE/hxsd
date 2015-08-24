{-# LANGUAGE ForeignFunctionInterface,CPP #-}
module Hxsd.FFICalls where

import Hxsd.Types
import Foreign.Ptr
import Foreign.C.String
import Foreign.C.Types

data SchemaValidContext = SchemaValidContext
type SchemaValidContextPtr = Ptr SchemaValidContext
data HXmlDoc = HXmlDoc
type HXmlDocPtr = Ptr HXmlDoc

foreign import ccall "hxsd-shim.h parseDocFile" parseDocFile :: CString -> IO HXmlDocPtr
